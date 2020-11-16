#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "employeeBox.h"
#include "availabilityBox.h"
#include "generationAdjustBox.h"
#include "calendarBox.h"
#include "buttonWithAvailability.h"
#include <math.h>
#include <Qt>
#include <QtGui>
#include <QDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QWidget>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QCheckBox>
#include <algorithm>
#include <set>
#include <fstream>
#include <stdio.h>

#define MAX_SIMULTANEOUS_EMPLOYEES 100
#define MAX_EMPLOYEE_PRIORITY 1000
#define CHECKBOX_COLUMN 0
#define FIRSTNAME_COLUMN 1
#define LASTNAME_COLUMN 2
#define MINIMUM_HOURS_COLUMN 3
#define MAXIMUM_HOURS_COLUMN 4
#define PRIORITY_COLUMN 5
#define SET_AVAILABILITY_COLUMN 6
#define MAX_NAME_LENGTH 100
#define MAX_TITLE_LENGTH 50

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    for(int i=0; i<7; i++){
        for(int j=0; j<48; j++){
            QTableWidgetItem* item = new QTableWidgetItem(0);
            item->setData(Qt::DisplayRole,0);
            ui->scheduleTable->setItem(i,j, item);
            //ui->scheduleBox->setText(QString::number(item->flags()));
        }
    }
    for(int i=0; i<48; i++){
        ui->scheduleTable->setColumnWidth(i,20);
    }
    ui->employeeTable->setColumnWidth(CHECKBOX_COLUMN,10);
    ui->employeeTable->setColumnWidth(MINIMUM_HOURS_COLUMN, 80);
    ui->employeeTable->setColumnWidth(MAXIMUM_HOURS_COLUMN, 80);
    ui->employeeTable->setColumnWidth(PRIORITY_COLUMN, 60);
    ui->employeeTable->setColumnWidth(SET_AVAILABILITY_COLUMN, 120);
    totalEmployeeHours = 0;
    minimumScheduledHours = 0;
    maximumScheduledHours = 0;
    employeesValue = 0;
    minimumShiftLength = 8;
    maximumShiftLength = 17;
    this->setWindowTitle("Scheduling Tool");
    ui->minimumLengthBox->setValue(4);
    ui->maximumLengthBox->setValue(8.5);
    ui->minimumLengthBox->setDecimals(1);
    ui->maximumLengthBox->setDecimals(1);
    ui->minimumLengthBox->setSingleStep(0.5);
    ui->maximumLengthBox->setSingleStep(0.5);
    ui->minimumLengthBox->setMinimum(0.5);
    ui->minimumLengthBox->setMaximum(168);
    ui->maximumLengthBox->setMinimum(0.5);
    ui->maximumLengthBox->setMaximum(168);
    ui->scheduleBox->setReadOnly(true);
    ui->employeeHours->setReadOnly(true);
    ui->minimumLengthBox->setReadOnly(true);
    ui->maximumLengthBox->setReadOnly(true);
    ui->employeeHours->setText(QString::number(totalEmployeeHours));
    ui->titleBox->setMaxLength(MAX_TITLE_LENGTH);
    QDate dateToSet = QDate::currentDate();
    QCalendar calendarForDayToday = QCalendar(QCalendar::System::Gregorian);
    int dayToday = calendarForDayToday.dayOfWeek(dateToSet);
    if(dayToday == Qt::Sunday){
        dateToSet = dateToSet.addDays(7);
    }
    else{
        dateToSet = dateToSet.addDays(7 - dayToday);
    }
    currentStartDate = dateToSet;
    ui->startDateBox->setDate(dateToSet);
    ui->startDateBox->setCalendarPopup(true);
    isManualRequirementEdit = true;
    enteringNewEmployee = false;
    removingEmployees = false;
    errorChangeBackMax = false;
    errorChangeBackMin = false;
    secondaryHoursChange = false;
    noSolution = false;
    changingDateBack = false;
    //availabilities = new std::vector<std::array<int,336>*>();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_createSchedule_clicked()
{
    //ui->scheduleBox->setText("blahblah");
    /*
    bool b;
    QString empName = QInputDialog::getText(this, tr("QInputDialog::getText()"),
        tr("Employee Name"), QLineEdit::Normal, QDir::home().dirName(), &b);
    if(b && !empName.isEmpty()){
        ui->scheduleBox->setText(empName);
    }
    double minHours = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
        tr("Minimum Hours"), 0, 0, 168, 1, &b, Qt::WindowFlags(), 0.5);
    double maxHours = QInputDialog::getDouble(this, tr("QInputDialog::getDouble()"),
        tr("Maximum Hours"), 0, 0, 168, 1, &b, Qt::WindowFlags(), 0.5);
    */
    //delete employeeBox when done
    //eb = new employeeBox(this);
    /*
    QInputDialog* qd = new QInputDialog;
    qd->setWindowTitle("A QDialog");
    qd->show();
    */
    bool error = false;
    QString errorMessage;
    if(minimumScheduledHours > totalEmployeeHours){
        error = true;
        errorMessage += "Total employee hours is less than the minimum to schedule\n";
    }
    if(maximumScheduledHours < totalEmployeeHours){
        error = true;
        errorMessage += "Total employee hours is greater than the maximum to schedule\n";
    }
    if(error){
        QMessageBox errorBox;
        errorBox.setWindowTitle(QString("Requirements Error"));
        errorBox.setText(errorMessage);
        errorBox.exec();
        return;
    }
    Schedule* scheduleToBuild = new Schedule();
    std::array<int,336>* requirements = new std::array<int,336>();
    std::array<int,336>* requirementsForTesting = new std::array<int,336>();
    for(int i=0; i<336; i++){
        requirements->at(i) = ui->scheduleTable->item(i / 48,i % 48)->data(Qt::EditRole).toInt();
        requirementsForTesting->at(i) = ui->scheduleTable->item(i / 48, i % 48)->data(Qt::EditRole).toInt();
    }

    std::vector<std::array<int,2>>* shiftsToPass = new std::vector<std::array<int,2>>();

    bool generate = true;
    bool removingTheShift = false;

    //Assign must work before creating shifts

    for(int i=0; i<ui->employeeTable->rowCount(); i++){
        std::array<int,336> availability = *(((buttonWithAvailability*)(ui->employeeTable->cellWidget(i,SET_AVAILABILITY_COLUMN)))->availability);
        for(int j=0; j<336; j++){
            if(availability.at(j) == 2){
                requirementsForTesting->at(j)--;
            }
        }
    }

    int problemTime = 0;
    for(int i=0; i<336 && generate; i++){
        if(requirementsForTesting->at(i) < 0){
            generate = false;
            problemTime = i;
        }
    }
    if(!generate){
        QMessageBox tooMany;
        tooMany.setWindowTitle(QString("Requirements Error"));
        std::string tooManyMessage = std::string("Too many employees must work on ");
        tooManyMessage.append(getDayTime(problemTime, true));
        tooManyMessage.append("\n");
        tooManyMessage.append("Number of employees required: ");
        tooManyMessage.append(std::to_string(requirements->at(problemTime)));
        tooManyMessage.append("\n");
        tooManyMessage.append("Employees who must work at that time\n");
        for(int i=0; i<ui->employeeTable->rowCount(); i++){
            if((*(((buttonWithAvailability*)(ui->employeeTable->cellWidget(i,SET_AVAILABILITY_COLUMN)))->availability)).at(problemTime)==2){
                tooManyMessage.append(ui->employeeTable->item(i,LASTNAME_COLUMN)->data(Qt::EditRole).toString().toStdString());
                tooManyMessage.append(", ");
                tooManyMessage.append(ui->employeeTable->item(i,FIRSTNAME_COLUMN)->data(Qt::EditRole).toString().toStdString());
                tooManyMessage.append("\n");
            }
        }
        tooMany.setText(tooManyMessage.c_str());
        tooMany.exec();
        delete requirements;
        delete requirementsForTesting;
        delete shiftsToPass;
        return;
    }

    QString shifts;
    for(int i=0; i<336; i++){
        while(requirementsForTesting->at(i) > 0){
            //bool assign = true;
            int halfHoursToAssign = 1;
            while((i+halfHoursToAssign) < 336 && halfHoursToAssign < maximumShiftLength && requirementsForTesting->at(i+halfHoursToAssign) >= requirementsForTesting->at(i+halfHoursToAssign-1)){
                halfHoursToAssign++;
            }
            if(halfHoursToAssign < minimumShiftLength){
                //problem creating shifts
                generationAdjustBox shiftProblemBox;
                shiftProblemBox.editShiftsBox(shifts);
                std::string text("Problem at:\n");
                text.append(getDayTime(i,true));
                text.append("-");
                if(i/48 == (i+halfHoursToAssign)/48){
                    text.append(getDayTime(i+halfHoursToAssign,false).c_str());
                }
                else{
                    text.append(getDayTime(i+halfHoursToAssign,true).c_str());
                }
                shiftProblemBox.editSecondLabel(text.c_str());
                bool repeat = true;
                while(repeat){
                    repeat = false;
                    int confirmation = shiftProblemBox.exec();
                    if(confirmation == EXTEND_VALUE){
                        //extend shift
                        bool ok;
                        bool repeat2 = true;
                        while(repeat2){
                            repeat2 = false;
                            double maxShiftLength = std::min(0.5*maximumShiftLength, 0.5*(336-i));
                            double newShiftLength = QInputDialog::getDouble(this, tr("Extend shift"), tr("How long should the shift be (multiple of 0.5)?"), 0.5*halfHoursToAssign, 0.5*halfHoursToAssign, maxShiftLength, 1, &ok, Qt::WindowFlags(), 0.5);
                            if(ok){
                                if(fmod(newShiftLength,0.5) == 0){
                                    isManualRequirementEdit = false;
                                    for(int j=i+halfHoursToAssign; j<i+newShiftLength*2; j++){
                                        requirementsForTesting->at(j)++;
                                        requirements->at(j)++;
                                        int currentNum = ui->scheduleTable->item(j/48, j%48)->data(Qt::EditRole).toInt();
                                        ui->scheduleTable->item(j/48, j%48)->setData(Qt::EditRole, currentNum + 1);
                                        totalEmployeeHours += 0.5;
                                    }
                                    ui->employeeHours->setText(QString::number(totalEmployeeHours));
                                    halfHoursToAssign = newShiftLength*2;
                                    isManualRequirementEdit = true;
                                }
                                else{
                                    repeat2 = true;
                                    QMessageBox invalidValue;
                                    invalidValue.setWindowTitle(QString("Setting Length Error"));
                                    invalidValue.setText("Must be a multiple of 0.5");
                                    invalidValue.exec();
                                }
                            }
                            else{
                                repeat = true;
                            }
                        }
                    }
                    if(confirmation == REMOVE_VALUE){
                        //remove shift
                        removingTheShift = true;
                        for(int j=i; j<i+halfHoursToAssign; j++){
                            requirements->at(j)--;
                            //requirementsForTesting->at(j)--;
                            int currentNum = ui->scheduleTable->item(j/48, j%48)->data(Qt::EditRole).toInt();
                            if(currentNum > 0){
                                isManualRequirementEdit = false;
                                ui->scheduleTable->item(j/48, j%48)->setData(Qt::EditRole,currentNum - 1);
                                totalEmployeeHours -= 0.5;
                                isManualRequirementEdit = true;
                            }
                        }
                        ui->employeeHours->setText(QString::number(totalEmployeeHours));
                        //assign = false;
                    }
                    if(confirmation == LEAVE_VALUE){
                        //no need to do anything
                    }
                    if(confirmation == DO_NOTHING_VALUE || confirmation == QDialog::Rejected){
                        //stop generation
                        delete requirements;
                        delete requirementsForTesting;
                        delete shiftsToPass;
                        return;
                        //generate = false;
                    }
                }
            }
            //if(assign){
                if(halfHoursToAssign == maximumShiftLength && i+maximumShiftLength < 336){
                    int nextMaxLength = 1;
                        if(requirementsForTesting->at(i+maximumShiftLength) > 0){
                        while((i+maximumShiftLength+nextMaxLength) < 336 && nextMaxLength < minimumShiftLength && requirementsForTesting->at(i+maximumShiftLength+nextMaxLength) >= requirementsForTesting->at(i+maximumShiftLength+nextMaxLength-1)){
                            nextMaxLength++;
                        }
                        if(nextMaxLength < minimumShiftLength){
                            halfHoursToAssign -= (minimumShiftLength - nextMaxLength);
                        }
                    }
                }
                for(int j=i; j<i+halfHoursToAssign; j++){
                    requirementsForTesting->at(j)--;
                }
                if(!removingTheShift){
                    if((i+halfHoursToAssign)/48 == i/48){
                        shifts.append(getDayTime(i,true).c_str());
                        shifts.append("-");
                        shifts.append(getDayTime(i+halfHoursToAssign,false).c_str());
                    }
                    else{
                        shifts.append(getDayTime(i,true).c_str());
                        shifts.append("-");
                        shifts.append(getDayTime(i+halfHoursToAssign,true).c_str());
                    }
                    shifts.append("\n");
                    std::array<int,2> shift;
                    shift.at(0) = i;
                    shift.at(1) = i+halfHoursToAssign;
                    shiftsToPass->push_back(shift);
                }
                removingTheShift = false;
            //}
        }
    }




    if(generate){
        firstNamesToPrint.resize(0);
        lastNamesToPrint.resize(0);
        scheduleToBuild->addRequirements(requirements);
        for(int i=0; i<ui->employeeTable->rowCount(); i++){
            scheduleToBuild->addEmployee(new Employee(
                                            ui->employeeTable->item(i,FIRSTNAME_COLUMN)->data(Qt::EditRole).toString(),
                                            ui->employeeTable->item(i,LASTNAME_COLUMN)->data(Qt::EditRole).toString(),
                                            2*ui->employeeTable->item(i,MINIMUM_HOURS_COLUMN)->data(Qt::EditRole).toDouble(),
                                            2*ui->employeeTable->item(i,MAXIMUM_HOURS_COLUMN)->data(Qt::EditRole).toDouble(),
                                            ui->employeeTable->item(i,PRIORITY_COLUMN)->data(Qt::EditRole).toInt(),
                                            ((buttonWithAvailability*)(ui->employeeTable->cellWidget(i,SET_AVAILABILITY_COLUMN)))->availability));
            firstNamesToPrint.push_back(ui->employeeTable->item(i,FIRSTNAME_COLUMN)->data(Qt::EditRole).toString().toStdString());
            lastNamesToPrint.push_back(ui->employeeTable->item(i,LASTNAME_COLUMN)->data(Qt::EditRole).toString().toStdString());
        }
        //Pass the generated shifts over to the function.
        QString scheduleToDisplay = scheduleToBuild->createSchedule(shiftsToPass, minimumShiftLength, maximumShiftLength);
        QString noSolutionQString = QString("No solution found.");

        if(scheduleToDisplay.compare(noSolutionQString,Qt::CaseSensitive)==0){
            noSolution = true;
        }
        else{
            noSolution = false;
        }

        scheduleToPrint = scheduleToBuild->scheduleToPass;

        //temp
        ui->scheduleBox->setText(scheduleToDisplay);
        delete scheduleToBuild;
    }
    delete requirements;
    delete requirementsForTesting;
    delete shiftsToPass;
}

