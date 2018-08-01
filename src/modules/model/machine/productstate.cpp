#include "productstate.h"

using namespace std;
ProductState::ProductState()
{
}

ProductState::ProductState(int s, int m, int xs, int xm)
{
    this->specificationState = s;
    this->mutationState = m;
    this->specificationCounter = xs;
    this->mutationCounter = xm;
    this->isGenerated = false;
    this->inputDistance = 0;
    this->timeDistance = 0;
    this->isAlreadyDistinguishabled = false;
}

std::string ProductState::getKey()
{
    return to_string(this->specificationState) + "#" + to_string(this->mutationState) + "#" + to_string(this->specificationCounter) + "#" + to_string(this->mutationCounter);
}

ProductSinkState::ProductSinkState(int s, int m, int xs, int xm)
{
    this->specificationState = -1;
    this->mutationState = -1;
    this->specificationCounter = -1;
    this->mutationCounter = -1;
    this->isGenerated = true;
}

ProductSinkState::ProductSinkState()
{
    this->specificationState = -1;
    this->mutationState = -1;
    this->specificationCounter = -1;
    this->mutationCounter = -1;
    this->isGenerated = true;
}

std::string ProductSinkState::getKey()
{
    return "sink";
}

ProductDeterministicExecutionState::ProductDeterministicExecutionState(std::set<std::string> states, std::map<string, bool> passingThroughDistinguishingTransitions, std::map<int, bool> passingThrough, std::string prefix, std::set<std::string> inputs)
{
    this->states = states;
    this->passingThroughMutatedTransitions = passingThrough;
    this->passingThroughDistinguishingTransitions = passingThroughDistinguishingTransitions;
    this->prefix = prefix;
}
