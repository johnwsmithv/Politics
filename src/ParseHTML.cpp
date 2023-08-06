#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <array>
#include <algorithm> 
#include <cctype>
#include <locale>

#include <curl/curl.h>
#include <gumbo.h>

#include "String_Helpers.hpp"
#include "Politician.hpp"
#include "House_of_Representatives.hpp"

struct MemoryStruct {
    char * memory;
    size_t memorySize;
};

/**
 * @brief Fancy little Signal Handler
 * 
 * @param sigNum The signal that is being caught
 */
void signalHandler(int sigNum) {
    std::cout << "Process " << getpid() << " got signal " << sigNum << "\n";
    signal(sigNum, SIG_DFL);
    kill(getpid(), sigNum);
}

void checkIfElemNull(void * element, std::string elementName) {
    if(element == NULL) {
        std::cerr << "The element, " << elementName << " is NULL. Terminating.\n";
        exit(EXIT_FAILURE);
    }
}

int getCurrentYear() {
    const auto now = std::chrono::system_clock::now();
    const auto today = std::chrono::time_point_cast<std::chrono::days>(now);
    const auto ymd = std::chrono::year_month_day(today);
    const auto year = ymd.year();
    return static_cast<int>(year);
}

static size_t WriteMemoryCallback(void * contents, size_t memorySize, size_t nmemb, void * userp) {
    size_t realSize = memorySize * nmemb;    
    struct MemoryStruct *mem = static_cast<struct MemoryStruct *>(userp);

    char *ptr = (char *)realloc(mem->memory, mem->memorySize + realSize + 1);

    if(!ptr) {
        // Whelp I guess we ran out of memory, somehow
        std::cout << "Not enough memory (realloc return NULL)\n";
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->memorySize]), contents, realSize);
    mem->memorySize += realSize;
    mem->memory[mem->memorySize] = 0;

    return realSize;
}

