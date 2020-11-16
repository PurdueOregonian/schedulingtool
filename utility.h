#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>

std::string getDayTime(int, bool);

int getSumOfLongest(int[7], bool[7]);

int gainFromAssignShift(int, int, int, int);

int gainFromRemoveShift(int, int, int, int);

bool alreadyShiftGapRestriction(std::vector<bool>, int, int, int, int, int);

bool gapRestriction(std::vector<bool>, int, int, int);

bool canTake(std::vector<bool>, int, int, int);

bool canSwap(std::vector<bool>, int, int, int, int, int, int);

#endif // UTILITY_H
