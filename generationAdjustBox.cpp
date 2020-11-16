#include "generationAdjustBox.h"

generationAdjustBox::generationAdjustBox(MainWindow* parent){
    this->setModal(true);
    this->setWindowTitle(QString("Problem Generating Shifts"));
    layoutForGenerationAdjustBox = new QFormLayout();
    text1 = new QLabel();
    shiftsBox = new QTextEdit();
    text2 = new QLabel();
    extendButton = new QPushButton();
    removeButton = new QPushButton();
    leaveButton = new QPushButton();
    cancelButton = new QPushButton();
    shiftsBox->setReadOnly(true);
    text1->setText("Problem generating shifts that are long enough.\nShifts generated:");
    extendButton->setText("Extend Shift");
    removeButton->setText("Remove Shift");
    leaveButton->setText("Leave As Is");
    cancelButton->setText("Cancel");
    layoutForGenerationAdjustBox->addRow(text1);
    layoutForGenerationAdjustBox->addRow(shiftsBox);
    layoutForGenerationAdjustBox->addRow(text2);
    layoutForGenerationAdjustBox->addRow(extendButton);
    layoutForGenerationAdjustBox->addRow(removeButton);
    layoutForGenerationAdjustBox->addRow(leaveButton);
    layoutForGenerationAdjustBox->addRow(cancelButton);
    this->setLayout(layoutForGenerationAdjustBox);
    QObject::connect(extendButton, &QPushButton::clicked, this, &generationAdjustBox::extendShift);
    QObject::connect(removeButton, &QPushButton::clicked, this, &generationAdjustBox::removeShift);
    QObject::connect(leaveButton, &QPushButton::clicked, this, &generationAdjustBox::leaveShift);
    QObject::connect(cancelButton, &QPushButton::clicked, this, &generationAdjustBox::cancelGeneration);
}

generationAdjustBox::~generationAdjustBox(){
    delete text1;
    delete shiftsBox;
    delete text2;
    delete extendButton;
    delete removeButton;
    delete leaveButton;
    delete cancelButton;
}

void generationAdjustBox::extendShift(){
    this->done(EXTEND_VALUE);
}

void generationAdjustBox::removeShift(){
    this->done(REMOVE_VALUE);
}

void generationAdjustBox::leaveShift(){
    this->done(LEAVE_VALUE);
}

void generationAdjustBox::cancelGeneration(){
    this->done(DO_NOTHING_VALUE);
}

void generationAdjustBox::editShiftsBox(QString text){
    shiftsBox->setText(text.toStdString().c_str());
}

void generationAdjustBox::editSecondLabel(const char* text){
    text2->setText(text);
}
