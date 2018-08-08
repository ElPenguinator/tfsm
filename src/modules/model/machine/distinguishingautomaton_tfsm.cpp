#include "distinguishingautomaton_tfsm.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <queue>
#include "../tools.h"
using namespace std;

DistinguishingAutomaton_TFSM::DistinguishingAutomaton_TFSM(FSM *S, FSM *M) : DistinguishingAutomaton_TFSM_TO(S, M)
{
    this->specification = S;
    this->mutationMachine = M;

    ProductState * initialState = new TimedProductState(S->initialState, M->initialState, 0, 0);
    this->states.clear();
    this->states.insert(make_pair(initialState->getKey(), initialState));
    this->initialState = initialState;
    this->hasNoSinkState = true;
    this->isConnected = true;
    //this->generateNext(initialState);
    //this->isConnected = this->isProductConnected();
}

void DistinguishingAutomaton_TFSM::insertState(ProductState * state, string i, Guard g, ProductState * newState, bool isTimeout, int id)
{
    if (this->states.find(newState->getKey()) == this->states.end()) {
        this->states.insert(make_pair(newState->getKey(), newState));
    }
    this->transitions.push_back(new GuardedProductTransition(state->getKey(), i, g, newState->getKey(), isTimeout, id));
}

void DistinguishingAutomaton_TFSM::generateNext(ProductState * state)
{

    for (IOTransition * mutationTransition : this->mutationMachine->lambda(state->mutationState)) {
        for (IOTransition * specificationTransition : this->specification->getXi(state->specificationState, mutationTransition->i)) {
            if (!mutationTransition->getGuard().isIntersectionEmpty(specificationTransition->getGuard())) {

                ProductState * newState;
                if (specificationTransition->o == mutationTransition->o)
                    newState = new TimedProductState(specificationTransition->tgt, mutationTransition->tgt, 0, 0);
                else {
                    newState = new TimedProductSinkState();
                    this->hasNoSinkState = false;
                }
                this->insertState(state, specificationTransition->i, mutationTransition->getGuard().intersect(specificationTransition->getGuard()), newState, false, mutationTransition->id);
            }
        }

    }

    TimeoutTransition * related = NULL;
    for (auto specificationTimeout : this->specification->delta(state->specificationState)) {
        related = new TimeoutTransition(specificationTimeout->src, specificationTimeout->t, specificationTimeout->tgt, specificationTimeout->id);//&specificationTimeout;
    }
    int spec_t = related->t;
    if (spec_t != inf)
        spec_t -= state->getSpecificationCounter();

    Guard dummy("[", 0, inf, ")");

    for (auto mutationTimeout : this->mutationMachine->delta(state->mutationState)) {
        int muta_t = mutationTimeout->t;
        if (muta_t != inf)
            muta_t -= state->getMutationCounter();
        if (muta_t > 0) {
            ProductState * newState;
            if (muta_t < spec_t && spec_t != inf) {
                newState = new TimedProductState(state->specificationState, mutationTimeout->tgt, state->getSpecificationCounter() + muta_t, 0);
                this->insertState(state, to_string(muta_t), dummy, newState, true, mutationTimeout->id);
            }
            else if (muta_t < spec_t && spec_t == inf) {
                newState = new TimedProductState(state->specificationState, mutationTimeout->tgt, inf, 0);
                this->insertState(state, to_string(muta_t), dummy, newState, true, mutationTimeout->id);
            }
            else if (muta_t == spec_t) {
                newState = new TimedProductState(related->tgt, mutationTimeout->tgt, 0, 0);
                this->insertState(state, to_string(muta_t), dummy, newState, true, mutationTimeout->id);
            }
            else if (muta_t > spec_t && muta_t != inf) {
                newState = new TimedProductState(related->tgt, state->mutationState, 0, state->getMutationCounter() + spec_t);
                this->insertState(state, to_string(spec_t), dummy, newState, true, mutationTimeout->id);
            }
            else if (muta_t > spec_t && muta_t == inf) {
                newState = new TimedProductState(related->tgt, state->mutationState, 0, inf);
                this->insertState(state, to_string(spec_t), dummy, newState, true, mutationTimeout->id);
            }
        }
    }
    delete related;
    state->isGenerated = true;

    vector<ProductTransition *> copyTransitions(this->transitions);
    for (auto transition : copyTransitions) {
        if (transition->src == state->getKey()) {
            ProductState * tgt = this->states.find(transition->tgt)->second;
            if (!tgt->isGenerated)
                this->generateNext(tgt);
        }
    }
}

