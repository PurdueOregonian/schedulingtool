#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "schedule.h"
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QDate>
#include <array>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class employeeBox;

class availabilityBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void okClicked();
    void addMustWorkClicked();
    void subtractMustWorkClicked();
    void availabilityClicked();
    void availabilityOkClicked();

private slots:
    void on_createSchedule_clicked();

    void on_addEmployee_clicked();

    void on_setSelected_clicked();

    void on_addOneButton_clicked();

    void on_subtractOneButton_clicked();

    void on_removeEmployees_clicked();

    void on_employeeTable_itemChanged(QTableWidgetItem *item);

    void on_employeeTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_scheduleTable_itemChanged(QTableWidgetItem *item);

    void on_scheduleTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_saveToFileButton_clicked();

    void on_loadButton_clicked();

    void on_calendarViewButton_clicked();

    void on_maximumLengthBox_valueChanged(double arg1);

    void on_minimumLengthBox_valueChanged(double arg1);

    void on_sortEmployeesButton_clicked();

    void on_startDateBox_userDateChanged(const QDate &date);

    //void on_startDateBox_dateChanged(const QDate &date);

private:
    Ui::MainWindow *ui;
    employeeBox* eb;
    availabilityBox* ab;

    //Maybe don't need this. Build the schedule in the function instead.
    Schedule scheduleToBuild;

    double hoursValue;
    int priorityValue;
    int employeesValue;
    double totalEmployeeHours;
    double minimumScheduledHours;
    double maximumScheduledHours;
    bool isManualRequirementEdit;
    bool enteringNewEmployee;
    bool removingEmployees;
    int minimumShiftLength;
    int maximumShiftLength;
    bool errorChangeBackMax;
    bool errorChangeBackMin;
    bool secondaryHoursChange;
    bool noSolution;
    bool changingDateBack;
    QDate currentStartDate;
    std::vector<bool> scheduleToPrint;
    std::vector<std::string> firstNamesToPrint;
    std::vector<std::string> lastNamesToPrint;
    //std::vector<std::array<int,336>*>* availabilities;
};

#endif // MAINWINDOW_H
