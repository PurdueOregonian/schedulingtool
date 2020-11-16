#ifndef BUTTONWITHAVAILABILITY_H
#define BUTTONWITHAVAILABILITY_H

#include <QPushButton>
#include <array>

class buttonWithAvailability : public QPushButton{
public:
    buttonWithAvailability(const QString&, QWidget*);
    ~buttonWithAvailability();
    std::array<int,336>* availability;
};

#endif // BUTTONWITHAVAILABILITY_H
