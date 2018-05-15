#ifndef ALPHA_PRODUCT_TFSM_TO_H
#define ALPHA_PRODUCT_TFSM_TO_H
#include "product_tfsm_to.h"
#include "alphaproductstate.h"
#include "productstate.h"
#include "structs.h"

class Alpha_Product_TFSM_TO
{
private:
    Product_TFSM_TO * product;
    std::map<std::string, AlphaProductState*> states;
    std::vector<ProductTransition> transitions;
    sequence alpha;
    AlphaProductState * initialState;
public:
    Alpha_Product_TFSM_TO(Product_TFSM_TO * P, sequence alpha);
    bool hasNoSinkState;
    void generateNext(AlphaProductState * state, int sequenceIndex);
    void insertState(AlphaProductState * state, std::string i, AlphaProductState * newState, bool isTimeout, int id);
    /*
    std::vector<path> revealingPaths();
    void revealingPathsRecursive(AlphaProductState * state, path currentPath, std::vector<path> & results, bool isWaiting);
    */
};

#endif // ALPHA_PRODUCT_TFSM_TO_H
