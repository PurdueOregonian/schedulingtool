#ifndef GENERATIONADJUSTBOX_H
#define GENERATIONADJUSTBOX_H

#define EXTEND_VALUE 2
#define REMOVE_VALUE 3
#define LEAVE_VALUE 4
#define DO_NOTHING_VALUE 5

#include <QFormLayout>
#include <QDialog>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QString>

class MainWindow;

class generationAdjustBox : public QDialog{
public:
    generationAdjustBox(MainWindow* parent = nullptr);
    ~generationAdjustBox();
    void editShiftsBox(QString);
    void editSecondLabel(const char*);
private:
    QFormLayout* layoutForGenerationAdjustBox;
    QLabel* text1;
    QTextEdit* shiftsBox;
    QLabel* text2;
    QPushButton* extendButton;
    QPushButton* removeButton;
    QPushButton* leaveButton;
    QPushButton* cancelButton;
    void extendShift();
    void removeShift();
    void leaveShift();
    void cancelGeneration();
};

#endif // GENERATIONADJUSTBOX_H
