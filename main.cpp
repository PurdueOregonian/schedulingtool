#include "mainwindow.h"

#include "schedule.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    /*
    Schedule someSchedule;
    Employee e = Employee("Vivian", "Rimmore", 80, 80, 1);
    e.setUnavailable(0,47);
    someSchedule.addEmployee(e);
    e = Employee("Joseph", "Sedgewick", 30, 40, 1);
    e.setUnavailable(144, 287);
    someSchedule.addEmployee(e);
    someSchedule.addEmployee(Employee("Adrielle", "Howell", 56, 64, 1));
    someSchedule.addEmployee(Employee("Kevin", "Dodgson", 80, 80, 1));
    someSchedule.addEmployee(Employee("Percival", "Goldsmith", 56, 64, 1));
    someSchedule.addEmployee(Employee("Sylvia", "Cavill", 30, 40, 1));
    someSchedule.addEmployee(Employee("Reuben", "Miller", 66, 76, 1));
    someSchedule.addEmployee(Employee("Catherine", "Fitchton", 56, 68, 1));
    int scheduleRequirements[] = {
        0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,3,3,3,3,4,4,4,4,2,2,2,2,2,2,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,3,3,3,3,4,4,4,4,2,2,2,2,2,2,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,2,2,2,2,2,2,0,0,0,0};
    someSchedule.addRequirements(scheduleRequirements);
    std::vector<bool> finishedSchedule = someSchedule.createSchedule();
    */



    /*
    std::vector<Employee>::iterator employeeIterator = someSchedule.employeesList.begin();
    employeeIterator++;
    */


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
