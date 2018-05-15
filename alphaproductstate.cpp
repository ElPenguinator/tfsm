#include "alphaproductstate.h"

using namespace std;

AlphaProductState::AlphaProductState()
{
}

AlphaProductState::AlphaProductState(int s, int m, int xs, int xm, string prefix, int tcpt, int tmax)
{
    this->specificationState = s;
    this->mutationState = m;
    this->specificationCounter = xs;
    this->mutationCounter = xm;
    this->prefix = prefix;
    this->actualTime = tmax;
    this->timeBuffer = tcpt;
    this->isGenerated = false;
}

string AlphaProductState::getKey()
{
    return this->prefix + to_string(this->specificationState) + "#" + to_string(this->mutationState) + "#" + to_string(this->specificationCounter) + "#" + to_string(this->mutationCounter)+ "#" + to_string(this->timeBuffer);
}

string AlphaProductState::getProductKey()
{
    return to_string(this->specificationState) + "#" + to_string(this->mutationState) + "#" + to_string(this->specificationCounter) + "#" + to_string(this->mutationCounter);
}

AlphaProductSinkState::AlphaProductSinkState(int s, int m, int xs, int xm, string prefix, int tcpt, int tmax)
{
    this->specificationState = -1;
    this->mutationState = -1;
    this->specificationCounter = -1;
    this->mutationCounter = -1;
    this->prefix = "";
    this->actualTime = -1;
    this->timeBuffer = -1;
    this->isGenerated = true;
}

AlphaProductSinkState::AlphaProductSinkState()
{
    this->specificationState = -1;
    this->mutationState = -1;
    this->specificationCounter = -1;
    this->mutationCounter = -1;
    this->prefix = "";
    this->actualTime = -1;
    this->timeBuffer = -1;
    this->isGenerated = true;
}

string AlphaProductSinkState::getKey()
{
    return "sink";
}

string AlphaProductSinkState::getProductKey()
{
    return "sink";
}
