#include "calendarBox.h"
#include <algorithm>
#include <QSize>

calendarBox::calendarBox(MainWindow* parent){
    this->setWindowTitle(QString("Calendar View"));
    layoutForCalendarBox = new QGridLayout();
    //layoutForCalendarBox->setRowMinimumHeight(0,80);
    calendar = new QTableWidget();
    printButton = new QPushButton();
    QSize size = QSize(300,75);
    printButton->setFixedSize(size);
    //printButton->resize(75,75);
    printButton->setText("Print");
    QObject::connect(printButton, &QPushButton::clicked, this, &calendarBox::printCalendar);
    layoutForCalendarBox->addWidget(printButton, 0, 1);
    layoutForCalendarBox->addWidget(calendar, 1, 0, 1, 3);
    this->setLayout(layoutForCalendarBox);
}

calendarBox::~calendarBox(){
    for(int i=0; i<numEmployees; i++){
        for(int j=0; j<7; j++){
            delete(calendar->item(i,j));
        }
    }
    delete calendar;
}

void calendarBox::createCalendar(std::vector<std::string> firstNames, std::vector<std::string> lastNames, std::vector<bool> schedule, QDate startDate, QString title){
    titleToPrint = title;
    numEmployees = firstNames.size();
    calendar->setRowCount(numEmployees);
    calendar->setColumnCount(7);
    for(int i=0; i<7; i++){
        calendar->setColumnWidth(i,160);
    }
    QStringList employeeNames;
    QStringList dayNames;
    for(int i=0; i<numEmployees; i++){
        std::string employeeName = lastNames.at(i);
        employeeName.append(", ");
        employeeName.append(firstNames.at(i));
        QString name = QString(employeeName.c_str());
        employeeNames.append(name);
    }
    QString dayName = "Sunday ";
    dayName.append(std::to_string(startDate.month()).c_str());
    dayName.append("/");
    dayName.append(std::to_string(startDate.day()).c_str());
    dayNames.append(dayName);
    startDate = startDate.addDays(1);
    dayName = "Monday ";
    dayName.append(std::to_string(startDate.month()).c_str());
    dayName.append("/");
    dayName.append(std::to_string(startDate.day()).c_str());
    dayNames.append(dayName);
    startDate = startDate.addDays(1);
    dayName = "Tuesday ";
    dayName.append(std::to_string(startDate.month()).c_str());
    dayName.append("/");
    dayName.append(std::to_string(startDate.day()).c_str());
    dayNames.append(dayName);
    startDate = startDate.addDays(1);
    dayName = "Wednesday ";
    dayName.append(std::to_string(startDate.month()).c_str());
    dayName.append("/");
    dayName.append(std::to_string(startDate.day()).c_str());
    dayNames.append(dayName);
    startDate = startDate.addDays(1);
    dayName = "Thursday ";
    dayName.append(std::to_string(startDate.month()).c_str());
    dayName.append("/");
    dayName.append(std::to_string(startDate.day()).c_str());
    dayNames.append(dayName);
    startDate = startDate.addDays(1);
    dayName = "Friday ";
    dayName.append(std::to_string(startDate.month()).c_str());
    dayName.append("/");
    dayName.append(std::to_string(startDate.day()).c_str());
    dayNames.append(dayName);
    startDate = startDate.addDays(1);
    dayName = "Saturday ";
    dayName.append(std::to_string(startDate.month()).c_str());
    dayName.append("/");
    dayName.append(std::to_string(startDate.day()).c_str());
    dayNames.append(dayName);
    employeeNamesToPrint = employeeNames;
    dayNamesToPrint = dayNames;
    calendar->setHorizontalHeaderLabels(dayNames);
    calendar->setVerticalHeaderLabels(employeeNames);
    for(int i=0; i<numEmployees; i++){
        for(int j=0; j<7; j++){
            QTableWidgetItem* cell = new QTableWidgetItem();
            cell->setFlags(cell->flags() & ~Qt::ItemIsEditable);
            calendar->setItem(i,j,cell);
        }
    }
    for(int i=0; i<numEmployees; i++){
        for(int j=0; j<336; j++){
            if(schedule[i*336+j]){
                int day = j/48;
                std::string toDisplay = getDayTime(j, false);
                while(schedule[i*336+j] && j!=336){
                    j++;
                }
                toDisplay.append("-");
                if(j/48 == day){
                    toDisplay.append(getDayTime(j, false));
                }
                else{
                    toDisplay.append(getDayTime(j,true));
                }
                QString displayString = QString(toDisplay.c_str());
                calendar->item(i, day)->setData(Qt::EditRole, displayString);
                j--;
            }
        }
    }
}

