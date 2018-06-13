#include "tfsm.h"
#include <iostream>
#include "structs.h"
#include <math.h>
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

bool Guard::isIntersectionEmpty(Guard other) {
    Bracket left;
    int tmin;
    int tmax;
    Bracket right;

    if (this->tmin > other.tmin) {
        tmin = this->tmin;
        left = this->left;
    }
    if (this->tmin < other.tmin) {
        tmin = other.tmin;
        left = other.left;
    }
    else {
        tmin = this->tmin;
        if (this->left == Bracket::Square && other.left == Bracket::Square) {
            left = Bracket::Square;
        }
        else {
            left = Bracket::Curly;
        }
    }

    if (this->tmax < other.tmax) {
        tmax = this->tmax;
        right = this->right;
    }
    if (this->tmax > other.tmax) {
        tmax = other.tmax;
        right = other.right;
    }
    else {
        tmax = this->tmax;
        if (this->right == Bracket::Square && other.right == Bracket::Square) {
            right = Bracket::Square;
        }
        else {
            right = Bracket::Curly;
        }
    }

    if (tmin == tmax && left != right) {
        return true;
    }
    return tmin > tmax;
}

Guard Guard::intersect(Guard other) {
    Bracket left;
    int tmin;
    int tmax;
    Bracket right;

    if (this->tmin > other.tmin) {
        tmin = this->tmin;
        left = this->left;
    }
    if (this->tmin < other.tmin) {
        tmin = other.tmin;
        left = other.left;
    }
    else {
        tmin = this->tmin;
        if (this->left == Bracket::Square && other.left == Bracket::Square) {
            left = Bracket::Square;
        }
        else {
            left = Bracket::Curly;
        }
    }

    if (this->tmax < other.tmax) {
        tmax = this->tmax;
        right = this->right;
    }
    if (this->tmax > other.tmax) {
        tmax = other.tmax;
        right = other.right;
    }
    else {
        tmax = this->tmax;
        if (this->right == Bracket::Square && other.right == Bracket::Square) {
            right = Bracket::Square;
        }
        else {
            right = Bracket::Curly;
        }
    }
    return Guard(left, tmin, tmax, right);
}

bool Guard::equals(Guard other) {
    return (this->left == other.left
            && this->tmin == other.tmin
            && this->tmax == other.tmax
            && this->right == other.right);
}

bool Guard::contains(int x) {
    bool guard1;
    if (this->left == Bracket::Square) {
        guard1 = x >= this->tmin;
    }
    else {
        guard1 = x > this->tmin;
    }
    bool guard2;
    if (this->right == Bracket::Square) {
        guard2 = x <= this->tmax;
    }
    else {
        guard2 = x < this->tmax;
    }
    return guard1 && guard2;
}

