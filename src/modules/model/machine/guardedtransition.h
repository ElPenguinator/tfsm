#ifndef GUARDEDTRANSITION_H
#define GUARDEDTRANSITION_H
#include "iotransition.h"
#include "guard.h"

class GuardedTransition : public IOTransition{
public:
    Guard g;
    GuardedTransition(int src, std::string i, Guard g, std::string o, int tgt, int id);
    GuardedTransition(const GuardedTransition & tr);
    Guard getGuard();
};

#endif // GUARDEDTRANSITION_H
