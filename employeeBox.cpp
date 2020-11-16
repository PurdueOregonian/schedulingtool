#include "employeeBox.h"
#include "mainwindow.h"

employeeBox::employeeBox(MainWindow* parent){
    this->setModal(true);
    this->setWindowTitle(QString("Add Employee"));
    layoutForEmployeeBox = new QFormLayout();
    firstNameBox = new QLineEdit();
    lastNameBox = new QLineEdit();
    minHoursBox = new QDoubleSpinBox();
    maxHoursBox = new QDoubleSpinBox();
    priorityBox = new QSpinBox();
    okButton = new QPushButton();
    cancelButton = new QPushButton();
    firstNameBox->setMaxLength(100);
    lastNameBox->setMaxLength(100);
    layoutForEmployeeBox->addRow(tr("Employee First Name:"),firstNameBox);
    layoutForEmployeeBox->addRow(tr("Employee Last Name:"),lastNameBox);
    minHoursBox->setDecimals(1);
    minHoursBox->setSingleStep(0.5);
    minHoursBox->setMaximum(168);
    layoutForEmployeeBox->addRow(tr("Minimum Hours:"),minHoursBox);
    maxHoursBox->setDecimals(1);
    maxHoursBox->setSingleStep(0.5);
    maxHoursBox->setMaximum(168);
    layoutForEmployeeBox->addRow(tr("Maximum Hours:"),maxHoursBox);
    priorityBox->setMinimum(1);
    priorityBox->setMaximum(1000);
    priorityBox->setValue(10);
    layoutForEmployeeBox->addRow(tr("Priority (1-1000) (higher means more important):"), priorityBox);
    okButton->setText("OK");
    QObject::connect(okButton, &QPushButton::clicked, parent, &MainWindow::okClicked);
    cancelButton->setText("Cancel");
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QWidget::close);
    layoutForEmployeeBox->addRow(okButton);
    layoutForEmployeeBox->addRow(cancelButton);
    this->setLayout(layoutForEmployeeBox);
    //employeeBox->resize(400,80);
    this->show();
}

employeeBox::~employeeBox(){
    delete layoutForEmployeeBox;
    delete firstNameBox;
    delete lastNameBox;
    delete minHoursBox;
    delete maxHoursBox;
    delete priorityBox;
    delete okButton;
    delete cancelButton;
}

QString employeeBox::getFirstName(){
    return firstNameBox->text();
}

QString employeeBox::getLastName(){
    return lastNameBox->text();
}

double employeeBox::getMinHours(){
    return minHoursBox->value();
}

double employeeBox::getMaxHours(){
    return maxHoursBox->value();
}

int employeeBox::getPriority(){
    return priorityBox->value();
}
