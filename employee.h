#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <QString>
#include <string>
#include <array>
#include "utility.h"

class Employee{
    std::array<int,336>* availability;

    public:
    int priority;
    std::string firstName;
    std::string lastName;
    int minHalfHours;
    int maxHalfHours;
    int getRemainingAvailability(int);
    void setAvailable(int, int);
    void setUnavailable(int, int);
    int getAvailability(int);
    bool isAvailable(int, int);

    Employee(QString firstName, QString lastName, int min, int max, int priority, std::array<int,336>* availabilityToSet){
        minHalfHours = min;
        maxHalfHours = max;
        this->priority = priority;
        this->firstName = firstName.toStdString();
        this->lastName = lastName.toStdString();
        availability = new std::array<int,336>();
        *availability = *availabilityToSet;
    }

    ~Employee(){
        delete availability;
    }
};

#endif // EMPLOYEE_H
