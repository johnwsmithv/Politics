#pragma once

#include "House.hpp"

#include <map>
#include <iostream>

class House_of_Representatives : public virtual House {
    public:
        House_of_Representatives(int numberOfMembers);
        ~House_of_Representatives();

        void setNumberOfMembers(int numberOfMembers);
        int getNumberOfMembers() const;

        void setCity(std::string city);
        std::string getCity() const;

        void setJurisdiction(std::string jurisdiction);
        std::string getJurisdiction() const;

        void addMember(Politician * politician);
        Politician * getMember(std::string name);

        void getPartyBreakdown() const;

    private:
        int numberOfMembers;

        std::map<std::string, Politician*> representativesMap;

        std::string city;
        std::string jurisdiction;
        
};