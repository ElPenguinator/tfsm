#include "guardedtransition.h"

using namespace std;


GuardedTransition::GuardedTransition(int src, string i, Guard g, string o, int tgt, int id) : IOTransition(src, i, o, tgt, id) {
    this->g = g;
}

GuardedTransition::GuardedTransition(const GuardedTransition &tr) : IOTransition(tr.src, tr.i, tr.o, tr.tgt, tr.id) {
    this->g = tr.g;
}

Guard GuardedTransition::getGuard() {
    return this->g;
}
