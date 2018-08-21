#include "fsm.h"
#include <iostream>
#include <sstream>
using namespace std;

FSM::FSM(set<int> S, int s0, set<string> I, set<string> O, vector<IOTransition *> lambda)
{
    this->states = S;
    this->initialState = s0;
    this->inputs = I;
    this->outputs = O;
    this->transitions = lambda;
    for (IOTransition * t : this->transitions) {
        this->mutatedTransitions.insert(make_pair(t->id, false));
    }
    this->computeMaps();
}

void FSM::computeMaps()
{
    this->transitionsPerState.clear();
    this->transitionIdMap.clear();
    for (int s : this->states) {
        this->transitionsPerState.insert(make_pair(s, vector<IOTransition *>()));
    }

    for (IOTransition * t : this->transitions) {
        this->transitionIdMap.insert(make_pair(t->id, t));
        this->transitionsPerState.find(t->src)->second.push_back(t);
    }
}

void FSM::addTransitions(vector<IOTransition *> transitions, bool isMutated)
{
    this->transitions.insert(this->transitions.end(), transitions.begin(), transitions.end());
    this->computeMaps();
    for (IOTransition * t : transitions) {
        this->mutatedTransitions.insert(make_pair(t->id, isMutated));
    }
}

vector<IOTransition *> FSM::getXi(int s, string i)
{
    vector<IOTransition *> result;
    for (auto transition : this->lambda(s)) {
        if (transition->i == i)
            result.push_back(transition);
    }
    return result;
}

vector<TimeoutTransition *> FSM::getXi(int s)
{
    return this->delta(s);
}

int FSM::getMaxDelta(int s)
{
    return 0;
}

vector<IOTransition *> FSM::lambda(int s)
{
    return this->transitionsPerState.find(s)->second;
}

vector<TimeoutTransition *> FSM::delta(int s)
{
    return vector<TimeoutTransition *>();
}

IOTransition * FSM::getTransitionFromId(int id)
{
    return this->transitionIdMap.find(id)->second;
}

TimeoutTransition * FSM::getTimeoutFromId(int id)
{
    return NULL;
}

bool FSM::isIdTimeout(int id) {
    return false;
}

void FSM::print()
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
}

int FSM::getTransitionSize()
{
    return this->transitions.size();
}

vector<IOTransition *> FSM::getTransitions()
{
    return this->transitions;
}

vector<TimeoutTransition *> FSM::getTimeouts()
{
    return vector<TimeoutTransition *>();
}

int FSM::getInitialState()
{
    return this->initialState;
}

set<set<int>> FSM::getEta(int s, string i)
{
    return set<set<int>>();
}

string FSM::generateDot()
{
    ostringstream res;
    res << "digraph FSM {" << endl;
    for (IOTransition * t : this->transitions) {
        res << t->src << " -> " << t->tgt;
        cout << "?: " << this->mutatedTransitions.find(t->id)->second << endl;
        if (this->mutatedTransitions.find(t->id)->second) {
            res << " [style=\"dashed\" label=\"" << t->i << " / " << t->o << " [" << t->id << "]\"];";
        }
        else {
            res << " [label=\"" << t->i << " / " << t->o << " [" << t->id << "]\"];";
        }
        res << endl;
    }
    res << "}" << endl;
    return res.str();
}
