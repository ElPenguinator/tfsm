#include "tools.h"
#include <iostream>
#include <math.h>
#include <time.h>
#include <queue>
using namespace std;
using namespace CMSat;
void printPath(path p)
{
    cout << "Path : ";
    for (int t : p) {
        cout << t << " -> ";
    }
    cout << endl;
}

void printSequence(sequence s)
{
    cout << "Sequence : ";
    for (ts timeState : s) {
        cout << "(" << timeState.first << "," << timeState.second << ")";
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

TFSM * generateRandomMutationMachine(TFSM * S, int maxTime, int numberOfMutations)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<IOTransition *> lambda(S->transitions);
    vector<TimeoutTransition *> delta(S->timeouts);
    TFSM * M = new TFSM(States, s0, I, O, lambda, delta);

    srand (time(NULL));
    vector<IOTransition *> newLambda;
    vector<TimeoutTransition *> newDelta;
    int i = 0 + (S->transitions.size() + S->timeouts.size());
    S->print();
    cout << "Size : " <<  i << " " << S->transitions.size() << " " << S->timeouts.size() << endl;
    while (i<numberOfMutations + (S->transitions.size() + S->timeouts.size())) {
        bool alreadyExisting = true;
        bool createTimeout = rand()%2;
        int randomSrc = floor(rand() % S->states.size());
        int randomTgt = floor(rand() % S->states.size());
        if (createTimeout) {
            int randomT = floor(rand() % maxTime);
            if (randomT == 0) {
                randomT = inf;
                randomTgt = randomSrc;
            }
            TimeoutTransition * newTimeout = new TimeoutTransition(randomSrc,randomT, randomTgt, i);
            if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                newDelta.push_back(newTimeout);
                i++;
            }
        }
        else {
            int nbPartition = 1 + floor(rand() % (maxTime-1));
            int lastMax = 0;
            cout << "nbPartition : " << nbPartition << " of : " << maxTime << endl;
            string randomI = getRandomStringFromSet(I);
            for (int j=0; j<nbPartition; j++) {
                cout << "!!" << endl;
                int tMin = round(lastMax);
                int tMax = round((maxTime/nbPartition)*(j+1));
                if (tMax == tMin)
                    tMax++;
                if (j == nbPartition-1)
                    tMax = inf;
                lastMax = tMax;
                int randomTgt = floor(rand() % S->states.size());
                string randomO = getRandomStringFromSet(O);
                Guard g("[", tMin, tMax, ")");
                cout << "- " << g.toString() << endl;
                GuardedTransition * newTransition = new GuardedTransition(randomSrc, randomI, g, randomO, randomTgt, i);
                lambda.push_back(newTransition);

                if (!guardedTransitionAlreadyExist(newTransition, lambda, newLambda)) {
                    newLambda.push_back(newTransition);
                    i++;
                }

            }
        }
    }
    cout << "New Transitions : {";
    for (IOTransition * t : newLambda) {
        cout << "(" << t->src << "," << t->i << "," << t->getGuard().toString() <<"," << t->o << "," << t->tgt << ") : " << t->id << "," << endl;
    }
    cout << "}" << endl;
    cout << "New Timeouts : {";
    for (auto t : newDelta) {
        if (t->t != inf) {
            cout << "(" << t->src << "," << t->t << "," << t->tgt << ") : " << t->id << "," << endl;
        }
        else {
            cout << "(" << t->src << "," << "INF" << "," << t->tgt << ") : " << t->id << "," << endl;
        }
    }
    cout << "}" << endl;
    M->addTransitions(newLambda);
    M->addTimeouts(newDelta);
    return M;
}