void MainWindow::okClicked(){
    bool error = false;
    QString errorMessage;
    if(eb->getMinHours() > eb->getMaxHours()){
        error = true;
        errorMessage += "Min hours cannot be greater than max hours\n";
    }
    if(fmod(eb->getMinHours(),0.5) != 0){
        error = true;
        errorMessage += "Min hours must be a multiple of 0.5\n";
    }
    if(fmod(eb->getMaxHours(),0.5) != 0){
        error = true;
        errorMessage += "Max hours must be a multiple of 0.5\n";
    }
    if(error){
        QMessageBox errorBox;
        errorBox.setWindowTitle(QString("Error Adding Employee"));
        errorBox.setText(errorMessage);
        errorBox.exec();
        return;
    }
    eb->close();
    //add to table
    enteringNewEmployee = true;
    int rowNumber = ui->employeeTable->rowCount();
    ui->employeeTable->insertRow(rowNumber);

    //watch out, potential memory leak
    QTableWidgetItem* item = new QTableWidgetItem();
    //item->setCheckState(Qt::Unchecked);
    //ui->employeeTable->setItem(rowNumber, CHECKBOX_COLUMN, item);
    ui->employeeTable->setItem(rowNumber,CHECKBOX_COLUMN, item);
    ui->employeeTable->setCellWidget(rowNumber, CHECKBOX_COLUMN, new QCheckBox());
    ui->employeeTable->setItem(rowNumber,FIRSTNAME_COLUMN, new QTableWidgetItem(eb->getFirstName()));
    ui->employeeTable->setItem(rowNumber,LASTNAME_COLUMN, new QTableWidgetItem(eb->getLastName()));

    item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, eb->getMinHours());
    ui->employeeTable->setItem(rowNumber,MINIMUM_HOURS_COLUMN, item);
    item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, eb->getMaxHours());
    ui->employeeTable->setItem(rowNumber,MAXIMUM_HOURS_COLUMN, item);
    item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, eb->getPriority());
    ui->employeeTable->setItem(rowNumber,PRIORITY_COLUMN, item);
    item = new QTableWidgetItem();
    ui->employeeTable->setItem(rowNumber, SET_AVAILABILITY_COLUMN, item);
    buttonWithAvailability* availabilityButton = new buttonWithAvailability("Set Availability", ui->employeeTable);
    QObject::connect(availabilityButton, &QPushButton::clicked, this, &MainWindow::availabilityClicked);
    ui->employeeTable->setCellWidget(rowNumber, SET_AVAILABILITY_COLUMN, availabilityButton);

    //Add the employees later
    /*
    Employee e = Employee(eb->getFirstName().toUtf8(), eb->getLastName().toUtf8(), eb->getMinHours(), eb->getMaxHours(), eb->getPriority());
    scheduleToBuild.addEmployee(e);
    */

    /*
    std::array<int,336>* availabilityArray = new std::array<int,336>();
    availabilityArray->fill(1);
    availabilities->push_back(availabilityArray);
    */

    delete eb;
    enteringNewEmployee = false;
}

