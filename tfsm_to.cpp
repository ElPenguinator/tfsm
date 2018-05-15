#include "tfsm_to.h"
#include <algorithm>
#include <iostream>
#include "structs.h"

using namespace std;

Transition::Transition(int src, string i, string o, int tgt, int id) {
    this->src = src;
    this->i = i;
    this->o = o;
    this->tgt = tgt;
    this->id = id;
}

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
}

void TFSM_TO::addTransitions(vector<Transition> transitions)
{
    this->transitions.insert(this->transitions.end(), transitions.begin(), transitions.end());
}

void TFSM_TO::addTimeouts(vector<Timeout> timeouts)
{
    this->timeouts.insert(this->timeouts.end(), timeouts.begin(), timeouts.end());
}

vector<Transition> TFSM_TO::getXi(int s, string i)
{
    vector<Transition> result;
    for (auto transition : this->transitions) {
        if (transition.src == s && transition.i == i)
            result.push_back(transition);
    }
    return result;
}

vector<Timeout> TFSM_TO::getXi(int s)
{
    vector<Timeout> result;
    for (auto timeout : this->timeouts) {
        if (timeout.src == s)
            result.push_back(timeout);
    }
    return result;
}

bool TFSM_TO::isIdTimeout(int id) {
    for (auto timeout : this->timeouts) {
        if (timeout.id == id)
            return true;
    }
    return false;
}

Transition TFSM_TO::getTransitionFromId(int id)
{
    for (auto transition : this->transitions) {
        if (transition.id == id)
            return transition;
    }
}

Timeout TFSM_TO::getTimeoutFromId(int id)
{
    for (auto timeout : this->timeouts) {
        if (timeout.id == id)
            return timeout;
    }
}

int TFSM_TO::getMaxDelta(int s)
{
    int res = 0;
    for (auto t : this->timeouts) {
        if (t.src == s) {
            if (t.t != inf) {
                if (t.t > res)
                    res = t.t;
            }
        }
    }
    return res;
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
