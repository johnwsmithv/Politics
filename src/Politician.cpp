#include "Politician.hpp"

#include <iostream>

Politician::Politician(std::string partyName, std::string state, std::string name,
                       int age, std::string district, int termBegin, int termEnd) 
    : partyName(partyName), stateName(state), name(name), age(age), district(district),
      termBegin(termBegin), termEnd(termEnd)
{}

Politician::Politician() {}

Politician::~Politician() {}

void Politician::setParty(std::string partyName) {
    this->partyName = partyName;
}

std::string Politician::getParty() const {
    return this->partyName;
}

void Politician::setState(std::string stateName) {
    this->stateName = stateName;
}

std::string Politician::getState() const {
    return this->stateName;
}

void Politician::setDistrict(std::string districtNumber) {
    this->district = district;
}

std::string Politician::getDistirct() const {
    return this->district;
}

void Politician::setName(std::string name) {
    this->name = name;
}

std::string Politician::getName() const {
    return this->name;
}

void Politician::setAge(int age) {
    this->age = age;
}

int Politician::getAge() const {
    return this->age;
}

void Politician::setTermBegin(int termBegin) {
    this->termBegin = termBegin;
}

int Politician::getTermBegin() const {
    return this->termBegin;
}

void Politician::setTermEnd(int termEnd) {
    this->termEnd = termEnd;
}

int Politician::getTermEnd() const {
    return this->termEnd;
}

std::ostream& operator<<(std::ostream& os, const Politician * politician) {
    os << "State: " << politician->getState() << "\n"
       << "District: " << politician->getDistirct() << "\n"
       << "Name: " << politician->getName() << "\n"
       << "Age: " << politician->getAge() << "\n"
       << "Party: " << politician->getParty() << "\n"
       << "Term Begin: " << politician->getTermBegin() << "\n"
       << "Term End: " << politician->getTermEnd() << "\n";
}