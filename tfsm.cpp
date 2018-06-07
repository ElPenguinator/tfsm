#include "tfsm.h"
#include <iostream>
#include "structs.h"
using namespace std;

Guard::Guard() {

}

Guard::Guard(std::string left, int tmin, int tmax, std::string right) {
    if (left == "[")
        this->left = Bracket::Square;
    else if (left == "(")
        this->left = Bracket::Curly;
    this->tmin = tmin;
    this->tmax = tmax;
    if (right == "]")
        this->right = Bracket::Square;
    else if (right == ")")
        this->right = Bracket::Curly;
}

Guard::Guard(Bracket left, int tmin, int tmax, Bracket right) {
    this->left = left;
    this->tmin = tmin;
    this->tmax = tmax;
    this->right = right;
}

GuardedTransition::GuardedTransition(int src, string i, Guard g, string o, int tgt, int id) {
    this->src = src;
    this->i = i;
    this->g = g;
    this->o = o;
    this->tgt = tgt;
    this->id = id;
}

TFSM::TFSM(set<int> S, int s0, set<string> I, set<string> O, vector<GuardedTransition> lambda, vector<Timeout> delta)
{
    this->states = S;
    this->initialState = s0;
    this->inputs = I;
    this->outputs = O;
    this->transitions = lambda;
    this->timeouts = delta;
    this->computeMaps();
}

void TFSM::computeMaps()
{
    this->transitionsPerState.clear();
    this->timeoutsPerState.clear();
    this->transitionIdMap.clear();
    this->timeoutIdMap.clear();
    for (int s : this->states) {
        this->transitionsPerState.insert(make_pair(s, vector<GuardedTransition>()));
        this->timeoutsPerState.insert(make_pair(s, vector<Timeout>()));
    }

    for (GuardedTransition t : this->transitions) {
        this->transitionIdMap.insert(make_pair(t.id, t));
        this->transitionsPerState.find(t.src)->second.push_back(t);
    }

    for (Timeout t : this->timeouts) {
        this->timeoutIdMap.insert(make_pair(t.id, t));
        this->timeoutsPerState.find(t.src)->second.push_back(t);
    }
}

void TFSM::addTransitions(vector<GuardedTransition> transitions)
{
    this->transitions.insert(this->transitions.end(), transitions.begin(), transitions.end());
    this->computeMaps();
}

void TFSM::addTimeouts(vector<Timeout> timeouts)
{
    this->timeouts.insert(this->timeouts.end(), timeouts.begin(), timeouts.end());
    this->computeMaps();
}

void TFSM::print()
{
    cout << "States : {";
    for (auto s : this->states) {
        cout << s << ", ";
    }
    cout << "}" << endl;
    cout << "Transitions : {";
    for (GuardedTransition t : this->transitions) {
        string guardString;
        if (t.g.left == Bracket::Square)
            guardString+="[";
        else
            guardString+="(";
        guardString += to_string(t.g.tmin) + ",";
        if (t.g.tmax != inf)
            guardString += to_string(t.g.tmax);
        else
            guardString += "INF";
        if (t.g.right == Bracket::Square)
            guardString+="]";
        else
            guardString+=")";
        cout << "(" << t.src << "," << t.i << "," << guardString <<"," << t.o << "," << t.tgt << ") : " << t.id << "," << endl;
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
