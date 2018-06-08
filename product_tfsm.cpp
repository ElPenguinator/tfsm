#include "product_tfsm.h"
#include <limits>
#include <algorithm>
#include <iostream>
#include <queue>
#include "tools.h"
using namespace std;

Product_TFSM::Product_TFSM(TFSM * S, TFSM * M)
{
    this->specification = S;
    this->mutationMachine = M;

    ProductState * initialState = new ProductState(S->initialState, M->initialState, 0, 0);
    this->states.insert(make_pair(initialState->getKey(), initialState));
    this->initialState = initialState;
    this->hasNoSinkState = true;
    this->isConnected = true;
    this->generateNext(initialState);
    this->isConnected = this->isProductConnected();
}

void Product_TFSM::insertState(ProductState * state, string i, Guard g, ProductState * newState, bool isTimeout, int id)
{
    if (this->states.find(newState->getKey()) == this->states.end()) {
        this->states.insert(make_pair(newState->getKey(), newState));
    }
    this->transitions.push_back(GuardedProductTransition(state->getKey(), i, g, newState->getKey(), isTimeout, id, false));
}

void Product_TFSM::generateNext(ProductState * state)
{
    for (GuardedTransition mutationTransition : this->mutationMachine->lambda(state->mutationState)) {
        for (GuardedTransition specificationTransition : this->specification->getXi(state->specificationState, mutationTransition.i)) {
            if (!mutationTransition.g.isIntersectionEmpty(specificationTransition.g)) {

                ProductState * newState;
                if (specificationTransition.o == mutationTransition.o)
                    newState = new ProductState(specificationTransition.tgt, mutationTransition.tgt, 0, 0);
                else {
                    newState = new ProductSinkState();
                    this->hasNoSinkState = false;
                }
                this->insertState(state, specificationTransition.i, mutationTransition.g.intersect(specificationTransition.g), newState, false, mutationTransition.id);
            }
        }

    }

    Timeout * related = NULL;
    for (auto specificationTimeout : this->specification->delta(state->specificationState)) {
        related = new Timeout(specificationTimeout.src, specificationTimeout.t, specificationTimeout.tgt, specificationTimeout.id);//&specificationTimeout;
    }
    int spec_t = related->t;
    if (spec_t != inf)
        spec_t -= state->specificationCounter;

    Guard dummy("[", 0, inf, ")");

    for (auto mutationTimeout : this->mutationMachine->delta(state->mutationState)) {
        int muta_t = mutationTimeout.t;
        if (muta_t != inf)
            muta_t -= state->mutationCounter;
        if (muta_t > 0) {
            ProductState * newState;
            if (muta_t < spec_t) {
                newState = new ProductState(state->specificationState, mutationTimeout.tgt, min(this->specification->getMaxDelta(state->specificationState), state->specificationCounter + muta_t), 0);
                this->insertState(state, to_string(muta_t), dummy, newState, true, mutationTimeout.id);
            }
            else if (muta_t == spec_t) {
                newState = new ProductState(related->tgt, mutationTimeout.tgt, 0, 0);
                this->insertState(state, to_string(muta_t), dummy, newState, true, mutationTimeout.id);
            }
            else if (muta_t > spec_t) {
                newState = new ProductState(related->tgt, state->mutationState, 0, min(this->specification->getMaxDelta(state->mutationState), state->mutationCounter + spec_t));
                this->insertState(state, to_string(spec_t), dummy, newState, true, mutationTimeout.id);
            }
        }
    }
    delete related;
    state->isGenerated = true;

    vector<GuardedProductTransition> copyTransitions(this->transitions);
    for (auto transition : copyTransitions) {
        if (transition.src == state->getKey()) {
            ProductState * tgt = this->states.find(transition.tgt)->second;
            if (!tgt->isGenerated)
                this->generateNext(tgt);
        }
    }
}

