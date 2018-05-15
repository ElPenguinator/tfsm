#include "alpha_product_tfsm_to.h"
#include <unordered_map>
using namespace std;

Alpha_Product_TFSM_TO::Alpha_Product_TFSM_TO(Product_TFSM_TO *P, sequence alpha)
{
    this->product = P;
    this->alpha = alpha;

    AlphaProductState * initialState = new AlphaProductState(P->specification->initialState, P->mutationMachine->initialState, 0, 0, "", 0, 0);
    this->states.insert(make_pair(initialState->getKey(), initialState));
    this->initialState = initialState;

    this->hasNoSinkState = false;
    this->generateNext(initialState, 0);
}

void Alpha_Product_TFSM_TO::insertState(AlphaProductState * state, string i, AlphaProductState * newState, bool isTimeout, int id)
{
    if (this->states.find(newState->getKey()) == this->states.end()) {
        this->states.insert(make_pair(newState->getKey(), newState));
    }
    this->transitions.push_back(ProductTransition(state->getKey(), i, newState->getKey(), isTimeout, id));
}

void Alpha_Product_TFSM_TO::generateNext(AlphaProductState * state, int sequenceIndex)
{
    ts timed_symbol = this->alpha[sequenceIndex];
    string symbol = timed_symbol.first;
    int symbol_time = timed_symbol.second;
    int t = timed_symbol.second - state->timeBuffer;
    if (sequenceIndex > 0) {
        t -= this->alpha[sequenceIndex-1].second;
        symbol_time -= this->alpha[sequenceIndex-1].second;
    }

    string prefix = "";
    for (int i=0; i<sequenceIndex+1; i++) {
        prefix += this->alpha[i].first;
    }

    bool needOutputGenerating = true;
    map<string, bool> needOutputGeneratingMap;

    for (auto transition : this->product->transitions) {
        needOutputGeneratingMap.insert(make_pair(transition.getKey(), true));
    }

    if (t > 0) {
        for (auto transition : this->product->transitions) {
            if (transition.src == state->getProductKey() && transition.isTimeout) {
                needOutputGenerating = false;
                int timeout = atoi(transition.i.c_str());
                AlphaProductState * newState;

                if (t >= timeout) {
                    auto it = needOutputGeneratingMap.find(transition.getKey());
                    if (it != needOutputGeneratingMap.end())
                        it->second = false;
                    ProductState * ps = this->product->states.find(transition.tgt)->second;
                    newState = new AlphaProductState(ps->specificationState, ps->mutationState, ps->specificationCounter, ps->mutationCounter, state->prefix, timeout + state->timeBuffer, symbol_time);
                }
                else {
                    newState = new AlphaProductState(state->specificationState, state->mutationState, state->specificationCounter, state->mutationCounter, state->prefix, t + state->timeBuffer, symbol_time);
                }
                this->insertState(state, transition.i, newState, true, transition.id);
            }
        }
    }

    if (needOutputGenerating) {
        for (auto transition : this->product->transitions) {
            if (transition.src == state->getProductKey()
                    && needOutputGeneratingMap.find(transition.getKey()) != needOutputGeneratingMap.end()
                    && !transition.isTimeout
                    && transition.i == symbol) {
                ProductState * ps = this->product->states.find(transition.tgt)->second;
                AlphaProductState * newState;
                if (ps->getKey() == "sink") {
                    newState = new AlphaProductSinkState();
                    this->hasNoSinkState = false;
                }
                else {
                    newState = new AlphaProductState(ps->specificationState, ps->mutationState, ps->specificationCounter, ps->mutationCounter, prefix, 0, 0);
                }
                this->insertState(state, transition.i, newState, false, transition.id);
            }
        }
    }
    state->isGenerated = true;

    vector<ProductTransition> copyTransitions(this->transitions);
    for (auto transition : copyTransitions) {
        if (transition.src == state->getKey()) {
            AlphaProductState * tgt = this->states.find(transition.tgt)->second;
            if (!tgt->isGenerated)
                this->generateNext(tgt, sequenceIndex + needOutputGenerating);
        }
    }
}
/*
std::vector<path> Alpha_Product_TFSM_TO::revealingPaths()
{
    vector<path> results;
    path currentPath;
    revealingPathsRecursive(this->initialState, currentPath, results, false);

}

void Alpha_Product_TFSM_TO::revealingPathsRecursive(AlphaProductState * state, path currentPath, vector<path> & results, bool isWaiting)
{
    if (state->getKey() == "sink") {
        results.push_back(currentPath);
    }
    else {
        for (auto transition : this->transitions) {
            if (transition.src == state->getKey()) {
                AlphaProductState * tgtNode = this->states.find(transition.tgt)->second;
                vector<path> newPaths;
                bool wait = false;
                if (!transition.isTimeout || tgtNode->mutationCounter == 0) {
                    path newPath(currentPath);
                    newPath.push_back(transition.id);
                    newPaths.push_back(newPath);
                }
                else {
                    if (!isWaiting) {
                        for (auto mutaTimeout : this->product->mutationMachine->timeouts) {
                            if (this->product->mutationMachine->getTimeoutFromId(mutaTimeout.id)) {
                                path newPath(currentPath);
                                newPath.push_back(transition.id);
                                newPaths.push_back(newPath);
                            }
                        }
                    }
                }
            }
        }
    }
}
*/
