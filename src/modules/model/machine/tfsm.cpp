#include "tfsm.h"
#include <iostream>
#include "../structs.h"
#include <math.h>
#include <sstream>
using namespace std;

TFSM::TFSM(set<int> S, int s0, set<string> I, set<string> O, vector<IOTransition *> lambda, vector<TimeoutTransition *> delta) : TFSM_TO(S, s0, I, O, lambda, delta)
{
    this->states = S;
    this->initialState = s0;
    this->inputs = I;
    this->outputs = O;
    this->transitions = lambda;
    this->timeouts = delta;
    this->computeMaps();
}

bool isIntersectionEmpty(vector<IOTransition *> toCheck) {
    for (IOTransition * g1 : toCheck) {
        for (IOTransition * g2 : toCheck) {
            if (g1->id != g2->id) {
                if (!g1->getGuard().isIntersectionEmpty(g2->getGuard())) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool isUnionEverything(vector<IOTransition *> toCheck) {
    vector<Guard> orderedGuards;
    int lastMax = 0;
    int size = toCheck.size();
    for (int i=0; i<size; i++) {
        int tmin = -1;
        vector<IOTransition *>::iterator minGuard = toCheck.end();
        for (auto it = toCheck.begin(); it != toCheck.end(); it++) {
            if (tmin == -1 || (*it)->getGuard().tmin < tmin) {
                tmin = (*it)->getGuard().tmin;
                minGuard = it;
            }
        }
        if (minGuard != toCheck.end()) {
            if ((*minGuard)->getGuard().tmax >= lastMax) {
                orderedGuards.push_back((*minGuard)->getGuard());
                lastMax = (*minGuard)->getGuard().tmax;
            }
            toCheck.erase(minGuard);
        }
    }
    lastMax = 0;
    Bracket lastBracket = Bracket::Curly;
    for (Guard g : orderedGuards) {
        if (g.tmin <= lastMax) {
            if (lastMax == g.tmin && lastBracket == g.left) {
                return false;
            }
        }
        else {
            if (lastMax == g.tmin && lastBracket == g.left) {
                return false;
            }
            else {
                return false;
            }
        }
        lastMax = g.tmax;
        lastBracket = g.right;
    }
    if (lastMax != inf) {
        return false;
    }
    return true;
}

void TFSM::computeMaps()
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

    this->combinationsMaps.clear();
    //std::map<int, std::map<std::string, std::set<int>>> combinationsMaps;
    for (int s : this->states) {
        map<string, set<set<int>>> elt;
        for (string i : this->inputs) {


            set<set<int>> combinations;
            vector<IOTransition *> xi = this->getXi(s, i);
            for (int word=0; word<pow(2, xi.size()); word++) {
                set<int> combination;
                vector<IOTransition *> toCheck;
                int j=0;
                for (IOTransition * transition : xi) {

                    bool value = (word >> j) & 1;
                    if (value) {
                        combination.insert(transition->id);
                        toCheck.push_back(transition);
                    }
                    j++;
                }
                if (isUnionEverything(toCheck) && isIntersectionEmpty(toCheck)) {
                    combinations.insert(combination);
                }
            }
            elt.insert(make_pair(i, combinations));
        }
        this->combinationsMaps.insert(make_pair(s, elt));
    }
}

void TFSM::addTransitions(vector<IOTransition *> transitions, bool isMutated)
{
    TFSM_TO::addTransitions(transitions, isMutated);
    this->computeMaps();
}

void TFSM::addTimeouts(vector<TimeoutTransition *> timeouts, bool isMutated)
{
    TFSM_TO::addTimeouts(timeouts, isMutated);
    this->computeMaps();
}

vector<IOTransition *> TFSM::getXi(int s, string i)
{
    return TFSM_TO::getXi(s, i);
}

vector<TimeoutTransition *> TFSM::getXi(int s)
{
    return TFSM_TO::getXi(s);
}

std::vector<IOTransition *> TFSM::lambda(int s)
{
    return TFSM_TO::lambda(s);
}

vector<TimeoutTransition *> TFSM::delta(int s)
{
    return TFSM_TO::delta(s);
}

bool TFSM::isIdTimeout(int id) {
    return TFSM_TO::isIdTimeout(id);
}

IOTransition * TFSM::getTransitionFromId(int id)
{
    return TFSM_TO::getTransitionFromId(id);
}

TimeoutTransition * TFSM::getTimeoutFromId(int id)
{
    return TFSM_TO::getTimeoutFromId(id);
}

int TFSM::getMaxDelta(int s)
{
    return TFSM_TO::getMaxDelta(s);
}

set<set<int>> TFSM::getEta(int s, string i)
{
    auto elemState = this->combinationsMaps.find(s);
    if (elemState != this->combinationsMaps.end()) {
        auto elemInput = elemState->second.find(i);
        if (elemInput != elemState->second.end()) {
            return elemInput->second;
        }
    }
}

void TFSM::print()
{
    cout << "States : {";
    for (auto s : this->states) {
        cout << s << ", ";
    }
    cout << "}" << endl;
    cout << "Transitions : {";
    for (IOTransition * t : this->transitions) {
        cout << "(" << t->src << "," << t->i << "," << t->getGuard().toString() <<"," << t->o << "," << t->tgt << ") : " << t->id << "," << endl;
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

int TFSM::getTransitionSize()
{
    return this->transitions.size() + this->timeouts.size();
}

vector<IOTransition *> TFSM::getTransitions()
{
    return TFSM_TO::getTransitions();
}

vector<TimeoutTransition *> TFSM::getTimeouts()
{
    return TFSM_TO::getTimeouts();
}

int TFSM::getInitialState()
{
    return TFSM_TO::getInitialState();
}

string TFSM::generateDot()
{
    ostringstream res;
    res << "digraph TFSM {" << endl;
    for (IOTransition * t : this->transitions) {
        res << t->src << " -> " << t->tgt;
        if (this->mutatedTransitions.find(t->id)->second) {
            res << " [style=\"dashed\" label=\"" << t->getGuard().toString() << " " << t->i << " / " << t->o << " [" << t->id << "]\"];";
        }
        else {
            res << " [label=\"" << t->getGuard().toString() << " " << t->i << " / " << t->o << " [" << t->id << "]\"];";
        }
        res << endl;
    }
    for (TimeoutTransition * t : this->timeouts) {
        res << t->src << " -> " << t->tgt;

        if (this->mutatedTransitions.find(t->id)->second) {
            if (t->t == inf) {
                res << " [style=\"dashed\" label=\"∞" << " [" << t->id << "]\"];";
            }
            else {
                res << " [style=\"dashed\" label=\"" << t->t << " [" << t->id << "]\"];";
            }
        }
        else {
            if (t->t == inf) {
                res << " [label=\"∞" << " [" << t->id << "]\"];";
            }
            else {
                res << " [label=\"" << t->t << " [" << t->id << "]\"];";
            }
        }
        res << endl;
    }
    res << "}" << endl;
    return res.str();
}

string TFSM::generateFormalism()
{
    ostringstream res;
    res << "digraph TFSM {" << endl;
    for (IOTransition * t : this->transitions) {
        res << "s" << t->src << " -> " << "s" << t->tgt;
        if (this->mutatedTransitions.find(t->id)->second) {
            res << " [" << t->getGuard().toString() << " " << t->i << " / " << t->o << "*];";
        }
        else {
            res << " [" << t->getGuard().toString() << " " << t->i << " / " << t->o << "];";
        }
        res << endl;
    }
    for (TimeoutTransition * t : this->timeouts) {
        res << "s" << t->src << " -> " << "s" << t->tgt;

        if (this->mutatedTransitions.find(t->id)->second) {
            if (t->t == inf) {
                res << " [#*];";
            }
            else {
                res << " [" << t->t << "*];";
            }
        }
        else {
            if (t->t == inf) {
                res << " [#];";
            }
            else {
                res << " [" << t->t << "];";
            }
        }
        res << endl;
    }
    res << "}" << endl;
    return res.str();
}
