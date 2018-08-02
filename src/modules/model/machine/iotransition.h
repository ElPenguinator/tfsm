#ifndef IOTRANSITION_H
#define IOTRANSITION_H
#include "transition.h"
#include <string>
#include "guard.h"

class IOTransition : public Transition {
public:
    std::string i;
    std::string o;
    IOTransition(int src, std::string i, std::string o, int tgt, int id);
    IOTransition(const IOTransition & tr);
    virtual Guard getGuard();
};

#endif // IOTRANSITION_H
