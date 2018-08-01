#include "fsm.h"
#include <iostream>
using namespace std;

FSM::FSM(set<int> S, int s0, set<string> I, set<string> O, vector<IOTransition> lambda)
{
    this->states = S;
    this->initialState = s0;
    this->inputs = I;
    this->outputs = O;
    this->transitions = lambda;
    this->computeMaps();
}

void FSM::computeMaps()
{
    this->transitionsPerState.clear();
    this->transitionIdMap.clear();
    for (int s : this->states) {
        this->transitionsPerState.insert(make_pair(s, vector<IOTransition>()));
    }

    for (IOTransition t : this->transitions) {
        this->transitionIdMap.insert(make_pair(t.id, t));
        this->transitionsPerState.find(t.src)->second.push_back(t);
    }
}

void FSM::addTransitions(vector<IOTransition> transitions)
{
    this->transitions.insert(this->transitions.end(), transitions.begin(), transitions.end());
    this->computeMaps();
}

vector<IOTransition> FSM::getXi(int s, string i)
{
    vector<IOTransition> result;
    for (auto transition : this->lambda(s)) {
        if (transition.i == i)
            result.push_back(transition);
    }
    return result;
}

vector<IOTransition> FSM::lambda(int s)
{
    return this->transitionsPerState.find(s)->second;
}

IOTransition FSM::getTransitionFromId(int id)
{
    return this->transitionIdMap.find(id)->second;
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
        cout << "(" << t.src << "," << t.i << "," << t.o << "," << t.tgt << ") : " << t.id << "," << endl;
    }
    cout << "}" << endl;
}
