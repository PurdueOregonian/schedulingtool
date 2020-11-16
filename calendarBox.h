#ifndef CALENDARBOX_H
#define CALENDARBOX_H

#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>
#include <QRectF>
#include <QStringList>
#include <QGridLayout>
#include <QDate>
#include <math.h>
#include "utility.h"

class MainWindow;

class calendarBox : public QDialog{
public:
    calendarBox(MainWindow* parent = nullptr);
    ~calendarBox();
    void createCalendar(std::vector<std::string>, std::vector<std::string>, std::vector<bool>, QDate, QString);
    void printCalendar();
private:
    QGridLayout* layoutForCalendarBox;
    QTableWidget* calendar;
    QPushButton* printButton;
    QStringList employeeNamesToPrint;
    QStringList dayNamesToPrint;
    QString titleToPrint;
    int numEmployees;
};

#endif // CALENDARBOX_H
