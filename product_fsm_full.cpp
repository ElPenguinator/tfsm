#include "product_fsm_full.h"
#include <iostream>

using namespace std;

Product_FSM_Full::Product_FSM_Full(TFSM_TO * S, TFSM_TO * M)
{
    this->specification = S;
    this->mutationMachine = M;

    ProductState * initialState = new ProductState(S->initialState, M->initialState, 0, 0);
    this->states.insert(make_pair(initialState->getKey(), initialState));
    this->initialState = initialState;
    this->generateNext(initialState);
}

void Product_FSM_Full::insertState(ProductState * state, string i, ProductState * newState, bool isTimeout, int id, bool isDistinguishable)
{

    if (this->states.find(newState->getKey()) == this->states.end()) {
        this->states.insert(make_pair(newState->getKey(), newState));
    }
    this->transitions.push_back(ProductTransition(state->getKey(), i, newState->getKey(), isTimeout, id, isDistinguishable));
}


void Product_FSM_Full::computeAlreadyDistinguishabled()
{
    bool noDifference = false;
    while (!noDifference) {
        noDifference = true;
        for (auto state : this->states) {
            if (!state.second->isAlreadyDistinguishabled) {
                bool isAlready = true;
                for (ProductTransition transition : this->transitions) {
                    if (transition.tgt == state.first && transition.src != state.first) {
                        if (!transition.isDistinguishable && !this->states.find(transition.src)->second->isAlreadyDistinguishabled) {
                            isAlready = false;
                        }
                    }
                }
                if (isAlready) {
                    state.second->isAlreadyDistinguishabled = true;
                    noDifference = false;
                }
            }
        }
    }
}


void Product_FSM_Full::generateNext(ProductState * state)
{
    for (auto mutationTransition : this->mutationMachine->lambda(state->mutationState)) {
        Transition * related = NULL;
        for (auto specificationTransition : this->specification->getXi(state->specificationState, mutationTransition.i)) {
            related = new Transition(specificationTransition.src, specificationTransition.i, specificationTransition.o, specificationTransition.tgt, specificationTransition.id);
        }
        if (related != NULL) {
            ProductState * newState;
            newState = new ProductState(related->tgt, mutationTransition.tgt, 0, 0);
            bool isDistinguishable = false;
            if (related->o != mutationTransition.o) {
                isDistinguishable = true;
            }
            this->insertState(state, related->i, newState, false, mutationTransition.id, isDistinguishable);
        }
        delete related;
    }

    /*
    Timeout * related = NULL;
    for (auto specificationTimeout : this->specification->delta(state->specificationState)) {
        related = new Timeout(specificationTimeout.src, specificationTimeout.t, specificationTimeout.tgt, specificationTimeout.id);//&specificationTimeout;
    }
    int spec_t = related->t;
    if (spec_t != inf)
        spec_t -= state->specificationCounter;

    for (auto mutationTimeout : this->mutationMachine->delta(state->mutationState)) {
        int muta_t = mutationTimeout.t;
        if (muta_t != inf)
            muta_t -= state->mutationCounter;
        if (muta_t > 0 && muta_t != inf) {
            ProductState * newState;
            if (muta_t < spec_t) {
                newState = new ProductState(state->specificationState, mutationTimeout.tgt, min(this->specification->getMaxDelta(state->specificationState), state->specificationCounter + muta_t), 0);
                this->insertState(state, to_string(muta_t), newState, true, mutationTimeout.id);
            }
            else if (muta_t == spec_t) {
                newState = new ProductState(related->tgt, mutationTimeout.tgt, 0, 0);
                this->insertState(state, to_string(muta_t), newState, true, mutationTimeout.id);
            }
            else if (muta_t > spec_t) {
                newState = new ProductState(related->tgt, state->mutationState, 0, min(this->specification->getMaxDelta(state->mutationState), state->mutationCounter + spec_t));
                this->insertState(state, to_string(spec_t), newState, true, mutationTimeout.id);
            }
        }
    }
    delete related;
    */
    state->isGenerated = true;

    vector<ProductTransition> copyTransitions(this->transitions);
    for (auto transition : copyTransitions) {
        if (transition.src == state->getKey()) {
            ProductState * tgt = this->states.find(transition.tgt)->second;
            if (!tgt->isGenerated)
                this->generateNext(tgt);
        }
    }
}

