#ifndef PRODUCT_FSM_TO_FULL_H
#define PRODUCT_FSM_TO_FULL_H
#include "tfsm_to.h"
#include "productstate.h"
#include "structs.h"
class Product_FSM_Full
{
public:
    TFSM_TO * specification;
    TFSM_TO * mutationMachine;
    std::map<std::string, ProductState*> states;
    ProductState * initialState;
    std::vector<ProductTransition> transitions;
    Product_FSM_Full(TFSM_TO * S, TFSM_TO * M);
    void generateNext(ProductState * state);
    void insertState(ProductState * state, std::string i, ProductState * newState, bool isTimeout, int id, bool isDistinguishable);
    void minimalCheckingSequence();
    void print();
    void computeAlreadyDistinguishabled();
};

#endif // PRODUCT_FSM_TO_FULL_H