MemoryStruct getHTML(std::string url) {
    CURL * curl_handle;
    CURLcode res;

    struct MemoryStruct chunk;
    chunk.memory = (char *)malloc(1);
    chunk.memorySize = 0;

    curl_global_init(CURL_GLOBAL_ALL);
 
    /* init the curl session */
    curl_handle = curl_easy_init();
    
    /* specify URL to get */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    
    /* some servers do not like requests that are made without a user-agent
        field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
    /* get it! */
    res = curl_easy_perform(curl_handle);
    
    /* check for errors */
    if(res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    else {
        /*
        * Now, our chunk.memory points to a memory block that is chunk.size
        * bytes big and contains the remote file.
        *
        * Do something nice with it!
        */
    
        printf("%lu bytes retrieved\n", (unsigned long)chunk.memorySize);
    }

    // std::cout << "The first 5000 bytes of the HTML is: " << std::string(chunk.memory).substr(0, 5000) << "\n";
    
    /* cleanup curl stuff */
    curl_easy_cleanup(curl_handle);
    
    // free(chunk.memory);
    
    /* we are done with libcurl, so clean it up */
    curl_global_cleanup();
    
    return chunk;
}

/**
 * @brief This is a useful function that can take a GumboVector and find a specific HTML tag based on attribute text
 * 
 * @param vectorChildren 
 * @param idText 
 * @return GumboNode* 
 */
GumboNode * findDivWithAttribute(const GumboVector* vectorChildren, const std::string attributeText) {
    GumboNode * divNode = NULL;
    for(unsigned int i = 0; i < vectorChildren->length; ++i) {
        GumboNode * child = static_cast<GumboNode *>(vectorChildren->data[i]);
        if(child->v.element.tag != GUMBO_TAG_DIV && child->v.element.tag != GUMBO_TAG_MAIN && child->v.element.tag != GUMBO_TAG_TABLE) continue;

        // This will essentially root out all of the children nodes that are just undefined
        auto foundCR = (std::string(child->v.text.text).find("\n") != std::string::npos);
        auto foundCRTab = (std::string(child->v.text.text).find("\t") != std::string::npos);
        bool containsTag = false;

        if(foundCR && foundCRTab == false) {
            if(child->v.element.original_tag.data != NULL) {
                std::cout << "Made it in here!\n";
                containsTag = std::string(child->v.element.original_tag.data).find(attributeText) != std::string::npos;
            }
        }

        if((!foundCR && !foundCRTab) || containsTag) {
            const GumboVector * attributeVector = &child->v.element.attributes;
            bool contentSectionFound = false;

            // We are looking for the id of "content"
            for(unsigned int j = 0; j < attributeVector->length; j++) {
                GumboNode * attributeNode = static_cast<GumboNode *>(attributeVector->data[j]);
                // The attributes are going to be in order, and underneath attributeNode->text.text
                if(std::string(attributeNode->v.text.text) == attributeText) {
                    contentSectionFound = true;
                    break;
                }
            }

            if(contentSectionFound) {
                divNode = child;
                break;
            }
        }
    }
    return divNode;
}

/**
 * @brief The point of this function is to take in a String and remove any bad chars
 * 
 * @param inputString 
 * @return std::string 
 */
std::string removeBadChars(const std::string inputString) {
    std::vector<char> acceptableChars = {'A', 'a', 'B', 'b', 'C', 'c', 'D', 'd', 'E', 'e', 'F', 'f', 'G', 'g',
                                     'H', 'h', 'I', 'i', 'J', 'j', 'K', 'k', 'L', 'l', 'M', 'm', 'N', 'n',
                                     'O', 'o', 'P', 'p', 'Q', 'q', 'R', 'r', 'S', 's', 'T', 't', 'U', 'u',
                                     'V', 'v', 'W', 'w', 'X', 'x', 'Y', 'y', 'Z', 'z', '0', '1', '2', '3',
                                     '4', '5', '6', '7', '8', '9', '-'};

    std::string newString = "";
    bool previousCharSpace = false;
    for(unsigned int i = 0; i < inputString.length(); i++) {
        char ascii = inputString[i];
        bool acceptableCharFound = false;
        for(auto &acceptableChar : acceptableChars) {
            if(ascii == acceptableChar) {
                acceptableCharFound = true;
                break;
            }
        }

        if(acceptableCharFound) {
            newString += ascii;
            previousCharSpace = false;
        } else if(!previousCharSpace) {
            // If an acceptable char was not found, we are going to add a space
            newString += " ";
            previousCharSpace = true;
        }
    }

    return newString;
}

void parseHouseHTML(const char * html, std::unique_ptr<House_of_Representatives> & usHOR) {
    signal(SIGSEGV, signalHandler);

    const GumboOutput * output = gumbo_parse(html);
    const GumboNode * node = output->root;

    const GumboVector * rootChildren = &node->v.element.children;
    GumboNode * body = NULL;
    
    // This for loop is going through the child elements of the <HTML><C1></C1><C2></C2></HTML> TAGS
    for(unsigned int i = 0; i < rootChildren->length; ++i) {
        GumboNode * child = static_cast<GumboNode *>(rootChildren->data[i]);
        // We are specifically searching for the body of the HTML which contains all of the important stuff
        if(child->type == GUMBO_NODE_ELEMENT && child->v.element.tag == GUMBO_TAG_BODY) {
            body = child;
            break;
        }
    }

    GumboVector* bodyChildren = &body->v.element.children;

    const std::array<std::string, 8> htmlTags {
        "mw-page-container", "mw-page-container-inner", "mw-content-container", "content",
        "bodyContent", "mw-content-text", "mw-parser-output", "votingmembers"
    };

    for(const std::string & htmlTag : htmlTags) {
        GumboNode * nextNode = findDivWithAttribute(bodyChildren, htmlTag);
        bodyChildren = &nextNode->v.element.children;

        if(htmlTag == "votingmembers") {
            // Parse all of the members out!
            GumboNode * child = static_cast<GumboNode *>(bodyChildren->data[1]);

            for(unsigned int i = 2; i < child->v.element.children.length; i += 2) {
                if(child->v.element.tag != GUMBO_TAG_TBODY) continue;
                GumboNode * repData = static_cast<GumboNode *>(child->v.element.children.data[i]);
                
                GumboNode * district_Tr_Th = static_cast<GumboNode *>(repData->v.element.children.data[1]);
                GumboNode * district_Tr_Th_Span = static_cast<GumboNode *>(district_Tr_Th->v.element.children.data[0]);
                GumboNode * district_Tr_Th_Span_A = static_cast<GumboNode *>(district_Tr_Th_Span->v.element.children.data[0]);
                GumboNode * district_Tr_Th_Span_A_Text = static_cast<GumboNode *>(district_Tr_Th_Span_A->v.element.children.data[0]);
                std::string stateAndDistrict = std::string(district_Tr_Th_Span_A_Text->v.text.text);
                std::string fixedStateAndDistrict = removeBadChars(stateAndDistrict);

                std::string state = fixedStateAndDistrict.substr(0, fixedStateAndDistrict.find_last_of(' '));
                trim(state);

                std::string district = fixedStateAndDistrict.substr(fixedStateAndDistrict.find_last_of(' '));
                trim(district);
                
                std::cout << "The District is " << fixedStateAndDistrict << ".\n";

                if(repData->v.element.children.length == 18) {
                    /** Getting the NAME of the Congressperson **/
                    GumboNode * name_Tr = static_cast<GumboNode *>(repData->v.element.children.data[3]);
                    GumboNode * name_Tr_Td = static_cast<GumboNode *>(name_Tr->v.element.children.data[2]);

                    /**
                     * RULES:
                     * We need to check and see if the length of any of these elements is 0; if they are then we need to go to the next child 
                     */
                    // For some reason, this is a special case.... Florida 20, Georgia 
                    size_t len_Tr_Td = name_Tr_Td->v.element.children.length;
                    int index = 3;
                    while(len_Tr_Td == 0 || len_Tr_Td > ((size_t)1 << (size_t)31)) {
                        name_Tr_Td = static_cast<GumboNode *>(name_Tr->v.element.children.data[index++]);
                        len_Tr_Td = name_Tr_Td->v.element.children.length;
                    }
                    GumboNode * name_Tr_Td_B = static_cast<GumboNode *>(name_Tr_Td->v.element.children.data[0]);
                    GumboNode * name_Tr_Td_B_A = static_cast<GumboNode *>(name_Tr_Td_B->v.element.children.data[0]);
                    std::string nameOfRepresentative(name_Tr_Td_B_A->v.text.text);
                    trim(nameOfRepresentative);

                    /** Getting Political Party **/
                    GumboNode * party_Tr = static_cast<GumboNode *>(repData->v.element.children.data[7]);
                    GumboNode * party_Tr_Td = static_cast<GumboNode *>(party_Tr->v.element.children.data[0]);
                    std::string partyOfRepresentative(std::string(party_Tr_Td->v.text.text).substr(0, std::string(party_Tr_Td->v.text.text).find("\n")));
                    trim(partyOfRepresentative);

                    /** Getting Start Date **/
                    GumboNode * startDate_Tr = static_cast<GumboNode *>(repData->v.element.children.data[13]);
                    GumboNode * startDate_Tr_Td = static_cast<GumboNode *>(startDate_Tr->v.element.children.data[0]);
                    const std::string startingYear(std::string(startDate_Tr_Td->v.text.text).substr(0, std::string(startDate_Tr_Td->v.text.text).find("\n")));
                    const int beginTerm = std::stoi(startingYear.c_str());

                    const int currentYear = getCurrentYear();
                    int endTerm = currentYear;
                    if(endTerm % 2 == 0) {
                        // For example, if the year is 2024 which is an election year, the term of said congressperson ends in January 2025
                        endTerm++;
                    } else {
                        // If the year is say 2023, which is one year before the election year, we need to add two years since their term expires in Jan. 2025
                        endTerm += 2;
                    }

                    /** Getting Age of Congressperson **/
                    GumboNode * birthDate_Tr = static_cast<GumboNode *>(repData->v.element.children.data[17]);
                    GumboNode * birthDate_Tr_Td = static_cast<GumboNode *>(birthDate_Tr->v.element.children.data[1]);
                    const std::string birthday(birthDate_Tr_Td->v.text.text);

                    struct tm tm;
                    strptime(birthday.c_str(), "%B %d, %Y", &tm);

                    // Since the Epoch began on 1/1/1970, anything before then is going to be negative.
                    const int secondsPerDay = 86400;
                    const int secondsPerMonth = 2629743;
                    const int secondsPerYear = 31556926;
                    time_t congressperonEpoch = 0;
                    
                    if(tm.tm_year < 70) {
                        congressperonEpoch = ((70 - tm.tm_year) * -secondsPerYear) + ((tm.tm_mon + 1) * secondsPerMonth) + (tm.tm_mday * secondsPerDay);
                    } else {
                        congressperonEpoch = ((tm.tm_year - 70) * secondsPerYear) + ((tm.tm_mon + 1) * secondsPerMonth) + (tm.tm_mday * secondsPerDay);
                    }

                    // The current system time            
                    time_t currentTimeEpoch;
                    time(&currentTimeEpoch);

                    const int congressPersonAge = (currentTimeEpoch - congressperonEpoch) / secondsPerYear;

                    Politician * tempPol = new Politician(partyOfRepresentative, state, nameOfRepresentative, congressPersonAge, district, beginTerm, endTerm);
                    std::cout << state << " " << district << " | " << partyOfRepresentative << " | " << nameOfRepresentative << ".\n";
                    usHOR->addMember(tempPol);
                } else {
                    // This either means something is wrong, or the seat is VACANT
                    // We want to entere a generic person for the State and District
                    Politician * tempPol = new Politician("VACANT", state, "VACANT", -1, district, -1, -1);
                    usHOR->addMember(tempPol);
                    std::cout << fixedStateAndDistrict << " | VACANT\n";
                }
            }
        }
    }
}


int main() {
    std::unique_ptr<House_of_Representatives> US_HouseOfRepresentatives = std::make_unique<House_of_Representatives>(435);

    // std::cout << "Getting the HTML from the Wikipedia page which has all of the Representatives\n";
    const MemoryStruct htmlChunk = getHTML("https://en.wikipedia.org/wiki/List_of_current_members_of_the_United_States_House_of_Representatives");

    // "<?xml version=\"1.0\"?>" + 
    const std::string htmlString(htmlChunk.memory);

    parseHouseHTML(htmlString.c_str(), US_HouseOfRepresentatives);

    // std::cout << "The number of representatives that have been parsed are: " << US_HouseOfRepresentatives->getNumberOfMembers() << "\n";

    while(true) {
        std::string key;
        
        std::cout << "Enter one the following:\n"
                  << "\t1: Name of a Congressperson to get their information\n"
                  << "\t2: Party Breakdown\n"
                  << "\t3: Quit the program.\n";
        std::getline(std::cin, key);
        
        int option = std::atoi(key.c_str());
        switch(option) {
            case 1:
            {
                std::string name;
                std::cout << "Enter the name of the Congressperson:\n";
                std::getline(std::cin, name);

                auto politician = US_HouseOfRepresentatives->getMember(name);
                if(politician == NULL) {
                    std::cerr << "The name you entered does not seem to exist. Enter another one!\n";
                } else {
                    std::cout << politician;
                }

                break;
            }
            case 2:
                US_HouseOfRepresentatives->getPartyBreakdown();
                break;
            case 3:
                std::cout << "Ending the program!\n";
                exit(EXIT_SUCCESS);
        }
    }
}