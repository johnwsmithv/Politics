#include "House.hpp"

House::House(int numberOfMembers) {
    this->setNumberOfMembers(numberOfMembers);
}

// House::~House(){}

void House::setNumberOfMembers(int numberOfMembers) {
    this->numberOfMembers = numberOfMembers;
    // this->representativesVector.resize(this->numberOfMembers);
}

int House::getNumberOfMembers() {
    return this->numberOfMembers;
}

void House::setCity(std::string cityLocated) {
    this->city = cityLocated;
}

std::string House::getCity() {
    return this->city;
}

void House::setJurisdiction(std::string jurisdiction) {
    this->jurisdiction = jurisdiction;
}

std::string House::getJurisdiction() {
    return this->jurisdiction;
}

void House::getPartyBreakdown() {
    std::cout << "Generic implementation of Party Breakdown.\n";
}

void House::addMember(Politician * politician) {
    representativesMap[politician->getName()] = politician;
}

Politician * House::getMember(std::string memberName) {
    Politician * politician = new Politician();
    return politician;
}