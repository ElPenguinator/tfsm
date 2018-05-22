#include "tfsm_to.h"
#include <algorithm>
#include <iostream>
#include "structs.h"

using namespace std;

Timeout::Timeout(int src, int t, int tgt, int id) {
    this->src = src;
    this->t = t;
    this->tgt = tgt;
    this->id = id;
}

TFSM_TO::TFSM_TO(set<int> S, int s0, set<string> I, set<string> O, vector<Transition> lambda, vector<Timeout> delta)
{
    this->states = S;
    this->initialState = s0;
    this->inputs = I;
    this->outputs = O;
    this->transitions = lambda;
    this->timeouts = delta;
    this->computeMaps();
}

void TFSM_TO::addTransitions(vector<Transition> transitions)
{
    this->transitions.insert(this->transitions.end(), transitions.begin(), transitions.end());
    this->computeMaps();
}

void TFSM_TO::addTimeouts(vector<Timeout> timeouts)
{
    this->timeouts.insert(this->timeouts.end(), timeouts.begin(), timeouts.end());
    this->computeMaps();
}

vector<Transition> TFSM_TO::getXi(int s, string i)
{
    vector<Transition> result;
    for (auto transition : this->lambda(s)) {
        if (transition.i == i)
            result.push_back(transition);
    }
    return result;
}

vector<Timeout> TFSM_TO::getXi(int s)
{
    return this->delta(s);
}

vector<Transition> TFSM_TO::lambda(int s)
{
    return this->transitionsPerState.find(s)->second;
}

vector<Timeout> TFSM_TO::delta(int s)
{
    return this->timeoutsPerState.find(s)->second;
}

bool TFSM_TO::isIdTimeout(int id) {
    return this->timeoutIdMap.find(id) != this->timeoutIdMap.end();
}

Transition TFSM_TO::getTransitionFromId(int id)
{
    return this->transitionIdMap.find(id)->second;
}

Timeout TFSM_TO::getTimeoutFromId(int id)
{
    return this->timeoutIdMap.find(id)->second;
}

int TFSM_TO::getMaxDelta(int s)
{
    int res = 0;
    for (auto t : this->delta(s)) {
        if (t.t != inf) {
            if (t.t > res)
                res = t.t;
        }
    }
    return res;
}

void TFSM_TO::computeMaps()
{
    this->transitionsPerState.clear();
    this->timeoutsPerState.clear();
    this->transitionIdMap.clear();
    this->timeoutIdMap.clear();
    for (int s : this->states) {
        this->transitionsPerState.insert(make_pair(s, vector<Transition>()));
        this->timeoutsPerState.insert(make_pair(s, vector<Timeout>()));
    }

    for (Transition t : this->transitions) {
        this->transitionIdMap.insert(make_pair(t.id, t));
        this->transitionsPerState.find(t.src)->second.push_back(t);
    }

    for (Timeout t : this->timeouts) {
        this->timeoutIdMap.insert(make_pair(t.id, t));
        this->timeoutsPerState.find(t.src)->second.push_back(t);
    }
}

void TFSM_TO::print()
{
    cout << "States : {";
    for (auto s : this->states) {
        cout << s << ", ";
    }
    cout << "}" << endl;
    cout << "Transitions : {";
    for (auto t : this->transitions) {
        cout << "(" << t.src << "," << t.i << "," << t.o << "," << t.tgt << ") : " << t.id << "," << endl;
    }
    cout << "}" << endl;
    cout << "Timeouts : {";
    for (auto t : this->timeouts) {
        if (t.t != inf) {
            cout << "(" << t.src << "," << t.t << "," << t.tgt << ") : " << t.id << "," << endl;
        }
        else {
            cout << "(" << t.src << "," << "INF" << "," << t.tgt << ") : " << t.id << "," << endl;
        }
    }
    cout << "}" << endl;
}