void calendarBox::printCalendar(){
    QPrinter calendarPrinter;
    QPainter calendarPainter;
    calendarPrinter.setPageOrientation(QPageLayout::Landscape);
    QPrintDialog calendarPrintDialog(&calendarPrinter, this);
    if(calendarPrintDialog.exec()==QDialog::Accepted){
        if(!calendarPainter.begin(&calendarPrinter)){
            return;
        }
        QRectF dimensions = calendarPrinter.pageRect(QPrinter::DevicePixel);
        int margin = 10;
        int pageMargin = 25;
        int titleHeight = 80;
        int nameColumnWidth = 200;
        qreal height = dimensions.height() - 2*pageMargin - titleHeight;
        qreal width = dimensions.width() - 2*pageMargin;
        int columnWidth = std::min((int)floor(((int)width-nameColumnWidth)/7), 160);
        int begin = std::max(0,(int)(width/2-(nameColumnWidth+7*columnWidth)/2));
        int rowsPerPage = height/50;
        int employeesThisPage;
        for(int i=0; i<=(numEmployees/(rowsPerPage - 1)); i++){
            if(i!=0){
                calendarPrinter.newPage();
            }
            if(i != (numEmployees/(rowsPerPage - 1))){
                employeesThisPage = rowsPerPage - 1;
            }
            else{
                employeesThisPage = numEmployees % (rowsPerPage - 1);
            }
            QFont titleFont = QFont();
            titleFont.setPointSize(24);
            QFontMetrics titleFontMetrics = QFontMetrics(titleFont);
            int textWidth = titleFontMetrics.horizontalAdvance(titleToPrint);
            calendarPainter.setFont(titleFont);
            calendarPainter.drawText(pageMargin+(int)(width/2) - textWidth/2, pageMargin+titleHeight*1/2, titleToPrint);
            QFont defaultFont = QFont();
            QFont smallerFont = QFont();
            QFontMetrics printMetrics = QFontMetrics(defaultFont);
            calendarPainter.setFont(defaultFont);
            for(int j=0; j<=employeesThisPage+1; j++){
                calendarPainter.drawLine(pageMargin+begin, pageMargin+titleHeight+j*50,pageMargin+begin+nameColumnWidth+7*columnWidth, pageMargin+titleHeight+j*50);
            }
            calendarPainter.drawLine(pageMargin+begin, pageMargin+titleHeight, pageMargin+begin, pageMargin+titleHeight+(employeesThisPage+1)*50);
            for(int j=0; j<=7; j++){
                calendarPainter.drawLine(pageMargin+begin+nameColumnWidth+j*columnWidth, pageMargin+titleHeight, pageMargin+begin+nameColumnWidth+j*columnWidth, pageMargin+titleHeight+(employeesThisPage+1)*50);
            }
            int maxDayWidth = 0;
            for(int i=0; i<7; i++){
                int dayWidth = printMetrics.horizontalAdvance(dayNamesToPrint.at(i));
                if(dayWidth > maxDayWidth){
                    maxDayWidth = dayWidth;
                }
            }
            if(maxDayWidth > (columnWidth - margin - 10)){
                int newFontSize = defaultFont.pointSize()*(columnWidth-25)/maxDayWidth;
                smallerFont.setPointSize(newFontSize);
                calendarPainter.setFont(smallerFont);
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+margin, pageMargin+titleHeight+margin+20-(defaultFont.pointSize()-newFontSize)/2, dayNamesToPrint.at(0));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+columnWidth+margin, pageMargin+titleHeight+margin+20-(defaultFont.pointSize()-newFontSize)/2, dayNamesToPrint.at(1));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+2*columnWidth+margin, pageMargin+titleHeight+margin+20-(defaultFont.pointSize()-newFontSize)/2, dayNamesToPrint.at(2));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+3*columnWidth+margin, pageMargin+titleHeight+margin+20-(defaultFont.pointSize()-newFontSize)/2, dayNamesToPrint.at(3));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+4*columnWidth+margin, pageMargin+titleHeight+margin+20-(defaultFont.pointSize()-newFontSize)/2, dayNamesToPrint.at(4));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+5*columnWidth+margin, pageMargin+titleHeight+margin+20-(defaultFont.pointSize()-newFontSize)/2, dayNamesToPrint.at(5));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+6*columnWidth+margin, pageMargin+titleHeight+margin+20-(defaultFont.pointSize()-newFontSize)/2, dayNamesToPrint.at(6));
                calendarPainter.setFont(defaultFont);
            }
            else{
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+margin, pageMargin+titleHeight+margin+20, dayNamesToPrint.at(0));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+columnWidth+margin, pageMargin+titleHeight+margin+20, dayNamesToPrint.at(1));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+2*columnWidth+margin, pageMargin+titleHeight+margin+20, dayNamesToPrint.at(2));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+3*columnWidth+margin, pageMargin+titleHeight+margin+20, dayNamesToPrint.at(3));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+4*columnWidth+margin, pageMargin+titleHeight+margin+20, dayNamesToPrint.at(4));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+5*columnWidth+margin, pageMargin+titleHeight+margin+20, dayNamesToPrint.at(5));
                calendarPainter.drawText(pageMargin+begin+nameColumnWidth+6*columnWidth+margin, pageMargin+titleHeight+margin+20, dayNamesToPrint.at(6));
            }
            for(int j=0; j<employeesThisPage; j++){
                QString nameToPrint = employeeNamesToPrint.at(i*(rowsPerPage-1)+j);
                int nameWidth = printMetrics.horizontalAdvance(nameToPrint);
                if(nameWidth > (nameColumnWidth - margin - 10)){
                    int newFontSize = defaultFont.pointSize()*(nameColumnWidth-25)/nameWidth;
                    smallerFont.setPointSize(newFontSize);
                    calendarPainter.setFont(smallerFont);
                    calendarPainter.drawText(pageMargin+begin+margin, pageMargin+titleHeight+margin+20+50*(j+1)-(defaultFont.pointSize()-newFontSize)/2, nameToPrint);
                    calendarPainter.setFont(defaultFont);
                }
                else{
                    calendarPainter.drawText(pageMargin+begin+margin, pageMargin+titleHeight+margin+20+50*(j+1), nameToPrint);
                }
                for(int k=0; k<=6; k++){
                    QString shiftToPrint = calendar->item(i*(rowsPerPage-1)+j,k)->data(Qt::DisplayRole).toString();
                    int shiftWidth = printMetrics.horizontalAdvance(shiftToPrint);
                    if(shiftWidth > (columnWidth - margin - 10)){
                        int newFontSize = defaultFont.pointSize()*(columnWidth-25)/shiftWidth;
                        smallerFont.setPointSize(newFontSize);
                        calendarPainter.setFont(smallerFont);
                        calendarPainter.drawText(pageMargin+begin+nameColumnWidth+k*columnWidth+margin, pageMargin+titleHeight+margin+20+50*(j+1)-(defaultFont.pointSize()-newFontSize)/2, shiftToPrint);
                        calendarPainter.setFont(defaultFont);
                    }
                    else{
                        calendarPainter.drawText(pageMargin+begin+nameColumnWidth+k*columnWidth+margin, pageMargin+titleHeight+margin+20+50*(j+1), shiftToPrint);
                    }
                }
            }
        }
    }
}
