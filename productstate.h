#ifndef PRODUCTSTATE_H
#define PRODUCTSTATE_H
#include <string>
#include <vector>
#include "tfsm_to.h"
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
};

class ProductState
{
public:
    int specificationState;
    int mutationState;
    int specificationCounter;
    int mutationCounter;
    int inputDistance;
    int timeDistance;
    bool isGenerated;
    ProductState();
    ProductState(int s, int m, int xs, int xm);
    virtual std::string getKey();

};

class ProductSinkState : public ProductState
{
public:
    ProductSinkState(int s, int m, int xs, int xm);
    ProductSinkState();
    std::string getKey() override;
};
#endif // PRODUCTSTATE_H
