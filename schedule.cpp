#include <vector>
#include "schedule.h"

#define UNAVAILABLE_PENALTY 200
#define MORE_THAN_FIVE_DAY_WEEK_PENALTY 20

/*
 * Copyright William Liang, 2020
 * Made by William Liang using Qt Creator
*/

void Schedule::addEmployee(Employee* employee){
    employeesList.push_back(employee);
}

void Schedule::addRequirements(std::array<int,336>* requirements){
    *employeesRequired = *requirements;
}

QString Schedule::createSchedule(std::vector<std::array<int,2>>* shiftsToPass, int minimumShiftLength, int maximumShiftLength){
    QString scheduleToDisplay;
    QString errorSchedule;
    Schedule s = *this;
    s.employeesRequired = new std::array<int,336>();
    *(s.employeesRequired) = *(this->employeesRequired);
    int numEmployees = s.employeesList.size();
    std::vector<int> totalAvailabilities;
    std::vector<bool> employeeSchedule;
    std::vector<int> shiftAssignments;

    std::array<int,7> negativeOnes = std::array<int,7>();
    negativeOnes.fill(-1);

    //indices for this: employee, day
    std::vector<std::array<int,7>>* worksOnMatrix = new std::vector<std::array<int,7>>();
    //std::array<bool,7> falseArray = std::array<bool,7>();
    //falseArray.fill(false);
    for(int i=0; i<numEmployees; i++){
        worksOnMatrix->push_back(negativeOnes);
    }

    for(int i=0; i<numEmployees; i++){
        for(int j=0; j<336; j++){
            if(s.employeesList[i]->getAvailability(j) == 2){
                /*
                int begin = j - j%48;
                for(int k=j; k<begin+48; k++){
                    if(s.employeesList[i]->getAvailability(k) == 2){
                        employeeSchedule.push_back(true);
                        s.employeesRequired->at(k)--;
                        employeesList[i]->minHalfHours--;
                        employeesList[i]->maxHalfHours--;
                    }
                    else{
                        employeeSchedule.push_back(false);
                    }
                }
                */
                //-2 is special value for unswappable shift
                worksOnMatrix->at(i).at(j/48) = -2;
                while(s.employeesList[i]->getAvailability(j) == 2){
                    employeeSchedule.push_back(true);
                    s.employeesRequired->at(j)--;
                    employeesList[i]->minHalfHours--;
                    employeesList[i]->maxHalfHours--;
                    j++;
                }
                j--;
                //don't do this
                //employeesList[i]->setUnavailable(begin, begin+47);
                //j = begin+47;
            }
            else{
                employeeSchedule.push_back(false);
            }
        }
        totalAvailabilities.push_back(s.employeesList[i]->getRemainingAvailability(0));
    }

    //sort the shifts by decreasing order of length
    //for(int i=0; i<7; i++){
        std::sort(shiftsToPass->begin(), shiftsToPass->end(), shiftSorter);
    //}

    std::vector<std::vector<bool>>* availabilityMatrix = new std::vector<std::vector<bool>>();
    for(int i=0; i<numEmployees; i++){
        availabilityMatrix->push_back(std::vector<bool>(shiftsToPass->size(), false));
    }
    //indices for this: employee, shift

    //TODO: what if it's a zero? Fix
    std::vector<std::array<int,7>>* mostHalfHoursPossibleMatrix = new std::vector<std::array<int,7>>();
    //-1 means no shift is possible that day
    for(int i=0; i<numEmployees; i++){
        mostHalfHoursPossibleMatrix->push_back(negativeOnes);
    }

    //Now let's compute the availability matrix, i.e. figure out for each employee and each shift whether the employee is available for the shift.
    //Availability matrix indices: employee, day, shift
    //bool halfHoursMatrixFilled;
    for(int i=0; i<numEmployees; i++){
            int numShifts = shiftsToPass->size();
            //halfHoursMatrixFilled = false;
            for(int k=numShifts-1; k>=0; k--){
                int begin = shiftsToPass->at(k).at(0);
                int end = shiftsToPass->at(k).at(1);
                bool available = s.employeesList[i]->isAvailable(begin,end) && worksOnMatrix->at(i).at(begin/48) != -2;
                availabilityMatrix->at(i).at(k) = available;
                if(available){
                    mostHalfHoursPossibleMatrix->at(i).at(begin/48) = k;
                    //halfHoursMatrixFilled = true;
                }
            }
    }

    //TODO: not for each day
    //for(int i=0; i<7; i++){
        //for each day
        int numShifts = shiftsToPass->size();
        for(int j=0; j<numShifts; j++){
            //for each shift, assign it
            int begin = shiftsToPass->at(j).at(0);
            int end = shiftsToPass->at(j).at(1);
            int shiftLength = end - begin;
            int employeeToAssign = -1;
            double currentEmployeeScore = 0.0;
            for(int k=0; k<numEmployees; k++){
                //check for each available employee that can be assigned for the number of hours
                //availabilityMatrix->at(k).at(i).at(j) is availability
                //TODO: probably also avoid 6 or 7 day weeks
                if(availabilityMatrix->at(k).at(j) && employeesList[k]->maxHalfHours >= shiftLength && worksOnMatrix->at(k).at(begin/48)==-1){
                    bool cannotAssign = gapRestriction(employeeSchedule, k, begin, end);
                    if(!cannotAssign){
                        if(currentEmployeeScore == 0.0){
                            employeeToAssign = k;
                        }
                        //int mostHalfHoursPossible = 0;
                        //TODO: after fixing mostHalfHoursPossibleMatrix, fix this too
                        //Find sum of highest that can make a 5 day week
                        int longestShiftsPossible[7];
                        bool worksOn[7];
                        for(int a = 0; a<7; a++){
                            if(mostHalfHoursPossibleMatrix->at(k).at(a)==-1){
                                longestShiftsPossible[a] = 0;
                            }
                            else{
                                longestShiftsPossible[a] = shiftsToPass->at(mostHalfHoursPossibleMatrix->at(k).at(a)).at(1) - shiftsToPass->at(mostHalfHoursPossibleMatrix->at(k).at(a)).at(0);
                            }
                            worksOn[a] = worksOnMatrix->at(k).at(a)!=-1;
                        }
                        /*
                        if(mostHalfHoursPossible == 0){
                            mostHalfHoursPossible = 1;
                        }
                        */
                        double sumOfLongest = getSumOfLongest(longestShiftsPossible, worksOn);
                        if(sumOfLongest == 0){
                            sumOfLongest = 1;
                        }
                        if((double)employeesList[k]->minHalfHours/sumOfLongest >= currentEmployeeScore){
                            employeeToAssign = k;
                            currentEmployeeScore = (double)employeesList[k]->minHalfHours/sumOfLongest;
                        }
                    }
                }
            }
            //if no such employee, check all employees
            if(employeeToAssign == -1){
                double howBad = 368001.0;
                for(int k=0; k<numEmployees; k++){
                    int thisIsHowBad;
                    //TODO: distinguish here between unavailability because of shift already assigned and unavailability because of preference
                    if(worksOnMatrix->at(k).at(begin/48)==-1){
                        bool cannotAssign = gapRestriction(employeeSchedule,k,begin,end);
                        if(!cannotAssign){
                            if(availabilityMatrix->at(k).at(j)){
                                //not as bad
                                thisIsHowBad = (shiftLength - employeesList[k]->maxHalfHours)*employeesList[k]->priority;
                            }
                            else{
                                //very bad
                                thisIsHowBad = (shiftLength - employeesList[k]->maxHalfHours + UNAVAILABLE_PENALTY)*employeesList[k]->priority;
                            }
                            if(thisIsHowBad < howBad){
                                employeeToAssign = k;
                                howBad = thisIsHowBad;
                            }
                        }
                    }
                    //TODO: record violations here
                }
            }
            if(employeeToAssign == -1){
                //No employees were available.
                //TODO: what to do if no employees are available
                delete availabilityMatrix;
                delete mostHalfHoursPossibleMatrix;
                //crash prevention
                scheduleToPass = employeeSchedule;
                return QString("No solution found.");
            }
            //int numShifts = shiftsToPass->at(i).size();
            /*
            for(int a=0; a<numShifts; a++){
                availabilityMatrix->at(employeeToAssign).at(i).at(a) = false;
            }
            */
            worksOnMatrix->at(employeeToAssign).at(begin/48) = j;
            s.employeesList[employeeToAssign]->minHalfHours -= shiftLength;
            s.employeesList[employeeToAssign]->maxHalfHours -= shiftLength;
            for(int a = begin; a < end; a++){
                employeeSchedule[employeeToAssign * 336 + a] = true;
            }
            for(int k=0; k<numEmployees; k++){
                if(mostHalfHoursPossibleMatrix->at(k).at(begin/48) == j){
                    mostHalfHoursPossibleMatrix->at(k).at(begin/48) = -1;
                    for(int a=numShifts-1; a>j; a--){
                        int start = shiftsToPass->at(a).at(0);
                        if(start/48 == begin/48){
                            int finish = shiftsToPass->at(a).at(1);
                            bool available = s.employeesList[k]->isAvailable(start,finish);
                            if(available){
                                mostHalfHoursPossibleMatrix->at(k).at(start/48) = a;
                                //halfHoursMatrixFilled = true;
                            }
                        }
                    }
                }
            }
            shiftAssignments.push_back(employeeToAssign);
        }
    //}

    //TODO: Implement swapping same-day shifts
    //int numMaxPenaltyPoints = 0;
    int hoursUnder = 0;
    int hoursOver = 0;
    int priority = 0;
    int pointsGained = 1;
    //keep going until there are no gains
    while(pointsGained > 0){
        pointsGained = 0;
        for(int k=0; k<numEmployees; k++){
            //if(s.employeesList[k]->minHalfHours > 0){
                //if(s.employeesList[k]->minHalfHours * s.employeesList[k]->priority > numMaxPenaltyPoints){
                    //numMaxPenaltyPoints = s.employeesList[k]->minHalfHours * s.employeesList[k]->priority;
                    hoursUnder = s.employeesList[k]->minHalfHours;
                    hoursOver = -s.employeesList[k]->maxHalfHours;
                    priority = s.employeesList[k]->priority;
                //}
            //}
            //else if(s.employeesList[k]->maxHalfHours < 0){
                //if(-s.employeesList[k]->maxHalfHours * s.employeesList[k]->priority > numMaxPenaltyPoints){
                    //numMaxPenaltyPoints = -s.employeesList[k]->maxHalfHours * s.employeesList[k]->priority;
                    //hoursOver = -s.employeesList[k]->maxHalfHours;
                    //hoursUnder = s.employeesList[k]->minHalfHours;
                    //priority = s.employeesList[k]->priority;
                //}
            //}
            int maxPointGain = 0;
            int shiftToPass = -1;
            int shiftToTake = -1;
            int employeeToSwap = -1;
            if(hoursOver > 0){
                //try to reduce the number of hours
                for(int i=0; i<numShifts; i++){
                    int begin = shiftsToPass->at(i).at(0);
                    int end = shiftsToPass->at(i).at(1);
                    int pointGainAddition = 0;
                    if(!availabilityMatrix->at(k).at(i)){
                        pointGainAddition = UNAVAILABLE_PENALTY * priority;
                    }
                    if(shiftAssignments.at(i) == k){
                        //This is the employee's shift. Try to give it to someone else
                        int shiftLength = end - begin;
                        int pointGain = gainFromRemoveShift(hoursUnder, hoursOver, shiftLength, priority) + pointGainAddition;
                        for(int j=0; j<numEmployees; j++){
                            if(availabilityMatrix->at(j).at(i) && worksOnMatrix->at(j).at(begin/48)==-1 && canTake(employeeSchedule,j,begin,end)){
                                int nextPointGain = gainFromAssignShift(s.employeesList[j]->minHalfHours, -s.employeesList[j]->maxHalfHours, shiftLength, s.employeesList[j]->priority);
                                if(pointGain + nextPointGain > maxPointGain){
                                    shiftToPass = i;
                                    employeeToSwap = j;
                                    shiftToTake = -1;
                                    maxPointGain = pointGain + nextPointGain;
                                }
                            }
                        }
                    }
                    else{
                        //This isn't the employee's shift. Try to swap for it if it's shorter
                        int shiftLength = end - begin;
                        for(int a=0; a<7; a++){
                            int currentWorkingShift = worksOnMatrix->at(k).at(a);
                            if(currentWorkingShift >= 0 && availabilityMatrix->at(k).at(i)){
                                int currentBegin = shiftsToPass->at(currentWorkingShift).at(0);
                                int currentEnd = shiftsToPass->at(currentWorkingShift).at(1);
                                bool alreadyShift = false;
                                if(begin/48 != currentBegin/48){
                                    if(worksOnMatrix->at(shiftAssignments.at(i)).at(a) != -1 || worksOnMatrix->at(k).at(begin/48) != -1){
                                        alreadyShift = true;
                                    }
                                }
                                //shiftLengthPass
                                if(!alreadyShift && availabilityMatrix->at(shiftAssignments.at(i)).at(currentWorkingShift) && canSwap(employeeSchedule,k,begin,end,shiftAssignments.at(i),currentBegin,currentEnd)){
                                    int shiftLengthCurrent = currentEnd - currentBegin;
                                    if(shiftLength < shiftLengthCurrent){
                                        int pointGain = gainFromRemoveShift(hoursUnder, hoursOver, shiftLengthCurrent - shiftLength, priority) + gainFromAssignShift(s.employeesList[shiftAssignments.at(i)]->minHalfHours, -s.employeesList[shiftAssignments.at(i)]->maxHalfHours, shiftLengthCurrent - shiftLength, s.employeesList[shiftAssignments.at(i)]->priority) + pointGainAddition;
                                        if(pointGain > maxPointGain){
                                            shiftToPass = currentWorkingShift;
                                            employeeToSwap = shiftAssignments.at(i);
                                            shiftToTake = i;
                                            maxPointGain = pointGain;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if(maxPointGain > 0){
                    //pass the shift here
                    int beginPass = shiftsToPass->at(shiftToPass).at(0);
                    int endPass = shiftsToPass->at(shiftToPass).at(1);
                    int shiftLengthPass = endPass-beginPass;
                    int shiftLengthTake;
                    int beginTake;
                    int endTake;
                    if(shiftToTake != -1){
                        beginTake = shiftsToPass->at(shiftToTake).at(0);
                        endTake = shiftsToPass->at(shiftToTake).at(1);
                        shiftLengthTake = endTake - beginTake;
                    }
                    for(int a = beginPass; a < endPass; a++){
                        employeeSchedule[k * 336 + a] = false;
                    }
                    if(shiftToTake != -1){
                        for(int a = beginTake; a < endTake; a++){
                            employeeSchedule[employeeToSwap * 336 + a] = false;
                        }
                    }
                    for(int a = beginPass; a < endPass; a++){
                        employeeSchedule[employeeToSwap * 336 + a] = true;
                    }
                    if(shiftToTake != -1){
                        for(int a = beginTake; a < endTake; a++){
                            employeeSchedule[k * 336 + a] = true;
                        }
                    }
                    s.employeesList[k]->minHalfHours += shiftLengthPass;
                    s.employeesList[k]->maxHalfHours += shiftLengthPass;
                    s.employeesList[employeeToSwap]->minHalfHours -= shiftLengthPass;
                    s.employeesList[employeeToSwap]->maxHalfHours -= shiftLengthPass;
                    shiftAssignments.at(shiftToPass) = employeeToSwap;
                    if(shiftToTake != -1){
                        s.employeesList[k]->minHalfHours -= shiftLengthTake;
                        s.employeesList[k]->maxHalfHours -= shiftLengthTake;
                        s.employeesList[employeeToSwap]->minHalfHours += shiftLengthTake;
                        s.employeesList[employeeToSwap]->maxHalfHours += shiftLengthTake;
                        shiftAssignments.at(shiftToTake) = k;
                    }
                    //change worksOnMatrix
                    worksOnMatrix->at(k).at(beginPass/48) = -1;
                    worksOnMatrix->at(employeeToSwap).at(beginPass/48) = shiftToPass;
                    if(shiftToTake != -1){
                        worksOnMatrix->at(k).at(beginTake/48) = shiftToTake;
                        worksOnMatrix->at(employeeToSwap).at(beginTake/48) = -1;
                    }
                }
            }
            else{
                //try to increase the number of hours
                for(int i=0; i<numShifts; i++){
                    int begin = shiftsToPass->at(i).at(0);
                    int end = shiftsToPass->at(i).at(1);
                    int employeeToTakeFrom = shiftAssignments.at(i);
                    if(employeeToTakeFrom != k){
                        //Not the employee's shift. See if we can take it
                        int shiftLength = end - begin;
                        int pointGain = gainFromRemoveShift(s.employeesList[employeeToTakeFrom]->minHalfHours, -s.employeesList[employeeToTakeFrom]->maxHalfHours, shiftLength, s.employeesList[employeeToTakeFrom]->priority);
                        if(!availabilityMatrix->at(employeeToTakeFrom).at(i)){
                            pointGain += UNAVAILABLE_PENALTY * s.employeesList[employeeToTakeFrom]->priority;
                        }
                            if(availabilityMatrix->at(k).at(i) && worksOnMatrix->at(k).at(begin/48)==-1 && canTake(employeeSchedule,k,begin,end)){
                                int nextPointGain = gainFromAssignShift(hoursUnder, hoursOver, shiftLength, priority);
                                int daysWorked = 0;
                                for(int a=0; a<7; a++){
                                    if(worksOnMatrix->at(k).at(a) != -1){
                                        daysWorked++;
                                    }
                                }
                                if(daysWorked >= 5){
                                    pointGain -= priority * MORE_THAN_FIVE_DAY_WEEK_PENALTY;
                                }
                                if(pointGain + nextPointGain > maxPointGain){
                                    shiftToPass = -1;
                                    shiftToTake = i;
                                    employeeToSwap = employeeToTakeFrom;
                                    maxPointGain = pointGain + nextPointGain;
                                }
                            }
                            //See if it's better to swap instead of just taking the shift
                            for(int a=0; a<7; a++){
                                int currentWorkingShift = worksOnMatrix->at(k).at(a);
                                if(currentWorkingShift >= 0 && availabilityMatrix->at(k).at(i)){
                                    int currentBegin = shiftsToPass->at(currentWorkingShift).at(0);
                                    int currentEnd = shiftsToPass->at(currentWorkingShift).at(1);
                                    bool alreadyShift = false;
                                    if(begin/48 != currentBegin/48){
                                        if(worksOnMatrix->at(shiftAssignments.at(i)).at(a) != -1 || worksOnMatrix->at(k).at(begin/48) != -1){
                                            alreadyShift = true;
                                        }
                                    }
                                    //shiftLengthPass
                                    if(!alreadyShift && availabilityMatrix->at(shiftAssignments.at(i)).at(currentWorkingShift) && canSwap(employeeSchedule,k,begin,end,employeeToTakeFrom,currentBegin,currentEnd)){
                                        int shiftLengthCurrent = shiftsToPass->at(currentWorkingShift).at(1) - shiftsToPass->at(currentWorkingShift).at(0);
                                        if(shiftLength > shiftLengthCurrent){
                                            int pointGain = gainFromAssignShift(hoursUnder, hoursOver, shiftLength - shiftLengthCurrent, priority) + gainFromRemoveShift(s.employeesList[employeeToTakeFrom]->minHalfHours, -s.employeesList[employeeToTakeFrom]->maxHalfHours, shiftLength - shiftLengthCurrent, s.employeesList[employeeToTakeFrom]->priority);
                                            if(pointGain > maxPointGain){
                                                shiftToPass = currentWorkingShift;
                                                employeeToSwap = shiftAssignments.at(i);
                                                shiftToTake = i;
                                                maxPointGain = pointGain;
                                            }
                                        }
                                    }
                                }
                            }
                    }
                }
                if(maxPointGain > 0){
                    //take the shift here
                    int beginPass;
                    int endPass;
                    int shiftLengthPass;
                    int beginTake = shiftsToPass->at(shiftToTake).at(0);
                    int endTake = shiftsToPass->at(shiftToTake).at(1);
                    int shiftLengthTake = endTake - beginTake;
                    if(shiftToPass != -1){
                        beginPass = shiftsToPass->at(shiftToPass).at(0);
                        endPass = shiftsToPass->at(shiftToPass).at(1);
                        shiftLengthPass = endPass - beginPass;
                    }
                    if(shiftToPass != -1){
                        for(int a = beginPass; a < endPass; a++){
                            employeeSchedule[k * 336 + a] = false;
                        }
                    }
                    for(int a = beginTake; a < endTake; a++){
                        employeeSchedule[employeeToSwap * 336 + a] = false;
                    }
                    if(shiftToPass != -1){
                        for(int a = beginPass; a < endPass; a++){
                            employeeSchedule[employeeToSwap * 336 + a] = true;
                        }
                    }
                    for(int a = beginTake; a < endTake; a++){
                        employeeSchedule[k * 336 + a] = true;
                    }
                    if(shiftToPass != -1){
                        s.employeesList[k]->minHalfHours += shiftLengthPass;
                        s.employeesList[k]->maxHalfHours += shiftLengthPass;
                        s.employeesList[employeeToSwap]->minHalfHours -= shiftLengthPass;
                        s.employeesList[employeeToSwap]->maxHalfHours -= shiftLengthPass;
                        shiftAssignments.at(shiftToPass) = employeeToSwap;
                    }
                    s.employeesList[k]->minHalfHours -= shiftLengthTake;
                    s.employeesList[k]->maxHalfHours -= shiftLengthTake;
                    s.employeesList[employeeToSwap]->minHalfHours += shiftLengthTake;
                    s.employeesList[employeeToSwap]->maxHalfHours += shiftLengthTake;
                    shiftAssignments.at(shiftToTake) = k;
                    //change worksOnMatrix
                    if(shiftToPass != -1){
                        worksOnMatrix->at(k).at(beginPass/48) = -1;
                        worksOnMatrix->at(employeeToSwap).at(beginPass/48) = shiftToPass;
                    }
                    worksOnMatrix->at(k).at(beginTake/48) = shiftToTake;
                    worksOnMatrix->at(employeeToSwap).at(beginTake/48) = -1;
                }
            }
            pointsGained += maxPointGain;
        }
    }

    //This is the old way to assign employees
    /*
    for(int i=0; i<336; i++){
        while(s.employeesRequired->at(i) > 0){

            //Find the max length of the shift
            int halfHoursToAssign = 1;
            //Find a decrease in the number of employees required and stop there
            while((i+halfHoursToAssign) < 336 && halfHoursToAssign < MAX_HALF_HOURS && s.employeesRequired->at(i+halfHoursToAssign) >= s.employeesRequired->at(i+halfHoursToAssign-1)){
                halfHoursToAssign++;
            }
            if(halfHoursToAssign < MIN_HALF_HOURS){
                //The assigned shift is shorter than the minimum allowed.
                scheduleToDisplay += "No solution found.\n";
                scheduleToDisplay.append(errorSchedule);
                scheduleToDisplay.append("Shift is too short at:\n");
                scheduleToDisplay.append(getDayTime(i,true).c_str());
                delete availabilityMatrix;
                delete mostHalfHoursPossibleMatrix;
                return scheduleToDisplay;
            }
            if(halfHoursToAssign == MAX_HALF_HOURS){
                int nextMaxLength = 1;
                //Maybe we should make this shift shorter. Find the next decrease
                //If 0 employees there will be no decrease, so skip
                if(s.employeesRequired->at(i+MAX_HALF_HOURS+nextMaxLength) > 0){
                    while((i+MAX_HALF_HOURS+nextMaxLength) < 336 && nextMaxLength < MIN_HALF_HOURS && s.employeesRequired->at(i+MAX_HALF_HOURS+nextMaxLength) >= s.employeesRequired->at(i+MAX_HALF_HOURS+nextMaxLength-1)){
                        nextMaxLength++;
                    }
                    if(nextMaxLength < MIN_HALF_HOURS){
                        halfHoursToAssign -= (MIN_HALF_HOURS - nextMaxLength);
                    }
                }
            }

            //Favor employees with a lower number of available half hours.
            int employeeToAssign = -1;
            //Score is hours left to schedule / remaining availability. Pick employees with higher scores
            //TODO: For longer shifts favor employees who must be scheduled for a lot of hours.
            //Should create shifts, then schedule them. Then no need to do this one by one.
            double currentEmployeeScore = 0.0;
            int currentMaxHalfHours = MIN_HALF_HOURS - 1;
            int maxHalfHoursToAssignEmployee = 1;
            int halfHoursToAssignEmployee = 0;
            //TODO: choice before this should be employee who must work today.
            //First choice: available for halfHoursToAssign and still must be assigned to meet minimum
            for(int j=0; j<numEmployees; j++){
                if(s.employeesList[j]->getAvailability(i)==1 && ((double)s.employeesList[j]->minHalfHours/(double)totalAvailabilities[j]) > currentEmployeeScore){
                    while(maxHalfHoursToAssignEmployee < halfHoursToAssign && s.employeesList[j]->getAvailability(i+maxHalfHoursToAssignEmployee)==1 && (i+maxHalfHoursToAssignEmployee) < 336){
                        maxHalfHoursToAssignEmployee++;
                    }
                    if(maxHalfHoursToAssignEmployee > s.employeesList[j]->maxHalfHours){
                        maxHalfHoursToAssignEmployee = s.employeesList[j]->maxHalfHours;
                    }
                    if(maxHalfHoursToAssignEmployee > (s.employeesList[j]->maxHalfHours - MIN_HALF_HOURS) && maxHalfHoursToAssignEmployee < s.employeesList[j]->minHalfHours){
                        maxHalfHoursToAssignEmployee = s.employeesList[j]->maxHalfHours - MIN_HALF_HOURS;
                    }
                    if(maxHalfHoursToAssignEmployee == halfHoursToAssign){
                        employeeToAssign = j;
                        halfHoursToAssignEmployee = halfHoursToAssign;
                        currentEmployeeScore = ((double)s.employeesList[j]->minHalfHours/(double)totalAvailabilities[j]);
                    }
                    maxHalfHoursToAssignEmployee = 0;
                }
            }
            currentEmployeeScore = 0.0;
            //Second choice: still must be assigned to meet minimum but available for less than halfHoursToAssign and at least the minimum
            //TODO: This is bad and creates problems because the next decrease is at the end of the halfHoursToAssign.
            //However, leftover minimum hours must be assigned somehow.
            if(employeeToAssign == -1){
                for(int j=0; j<numEmployees; j++){
                    if(s.employeesList[j]->getAvailability(i) && ((double)s.employeesList[j]->minHalfHours/(double)totalAvailabilities[j]) > currentEmployeeScore){
                        while(maxHalfHoursToAssignEmployee < halfHoursToAssign && s.employeesList[j]->getAvailability(i+maxHalfHoursToAssignEmployee)==1 && (i+maxHalfHoursToAssignEmployee) < 336){
                            maxHalfHoursToAssignEmployee++;
                        }
                        if(maxHalfHoursToAssignEmployee > s.employeesList[j]->maxHalfHours){
                            maxHalfHoursToAssignEmployee = s.employeesList[j]->maxHalfHours;
                        }
                        if(maxHalfHoursToAssignEmployee > (s.employeesList[j]->maxHalfHours - MIN_HALF_HOURS) && maxHalfHoursToAssignEmployee < s.employeesList[j]->minHalfHours){
                            maxHalfHoursToAssignEmployee = s.employeesList[j]->maxHalfHours - MIN_HALF_HOURS;
                        }
                        if(maxHalfHoursToAssignEmployee >= MIN_HALF_HOURS){
                            employeeToAssign = j;
                            halfHoursToAssignEmployee = maxHalfHoursToAssignEmployee;
                            currentEmployeeScore = ((double)s.employeesList[j]->minHalfHours/(double)totalAvailabilities[j]);
                        }
                        maxHalfHoursToAssignEmployee = 0;
                    }
                }
            }
            //Third choice: already fulfilled minimum but still less than maximum. Find the person the most hours can be assigned to
            //TODO: Bad for the same reason as above. Actually, assigning more hours will probably create more problems.
            if(employeeToAssign == -1){
                for(int j=0; j<numEmployees; j++){
                    if(s.employeesList[j]->getAvailability(i)==1){
                        while(maxHalfHoursToAssignEmployee < halfHoursToAssign && s.employeesList[j]->getAvailability(i+maxHalfHoursToAssignEmployee)==1 && (i+maxHalfHoursToAssignEmployee) < 336){
                            maxHalfHoursToAssignEmployee++;
                        }
                        if(maxHalfHoursToAssignEmployee > s.employeesList[j]->maxHalfHours){
                            maxHalfHoursToAssignEmployee = s.employeesList[j]->maxHalfHours;
                        }
                        if(maxHalfHoursToAssignEmployee > (s.employeesList[j]->maxHalfHours - MIN_HALF_HOURS) && maxHalfHoursToAssignEmployee < s.employeesList[j]->minHalfHours){
                            maxHalfHoursToAssignEmployee = s.employeesList[j]->maxHalfHours - MIN_HALF_HOURS;
                        }
                        if(maxHalfHoursToAssignEmployee > currentMaxHalfHours){
                            employeeToAssign = j;
                            halfHoursToAssignEmployee = maxHalfHoursToAssignEmployee;
                            currentMaxHalfHours = maxHalfHoursToAssignEmployee;
                        }
                        maxHalfHoursToAssignEmployee = 0;
                    }
                }
            }
            if(employeeToAssign == -1){
                //No employees were available.
                scheduleToDisplay += "No solution found.\n";
                scheduleToDisplay.append(errorSchedule);
                scheduleToDisplay.append("No employees available at:\n");
                scheduleToDisplay.append(getDayTime(i,true).c_str());
                delete availabilityMatrix;
                delete mostHalfHoursPossibleMatrix;
                return scheduleToDisplay;
                //TODO: Instead assign a low priority employee.
                //Might not need to do it here, but above.
            }
            else{
                //Assign the employee to the block.
                for(int j=i; j<i+halfHoursToAssignEmployee; j++){
                    s.employeesRequired->at(j) -= 1;
                    employeeSchedule[employeeToAssign*336+j] = true;
                }
                s.employeesList[employeeToAssign]->minHalfHours -= halfHoursToAssignEmployee;
                s.employeesList[employeeToAssign]->maxHalfHours -= halfHoursToAssignEmployee;

                //TODO: Avoid leaving minimum scheduled time for the employee lower than the minimum.
                //Swap shifts around based on priority.

                //For now, print what was assigned.
                //printf("%s %s %s\n", s.employeesList[employeeToAssign].name.c_str(), getDayTime(i, true).c_str(), getDayTime(i+halfHoursToAssignEmployee, true).c_str());

                //Make employee unavailable the rest of the day to avoid split shifts.
                int day = i/48;
                for(int j = i; j<day*48+48; j++){
                    if(s.employeesList[employeeToAssign]->getAvailability(j)==1){
                        s.employeesList[employeeToAssign]->setUnavailable(j,j);
                        totalAvailabilities[employeeToAssign] -= 1;
                    }
                }
                if((i+halfHoursToAssignEmployee) / 48 == i / 48){
                    errorSchedule.append(getDayTime(i, true).c_str());
                    errorSchedule.append("-");
                    errorSchedule.append(getDayTime(i + halfHoursToAssignEmployee,false).c_str());
                }
                else{
                    errorSchedule.append(getDayTime(i, true).c_str());
                    errorSchedule.append("-");
                    errorSchedule.append(getDayTime(i + halfHoursToAssignEmployee,true).c_str());
                }
                errorSchedule.append(" ");
                errorSchedule.append(s.employeesList[employeeToAssign]->lastName.c_str());
                errorSchedule.append(", ");
                errorSchedule.append(s.employeesList[employeeToAssign]->firstName.c_str());
                errorSchedule.append("\n");
            }
        }
        for(int j=0; j<numEmployees; j++){
            if(s.employeesList[j]->getAvailability(i)==1){
                totalAvailabilities[j] -= 1;
            }
        }
    }
    */

    //Print the schedule
    int totalHalfHours = 0;
    for(int j=0; j<numEmployees; j++){
        totalHalfHours = 0;
        scheduleToDisplay += s.employeesList[j]->lastName.c_str();
        scheduleToDisplay += ", ";
        scheduleToDisplay += s.employeesList[j]->firstName.c_str();
        scheduleToDisplay += "\n";
        for(int k=0; k<336; k++){
            if(employeeSchedule[j*336+k]){
                //begin shift
                int day = k/48;
                scheduleToDisplay += getDayTime(k, true).c_str();
                while(employeeSchedule[j*336+k] && k != 336){
                    k++;
                    totalHalfHours++;
                }
                if(k/48 == day){
                    scheduleToDisplay += "-";
                    scheduleToDisplay += getDayTime(k, false).c_str();
                    scheduleToDisplay += "\n";
                }
                else{
                    scheduleToDisplay += "-";
                    scheduleToDisplay += getDayTime(k, true).c_str();
                    scheduleToDisplay += "\n";
                }
                k--;
            }
        }
        scheduleToDisplay += "Total hours: ";
        scheduleToDisplay += QString::number(totalHalfHours * 1.0 / 2);
        /*
        if(totalHalfHours % 2 == 1){
            scheduleToDisplay += ".5";
        }
        */
        scheduleToDisplay += "\n";
        scheduleToDisplay += "\n";
    }
    scheduleToDisplay += "Availability violations:\n";
    bool violation = false;
    for(int i=0; i<numEmployees; i++){
        violation = false;
        for(int j=0; j<336; j++){
            if(employeeSchedule[i*336+j] && s.employeesList[i]->getAvailability(j)==0){
                if(!violation){
                    violation = true;
                    scheduleToDisplay += s.employeesList[i]->lastName.c_str();
                    scheduleToDisplay += ", ";
                    scheduleToDisplay += s.employeesList[i]->firstName.c_str();
                    scheduleToDisplay += "\n";
                }
                scheduleToDisplay += getDayTime(j, true).c_str();
                scheduleToDisplay += "-";
                int begin = j;
                while(employeeSchedule[i*336+j] && s.employeesList[i]->getAvailability(j)==0 && j<336){
                    j++;
                }
                if(j/48 == begin/48){
                    scheduleToDisplay += getDayTime(j, false).c_str();
                }
                else{
                    scheduleToDisplay += getDayTime(j, true).c_str();
                }
                scheduleToDisplay += "\n";
            }
        }
        if(violation){
            scheduleToDisplay += "\n";
        }
    }
    scheduleToPass = employeeSchedule;
    delete availabilityMatrix;
    delete mostHalfHoursPossibleMatrix;
    return scheduleToDisplay;
}

bool shiftSorter(std::array<int,2> firstShift, std::array<int,2> secondShift){
    if(firstShift.at(1)-firstShift.at(0) > secondShift.at(1)-secondShift.at(0)){
        return true;
    }
    return false;
}