void MainWindow::addMustWorkClicked(){
    std::array<int,336>* availability = ab->getAvailability();
    isManualRequirementEdit = false;
    for(int i=0; i<336; i++){
        if(availability->at(i) == 2){
            QTableWidgetItem* toEdit = ui->scheduleTable->item(i/48, i%48);
            int value = toEdit->data(Qt::EditRole).toInt();
            if(value < 100){
                toEdit->setData(Qt::EditRole, value + 1);
                totalEmployeeHours += 0.5;
            }
        }
    }
    isManualRequirementEdit = true;
    ui->employeeHours->setText(QString::number(totalEmployeeHours));
}

void MainWindow::subtractMustWorkClicked(){
    std::array<int,336>* availability = ab->getAvailability();
    isManualRequirementEdit = false;
    for(int i=0; i<336; i++){
        if(availability->at(i) == 2){
            QTableWidgetItem* toEdit = ui->scheduleTable->item(i/48, i%48);
            int value = toEdit->data(Qt::EditRole).toInt();
            if(value > 0){
                toEdit->setData(Qt::EditRole, value - 1);
                totalEmployeeHours -= 0.5;
            }
        }
    }
    isManualRequirementEdit = true;
    ui->employeeHours->setText(QString::number(totalEmployeeHours));
}

