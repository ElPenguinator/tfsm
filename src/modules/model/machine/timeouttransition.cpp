#include "timeouttransition.h"

TimeoutTransition::TimeoutTransition(int src, int t, int tgt, int id) : Transition(src, tgt, id) {
    this->t = t;
}

TimeoutTransition::TimeoutTransition(const TimeoutTransition &tr) : Transition(tr.src, tr.tgt, tr.id) {
    this->t = tr.t;
}
