#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <vector>
#include "employee.h"

class Schedule{
    std::vector<Employee*> employeesList;
    std::array<int,336>* employeesRequired;

    public:
    QString createSchedule(std::vector<std::array<int,2>>*, int, int);
    void addEmployee(Employee*);
    void addRequirements(std::array<int,336>*);
    std::vector<bool> scheduleToPass;

    Schedule(){
        employeesRequired = new std::array<int,336>();
        for(int i=0; i<employeesList.size(); i++){
            delete employeesList.at(i);
        }
    }

    ~Schedule(){
        delete employeesRequired;
    }
};

bool shiftSorter(std::array<int,2>, std::array<int,2>);

#endif // SCHEDULE_H
