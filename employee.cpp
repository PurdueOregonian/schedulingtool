#include <string>
#include "employee.h"

/*
 * Made by William Liang in 2020
 * Copyright William Liang
 */

int Employee::getRemainingAvailability(int begin){
    int available = 0;
    for(int i=begin; i<336; i++){
        if(availability->at(i)==1){
            available++;
        }
    }
    return available;
}

void Employee::setAvailable(int begin, int end){
    for(int i=begin; i<=end; i++){
        availability->at(i) = 1;
    }
}

void Employee::setUnavailable(int begin, int end){
    for(int i=begin; i<=end; i++){
        availability->at(i) = 0;
    }
}

int Employee::getAvailability(int index){
    return availability->at(index);
}

bool Employee::isAvailable(int begin, int end){
    for(int i=begin; i<end; i++){
        if(availability->at(i) == 0){
            return false;
        }
    }
    return true;
}
