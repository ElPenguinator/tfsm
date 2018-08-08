#include "tfsm_to.h"
#include <algorithm>
#include <iostream>
#include "../structs.h"
#include <sstream>
using namespace std;

TFSM_TO::TFSM_TO(set<int> S, int s0, set<string> I, set<string> O, vector<IOTransition *> lambda, vector<TimeoutTransition *> delta)
    : FSM(S, s0, I, O, lambda)
{
    this->timeouts = delta;
    this->computeMaps();
}

void TFSM_TO::addTransitions(vector<IOTransition *> transitions, bool isMutated)
{
    FSM::addTransitions(transitions, isMutated);
    this->computeMaps();
}

void TFSM_TO::addTimeouts(vector<TimeoutTransition *> timeouts, bool isMutated)
{
    this->timeouts.insert(this->timeouts.end(), timeouts.begin(), timeouts.end());
    this->computeMaps();
}

vector<IOTransition *> TFSM_TO::getXi(int s, string i)
{
    return FSM::getXi(s, i);
}

vector<TimeoutTransition *> TFSM_TO::getXi(int s)
{
    return this->delta(s);
}

std::vector<IOTransition *> TFSM_TO::lambda(int s)
{
    return FSM::lambda(s);
}

vector<TimeoutTransition *> TFSM_TO::delta(int s)
{
    return this->timeoutsPerState.find(s)->second;
}

bool TFSM_TO::isIdTimeout(int id) {
    return this->timeoutIdMap.find(id) != this->timeoutIdMap.end();
}

IOTransition * TFSM_TO::getTransitionFromId(int id)
{
    return FSM::getTransitionFromId(id);
}

TimeoutTransition * TFSM_TO::getTimeoutFromId(int id)
{
    return this->timeoutIdMap.find(id)->second;
}

int TFSM_TO::getMaxDelta(int s)
{
    int res = 0;
    for (auto t : this->delta(s)) {
        if (t->t != inf) {
            if (t->t > res)
                res = t->t;
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
        this->transitionsPerState.insert(make_pair(s, vector<IOTransition *>()));
        this->timeoutsPerState.insert(make_pair(s, vector<TimeoutTransition *>()));
    }

    for (IOTransition * t : this->transitions) {
        this->transitionIdMap.insert(make_pair(t->id, t));
        this->transitionsPerState.find(t->src)->second.push_back(t);
    }

    for (TimeoutTransition * t : this->timeouts) {
        this->timeoutIdMap.insert(make_pair(t->id, t));
        this->timeoutsPerState.find(t->src)->second.push_back(t);
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
        cout << "(" << t->src << "," << t->i << "," << t->o << "," << t->tgt << ") : " << t->id << "," << endl;
    }
    cout << "}" << endl;
    cout << "Timeouts : {";
    for (auto t : this->timeouts) {
        if (t->t != inf) {
            cout << "(" << t->src << "," << t->t << "," << t->tgt << ") : " << t->id << "," << endl;
        }
        else {
            cout << "(" << t->src << "," << "INF" << "," << t->tgt << ") : " << t->id << "," << endl;
        }
    }
    cout << "}" << endl;
}

int TFSM_TO::getTransitionSize()
{
    return this->transitions.size() + this->timeouts.size();
}

vector<IOTransition *> TFSM_TO::getTransitions()
{
    return FSM::getTransitions();
}

vector<TimeoutTransition *> TFSM_TO::getTimeouts()
{
    return this->timeouts;
}

int TFSM_TO::getInitialState()
{
    return FSM::getInitialState();
}

set<set<int>> TFSM_TO::getEta(int s, string i)
{
    return set<set<int>>();
}

string TFSM_TO::generateDot()
{
    ostringstream res;
    res << "digraph S {" << endl;
    for (IOTransition * t : this->transitions) {
        res << t->src << " -> " << t->tgt;
        if (this->mutatedTransitions.find(t->id)->second) {
            res << " [style=\"dashed\" label=\"" << t->i << " / " << t->o << "\"];";
        }
        else {
            res << " [label=\"" << t->i << " / " << t->o << "\"];";
        }
        res << endl;
    }
    for (TimeoutTransition * t : this->timeouts) {
        res << t->src << " -> " << t->tgt;
        if (this->mutatedTransitions.find(t->id)->second) {
            res << " [style=\"dashed\" label=\"" << t->t << "\"];";
        }
        else {
            res << " [label=\"" << t->t << "\"];";
        }
        res << endl;
    }
    res << "}" << endl;
    return res.str();
}
