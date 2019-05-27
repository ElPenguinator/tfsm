#include "distinguishingautomaton_tfsm.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <queue>
#include "../tools.h"
#include "../algorithm/timedinputsequence.h"
using namespace std;

DistinguishingAutomaton_TFSM::DistinguishingAutomaton_TFSM(FSM *S, FSM *M) : DistinguishingAutomaton_TFSM_TO(S, M)
{
    this->specification = S;
    this->mutationMachine = M;
    this->nextID = 0;
    ProductState * initialState = new TimedProductState(S->initialState, M->initialState, 0, 0);
    initialState->id = nextID++;
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
        newState->id = nextID++;
        this->states.insert(make_pair(newState->getKey(), newState));
    }
    this->transitions.push_back(new GuardedProductTransition(state->getKey(), i, g, newState->getKey(), isTimeout, id));
}

void DistinguishingAutomaton_TFSM::generateNext(ProductState * state)
{
    for (IOTransition * mutationTransition : this->mutationMachine->lambda(state->mutationState)) {
        for (IOTransition * specificationTransition : this->specification->getXi(state->specificationState, mutationTransition->i)) {

            if (!mutationTransition->getGuard().substracted(state->getMutationCounter()).isIntersectionEmpty(specificationTransition->getGuard().substracted(state->getSpecificationCounter()))) {
                int maxTimeout = 0;
                for (auto t : this->mutationMachine->delta(state->mutationState)) {
                        if (t->t > maxTimeout)
                            maxTimeout = t->t;
                }
                maxTimeout = min(maxTimeout, this->specification->delta(state->specificationState).front()->t);
                Guard newGuard = mutationTransition->getGuard().substracted(state->getMutationCounter()).intersect(specificationTransition->getGuard().substracted(state->getSpecificationCounter()));

                if (newGuard.tmin < maxTimeout) {
                ProductState * newState;
                if (specificationTransition->o == mutationTransition->o)
                    newState = new TimedProductState(specificationTransition->tgt, mutationTransition->tgt, 0, 0);
                else {
                    newState = new TimedProductSinkState();
                    this->hasNoSinkState = false;
                }
                //cout << "State : " << state->getKey() << " MaxTimeout : " << maxTimeout << " Guard : " << newGuard.toString() << " Target : " << newState->getKey() << endl;
                this->insertState(state, specificationTransition->i, newGuard, newState, false, mutationTransition->id);
                }
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

std::vector<executingPath> DistinguishingAutomaton_TFSM::revealingPaths(Sequence * alpha)
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

void DistinguishingAutomaton_TFSM::revealingPathsRecursive(ProductState * state, executingPath currentPath, vector<executingPath> &results, Sequence * alpha, int sequenceIndex, double timeBuffer, double timeLeftOver)
{
    if (state->getKey() == "sink") {
        results.push_back(currentPath);
    }
    else if (sequenceIndex < alpha->getSize()) {
        ts timed_symbol = dynamic_cast<TimedInputSequence *>(alpha)->getElement(sequenceIndex);
        string symbol = timed_symbol.first;
        double symbol_time = timed_symbol.second;
        double t = symbol_time - timeBuffer;
        //cout << "State : " << state->getKey() << " Time : " << std::setprecision(3) << t << " Symbol : " << symbol << endl;
        /*
        if (sequenceIndex > 0) {
            t -= dynamic_cast<TimedIntervalInputSequence *>(alpha)->getElement(sequenceIndex - 1).second.tmin;
            symbol_time = symbol_time.substracted(dynamic_cast<TimedIntervalInputSequence *>(alpha)->getElement(sequenceIndex - 1).second);
        }
        */
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
                            this->revealingPathsRecursive(tgtNode, newPath, results, alpha, sequenceIndex, timeBuffer + timeout, 0);
                        }
                    }
                    else {
                        for (auto mutaTimeout : this->mutationMachine->delta(state->mutationState)) {
                            if (t <= mutaTimeout->t) {
                                executingPath newPath(currentPath);
                                newPath.push_back(transition->id);
                                if (this->isPathDeterministic(newPath)) {
                                    this->revealingPathsRecursive(state, newPath, results, alpha, sequenceIndex, symbol_time, t);
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
    //this->print();
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
                    int maxTimeout = 0;
                    for (auto t : this->mutationMachine->delta(this->states.find(transition->src)->second->mutationState)) {
                            if (t->t > maxTimeout)
                                maxTimeout = t->t;
                    }
                    maxTimeout = min(maxTimeout, this->specification->delta(this->states.find(transition->src)->second->specificationState).front()->t);
                    if (transition->getGuard().tmin < maxTimeout) {
                        DijkstraUpdateDistancesMin(distances, predecessors, s1, transition->tgt, transition);
                    }
                }
            }
        }
    }
    /*
    cout << "Dijkstra (distances) : " << endl;
    for (const auto dist : distances) {
        cout << dist.first << " " << dist.second << endl;
    }
    cout << "Dijkstra (predcessors) : " << endl;
    for (const auto pred : predecessors) {
        cout << pred.first << " " << pred.second->src << endl;
    }
    */

    string currentStateKey = "sink";
    deque<ProductTransition *> results;

    if (distances.find("sink")->second != inf) {
        //cout << "Sink accessible" << endl;
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

void DistinguishingAutomaton_TFSM::reachableStates(ProductState * state, executingPath currentPath, set<string> * results, Sequence * alpha, int sequenceIndex, int timeBuffer, int timeLeftOver)
{
    //cout << "State : " << state->getKey() << endl;
    if (state->getKey() != "sink") {
        if (sequenceIndex < alpha->getSize()) {

            ts timed_symbol = dynamic_cast<TimedInputSequence *>(alpha)->getElement(sequenceIndex);
            string symbol = timed_symbol.first;

            int symbol_time = timed_symbol.second;
            //cout << "Symbol : " << symbol << " " << symbol_time.toString() << endl;
            int t = symbol_time - timeBuffer;
            /*
            if (sequenceIndex > 0) {
                t -= dynamic_cast<TimedIntervalInputSequence *>(alpha)->getElement(sequenceIndex - 1).second.tmin;
                symbol_time = symbol_time.substracted(dynamic_cast<TimedIntervalInputSequence *>(alpha)->getElement(sequenceIndex - 1).second);
            }
            */
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
                                this->reachableStates(tgtNode, newPath, results, alpha, sequenceIndex, timeBuffer + timeout, 0);
                            }
                        }
                        else {
                            for (auto mutaTimeout : this->mutationMachine->delta(state->mutationState)) {
                                if (t <= mutaTimeout->t) {
                                    executingPath newPath(currentPath);
                                    newPath.push_back(transition->id);
                                    if (this->isPathDeterministic(newPath)) {
                                        this->reachableStates(state, newPath, results, alpha, sequenceIndex, symbol_time, t);
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
                        if (transition->i == symbol && transition->getGuard().contains(timeLeftOver)) {
                            ProductState * tgtNode = this->states.find(transition->tgt)->second;
                            executingPath newPath(currentPath);
                            newPath.push_back(transition->id);
                            if (this->isPathDeterministic(newPath))
                                this->reachableStates(tgtNode, newPath, results, alpha, sequenceIndex+1, 0, 0);
                        }
                    }
                }
            }
        }
        else {
            //cout << "Add to result : " << state->getKey() << endl;
            results->insert(state->getKey());
        }
    }

}

Sequence * DistinguishingAutomaton_TFSM::inputSequenceFromAcceptedLanguage(set<string> beginningStates, Sequence * prefix)
{
    TimedInputSequence * input = new TimedInputSequence();
    if (!this->hasNoSinkState && this->isConnected) {
        cout << "Okay ?" << endl;
        set<string> * results = new set<string>();
        executingPath currentPath;
        reachableStates(this->initialState, currentPath, results, prefix, 0, 0, 0);
        for (string key : (*results)) {
            cout << "Result : " << key << " : ";
            deque<ProductTransition *> res = Dijkstra(key);
            double time = 0;
            if (prefix->getSize() > 0) {
                time = dynamic_cast<TimedInputSequence *>(prefix)->getElement(prefix->getSize()-1).second;
            }
            for (auto transition : res) {
                cout << "Transition" << endl;
                GuardedProductTransition * guardedTransition = dynamic_cast<GuardedProductTransition *>(transition);
                if (guardedTransition->isTimeout) {
                    time = time + atoi(guardedTransition->i.c_str());
                }
                else {
                    //input->addElement(pair<string, Guard>(guardedTransition->i, guardedTransition->getGuard().added(time)));
                    time = guardedTransition->getGuard().tmin + time;
                    if (guardedTransition->getGuard().left == Bracket::Curly) {
                        time += 0.01;
                    }
                    input->addElement(ts(transition->i, time));

                    //time = 0;
                }
            }
            if (res.size() > 0) {
                return input;
            }
        }
        delete results;
    }
    else {
        cout << "Sink or not connected ! No Sink : " << this->hasNoSinkState << " Connected : " << this->isConnected << endl;
    }
    return input;
}

void DistinguishingAutomaton_TFSM::initialize() {
    this->generateNext(this->initialState);
    //this->isConnected = this->isProductConnected();
    this->isConnected = true;
}

string DistinguishingAutomaton_TFSM::generateDot()
{
    ostringstream res;
    res << "digraph DistinguishingTFSM {" << endl << "forcelabels=true;" << endl;
    for (auto s : this->states) {
        //res << s.first << " [label=\"" << s.second->specificationState << " " << s.second->mutationState << " " << s.second->specificationCounter << " " << s.second->mutationCounter << "\"];" << endl;
        //res << s.first << " [label=\"" << s.second->specificationState << " " << s.second->mutationState << "\"];" << endl;
        if (s.first == "sink") {
            res << s.second->id << " [label=\"∇\"];" << endl;
        }
        else {
            //res << s.second->id << " [label=\"" << s.second->specificationState << " " << s.second->mutationState << " " << s.second->getSpecificationCounter() << " " << s.second->getMutationCounter() << "\"];" << endl;
            res << s.second->id << " [label=\"" << s.second->specificationState << " " << s.second->mutationState << " ";
            if (s.second->getSpecificationCounter() == inf)
                res << "∞";
            else
                res << s.second->getSpecificationCounter();
            res << " ";
            if (s.second->getMutationCounter() == inf)
                res << "∞";
            else
                res << s.second->getMutationCounter();
            res << "\"];" << endl;
        }
    }
    /*
    for (ProductTransition * t : this->transitions) {
        res << t->src << " -> " << t->tgt << " [label=\"" << t->i<< "\"];" << endl;
    }
    */
    for (ProductTransition * t : this->transitions) {
        if (t->isTimeout) {
            if (atoi(t->i.c_str()) == inf) {
                res << (*this->states.find(t->src)).second->id << " -> " << (*this->states.find(t->tgt)).second->id << " [label=\"" << "∞" << " [" << t->id << "]\"];" << endl;
            }
            else {
                res << (*this->states.find(t->src)).second->id << " -> " << (*this->states.find(t->tgt)).second->id << " [label=\"" << t->i << " [" << t->id << "]\"];" << endl;
            }
        }
        else {
            res << (*this->states.find(t->src)).second->id << " -> " << (*this->states.find(t->tgt)).second->id << " [label=\"" << t->getGuard().toString() << " " << t->i << " [" << t->id << "]\"];" << endl;
        }
    }
    res << "}" << endl;
    return res.str();
}
