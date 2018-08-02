#include "productstate.h"
using namespace std;
ProductState::ProductState()
{
}

ProductState::ProductState(int s, int m)
{
    this->specificationState = s;
    this->mutationState = m;
    this->isGenerated = false;
}

std::string ProductState::getKey()
{
    return to_string(this->specificationState) + "#" + to_string(this->mutationState);
}

ProductSinkState::ProductSinkState(int s, int m)
{
    this->specificationState = -1;
    this->mutationState = -1;
    this->isGenerated = true;
}

ProductSinkState::ProductSinkState()
{
    this->specificationState = -1;
    this->mutationState = -1;
    this->isGenerated = true;
}

std::string ProductSinkState::getKey()
{
    return "sink";
}

TimedProductState::TimedProductState()
{
}

TimedProductState::TimedProductState(int s, int m, int xs, int xm) : ProductState(s, m)
{
    this->specificationCounter = xs;
    this->mutationCounter = xm;
}

std::string TimedProductState::getKey()
{
    return to_string(this->specificationState) + "#" + to_string(this->mutationState) + "#" + to_string(this->specificationCounter) + "#" + to_string(this->mutationCounter);
}

TimedProductSinkState::TimedProductSinkState(int s, int m, int xs, int xm)
{
    this->specificationState = -1;
    this->mutationState = -1;
    this->specificationCounter = -1;
    this->mutationCounter = -1;
    this->isGenerated = true;
}

TimedProductSinkState::TimedProductSinkState()
{
    this->specificationState = -1;
    this->mutationState = -1;
    this->specificationCounter = -1;
    this->mutationCounter = -1;
    this->isGenerated = true;
}

std::string TimedProductSinkState::getKey()
{
    return "sink";
}

int ProductState::getSpecificationCounter()
{
    return 0;
}

int ProductState::getMutationCounter()
{
    return 0;
}

int TimedProductState::getSpecificationCounter()
{
    return this->specificationCounter;
}

int TimedProductState::getMutationCounter()
{
    return this->mutationCounter;
}
