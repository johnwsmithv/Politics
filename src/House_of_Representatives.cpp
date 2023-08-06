/**
 * @file House_of_Representatives.cpp
 * @author John W. Smith V (jwsv61099@gmail.com)
 * @brief This is a Class which will represent the U.S. House of Representatives
 * It is inheriting virtual functions from a base House Class
 * @version 0.1
 * @date 2022-08-06
 * 
 * @copyright Copyright © 2022
 * 
 */

#include "House_of_Representatives.hpp"

#include <memory>

House_of_Representatives::House_of_Representatives(int numberOfMembers) 
    : House(numberOfMembers) 
{}

House_of_Representatives::~House_of_Representatives(){
    for(const auto & politician : this->representativesMap) {
        delete politician.second;
    }
}

void House_of_Representatives::setNumberOfMembers(int numberOfMembers) {
    this->numberOfMembers = numberOfMembers;
}

int House_of_Representatives::getNumberOfMembers() const {
    return this->numberOfMembers;
}

void House_of_Representatives::setCity(std::string cityLocated) {
    this->city = cityLocated;
}

std::string House_of_Representatives::getCity() const {
    return this->city;
}

void House_of_Representatives::setJurisdiction(std::string jurisdiction) {
    this->jurisdiction = jurisdiction;
}

std::string House_of_Representatives::getJurisdiction() const {
    return this->jurisdiction;
}

void House_of_Representatives::getPartyBreakdown() const {
    std::map<std::string, int> partyBreakdown;

    for(const auto & representative : this->representativesMap) {
        auto politician = representative.second;
        std::string politicalParty = politician->getParty();

        if(partyBreakdown.find(politicalParty) != partyBreakdown.end()) {
            partyBreakdown[politicalParty]++;
        } else {
            partyBreakdown[politicalParty] = 1;
        }
    }

    std::string majorityPartyName;
    int majorityPartyMembers = 0;
    for(const auto & party : partyBreakdown) {
        std::cout << "The " << party.first << " Party has " << party.second << " members.\n"; 
        if(party.second > majorityPartyMembers) {
            majorityPartyName = party.first;
            majorityPartyMembers = party.second;
        }
    }
    std::cout << "Since the " << majorityPartyName << " Party has " << majorityPartyMembers << " it is the majority party.\n";
}

void House_of_Representatives::addMember(Politician * politician) {
    representativesMap[politician->getName()] = politician;
}

Politician * House_of_Representatives::getMember(std::string memberName) {
    Politician * politician = NULL;
    if(representativesMap.find(memberName) != representativesMap.end()) {
        politician = representativesMap[memberName];    
    } else {
        std::cerr << "This Representative does not exist.\n";
    }
    return politician;
}

// int main() {
//     std::unique_ptr<House_of_Representatives> usHouseOfRepresentatives = std::make_unique<House_of_Representatives>(435);

//     std::cout << "The number of members in the U.S. House of Representatives is " << usHouseOfRepresentatives->getNumberOfMembers() << "\n";
// }