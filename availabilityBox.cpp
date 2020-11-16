#include "availabilityBox.h"
#include "mainwindow.h"

availabilityBox::availabilityBox(MainWindow* parent){
    this->setModal(true);
    this->setWindowTitle(QString("Set Availability"));
    availability = new std::array<int,336>();
    unavailable.setStyle(Qt::SolidPattern);
    available.setStyle(Qt::SolidPattern);
    mustWork.setStyle(Qt::SolidPattern);
    unavailable.setColor(QColor(255,127,127));
    available.setColor(QColor(127,255,127));
    mustWork.setColor(QColor(127,127,255));
    layoutForAvailabilityBox = new QGridLayout();
    availabilityTable = new QTableWidget();
    availabilityTable->setColumnCount(48);
    availabilityTable->setRowCount(7);
    QStringList rowList;
    QStringList columnList;
    for(int i=0; i<48; i++){
        QString timeString;
        int hour = (i/2) % 12;
        if(hour == 0){
            hour = 12;
        }
        timeString += QString::number(hour);
        if(i % 2 == 0){
            timeString += ":00";
        }
        else{
            timeString += ":30";
        }
        if(i<24){
            timeString += "a";
        }
        else{
            timeString += "p";
        }
        rowList.append(timeString);
        availabilityTable->setColumnWidth(i,20);
    }
    columnList.append("Sunday");
    columnList.append("Monday");
    columnList.append("Tuesday");
    columnList.append("Wednesday");
    columnList.append("Thursday");
    columnList.append("Friday");
    columnList.append("Saturday");
    availabilityTable->setHorizontalHeaderLabels(rowList);
    availabilityTable->setVerticalHeaderLabels(columnList);
    for(int i=0; i<7; i++){
        for(int j=0; j<48; j++){
            QTableWidgetItem* cell = new QTableWidgetItem();
            cell->setFlags(cell->flags() & ~Qt::ItemIsEditable);
            availabilityTable->setItem(i,j,cell);
        }
    }
    okButton = new QPushButton();
    cancelButton = new QPushButton();
    unavailableButton = new QPushButton();
    availableButton = new QPushButton();
    mustWorkButton = new QPushButton();
    addMustWorkButton = new QPushButton();
    subtractMustWorkButton = new QPushButton();
    okButton->setText("OK");
    QObject::connect(okButton, &QPushButton::clicked, parent, &MainWindow::availabilityOkClicked);
    cancelButton->setText("Cancel");
    QObject::connect(cancelButton, &QPushButton::clicked, this, &QWidget::close);
    unavailableButton->setText("Set Unavailable");
    QObject::connect(unavailableButton, &QPushButton::clicked, this, &availabilityBox::unavailableClicked);
    availableButton->setText("Set Available");
    QObject::connect(availableButton, &QPushButton::clicked, this, &availabilityBox::availableClicked);
    mustWorkButton->setText("Set Must Work");
    QObject::connect(mustWorkButton, &QPushButton::clicked, this, &availabilityBox::mustWorkClicked);
    addMustWorkButton->setText("Add Must Work");
    QObject::connect(addMustWorkButton, &QPushButton::clicked, parent, &MainWindow::addMustWorkClicked);
    subtractMustWorkButton->setText("Subtract Must Work");
    QObject::connect(subtractMustWorkButton, &QPushButton::clicked, parent, &MainWindow::subtractMustWorkClicked);
    layoutForAvailabilityBox->addWidget(unavailableButton,0,0);
    layoutForAvailabilityBox->addWidget(availableButton,0,1);
    layoutForAvailabilityBox->addWidget(mustWorkButton,0,2);
    layoutForAvailabilityBox->addWidget(addMustWorkButton,1,0);
    layoutForAvailabilityBox->addWidget(subtractMustWorkButton,1,1);
    layoutForAvailabilityBox->addWidget(availabilityTable,2,0,1,3);
    layoutForAvailabilityBox->addWidget(okButton,3,0);
    layoutForAvailabilityBox->addWidget(cancelButton,3,1);
    this->setLayout(layoutForAvailabilityBox);
    this->resize(1200,410);
    this->show();
}

availabilityBox::~availabilityBox(){
    for(int i=0; i<7; i++){
        for(int j=0; j<48; j++){
            delete(availabilityTable->item(i,j));
        }
    }
    delete layoutForAvailabilityBox;
    delete availabilityTable;
    delete okButton;
    delete cancelButton;
    delete unavailableButton;
    delete availableButton;
    delete mustWorkButton;
    delete availability;
}

void availabilityBox::fillTable(std::array<int,336>* availabilityToSet){
    for(int i=0; i<336; i++){
        if(availabilityToSet->at(i)==0){
            availabilityTable->item(i / 48,i % 48)->setBackground(unavailable);
        }
        else if(availabilityToSet->at(i)==1){
            availabilityTable->item(i / 48,i % 48)->setBackground(available);
        }
        else if(availabilityToSet->at(i)==2){
            availabilityTable->item(i / 48,i % 48)->setBackground(mustWork);
        }
    }
    *availability = *availabilityToSet;
    //availability[335] = availabilityToSet[335];
}

void availabilityBox::unavailableClicked(){
    QList<QTableWidgetItem*> selectedCells = availabilityTable->selectedItems();
    for(int i=0; i<selectedCells.size(); i++){
        int cellNumber = selectedCells.at(i)->row()*48+selectedCells.at(i)->column();
        availability->at(cellNumber) = 0;
        availabilityTable->item(cellNumber / 48,cellNumber % 48)->setBackground(unavailable);
    }
    availabilityTable->clearSelection();
}

void availabilityBox::availableClicked(){
    QList<QTableWidgetItem*> selectedCells = availabilityTable->selectedItems();
    for(int i=0; i<selectedCells.size(); i++){
        int cellNumber = selectedCells.at(i)->row()*48+selectedCells.at(i)->column();
        availability->at(cellNumber) = 1;
        availabilityTable->item(cellNumber / 48,cellNumber % 48)->setBackground(available);
    }
    availabilityTable->clearSelection();
}

void availabilityBox::mustWorkClicked(){
    QList<QTableWidgetItem*> selectedCells = availabilityTable->selectedItems();
    for(int i=0; i<selectedCells.size(); i++){
        int cellNumber = selectedCells.at(i)->row()*48+selectedCells.at(i)->column();
        availability->at(cellNumber) = 2;
        availabilityTable->item(cellNumber / 48,cellNumber % 48)->setBackground(mustWork);
    }
    availabilityTable->clearSelection();
}

std::array<int,336>* availabilityBox::getAvailability(){
    return availability;
}