void MainWindow::availabilityClicked(){
    //ui->scheduleBox->setText(QString::number(ui->employeeTable->currentRow()));
    ab = new availabilityBox(this);
    ab->fillTable(((buttonWithAvailability*)(ui->employeeTable->cellWidget(ui->employeeTable->currentRow(),SET_AVAILABILITY_COLUMN)))->availability);
}

void MainWindow::availabilityOkClicked(){
    *((buttonWithAvailability*)(ui->employeeTable->cellWidget(ui->employeeTable->currentRow(),SET_AVAILABILITY_COLUMN)))->availability = *(ab->getAvailability());
    //ui->scheduleBox->setText("Hooray!");
    ab->close();
}

void MainWindow::on_addEmployee_clicked()
{
    eb = new employeeBox(this);
}

void MainWindow::on_setSelected_clicked()
{
    isManualRequirementEdit = false;
    bool b;
    int numToSet = QInputDialog::getInt(this, tr("Set Selected"),
        tr("Number to set"), 0, 0, MAX_SIMULTANEOUS_EMPLOYEES, 1, &b, Qt::WindowFlags());
    if(b){
        QList<QTableWidgetItem*> selectedCells = ui->scheduleTable->selectedItems();
        for(int i=0; i<selectedCells.size(); i++){
            int currentNum = selectedCells.at(i)->data(Qt::EditRole).toInt();
            totalEmployeeHours += (numToSet - currentNum) * 0.5;
            selectedCells.at(i)->setData(Qt::EditRole,numToSet);
        }
    }
    ui->employeeHours->setText(QString::number(totalEmployeeHours));
    isManualRequirementEdit = true;
}