std::string Guard::toString() {
    string guardString;
    if (this->left == Bracket::Square)
        guardString+="[";
    else
        guardString+="(";
    guardString += to_string(this->tmin) + ",";
    if (this->tmax != inf)
        guardString += to_string(this->tmax);
    else
        guardString += "INF";
    if (this->right == Bracket::Square)
        guardString+="]";
    else
        guardString+=")";
    return guardString;
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

bool isIntersectionEmpty(vector<GuardedTransition> toCheck) {
    for (GuardedTransition g1 : toCheck) {
        for (GuardedTransition g2 : toCheck) {
            if (g1.id != g2.id) {
                if (!g1.g.isIntersectionEmpty(g2.g)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool isUnionEverything(vector<GuardedTransition> toCheck) {
    vector<Guard> orderedGuards;
    int lastMax = 0;
    int size = toCheck.size();
    for (int i=0; i<size; i++) {
        int tmin = -1;
        vector<GuardedTransition>::iterator minGuard = toCheck.end();
        for (auto it = toCheck.begin(); it != toCheck.end(); it++) {
            if (tmin == -1 || (*it).g.tmin < tmin) {
                tmin = (*it).g.tmin;
                minGuard = it;
            }
        }
        if (minGuard != toCheck.end()) {
            if ((*minGuard).g.tmax >= lastMax) {
                orderedGuards.push_back((*minGuard).g);
                lastMax = (*minGuard).g.tmax;
            }
            toCheck.erase(minGuard);
        }
    }
    /*
    cout << "Ordered : ";
    for (Guard g : orderedGuards) {
        cout << g.toString() << " ";
    }
    cout << endl;
*/
    lastMax = 0;
    Bracket lastBracket = Bracket::Curly;
    for (Guard g : orderedGuards) {
        //cout << lastMax << " " << g.tmin << " " << lastBracket << " " << g.left << endl;
        if (g.tmin <= lastMax) {
            if (lastMax == g.tmin && lastBracket == g.left) {
                //cout << "Here!" << endl;
                return false;
            }
        }
        else {
            if (lastMax == g.tmin && lastBracket == g.left) {
                //cout << "Here? " << g.tmin << " " << lastMax << endl;
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
        //cout << "Here:" << endl;
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

    this->combinationsMaps.clear();
    //std::map<int, std::map<std::string, std::set<int>>> combinationsMaps;
    for (int s : this->states) {
        map<string, set<set<int>>> elt;
        for (string i : this->inputs) {


            set<set<int>> combinations;
            vector<GuardedTransition> xi = this->getXi(s, i);
            for (int word=0; word<pow(2, xi.size()); word++) {
                set<int> combination;
                vector<GuardedTransition> toCheck;
                int j=0;
                for (GuardedTransition transition : xi) {

                    bool value = (word >> j) & 1;
                    if (value) {
                        combination.insert(transition.id);
                        toCheck.push_back(transition);
                    }
                    j++;
                }
                /*
                cout << "Combination : {";
                for (auto d : combination) {
                    cout << d << ",";
                }
                cout << "}" << endl;
                cout << "guards : ";
                for (Guard g : guards) {
                    cout << g.toString() << " ";
                }
                cout << endl;
                */
                /*
                if (isUnionEverything(guards)) {
                    cout << "Union ok" << endl;
                }
                */
                /*
                if (isIntersectionEmpty(guards)) {
                    cout << "Intersection ok" << endl;
                }
                */
                if (isUnionEverything(toCheck) && isIntersectionEmpty(toCheck)) {
                    combinations.insert(combination);
                }
            }
            elt.insert(make_pair(i, combinations));
        }
        this->combinationsMaps.insert(make_pair(s, elt));
    }
    /*
    for (auto a : this->combinationsMaps) {
        cout << "State : " << a.first << endl;
        for (auto b : a.second) {
            cout << "Input : " << b.first << endl;
            for (auto c : b.second) {
                cout << "Res : {";
                for (auto d : c) {
                    cout << d << " ";
                    cout << this->getTransitionFromId(d).g.toString() << ", ";
                }
                cout << "}" << endl;
            }
        }
    }
    */

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

vector<GuardedTransition> TFSM::getXi(int s, string i)
{
    vector<GuardedTransition> result;
    for (auto transition : this->lambda(s)) {
        if (transition.i == i)
            result.push_back(transition);
    }
    return result;
}

vector<Timeout> TFSM::getXi(int s)
{
    return this->delta(s);
}

vector<GuardedTransition> TFSM::lambda(int s)
{
    return this->transitionsPerState.find(s)->second;
}

vector<Timeout> TFSM::delta(int s)
{
    return this->timeoutsPerState.find(s)->second;
}

bool TFSM::isIdTimeout(int id) {
    return this->timeoutIdMap.find(id) != this->timeoutIdMap.end();
}

GuardedTransition TFSM::getTransitionFromId(int id)
{
    return this->transitionIdMap.find(id)->second;
}

Timeout TFSM::getTimeoutFromId(int id)
{
    return this->timeoutIdMap.find(id)->second;
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

int TFSM::getMaxDelta(int s)
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

void TFSM::print()
{
    cout << "States : {";
    for (auto s : this->states) {
        cout << s << ", ";
    }
    cout << "}" << endl;
    cout << "Transitions : {";
    for (GuardedTransition t : this->transitions) {
        cout << "(" << t.src << "," << t.i << "," << t.g.toString() <<"," << t.o << "," << t.tgt << ") : " << t.id << "," << endl;
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
