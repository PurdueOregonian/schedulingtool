#ifndef AVAILABILITYBOX_H
#define AVAILABILITYBOX_H

#include <QWidget>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QStringList>
#include <QBrush>
#include <QColor>
#include <array>
#include <QHBoxLayout>
#include <QVBoxLayout>

class MainWindow;

class availabilityBox : public QDialog{
public:
    availabilityBox(MainWindow* parent = nullptr);
    ~availabilityBox();
    void fillTable(std::array<int,336>*);
    void unavailableClicked();
    void availableClicked();
    void mustWorkClicked();
    std::array<int,336>* getAvailability();

private:
    QGridLayout* layoutForAvailabilityBox;
    QTableWidget* availabilityTable;
    QPushButton* okButton;
    QPushButton* cancelButton;
    QPushButton* unavailableButton;
    QPushButton* availableButton;
    QPushButton* mustWorkButton;
    QPushButton* addMustWorkButton;
    QPushButton* subtractMustWorkButton;
    QBrush unavailable;
    QBrush available;
    QBrush mustWork;
    std::array<int,336>* availability;
};

#endif // AVAILABILITY_H