void MainWindow::on_addOneButton_clicked()
{
    isManualRequirementEdit = false;
    QList<QTableWidgetItem*> selectedCells = ui->scheduleTable->selectedItems();
    for(int i=0; i<selectedCells.size(); i++){
        int currentNum = selectedCells.at(i)->data(Qt::EditRole).toInt();
        if(currentNum < MAX_SIMULTANEOUS_EMPLOYEES){
            selectedCells.at(i)->setData(Qt::EditRole,currentNum + 1);
            totalEmployeeHours += 0.5;
        }
    }
    ui->employeeHours->setText(QString::number(totalEmployeeHours));
    isManualRequirementEdit = true;
}

void MainWindow::on_subtractOneButton_clicked()
{
    isManualRequirementEdit = false;
    QList<QTableWidgetItem*> selectedCells = ui->scheduleTable->selectedItems();
    for(int i=0; i<selectedCells.size(); i++){
        int currentNum = selectedCells.at(i)->data(Qt::EditRole).toInt();
        if(currentNum > 0){
            selectedCells.at(i)->setData(Qt::EditRole,currentNum - 1);
            totalEmployeeHours -= 0.5;
        }
    }
    ui->employeeHours->setText(QString::number(totalEmployeeHours));
    isManualRequirementEdit = true;
}

void MainWindow::on_removeEmployees_clicked()
{
    removingEmployees = true;
    QMessageBox confirmBox;
    confirmBox.setWindowTitle(QString("Removing Employees"));
    confirmBox.setText("Are you sure to remove the employees?");
    confirmBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    confirmBox.setDefaultButton(QMessageBox::Cancel);
    int confirmation = confirmBox.exec();
    if(confirmation == QMessageBox::Ok){
        std::list<int> rowsToRemove;
        for(int i=0; i<ui->employeeTable->rowCount(); i++){
            if(((QCheckBox*)(ui->employeeTable->cellWidget(i,CHECKBOX_COLUMN)))->checkState() == Qt::Checked){
                rowsToRemove.push_back(i);
            }
        }
        std::list<int>::reverse_iterator rowIterator;
        /*
        ui->employeeTable->removeRow(1);
        ui->employeeTable->removeRow(0);
        */

        for(rowIterator = rowsToRemove.rbegin(); rowIterator != rowsToRemove.rend(); rowIterator++){
            //int a = *rowIterator;
            //int b = ui->employeeTable->rowCount();
            double minHoursRemoved = ui->employeeTable->item(*rowIterator, MINIMUM_HOURS_COLUMN)->data(Qt::EditRole).toDouble();
            double maxHoursRemoved = ui->employeeTable->item(*rowIterator, MAXIMUM_HOURS_COLUMN)->data(Qt::EditRole).toDouble();
            minimumScheduledHours -= minHoursRemoved;
            maximumScheduledHours -= maxHoursRemoved;
            /*
            delete ui->employeeTable->cellWidget(*rowIterator, CHECKBOX_COLUMN);
            delete ui->employeeTable->cellWidget(*rowIterator, SET_AVAILABILITY_COLUMN);
            for(int i=0; i<6; i++){
                delete ui->employeeTable->item(*rowIterator, i);
            }
            */
            ui->employeeTable->removeRow(*rowIterator);
            /*
            std::array<int,336>* toDelete = availabilities->at(*rowIterator);
            availabilities->erase(availabilities->begin()+*rowIterator);
            delete(toDelete);
            */
        }

    }
    ui->minHours->setText(QString::number(minimumScheduledHours));
    ui->maxHours->setText(QString::number(maximumScheduledHours));
    removingEmployees = false;
}

