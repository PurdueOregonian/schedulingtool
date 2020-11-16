#include "buttonWithAvailability.h"

buttonWithAvailability::buttonWithAvailability(const QString &text, QWidget* parent) : QPushButton(text, parent){
    availability = new std::array<int,336>();
    availability->fill(1);
}

buttonWithAvailability::~buttonWithAvailability(){
    delete availability;
}
