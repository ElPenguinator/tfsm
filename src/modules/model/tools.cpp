#include "tools.h"
#include <iostream>
#include <math.h>
#include <time.h>
#include <queue>
using namespace std;
using namespace CMSat;
void printPath(executingPath p)
{
    cout << "Path : ";
    for (int t : p) {
        cout << t << " -> ";
    }
    cout << endl;
}

void printClause(vector<Lit> clause)
{
    for (Lit l : clause) {
        if (l.sign())
            cout << "¬";
        cout << l.var() << " ∨";
    }
    cout << " ∧ " << endl;
}

bool isTFSMConnected(TFSM * S)
{
    queue<int> fifo;
    set<int> marked;
    fifo.push(S->initialState);
    marked.insert(S->initialState);
    while (!fifo.empty()) {
        int element = fifo.front();
        fifo.pop();
        for (auto transition : S->transitions) {
            if (transition->src == element && marked.find(transition->tgt) == marked.end()) {
                fifo.push(transition->tgt);
                marked.insert(transition->tgt);
            }
        }
        for (auto timeout : S->timeouts) {
            if (timeout->src == element && marked.find(timeout->tgt) == marked.end()) {
                fifo.push(timeout->tgt);
                marked.insert(timeout->tgt);
            }
        }
    }

    return marked.size() == S->states.size();
}

bool isTFSMConnected(TFSM_TO * S)
{
    queue<int> fifo;
    set<int> marked;
    fifo.push(S->initialState);
    marked.insert(S->initialState);
    while (!fifo.empty()) {
        int element = fifo.front();
        fifo.pop();
        for (auto transition : S->transitions) {
            if (transition->src == element && marked.find(transition->tgt) == marked.end()) {
                fifo.push(transition->tgt);
                marked.insert(transition->tgt);
            }
        }
        for (auto timeout : S->timeouts) {
            if (timeout->src == element && marked.find(timeout->tgt) == marked.end()) {
                fifo.push(timeout->tgt);
                marked.insert(timeout->tgt);
            }
        }
    }

    return marked.size() == S->states.size();
}

bool guardedTransitionAlreadyExist(IOTransition * newTransition, vector<IOTransition *> lambda, vector<IOTransition *> newLambda)
{
    for (IOTransition * transition : lambda) {
        if (transition->src == newTransition->src
                && transition->i == newTransition->i
                && transition->o == newTransition->o
                && transition->tgt == newTransition->tgt
                && !transition->getGuard().equals(newTransition->getGuard()))
            return true;
    }
    for (IOTransition * transition : newLambda) {
        if (transition->src == newTransition->src
                && transition->i == newTransition->i
                && transition->o == newTransition->o
                && transition->tgt == newTransition->tgt
                && !transition->getGuard().equals(newTransition->getGuard()))
            return true;
    }
    return false;
}

bool transitionAlreadyExist(IOTransition * newTransition, vector<IOTransition *> lambda, vector<IOTransition *> newLambda)
{
    for (IOTransition * transition : lambda) {
        if (transition->src == newTransition->src
                && transition->i == newTransition->i
                && transition->o == newTransition->o
                && transition->tgt == newTransition->tgt)
            return true;
    }
    for (IOTransition * transition : newLambda) {
        if (transition->src == newTransition->src
                && transition->i == newTransition->i
                && transition->o == newTransition->o
                && transition->tgt == newTransition->tgt)
            return true;
    }
    return false;
}

bool timeoutAlreadyExist(TimeoutTransition * newTimeout, vector<TimeoutTransition *> delta, vector<TimeoutTransition *> newDelta)
{
    for (TimeoutTransition * timeout : delta) {
        if (timeout->src == newTimeout->src
                && timeout->t == newTimeout->t
                && timeout->tgt == newTimeout->tgt)
            return true;
    }
    for (TimeoutTransition * timeout : newDelta) {
        if (timeout->src == newTimeout->src
                && timeout->t == newTimeout->t
                && timeout->tgt == newTimeout->tgt)
            return true;
    }
    return false;
}

string getRandomStringFromSet(set<string> samples)
{
    set<string>::const_iterator it(samples.begin());
    int index = floor(rand() % samples.size());
    advance(it,index);
    return (*it);
}

TFSM_TO * generateChaosMachine(TFSM_TO * S, int maxTime)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<IOTransition *> lambda(S->transitions);
    vector<TimeoutTransition *> delta(S->timeouts);
    TFSM_TO * M = new TFSM_TO(States, s0, I, O, lambda, delta);

    srand (time(NULL));
    vector<IOTransition *> newLambda;
    vector<TimeoutTransition *> newDelta;

    int newId = (S->transitions.size() + S->timeouts.size());

    for (int src : States) {
        for (int tgt : States) {
            for (string i : I) {
                for (string o : O) {
                    IOTransition * newTransition = new IOTransition(src,i, o, tgt, newId);
                    if (!transitionAlreadyExist(newTransition, lambda, newLambda)) {
                        newLambda.push_back(newTransition);
                        newId++;
                    }
                }
            }
            for (int t = 0; t <= maxTime; t++) {
                int newT = t;
                if (newT == 0) {
                    newT = inf;
                }
                TimeoutTransition * newTimeout = new TimeoutTransition(src,newT, tgt, newId);
                if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                    newDelta.push_back(newTimeout);
                    newId++;
                }
            }
        }
    }
    M->addTransitions(newLambda, true);
    M->addTimeouts(newDelta, true);
    return M;
}