void MainWindow::on_employeeTable_itemChanged(QTableWidgetItem *item)
{
    if(!secondaryHoursChange){
        int row = item->row();
        int column = item->column();
        if(enteringNewEmployee){
            if(column == MINIMUM_HOURS_COLUMN){
                minimumScheduledHours += item->data(Qt::EditRole).toDouble();
                ui->minHours->setText(QString::number(minimumScheduledHours));
            }
            if(column == MAXIMUM_HOURS_COLUMN){
                maximumScheduledHours += item->data(Qt::EditRole).toDouble();
                ui->maxHours->setText(QString::number(maximumScheduledHours));
            }
            return;
        }
        /*
        if(ui->employeeTable->rowCount()-1 < row){
            return;
        }
        */
        /*
        bool error = false;
        */
        QString errorMessage;
        if(column == FIRSTNAME_COLUMN || column == LASTNAME_COLUMN){
            QString valueEntered = item->data(Qt::EditRole).toString();
            if(valueEntered.length() > MAX_NAME_LENGTH){
                valueEntered.truncate(MAX_NAME_LENGTH);
                item->setData(Qt::EditRole,valueEntered);
            }
        }
        if(column == MINIMUM_HOURS_COLUMN){
            double valueEntered = item->data(Qt::EditRole).toDouble();
            QTableWidgetItem* maxHoursItem = ui->employeeTable->item(row, MAXIMUM_HOURS_COLUMN);
            if(maxHoursItem){
                double maximumHoursValue = maxHoursItem->data(Qt::EditRole).toDouble();
                if(fmod(valueEntered,0.5) != 0){
                    item->setData(Qt::EditRole, valueEntered - fmod(valueEntered, 0.5));
                    return;
                    /*
                    error = true;
                    errorMessage += "Min hours must be a multiple of 0.5\n";
                    */
                }
                if(valueEntered < 0){
                    item->setData(Qt::EditRole, 0);
                    return;
                    /*
                    error = true;
                    errorMessage += "Min hours must be between 0 and 168\n";
                    */
                }
                if(valueEntered > 168){
                    item->setData(Qt::EditRole, 168);
                    return;
                }
                /*
                if(!error){
                */
                    if(valueEntered > maximumHoursValue){
                        secondaryHoursChange = true;
                        maximumScheduledHours += valueEntered - maximumHoursValue;
                        maxHoursItem->setData(Qt::EditRole, valueEntered);
                        ui->maxHours->setText(QString::number(maximumScheduledHours));
                        secondaryHoursChange = false;
                    }
                    /*
                }
                */
            }
            /*
            if(!error){
            */
                minimumScheduledHours += valueEntered - hoursValue;
                hoursValue = valueEntered;
                ui->minHours->setText(QString::number(minimumScheduledHours));
                /*
            }
            */
        }
        if(column == MAXIMUM_HOURS_COLUMN){
            double minimumHoursValue = ui->employeeTable->item(row, MINIMUM_HOURS_COLUMN)->data(Qt::EditRole).toDouble();
            double valueEntered = item->data(Qt::EditRole).toDouble();
            if(fmod(valueEntered,0.5) != 0){
                item->setData(Qt::EditRole, valueEntered - fmod(valueEntered, 0.5));
                return;
                /*
                error = true;
                errorMessage += "Max hours must be a multiple of 0.5\n";
                */
            }
            if(valueEntered < 0){
                item->setData(Qt::EditRole, 0);
                return;
                /*
                error = true;
                errorMessage += "Max hours must be between 0 and 168\n";
                */
            }
            if(valueEntered > 168){
                item->setData(Qt::EditRole, 168);
                return;
            }
            /*
            if(!error){
            */
                if(valueEntered < minimumHoursValue){
                    secondaryHoursChange = true;
                    minimumScheduledHours += valueEntered - minimumHoursValue;
                    ui->employeeTable->item(row, MINIMUM_HOURS_COLUMN)->setData(Qt::EditRole, valueEntered);
                    ui->minHours->setText(QString::number(minimumScheduledHours));
                    secondaryHoursChange = false;
                }
                maximumScheduledHours += valueEntered - hoursValue;
                hoursValue = valueEntered;
                ui->maxHours->setText(QString::number(maximumScheduledHours));
                /*
            }
            */
        }
        if(column == PRIORITY_COLUMN){
            double valueEntered = item->data(Qt::EditRole).toDouble();
            if(valueEntered < 1){
                item->setData(Qt::EditRole,1);
                /*
                error = true;
                errorMessage += "Priority must be between 1 and " + QString::number(MAX_EMPLOYEE_PRIORITY) + "\n";
                */
            }
            if(valueEntered > MAX_EMPLOYEE_PRIORITY){
                item->setData(Qt::EditRole, MAX_EMPLOYEE_PRIORITY);
            }
        }
        /*
        if(error){
            QMessageBox errorBox;
            errorBox.setText(errorMessage);
            errorBox.exec();
            if(column == MINIMUM_HOURS_COLUMN || column == MAXIMUM_HOURS_COLUMN){
                item->setData(Qt::EditRole, hoursValue);
            }
            else{
                item->setData(Qt::EditRole, priorityValue);
            }
        }
        else{
        */
            if(column == MINIMUM_HOURS_COLUMN || column == MAXIMUM_HOURS_COLUMN){
                hoursValue = item->data(Qt::EditRole).toDouble();
            }
            else{
                priorityValue = item->data(Qt::EditRole).toInt();
            }
            /*
        }
        */
    }
}

void MainWindow::on_employeeTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if(!removingEmployees){
        int columnNumber = current->column();
        if(columnNumber == MINIMUM_HOURS_COLUMN){
            hoursValue = current->data(Qt::EditRole).toDouble();
        }
        if(columnNumber == MAXIMUM_HOURS_COLUMN){
            hoursValue = current->data(Qt::EditRole).toDouble();
        }
        if(columnNumber == PRIORITY_COLUMN){
            priorityValue = current->data(Qt::EditRole).toInt();
        }
    }
}

void MainWindow::on_scheduleTable_itemChanged(QTableWidgetItem *item)
{
    if(isManualRequirementEdit){
        int valueToAssign = item->data(Qt::EditRole).toInt();
        if(valueToAssign < 0){
            valueToAssign = 0;
        }
        if(valueToAssign > MAX_SIMULTANEOUS_EMPLOYEES){
            valueToAssign = MAX_SIMULTANEOUS_EMPLOYEES;
        }
        item->setData(Qt::EditRole, valueToAssign);
        totalEmployeeHours += (valueToAssign - employeesValue) * 0.5;
        ui->employeeHours->setText(QString::number(totalEmployeeHours));
        employeesValue = valueToAssign;
    }
}

void MainWindow::on_scheduleTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    employeesValue = current->data(Qt::EditRole).toInt();
}

