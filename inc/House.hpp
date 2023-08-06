#pragma once

#include "Politician.hpp"

#include <string>
#include <map>
#include <iostream>

class House {
    public:
        House(int numberOfMembers);
        virtual ~House() {};

        virtual void setNumberOfMembers(int numberOfMembers);
        virtual int getNumberOfMembers();

        virtual void setCity(std::string city);
        virtual std::string getCity();

        virtual void setJurisdiction(std::string jurisdiction);
        virtual std::string getJurisdiction();

        virtual void addMember(Politician * politician);
        virtual Politician * getMember(std::string name);

        virtual void getPartyBreakdown();

    private:
        int numberOfMembers;

        // Politician * representatives;
        // std::vector<Politician> representativesVector;
        std::map<std::string, Politician*> representativesMap;

        std::string city;
        std::string jurisdiction;
};