void Product_FSM_Full::minimalCheckingSequence()
{
    this->computeAlreadyDistinguishabled();

    /*
    for (auto state : this->states) {
        cout << state.first << " " << state.second->isAlreadyDistinguishabled << endl;
    }
    */

    set<string> initialState = {this->initialState->getKey()};
    map<int, bool> passingThrough;
    for (int i=this->specification->transitions.size() + this->specification->timeouts.size(); i<this->mutationMachine->transitions.size()  + this->mutationMachine->timeouts.size(); i++) {
        passingThrough.insert(make_pair(i, false));
    }

    map<string, bool> passingThroughDistinguishing;
    for (ProductTransition transition : this->transitions) {
        if (transition.isDistinguishable && !this->states.find(transition.src)->second->isAlreadyDistinguishabled)
            passingThroughDistinguishing.insert(make_pair(transition.getKey(), false));
    }

    ProductDeterministicExecutionState * firstState = new ProductDeterministicExecutionState(initialState, passingThroughDistinguishing, passingThrough, "", this->mutationMachine->inputs);

    bool haveFound = false;
    set<ProductDeterministicExecutionState*> toTreat = {firstState};
    int cpt = 0;

    /*
    this->mutationMachine->print();
    for (auto value : passingThrough) {
        cout << value.first << " " << value.second << endl;
    }
    for (auto value : passingThroughDistinguishing) {
        cout << value.first << " " << value.second << endl;
    }
    */

    while (!haveFound && cpt < 5) {
        set<ProductDeterministicExecutionState*> toTreatCopy = set<ProductDeterministicExecutionState*>(toTreat);
        toTreat.clear();
        for (auto state : toTreatCopy) {
            string prefix = state->prefix;
            for (string input : this->mutationMachine->inputs) {
                map<int, bool> passingThrough = map<int, bool>(state->passingThroughMutatedTransitions);
                map<string, bool> passingThroughDistinguishing = map<string, bool>(state->passingThroughDistinguishingTransitions);
                set<string> reachables;
                for (ProductTransition transition : this->transitions) {
                    if (state->states.find(transition.src) != state->states.end() && transition.i == input) {



                        if (passingThrough.find(transition.id) != passingThrough.end()) {
                            passingThrough.find(transition.id)->second = true;
                        }
                        if (transition.isDistinguishable) {
                            passingThroughDistinguishing.find(transition.getKey())->second = true;
                        }

                        //Check if this is a checking sequence
                        bool isOkay = true;
                        for (auto value : passingThrough) {
                            if (value.second == false)
                                isOkay = false;
                        }
                        for (auto value : passingThroughDistinguishing) {
                            if (value.second == false)
                                isOkay = false;
                        }
                        if (isOkay)
                            cout << "Yes ! " << prefix + input <<  " " << transition.getKey() << endl;


                        reachables.insert(transition.tgt);
                    }
                }
                ProductDeterministicExecutionState * child = new ProductDeterministicExecutionState(reachables, passingThroughDistinguishing, passingThrough, prefix + input, this->mutationMachine->inputs);
                state->children.find(input)->second = child;
                toTreat.insert(child);
            }
        }
        cpt++;
    }
}

void Product_FSM_Full::print()
{
    cout << "States : {";
    for (auto s : this->states) {
        cout << s.first << endl;
    }
    cout << "}" << endl;
    cout << "Transitions : {";
    for (auto t : this->transitions) {
        cout << "(" << t.src << "," << t.i << "," << t.tgt << ") : " << t.id << "," << endl;
    }
    cout << "}" << endl;
}