void MainWindow::on_saveToFileButton_clicked()
{
    //bool b;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save To File"),"",tr("Text files (*.txt)"));
    //QString fileName = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("File Name"), QLineEdit::Normal, QDir::home().dirName(), &b);
    //if(b){
        std::string part = fileName.toUtf8().data();
        //part.append(".txt");
        FILE * toSave = fopen(part.c_str(), "w");
        if(toSave != NULL){
            fprintf(toSave, "%s\n", ui->titleBox->text().toUtf8().data());
            for(int i=0; i<336; i++){
                fprintf(toSave, "%d ",ui->scheduleTable->item(i/48, i%48)->data(Qt::EditRole).toInt());
            }
            fprintf(toSave, "\n");
            for(int i=0; i<ui->employeeTable->rowCount(); i++){
                fprintf(toSave, "%s ", ui->employeeTable->item(i, FIRSTNAME_COLUMN)->data(Qt::EditRole).toString().toUtf8().data());
                fprintf(toSave, "%s ", ui->employeeTable->item(i, LASTNAME_COLUMN)->data(Qt::EditRole).toString().toUtf8().data());
                fprintf(toSave, "%.1f ", ui->employeeTable->item(i, MINIMUM_HOURS_COLUMN)->data(Qt::EditRole).toDouble());
                fprintf(toSave, "%.1f ", ui->employeeTable->item(i, MAXIMUM_HOURS_COLUMN)->data(Qt::EditRole).toDouble());
                fprintf(toSave, "%d\n", ui->employeeTable->item(i, PRIORITY_COLUMN)->data(Qt::EditRole).toInt());
                std::array<int,336>* availability = ((buttonWithAvailability*)(ui->employeeTable->cellWidget(i,SET_AVAILABILITY_COLUMN)))->availability;
                for(int j=0; j<336; j++){
                    fprintf(toSave,"%d ", availability->at(j));
                }
                fprintf(toSave, "\n");
            }
            fclose(toSave);
        }
    //}
}

void MainWindow::on_loadButton_clicked()
{
    //bool b;
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load File"),"",tr("Text files (*.txt)"));
    //if(b){
        std::string part = fileName.toUtf8().data();
        //part.append(".txt");
        FILE * toLoad = fopen(part.c_str(), "r");
        if(toLoad != NULL){
            /*
            while(availabilities->size() > 0){
                std::array<int,336>* toErase = availabilities->at(0);
                availabilities->erase(availabilities->begin());
                delete(toErase);
            }
            */
            char title[MAX_TITLE_LENGTH+2];
            fgets(title, MAX_TITLE_LENGTH+2, toLoad);
            title[strcspn(title, "\n")] = 0;
            QString titleQString = QString(title);
            ui->titleBox->setText(titleQString);
            removingEmployees = true;
            /*
            for(int i=0; i<ui->employeeTable->rowCount(); i++){
                delete ui->employeeTable->cellWidget(i,CHECKBOX_COLUMN);
                delete ui->employeeTable->cellWidget(i,SET_AVAILABILITY_COLUMN);
                for(int j=0; j<6; j++){
                    delete ui->employeeTable->item(i,j);
                }
            }
            */
            ui->employeeTable->setRowCount(0);
            minimumScheduledHours = 0;
            maximumScheduledHours = 0;
            removingEmployees = false;
            int numEmployees;
            for(int i=0; i<336; i++){
                fscanf(toLoad, "%d", &numEmployees);
                ui->scheduleTable->item(i/48,i%48)->setData(Qt::EditRole, numEmployees);
            }
            totalEmployeeHours = 0;
            for(int i=0; i<336; i++){
                totalEmployeeHours += ui->scheduleTable->item(i/48,i%48)->data(Qt::EditRole).toDouble()/2;
            }
            ui->employeeHours->setText(QString::number(totalEmployeeHours));
            char name[MAX_NAME_LENGTH];
            char name2[MAX_NAME_LENGTH];
            double minHours;
            double maxHours;
            int priority;
            int available;
            while(fscanf(toLoad, "%s", name) != EOF){
                fscanf(toLoad, "%s", name2);
                fscanf(toLoad, "%lf", &minHours);
                fscanf(toLoad, "%lf", &maxHours);
                fscanf(toLoad, "%d", &priority);
                std::array<int,336>* availabilityFromFile = new std::array<int,336>();
                for(int i=0; i<336; i++){
                    fscanf(toLoad, "%d", &available);
                    availabilityFromFile->at(i) = available;
                }
                //availabilities->push_back(availability);

                enteringNewEmployee = true;
                int rowNumber = ui->employeeTable->rowCount();
                ui->employeeTable->insertRow(rowNumber);

                //watch out, potential memory leak
                QTableWidgetItem* item = new QTableWidgetItem();
                //item->setCheckState(Qt::Unchecked);
                //ui->employeeTable->setItem(rowNumber, CHECKBOX_COLUMN, item);
                ui->employeeTable->setItem(rowNumber,CHECKBOX_COLUMN, item);
                ui->employeeTable->setCellWidget(rowNumber, CHECKBOX_COLUMN, new QCheckBox());
                ui->employeeTable->setItem(rowNumber,FIRSTNAME_COLUMN, new QTableWidgetItem(name));
                ui->employeeTable->setItem(rowNumber,LASTNAME_COLUMN, new QTableWidgetItem(name2));

                item = new QTableWidgetItem();
                item->setData(Qt::DisplayRole, minHours);
                ui->employeeTable->setItem(rowNumber,MINIMUM_HOURS_COLUMN, item);
                item = new QTableWidgetItem();
                item->setData(Qt::DisplayRole, maxHours);
                ui->employeeTable->setItem(rowNumber,MAXIMUM_HOURS_COLUMN, item);
                item = new QTableWidgetItem();
                item->setData(Qt::DisplayRole, priority);
                ui->employeeTable->setItem(rowNumber,PRIORITY_COLUMN, item);
                item = new QTableWidgetItem();
                ui->employeeTable->setItem(rowNumber, SET_AVAILABILITY_COLUMN, item);
                buttonWithAvailability* availabilityButton = new buttonWithAvailability("Set Availability", ui->employeeTable);
                *availabilityButton->availability = *availabilityFromFile;
                QObject::connect(availabilityButton, &QPushButton::clicked, this, &MainWindow::availabilityClicked);
                ui->employeeTable->setCellWidget(rowNumber, SET_AVAILABILITY_COLUMN, availabilityButton);

                //Add the employees later
                /*
                Employee e = Employee(eb->getFirstName().toUtf8(), eb->getLastName().toUtf8(), eb->getMinHours(), eb->getMaxHours(), eb->getPriority());
                scheduleToBuild.addEmployee(e);
                */

                enteringNewEmployee = false;
            }
            fclose(toLoad);
        }
    //}
}

