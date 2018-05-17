#ifndef PRODUCT_TFSM_TO_H
#define PRODUCT_TFSM_TO_H
#include "tfsm_to.h"
#include <map>
#include <deque>
#include "productstate.h"
#include "structs.h"
class Product_TFSM_TO
{
private:
    bool isProductConnected();
public:
    TFSM_TO * specification;
    TFSM_TO * mutationMachine;
    std::map<std::string, ProductState*> states;
    ProductState * initialState;
    std::vector<ProductTransition> transitions;
    Product_TFSM_TO(TFSM_TO * S, TFSM_TO * M);
    bool hasNoSinkState;
    bool isConnected;
    void generateNext(ProductState * state);
    void insertState(ProductState * state, std::string i, ProductState * newState, bool isTimeout, int id);
    std::vector<path> revealingPaths(sequence alpha);
    bool isPathDeterministic(const path p);
    void revealingPathsRecursive(ProductState * state, path currentPath, std::vector<path> & results, sequence alpha, int sequenceIndex, int timeBuffer);
    void print();
    sequence inputSequenceFromAcceptedLanguage(std::set<std::string> beginningStates, sequence prefix);
    void reachableStates(ProductState * state, path currentPath, std::set<std::string> &results, sequence alpha, int sequenceIndex, int timeBuffer);
    std::deque<ProductTransition> Dijkstra(std::string key);
    std::vector<path> revealingPathsPrefixed(std::string beginningStateKey, sequence alpha);
};

#endif // PRODUCT_TFSM_TO_H
