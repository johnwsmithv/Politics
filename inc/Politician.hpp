#pragma once

#include <string>
#include <time.h>

class Politician {
    public:
        Politician(std::string partyName, std::string state, std::string name,
                   int age, std::string district, int termBegin, int termEnd);

        Politician();

        ~Politician();

        void setParty(std::string partyName);
        std::string getParty() const;

        void setState(std::string stateName);
        std::string getState() const;

        void setDistrict(std::string districtNumber);
        std::string getDistirct() const;

        void setName(std::string name);
        std::string getName() const;

        void setAge(int age);
        int getAge() const;

        void setTermBegin(int termBegin);
        int getTermBegin() const;

        void setTermEnd(int termEnd);
        int getTermEnd() const;

        friend std::ostream& operator<<(std::ostream& os, const Politician * politician);

    private:
        std::string partyName;
        std::string stateName;
        std::string name;

        std::string district;
        int age;

        int termBegin;
        int termEnd;

};