bool Product_TFSM::isPathDeterministic(const path p)
{
    for (int id : p) {
        if (this->mutationMachine->isIdTimeout(id)) {
            vector<Timeout> xiTimeouts = this->mutationMachine->getXi(this->mutationMachine->getTimeoutFromId(id).src);
            for (auto otherTimeout : xiTimeouts) {
                if (find(p.begin(), p.end(), otherTimeout.id) != p.end() && otherTimeout.id != id)
                    return false;
            }
        }
        else {
            vector<GuardedTransition> xiTransitions = this->mutationMachine->getXi(this->mutationMachine->getTransitionFromId(id).src, this->mutationMachine->getTransitionFromId(id).i);
            for (auto otherTransition : xiTransitions) {
                if (find(p.begin(), p.end(), otherTransition.id) != p.end() && otherTransition.id != id)
                    return false;
            }
        }
    }
    return true;
}

bool Product_TFSM::isProductConnected()
{
    queue<string> fifo;
    set<string> marked;
    fifo.push(this->initialState->getKey());
    marked.insert(this->initialState->getKey());
    while (!fifo.empty()) {
        string element = fifo.front();
        fifo.pop();
        for (GuardedProductTransition transition : this->transitions) {
            if (!transition.isTimeout) {
                if (transition.src == element && marked.find(transition.tgt) == marked.end()) {
                    fifo.push(transition.tgt);
                    marked.insert(transition.tgt);
                }
            }
            else {
                if (atoi(transition.i.c_str()) != inf) {
                    if (transition.src == element && marked.find(transition.tgt) == marked.end()) {
                        fifo.push(transition.tgt);
                        marked.insert(transition.tgt);
                    }
                }
            }
        }
    }

    return marked.size() == this->states.size();
}

std::vector<path> Product_TFSM::revealingPaths(sequence alpha)
{
    vector<path> results;
    path currentPath;
    revealingPathsRecursive(this->initialState, currentPath, results, alpha, 0, 0);
    return results;
}

void Product_TFSM::revealingPathsRecursive(ProductState * state, path currentPath, vector<path> &results, sequence alpha, int sequenceIndex, int timeBuffer)
{
    if (state->getKey() == "sink") {
        results.push_back(currentPath);
    }
    else if (sequenceIndex < alpha.size()) {
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
                if (transition.src == state->getKey() && transition.isTimeout) {
                    int timeout = atoi(transition.i.c_str());
                    ProductState * tgtNode = this->states.find(transition.tgt)->second;
                    if (timeout <= t) {
                        path newPath(currentPath);
                        newPath.push_back(transition.id);
                        if (this->isPathDeterministic(newPath)) {
                            this->revealingPathsRecursive(tgtNode, newPath, results, alpha, sequenceIndex, timeBuffer + timeout);
                        }
                    }
                    else {
                        for (auto mutaTimeout : this->mutationMachine->delta(state->mutationState)) {
                            if (timeout < mutaTimeout.t) {
                                path newPath(currentPath);
                                newPath.push_back(transition.id);
                                if (this->isPathDeterministic(newPath)) {
                                    this->revealingPathsRecursive(state, newPath, results, alpha, sequenceIndex, symbol_time);
                                }
                            }
                        }
                    }
                }
            }
        }
        else {
            for (auto transition : this->transitions) {
                if (transition.src == state->getKey() && !transition.isTimeout) {
                    if (transition.i == symbol) {
                        ProductState * tgtNode = this->states.find(transition.tgt)->second;
                        path newPath(currentPath);
                        newPath.push_back(transition.id);
                        if (this->isPathDeterministic(newPath))
                            this->revealingPathsRecursive(tgtNode, newPath, results, alpha, sequenceIndex+1, 0);
                    }
                }
            }
        }
    }
}

void Product_TFSM::print()
{
    cout << "States : {";
    for (auto s : this->states) {
        cout << s.first << endl;
    }
    cout << "}" << endl;
    cout << "Transitions : {";
    for (auto t : this->transitions) {
        cout << "(" << t.src << "," << t.i << "," << t.g.toString() << "," << t.tgt << ") : " << t.id << "," << endl;
    }
    cout << "}" << endl;
}
