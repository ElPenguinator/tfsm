#include "iotransition.h"
#include "../structs.h"
using namespace std;


IOTransition::IOTransition(int src, string i, string o, int tgt, int id) : Transition(src, tgt, id) {
    this->i = i;
    this->o = o;
}

IOTransition::IOTransition(const IOTransition &tr) : Transition(tr.src, tr.tgt, tr.id) {
    this->i = tr.i;
    this->o = tr.o;
}

Guard IOTransition::getGuard() {
    return Guard("[", 0, inf, ")");
}
