#ifndef TIMEOUTTRANSITION_H
#define TIMEOUTTRANSITION_H
#include "transition.h"

class TimeoutTransition : public Transition{
public:
    int t;
    TimeoutTransition(int src, int t, int tgt, int id);
    TimeoutTransition(const TimeoutTransition & tr);
};

#endif // TIMEOUTTRANSITION_H
