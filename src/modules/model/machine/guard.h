#ifndef GUARD_H
#define GUARD_H
#include <string>
//#include "../structs.h"

enum Bracket { Square, Curly };

class Guard {
public:
    Bracket left;
    int tmin;
    int tmax;
    Bracket right;
    Guard();
    Guard(std::string left, int tmin, int tmax, std::string right);
    Guard(Bracket left, int tmin, int tmax, Bracket right);
    std::string toString();
    bool isIntersectionEmpty(Guard other);
    Guard intersect(Guard other);
    bool contains(double x);
    bool equals(Guard other);
    Guard substracted(int counter);
    Guard substracted(Guard other);
    Guard added(int counter);
    Guard added(Guard other);
};

#endif // GUARD_H