TFSM_TO * generateRandomMutationMachine_TO(TFSM_TO * S, int maxTime, int numberOfMutations)
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
    for (int i=0; i<numberOfMutations; i++) {
        bool alreadyExisting = true;
        while (alreadyExisting) {
            bool createTimeout = rand()%2;
            int randomSrc = floor(rand() % S->states.size());
            int randomTgt = floor(rand() % S->states.size());
            int newId = i + (S->transitions.size() + S->timeouts.size());
            if (createTimeout) {
                int randomT = floor(rand() % (maxTime+1));
                if (randomT == 0) {
                    randomT = inf;
                    randomTgt = randomSrc;
                }
                TimeoutTransition  * newTimeout = new TimeoutTransition(randomSrc,randomT, randomTgt, newId);
                if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                    newDelta.push_back(newTimeout);
                    alreadyExisting = false;
                }
            }
            else {
                string randomI = getRandomStringFromSet(I);
                string randomO = getRandomStringFromSet(O);
                IOTransition * newTransition = new IOTransition(randomSrc,randomI, randomO, randomTgt, newId);
                if (!transitionAlreadyExist(newTransition, lambda, newLambda)) {
                    newLambda.push_back(newTransition);
                    alreadyExisting = false;
                }
            }
        }
    }
    M->addTransitions(newLambda);
    M->addTimeouts(newDelta);
    return M;
}


TFSM * generateRandomSpecification(int nbOfStates, int maxTime, set<string> I, set<string> O)
{
    set<int> S;
    int s0 = 0;
    vector<IOTransition *> lambda;
    vector<TimeoutTransition *> delta;
    int transitionId;
    srand (time(NULL));
    bool isConnected = false;

    TFSM * res;

    while (!isConnected) {
        transitionId = 0;
        S.clear();
        lambda.clear();
        delta.clear();
        for (int s=0; s<nbOfStates; s++) {
            S.insert(s);

            int randomT = floor(rand() % maxTime);
            int randomTgt = floor(rand() % nbOfStates);
            if (randomT == 0) {
                randomT = inf;
                randomTgt = s;
            }
            delta.push_back(new TimeoutTransition(s, randomT, randomTgt, transitionId));
            transitionId++;
            if (randomT == inf)
                randomT = maxTime;
            for (string i : I) {
                int nbPartition = 1;
                if (randomT > 1)
                    nbPartition = 1 + floor(rand() % (randomT-1));
                int lastMax = 0;
                cout << "nbPartition : " << nbPartition << " of : " << randomT << endl;
                for (int j=0; j<nbPartition; j++) {
                    cout << "!!" << endl;
                    int tMin = round(lastMax);
                    int tMax = round((randomT/nbPartition)*(j+1));
                    if (tMax == tMin)
                        tMax++;
                    if (j == nbPartition-1)
                        tMax = inf;
                    lastMax = tMax;
                    string randomO = getRandomStringFromSet(O);
                    int randomTgt = floor(rand() % nbOfStates);
                    Guard g("[", tMin, tMax, ")");
                    cout << "- " << g.toString() << endl;
                    lambda.push_back(new GuardedTransition(s, i, g, randomO, randomTgt, transitionId));
                    transitionId++;
                }
            }
        }
        res = new TFSM(S, s0, I, O, lambda, delta);
        isConnected = isTFSMConnected(res);
        if (!isConnected)
            delete res;
    }
    return res;
}



TFSM_TO * generateRandomSpecification_TO(int nbOfStates, int maxTime, set<string> I, set<string> O)
{
    set<int> S;
    int s0 = 0;
    vector<IOTransition *> lambda;
    vector<TimeoutTransition *> delta;
    int transitionId;
    srand (time(NULL));
    bool isConnected = false;

    TFSM_TO * res;

    while (!isConnected) {
        transitionId = 0;
        S.clear();
        lambda.clear();
        delta.clear();
        for (int s=0; s<nbOfStates; s++) {
            S.insert(s);
            for (string i : I) {
                string randomO = getRandomStringFromSet(O);
                int randomTgt = floor(rand() % nbOfStates);

                lambda.push_back(new IOTransition(s, i, randomO, randomTgt, transitionId));
                transitionId++;
            }

            int randomT = floor(rand() % maxTime);
            int randomTgt = floor(rand() % nbOfStates);
            if (randomT == 0) {
                randomT = inf;
                randomTgt = s;
            }
            delta.push_back(new TimeoutTransition(s, randomT, randomTgt, transitionId));
            transitionId++;
        }
        res = new TFSM_TO(S, s0, I, O, lambda, delta);
        isConnected = isTFSMConnected(res);
        if (!isConnected)
            delete res;
    }
    return res;
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
    M->addTransitions(newLambda);
    M->addTimeouts(newDelta);
    return M;
}
