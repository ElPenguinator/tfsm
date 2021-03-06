#include "distinguishingautomaton_fsm.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <queue>
#include "../tools.h"
#include "../algorithm/inputsequence.h"
using namespace std;

DistinguishingAutomaton_FSM::DistinguishingAutomaton_FSM(FSM * S, FSM * M)
{
    this->specification = S;
    this->mutationMachine = M;
    this->nextID = 0;
    ProductState * initialState = new ProductState(S->getInitialState(), M->getInitialState());
    initialState->id = nextID++;
    this->states.insert(make_pair(initialState->getKey(), initialState));
    this->initialState = initialState;
    this->hasNoSinkState = true;
    this->isConnected = true;
}

void DistinguishingAutomaton_FSM::insertState(ProductState * state, string i, ProductState * newState, bool isTimeout, int id)
{
    if (this->states.find(newState->getKey()) == this->states.end()) {
        newState->id = nextID++;
        this->states.insert(make_pair(newState->getKey(), newState));
    }
    this->transitions.push_back(new ProductTransition(state->getKey(), i, newState->getKey(), isTimeout, id));
}

void DistinguishingAutomaton_FSM::generateNext(ProductState * state)
{
    for (auto mutationTransition : this->mutationMachine->lambda(state->mutationState)) {
        IOTransition * related = NULL;
        for (auto specificationTransition : this->specification->getXi(state->specificationState, mutationTransition->i)) {
            related = new IOTransition(specificationTransition->src, specificationTransition->i, specificationTransition->o, specificationTransition->tgt, specificationTransition->id);
        }
        if (related != NULL) {
            ProductState * newState;
            if (related->o == mutationTransition->o)
                newState = new ProductState(related->tgt, mutationTransition->tgt);
            else {
                newState = new ProductSinkState();
                this->hasNoSinkState = false;
            }

            this->insertState(state, related->i, newState, false, mutationTransition->id);
        }
        delete related;
    }
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

std::vector<executingPath> DistinguishingAutomaton_FSM::revealingPaths(Sequence * alpha)
{
    vector<executingPath> results;
    executingPath currentPath;
    revealingPathsRecursive(this->initialState, currentPath, results, alpha, 0, 0);
    return results;
}

bool DistinguishingAutomaton_FSM::isPathDeterministic(const executingPath p)
{
    for (int id : p) {
        vector<IOTransition *> xiTransitions = this->mutationMachine->getXi(this->mutationMachine->getTransitionFromId(id)->src, this->mutationMachine->getTransitionFromId(id)->i);
        for (auto otherTransition : xiTransitions) {
            if (find(p.begin(), p.end(), otherTransition->id) != p.end() && otherTransition->id != id)
                return false;
        }
    }
    return true;
}

void DistinguishingAutomaton_FSM::revealingPathsRecursive(ProductState * state, executingPath currentPath, vector<executingPath> &results, Sequence * alpha, int sequenceIndex, int timeBuffer)
{
    if (state->getKey() == "sink") {
        results.push_back(currentPath);
    }
    else if (sequenceIndex < alpha->getSize()) {
        string symbol = dynamic_cast<InputSequence *>(alpha)->getElement(sequenceIndex);
        //Time to spend, so take only timeouts
        for (auto transition : this->transitions) {
            if (transition->src == state->getKey() && !transition->isTimeout) {
                if (transition->i == symbol) {
                    ProductState * tgtNode = this->states.find(transition->tgt)->second;
                    executingPath newPath(currentPath);
                    newPath.push_back(transition->id);
                    if (this->isPathDeterministic(newPath))
                        this->revealingPathsRecursive(tgtNode, newPath, results, alpha, sequenceIndex+1, 0);
                }
            }
        }
    }
}


void DistinguishingAutomaton_FSM::print()
{
    cout << "States : {";
    for (auto s : this->states) {
        cout << s.first << endl;
    }
    cout << "}" << endl;
    cout << "Transitions : {";
    for (auto t : this->transitions) {
        cout << "(" << t->src << "," << t->i << "," << t->tgt << ") : " << t->id << "," << endl;
    }
    cout << "}" << endl;
}

string DistinguishingAutomaton_FSM::DijkstraFindMin(map<string, int> distances, set<string> Q) {
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

void DistinguishingAutomaton_FSM::DijkstraUpdateDistancesMin(map<string, int> & distances, std::map<string, ProductTransition *> &predecessors, string s1, string s2, ProductTransition *transition) {
    if (distances.find(s2)->second > distances.find(s1)->second + 1) {
        if (distances.find(s1)->second != inf)
            distances.find(s2)->second = distances.find(s1)->second + 1;
        predecessors.find(s2)->second = transition;
    }
}

bool DistinguishingAutomaton_FSM::isProductConnected()
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

std::deque<ProductTransition *> DistinguishingAutomaton_FSM::Dijkstra(string key)
{
    map<string, ProductTransition *> predecessors;
    set<string> Q;
    map<string, int> distances;
    for (const auto &s : this->states) {
        string stateKey = s.first;
        Q.insert(stateKey);
        predecessors.insert(make_pair(stateKey, new ProductTransition("", "", "", false, -1)));
        distances.insert(make_pair(stateKey, inf));
    }
    distances.find(key)->second = 0;

    while (Q.size() > 0) {
        string s1 = DijkstraFindMin(distances, Q);
        Q.erase(Q.find(s1));
        for (auto transition : this->transitions) {
            if (transition->src == s1) {
                if (atoi(transition->i.c_str()) != inf) {
                    DijkstraUpdateDistancesMin(distances, predecessors, s1, transition->tgt, transition);
                }
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

            results.push_front(predecessors.find(currentStateKey)->second);
            currentStateKey = predecessors.find(currentStateKey)->second->src;
        }
    }
    return results;
}

void DistinguishingAutomaton_FSM::reachableStates(ProductState * state, executingPath currentPath, set<string> * results, Sequence * alpha, int sequenceIndex, int timeBuffer)
{
    if (state->getKey() != "sink") {
        if (sequenceIndex < alpha->getSize()) {
            string symbol = dynamic_cast<InputSequence *>(alpha)->getElement(sequenceIndex);
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
        else {
            results->insert(state->getKey());
        }
    }
}

Sequence * DistinguishingAutomaton_FSM::inputSequenceFromAcceptedLanguage(set<string> beginningStates, Sequence * prefix)
{
    InputSequence * input = new InputSequence();
    if (!this->hasNoSinkState && this->isConnected) {
        set<string> * results = new set<string>();
        executingPath currentPath;
        reachableStates(this->initialState, currentPath, results, prefix, 0, 0);
        for (string key : (*results)) {
            deque<ProductTransition *> res = Dijkstra(key);
            for (auto transition : res) {
                input->addElement(transition->i);
            }
            if (res.size() > 0)
                return input;
        }
        delete results;
    }
    return input;
}

void DistinguishingAutomaton_FSM::initialize() {
    this->generateNext(this->initialState);
    this->isConnected = this->isProductConnected();
}

string DistinguishingAutomaton_FSM::generateDot()
{
    ostringstream res;
    res << "digraph DistinguishingFSM {" << endl << "forcelabels=true;" << endl;
    for (auto s : this->states) {
        if (s.first == "sink") {
            res << s.second->id << " [label=\"∇\"];" << endl;
        }
        else {
            res << s.second->id << " [label=\"" << s.second->specificationState << " " << s.second->mutationState << "\"];" << endl;
        }
    }
    for (ProductTransition * t : this->transitions) {
        res << (*this->states.find(t->src)).second->id << " -> " << (*this->states.find(t->tgt)).second->id << " [label=\"" << t->i << " [" << t->id << "]\"];" << endl;
    }
    res << "}" << endl;
    return res.str();
}
