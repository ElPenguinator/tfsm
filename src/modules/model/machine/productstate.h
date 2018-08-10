#ifndef PRODUCTSTATE_H
#define PRODUCTSTATE_H
#include <string>
#include <vector>
#include "tfsm_to.h"
#include "tfsm.h"

class ProductTransition {
public:
    std::string src;
    std::string i;
    std::string tgt;
    int id;
    bool isTimeout;
    ProductTransition(std::string src, std::string i, std::string tgt, bool isTimeout, int id) {
        this->src = src;
        this->i = i;
        this->tgt = tgt;
        this->isTimeout = isTimeout;
        this->id = id;
    }
    std::string getKey()
    {
        return this->src + "|" + this->i + "|" + this->tgt;
    }

    virtual Guard getGuard() {
        return Guard("[", 0, inf, ")");
    }
};

class GuardedProductTransition : public ProductTransition {
public:
    Guard g;
    GuardedProductTransition(std::string src, std::string i, Guard g, std::string tgt, bool isTimeout, int id) : ProductTransition(src, i, tgt, isTimeout, id){
        this->g = g;
    }
    std::string getKey()
    {
        return this->src + "|" + this->i +"|" + this->g.toString() +"|" + this->tgt;
    }

    Guard getGuard()
    {
        return this->g;
    }
};

class ProductState
{
public:
    int specificationState;
    int mutationState;
    int id;
    bool isGenerated;
    ProductState();
    ProductState(int s, int m);
    virtual std::string getKey();
    virtual int getSpecificationCounter();
    virtual int getMutationCounter();
};

class ProductSinkState : public ProductState
{
public:
    ProductSinkState();
    ProductSinkState(int s, int m);
    virtual std::string getKey();

};

class TimedProductState : public ProductState
{
public:
    int specificationCounter;
    int mutationCounter;
    TimedProductState();
    TimedProductState(int s, int m, int xs, int xm);
    virtual std::string getKey();
    virtual int getSpecificationCounter();
    virtual int getMutationCounter();
};

class TimedProductSinkState : public TimedProductState
{
public:
    TimedProductSinkState(int s, int m, int xs, int xm);
    TimedProductSinkState();
    std::string getKey() override;
};

#endif // PRODUCTSTATE_H
