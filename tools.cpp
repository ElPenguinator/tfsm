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
            if (transition.src == element && marked.find(transition.tgt) == marked.end()) {
                fifo.push(transition.tgt);
                marked.insert(transition.tgt);
            }
        }
        for (auto timeout : S->timeouts) {
            if (timeout.src == element && marked.find(timeout.tgt) == marked.end()) {
                fifo.push(timeout.tgt);
                marked.insert(timeout.tgt);
            }
        }
    }

    return marked.size() == S->states.size();
}

bool transitionAlreadyExist(Transition newTransition, vector<Transition> lambda, vector<Transition> newLambda)
{
    for (Transition transition : lambda) {
        if (transition.src == newTransition.src
                && transition.i == newTransition.i
                && transition.o == newTransition.o
                && transition.tgt == newTransition.tgt)
            return true;
    }
    for (Transition transition : newLambda) {
        if (transition.src == newTransition.src
                && transition.i == newTransition.i
                && transition.o == newTransition.o
                && transition.tgt == newTransition.tgt)
            return true;
    }
    return false;
}

bool timeoutAlreadyExist(Timeout newTimeout, vector<Timeout> delta, vector<Timeout> newDelta)
{
    for (Timeout timeout : delta) {
        if (timeout.src == newTimeout.src
                && timeout.t == newTimeout.t
                && timeout.tgt == newTimeout.tgt)
            return true;
    }
    for (Timeout timeout : newDelta) {
        if (timeout.src == newTimeout.src
                && timeout.t == newTimeout.t
                && timeout.tgt == newTimeout.tgt)
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

TFSM_TO * generateRandomMutationMachine(TFSM_TO * S, int maxTime, int numberOfMutations)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<Transition> lambda(S->transitions);
    vector<Timeout> delta(S->timeouts);
    TFSM_TO * M = new TFSM_TO(States, s0, I, O, lambda, delta);

    srand (time(NULL));
    vector<Transition> newLambda;
    vector<Timeout> newDelta;
    for (int i=0; i<numberOfMutations; i++) {
        bool alreadyExisting = true;
        while (alreadyExisting) {
            bool createTimeout = rand()%2;
            int randomSrc = floor(rand() % S->states.size());
            int randomTgt = floor(rand() % S->states.size());
            int newId = i + (S->transitions.size() + S->timeouts.size());
            if (createTimeout) {
                int randomT = floor(rand() % maxTime);
                if (randomT == 0) {
                    randomT = inf;
                    randomTgt = randomSrc;
                }
                Timeout newTimeout(randomSrc,randomT, randomTgt, newId);
                if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                    cout << i << endl;
                    newDelta.push_back(newTimeout);
                    alreadyExisting = false;
                }
            }
            else {
                string randomI = getRandomStringFromSet(I);
                string randomO = getRandomStringFromSet(O);
                Transition newTransition(randomSrc,randomI, randomO, randomTgt, newId);
                if (!transitionAlreadyExist(newTransition, lambda, newLambda)) {
                    cout << i << endl;
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



TFSM_TO * generateRandomSpecification(int nbOfStates, int maxTime, set<string> I, set<string> O)
{
    set<int> S;
    int s0 = 0;
    vector<Transition> lambda;
    vector<Timeout> delta;
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

                lambda.push_back(Transition(s, i, randomO, randomTgt, transitionId));
                transitionId++;
            }

            int randomT = floor(rand() % maxTime);
            int randomTgt = floor(rand() % nbOfStates);
            if (randomT == 0) {
                randomT = inf;
                randomTgt = s;
            }
            delta.push_back(Timeout(s, randomT, randomTgt, transitionId));
            transitionId++;
        }
        res = new TFSM_TO(S, s0, I, O, lambda, delta);
        isConnected = isTFSMConnected(res);
        if (!isConnected)
            delete res;
    }
    return res;
}

/*
TFSM_TO * generateRandomMutationMachine(TFSM_TO * S, int maxTime, int numberOfMutationsTransition, int numberOfMutationsTimeout)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<Transition> lambda(S->transitions);
    vector<Timeout> delta(S->timeouts);
    TFSM_TO * M = new TFSM_TO(States, s0, I, O, lambda, delta);

    srand (time(NULL));
    vector<Transition> newLambda;
    vector<Timeout> newDelta;
    for (int i=0; i<numberOfMutationsTransition; i++) {
        bool alreadyExisting = true;
        while (alreadyExisting) {
            int randomSrc = floor(rand() % S->states.size());
            int randomTgt = floor(rand() % S->states.size());
            int newId = i + (S->transitions.size() + S->timeouts.size());
            string randomI = getRandomStringFromSet(I);
            string randomO = getRandomStringFromSet(O);
            Transition newTransition(randomSrc,randomI, randomO, randomTgt, newId);
            if (!transitionAlreadyExist(newTransition, lambda, newLambda)) {
                cout << i << endl;
                newLambda.push_back(newTransition);
                alreadyExisting = false;
            }
        }
    }
    for (int i=0; i<numberOfMutationsTimeout; i++) {
        bool alreadyExisting = true;
        while (alreadyExisting) {
            int randomSrc = floor(rand() % S->states.size());
            int randomTgt = floor(rand() % S->states.size());
            int newId = numberOfMutationsTransition + i + (S->transitions.size() + S->timeouts.size());
            int randomT = floor(rand() % maxTime);
            if (randomT == 0) {
                randomT = inf;
                randomTgt = randomSrc;
            }
            Timeout newTimeout(randomSrc,randomT, randomTgt, newId);
            if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                cout << i << endl;
                newDelta.push_back(newTimeout);
                alreadyExisting = false;
            }
        }
    }
    M->addTransitions(newLambda);
    M->addTimeouts(newDelta);
    return M;
}
*/