void MainWindow::on_calendarViewButton_clicked()
{
    if(noSolution){
        QMessageBox errorBox;
        errorBox.setWindowTitle(QString("No solution"));
        QString errorMessage = QString("No calendar to display");
        errorBox.setText(errorMessage);
        errorBox.exec();
    }
    else{
        calendarBox cb;
        cb.createCalendar(firstNamesToPrint, lastNamesToPrint, scheduleToPrint, currentStartDate, ui->titleBox->text());
        cb.resize(1400,830);
        cb.exec();
    }
}

void MainWindow::on_maximumLengthBox_valueChanged(double arg1)
{
    if(!errorChangeBackMax){
        bool error = false;
        QString errorMessage;
        if(arg1 < 0){
            error = true;
            errorMessage += "Maximum length cannot be negative";
        }
        if(arg1 > 336){
            error = true;
            errorMessage += "Maximum length cannot be greater than 168 hours";
        }
        if(fmod(arg1, 0.5) != 0){
            error = true;
            errorMessage += "Maximum length must be a multiple of 0.5";
        }
        if(error){
            QMessageBox errorBox;
            errorBox.setText(errorMessage);
            errorBox.exec();
            errorChangeBackMax = true;
            ui->maximumLengthBox->setValue(((double)maximumShiftLength)/2);
            errorChangeBackMax = false;
        }
        else{
            maximumShiftLength = arg1*2;
            if(arg1*2 < minimumShiftLength){
                ui->minimumLengthBox->setValue(arg1);
            }
        }
    }
}

void MainWindow::on_minimumLengthBox_valueChanged(double arg1)
{
    if(!errorChangeBackMin){
        bool error = false;
        QString errorMessage;
        if(arg1 < 0){
            error = true;
            errorMessage += "Maximum length cannot be negative";
        }
        if(arg1 > 336){
            error = true;
            errorMessage += "Maximum length cannot be greater than 168 hours";
        }
        if(fmod(arg1, 0.5) != 0){
            error = true;
            errorMessage += "Maximum length must be a multiple of 0.5";
        }
        if(error){
            QMessageBox errorBox;
            errorBox.setText(errorMessage);
            errorBox.exec();
            errorChangeBackMin = true;
            ui->minimumLengthBox->setValue(((double)minimumShiftLength)/2);
            errorChangeBackMin = false;
        }
        else{
            minimumShiftLength = arg1*2;
            if(arg1*2 > maximumShiftLength){
                ui->maximumLengthBox->setValue(arg1);
            }
        }
    }
}

void MainWindow::on_sortEmployeesButton_clicked()
{
    ui->employeeTable->sortItems(LASTNAME_COLUMN, Qt::AscendingOrder);
}

void MainWindow::on_startDateBox_userDateChanged(const QDate &date)
{
    if(!changingDateBack){
        changingDateBack = true;
        QCalendar calendarForCheckDay = QCalendar(QCalendar::System::Gregorian);
        int newDay = calendarForCheckDay.dayOfWeek(ui->startDateBox->date());
        if(newDay != Qt::Sunday){
            QDate newDate = date.addDays(-1 * newDay);
            //changingDateBack = true;
            /*
            QString errorMessage = QString("Start date must be a Sunday");
            QMessageBox errorBox;
            errorBox.setText(errorMessage);
            errorBox.exec();
            */
            ui->startDateBox->setDate(newDate);
            currentStartDate = newDate;
            //changingDateBack = false;
        }
        else{
            currentStartDate = ui->startDateBox->date();
        }
        changingDateBack = false;
    }
}

/*
void MainWindow::on_startDateBox_dateChanged(const QDate &date)
{
    if(!changingDateBack){
        QCalendar calendarForCheckDay = QCalendar(QCalendar::System::Gregorian);
        int newDay = calendarForCheckDay.dayOfWeek(date);
        if(newDay != Qt::Sunday){
            QString errorMessage = QString("Start date must be a Sunday");
            QMessageBox errorBox;
            errorBox.setText(errorMessage);
            errorBox.exec();
            changingDateBack = true;
            ui->startDateBox->setDate(currentStartDate);
            changingDateBack = false;
        }
        else{
            currentStartDate = date;
        }
    }
}
*/