std::vector<executingPath> DistinguishingAutomaton_TFSM::revealingPaths(sequence alpha)
{
    vector<executingPath> results;
    executingPath currentPath;
    revealingPathsRecursive(this->initialState, currentPath, results, alpha, 0, 0, 0);
    return results;
}

bool DistinguishingAutomaton_TFSM::isPathDeterministic(const executingPath p)
{
    for (int id : p) {
        if (this->mutationMachine->isIdTimeout(id)) {
            vector<TimeoutTransition *> xiTimeouts = this->mutationMachine->getXi(this->mutationMachine->getTimeoutFromId(id)->src);
            for (auto otherTimeout : xiTimeouts) {
                if (find(p.begin(), p.end(), otherTimeout->id) != p.end() && otherTimeout->id != id)
                    return false;
            }
        }
        else {
            set<set<int>> eta = this->mutationMachine->getEta(this->mutationMachine->getTransitionFromId(id)->src, this->mutationMachine->getTransitionFromId(id)->i);
            for (set<int> combi : eta) {
                if (find(combi.begin(), combi.end(), id) == combi.end()) {
                    for (int elt : combi) {
                        if (find(p.begin(), p.end(), elt) != p.end() && elt != id) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

void DistinguishingAutomaton_TFSM::revealingPathsRecursive(ProductState * state, executingPath currentPath, vector<executingPath> &results, sequence alpha, int sequenceIndex, int timeBuffer, int timeLeftOver)
{
    if (state->getKey() == "sink") {
        results.push_back(currentPath);
    }
    else if (sequenceIndex < alpha.size()) {
        ts timed_symbol = alpha[sequenceIndex];
        string symbol = timed_symbol.first;
        int symbol_time = timed_symbol.second;
        cout << "State : " << state->getKey() << endl;
        cout << "TimeState : (" << symbol << ", " << symbol_time << ")" << endl;
        int t = symbol_time - timeBuffer;
        if (sequenceIndex > 0) {
            t -= alpha[sequenceIndex -1].second;
            symbol_time -= alpha[sequenceIndex -1].second;
        }
        cout << "Time left : " << t << endl;
        //Time to spend, so take only timeouts
        if (t > 0) {
            for (auto transition : this->transitions) {
                if (transition->src == state->getKey() && transition->isTimeout) {
                    int timeout = atoi(transition->i.c_str());
                    ProductState * tgtNode = this->states.find(transition->tgt)->second;
                    if (timeout <= t) {
                        cout << state->getKey() << " Timeout takeable " << transition->id << " -> " << timeout << " <= " << t << endl;
                        executingPath newPath(currentPath);
                        newPath.push_back(transition->id);
                        if (this->isPathDeterministic(newPath)) {
                            cout << state->getKey() << " Timeout took " << transition->id << endl;
                            this->revealingPathsRecursive(tgtNode, newPath, results, alpha, sequenceIndex, timeBuffer + timeout, 0);
                        }
                    }
                    else {
                        for (auto mutaTimeout : this->mutationMachine->delta(state->mutationState)) {
                            cout << state->getKey() << " Timeout waitable " << transition->id << " -> " << t << " <= " << mutaTimeout->t << endl;
                            if (t < mutaTimeout->t) {
                                cout << state->getKey() << " Timeout waitable " << transition->id << endl;
                                executingPath newPath(currentPath);
                                newPath.push_back(transition->id);
                                if (this->isPathDeterministic(newPath)) {
                                    cout << state->getKey() << " Timeout waited " << transition->id << endl;
                                    this->revealingPathsRecursive(state, newPath, results, alpha, sequenceIndex, symbol_time, t);
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            cout << state->getKey() << " Take transition" << endl;
            for (auto transition : this->transitions) {
                if (transition->src == state->getKey() && !transition->isTimeout) {
                    if (transition->i == symbol && transition->getGuard().contains(timeLeftOver)) {
                        ProductState * tgtNode = this->states.find(transition->tgt)->second;
                        executingPath newPath(currentPath);
                        newPath.push_back(transition->id);
                        if (this->isPathDeterministic(newPath))
                            this->revealingPathsRecursive(tgtNode, newPath, results, alpha, sequenceIndex+1, 0, 0);
                    }
                }
            }
        }
    }
}


void DistinguishingAutomaton_TFSM::print()
{
    cout << "States : {";
    for (auto s : this->states) {
        cout << s.first << endl;
    }
    cout << "}" << endl;
    cout << "Transitions : {";
    for (auto t : this->transitions) {
        cout << "(" << t->src << "," << t->i << "," << t->getGuard().toString() << "," << t->tgt << ") : " << t->id << "," << endl;
    }
    cout << "}" << endl;
}

string DistinguishingAutomaton_TFSM::DijkstraFindMin(map<string, int> distances, set<string> Q) {
    int min = inf;
    string minState = "";
    for (auto state : Q) {
        if (distances.find(state)->second <= min) {
            min = distances.find(state)->second;
            minState = state;
        }
    }
    return minState;
}

void DistinguishingAutomaton_TFSM::DijkstraUpdateDistancesMin(map<string, int> & distances, map<string, ProductTransition *> & predecessors, string s1, string s2, ProductTransition * transition) {
    if (distances.find(s2)->second > distances.find(s1)->second + 1) {
        if (distances.find(s1)->second != inf)
            distances.find(s2)->second = distances.find(s1)->second + 1;
        predecessors.find(s2)->second = transition;
    }
}

bool DistinguishingAutomaton_TFSM::isProductConnected()
{
    queue<string> fifo;
    set<string> marked;
    fifo.push(this->initialState->getKey());
    marked.insert(this->initialState->getKey());
    while (!fifo.empty()) {
        string element = fifo.front();
        fifo.pop();
        for (ProductTransition * transition : this->transitions) {
            if (!transition->isTimeout) {
                if (transition->src == element && marked.find(transition->tgt) == marked.end()) {
                    fifo.push(transition->tgt);
                    marked.insert(transition->tgt);
                }
            }
            else {
                if (atoi(transition->i.c_str()) != inf) {
                    if (transition->src == element && marked.find(transition->tgt) == marked.end()) {
                        fifo.push(transition->tgt);
                        marked.insert(transition->tgt);
                    }
                }
            }
        }
    }

    return marked.size() == this->states.size();
}

std::deque<ProductTransition *> DistinguishingAutomaton_TFSM::Dijkstra(string key)
{
    map<string, ProductTransition *> predecessors;
    set<string> Q;
    map<string, int> distances;
    for (const auto &s : this->states) {
        string stateKey = s.first;
        Q.insert(stateKey);
        predecessors.insert(make_pair(stateKey, new GuardedProductTransition("", "", Guard(), "", false, -1)));
        distances.insert(make_pair(stateKey, inf));
    }
    distances.find(key)->second = 0;

    while (Q.size() > 0) {
        string s1 = DijkstraFindMin(distances, Q);
        Q.erase(Q.find(s1));
        for (auto transition : this->transitions) {
            if (transition->src == s1) {
                if (atoi(transition->i.c_str()) != inf) {
                    if (transition->getGuard().tmin < this->mutationMachine->getMaxDelta(this->states.find(transition->src)->second->mutationState) - this->states.find(transition->src)->second->getMutationCounter()
                            || this->mutationMachine->getMaxDelta(this->states.find(transition->src)->second->mutationState) == 0) {
                        DijkstraUpdateDistancesMin(distances, predecessors, s1, transition->tgt, transition);
                    }}
            }
        }
    }

    string currentStateKey = "sink";
    deque<ProductTransition *> results;

    if (distances.find("sink")->second != inf) {
        while (currentStateKey != key) {
            if (distances.find(currentStateKey)->second == inf) {
                results.clear();
                return results;
            }
            /*
            cout << "Res :" << endl;
            cout << currentStateKey << endl;
            cout << predecessors.find(currentStateKey)->second.getKey() << endl;
            */
            results.push_front(predecessors.find(currentStateKey)->second);
            currentStateKey = predecessors.find(currentStateKey)->second->src;
        }
    }
    return results;
}

void DistinguishingAutomaton_TFSM::reachableStates(ProductState * state, executingPath currentPath, set<string> &results, sequence alpha, int sequenceIndex, int timeBuffer)
{
    if (state->getKey() != "sink") {
        if (sequenceIndex < alpha.size()) {
            ts timed_symbol = alpha[sequenceIndex];
            string symbol = timed_symbol.first;
            int symbol_time = timed_symbol.second;
            int t = symbol_time - timeBuffer;
            if (sequenceIndex > 0) {
                t -= alpha[sequenceIndex -1].second;
                symbol_time -= alpha[sequenceIndex -1].second;
            }
            //Time to spend, so take only timeouts
            if (t > 0) {
                for (auto transition : this->transitions) {
                    if (transition->src == state->getKey() && transition->isTimeout) {
                        int timeout = atoi(transition->i.c_str());
                        ProductState * tgtNode = this->states.find(transition->tgt)->second;
                        if (timeout <= t) {
                            executingPath newPath(currentPath);
                            newPath.push_back(transition->id);
                            if (this->isPathDeterministic(newPath)) {
                                this->reachableStates(tgtNode, newPath, results, alpha, sequenceIndex, timeBuffer + timeout);
                            }
                        }
                        else {
                            for (auto mutaTimeout : this->mutationMachine->delta(state->mutationState)) {
                                if (timeout <= mutaTimeout->t) {
                                    executingPath newPath(currentPath);
                                    newPath.push_back(transition->id);
                                    if (this->isPathDeterministic(newPath)) {
                                        this->reachableStates(state, newPath, results, alpha, sequenceIndex, symbol_time);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else {
                for (auto transition : this->transitions) {
                    if (transition->src == state->getKey() && !transition->isTimeout) {
                        if (transition->i == symbol) {
                            ProductState * tgtNode = this->states.find(transition->tgt)->second;
                            executingPath newPath(currentPath);
                            newPath.push_back(transition->id);
                            if (this->isPathDeterministic(newPath))
                                this->reachableStates(tgtNode, newPath, results, alpha, sequenceIndex+1, 0);
                        }
                    }
                }
            }
        }
        else {
            results.insert(state->getKey());
        }
    }
}

sequence DistinguishingAutomaton_TFSM::inputSequenceFromAcceptedLanguage(set<string> beginningStates, sequence prefix)
{
    sequence input;
    if (!this->hasNoSinkState && this->isConnected) {
        set<string> results;
        executingPath currentPath;
        reachableStates(this->initialState, currentPath, results, prefix, 0, 0);
        for (string key : results) {
            deque<ProductTransition *> res = Dijkstra(key);
            int time = 0;
            if (prefix.size() > 0)
                time = prefix[prefix.size()-1].second;
            for (auto transition : res) {
                if (transition->isTimeout)
                    time += atoi(transition->i.c_str());
                else {
                    time += transition->getGuard().tmin;
                    input.push_back(ts(transition->i, time));
                }
            }
            if (res.size() > 0)
                return input;
        }
    }
    return input;
}

void DistinguishingAutomaton_TFSM::initialize() {
    this->generateNext(this->initialState);
    this->isConnected = this->isProductConnected();
}
