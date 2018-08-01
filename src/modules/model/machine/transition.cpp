#include "transition.h"
#include <iostream>
using namespace std;

Transition::Transition(int src, int tgt, int id) {
    this->src = src;
    this->tgt = tgt;
    this->id = id;
}
