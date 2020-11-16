#include <string>
#include <algorithm>
#include "utility.h"

#define GAP_BETWEEN_SHIFTS 16

std::string getDayTime(int halfHour, bool getDay){
    std::string dayTime;
    int day = halfHour/48;
    int time = halfHour % 48;
    if(getDay){
        if(day==0){
            dayTime.append("Sun");
        }
        if(day==1){
            dayTime.append("Mon");
        }
        if(day==2){
            dayTime.append("Tue");
        }
        if(day==3){
            dayTime.append("Wed");
        }
        if(day==4){
            dayTime.append("Thu");
        }
        if(day==5){
            dayTime.append("Fri");
        }
        if(day==6){
            dayTime.append("Sat");
        }
        if(day==7){
            dayTime.append("Sun");
        }
        dayTime.append(" ");
    }
    dayTime.append(std::to_string(((time/2 - 1) + 12) % 12 + 1));
    if(time % 2 == 0){
        dayTime.append(":00");
    }
    else{
        dayTime.append(":30");
    }
    if(time / 2 < 12){
        dayTime.append("am");
    }
    else{
        dayTime.append("pm");
    }
    return dayTime;
}

int getSumOfLongest(int longestShiftsPossible[7], bool worksOn[7]){
    int sumOfLongest = 0;
    int smallest = 336;
    int secondSmallest = 336;
    int days = 0;
    for(int i=0; i<7; i++){
        if(!worksOn[i]){
            if(longestShiftsPossible[i] < smallest){
                smallest = longestShiftsPossible[i];
            }
            else if(longestShiftsPossible[i] < secondSmallest){
                secondSmallest = longestShiftsPossible[i];
            }
            sumOfLongest += longestShiftsPossible[i];
            days++;
        }
    }
    if(days == 6){
        sumOfLongest -= smallest;
    }
    if(days == 7){
        sumOfLongest -= smallest;
        sumOfLongest -= secondSmallest;
    }
    return sumOfLongest;
}

int gainFromAssignShift(int halfHoursUnder, int halfHoursOver, int shiftLength, int priority){
    if(halfHoursOver > 0){
        return -shiftLength * priority;
    }
    else if (halfHoursUnder > shiftLength){
        return shiftLength * priority;
    }
    else if(halfHoursUnder > 0 && (shiftLength + halfHoursOver) < 0){
        return halfHoursUnder * priority;
    }
    else if(halfHoursUnder <= 0 && (shiftLength + halfHoursOver) < 0){
        return 0;
    }
    else if(halfHoursUnder <= 0){
        return (shiftLength + halfHoursOver) * priority * -1;
    }
    else{
        return (halfHoursUnder + (shiftLength + halfHoursOver) * -1) * priority;
    }
}

int gainFromRemoveShift(int halfHoursUnder, int halfHoursOver, int shiftLength, int priority){
    if(halfHoursUnder > 0){
        return -shiftLength * priority;
    }
    else if (halfHoursOver > shiftLength){
        return shiftLength * priority;
    }
    else if(halfHoursOver > 0 && (shiftLength + halfHoursUnder) < 0){
        return halfHoursOver * priority;
    }
    else if(halfHoursOver <= 0 && (shiftLength + halfHoursUnder) < 0){
        return 0;
    }
    else if(halfHoursOver <= 0){
        return (shiftLength + halfHoursUnder) * priority * -1;
    }
    else{
        return (halfHoursOver + (shiftLength + halfHoursUnder) * -1) * priority;
    }
}

bool alreadyShiftGapRestriction(std::vector<bool> employeeSchedule, int employeeNumber, int begin, int end, int alreadyBegin, int alreadyEnd){
    for(int i=std::max(0,begin-GAP_BETWEEN_SHIFTS); i<begin; i++){
        if(i<alreadyBegin && i>alreadyEnd && employeeSchedule[employeeNumber*336+i]){
            return true;
        }
    }
    for(int i=end; i<std::min(336,end+GAP_BETWEEN_SHIFTS); i++){
        if(i<alreadyBegin && i>alreadyEnd && employeeSchedule[employeeNumber*336+i]){
            return true;
        }
    }
    return false;
}

bool gapRestriction(std::vector<bool> employeeSchedule, int employeeNumber, int begin, int end){
    for(int i=std::max(0,begin-GAP_BETWEEN_SHIFTS); i<begin; i++){
        if(employeeSchedule[employeeNumber*336+i]){
            return true;
        }
    }
    for(int i=end; i<std::min(336,end+GAP_BETWEEN_SHIFTS); i++){
        if(employeeSchedule[employeeNumber*336+i]){
            return true;
        }
    }
    return false;
}

bool canTake(std::vector<bool> employeeSchedule, int employeeNumber, int begin, int end){
    int start = std::max(begin-GAP_BETWEEN_SHIFTS,0);
    int finish = std::min(end+GAP_BETWEEN_SHIFTS,336);
    for(int i=start; i<finish; i++){
        if(employeeSchedule[employeeNumber*336+i]){
            return false;
        }
    }
    return true;
}

bool canSwap(std::vector<bool> employeeSchedule, int employeeNumber, int begin, int end, int secondEmployeeNumber, int secondBegin, int secondEnd){
    int start = std::max(begin-GAP_BETWEEN_SHIFTS,0);
    int finish = std::min(end+GAP_BETWEEN_SHIFTS,336);
    for(int i=start; i<finish; i++){
        if(i < secondBegin || i > secondEnd){
            if(employeeSchedule[employeeNumber*336+i]){
                return false;
            }
        }
    }
    start = std::max(secondBegin-GAP_BETWEEN_SHIFTS,0);
    finish = std::min(secondEnd+GAP_BETWEEN_SHIFTS,336);
    for(int i=start; i<finish; i++){
        if(i < begin || i > end){
            if(employeeSchedule[secondEmployeeNumber*336+i]){
                return false;
            }
        }
    }
    return true;
}
