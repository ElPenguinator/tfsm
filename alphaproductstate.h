#ifndef ALPHAPRODUCTSTATE_H
#define ALPHAPRODUCTSTATE_H
#include <string>

class AlphaProductState
{
public:
    AlphaProductState(int s, int m, int xs, int xm, std::string prefix, int tcpt, int tmax);
    AlphaProductState();
    int specificationState;
    int mutationState;
    int specificationCounter;
    int mutationCounter;
    std::string prefix;
    int timeBuffer;
    int actualTime;
    bool isGenerated;
    virtual std::string getKey();
    virtual std::string getProductKey();
};

class AlphaProductSinkState : public AlphaProductState
{
public:
    AlphaProductSinkState(int s, int m, int xs, int xm, std::string prefix, int tcpt, int tmax);
    AlphaProductSinkState();
    std::string getKey() override;
    std::string getProductKey() override;
};

#endif // ALPHAPRODUCTSTATE_H
