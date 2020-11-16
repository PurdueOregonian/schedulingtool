#ifndef EMPLOYEEBOX_H
#define EMPLOYEEBOX_H

#include <QWidget>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>

class MainWindow;

class employeeBox : public QDialog{
public:
    employeeBox(MainWindow* parent = nullptr);
    ~employeeBox();
    QString getFirstName();
    QString getLastName();
    double getMinHours();
    double getMaxHours();
    int getPriority();
private:
    QFormLayout* layoutForEmployeeBox;
    QLineEdit* firstNameBox;
    QLineEdit* lastNameBox;
    QDoubleSpinBox* minHoursBox;
    QDoubleSpinBox* maxHoursBox;
    QSpinBox* priorityBox;
    QPushButton* okButton;
    QPushButton* cancelButton;
};

#endif // EMPLOYEEBOX_H
