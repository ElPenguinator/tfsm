#ifndef PRODUCT_TFSM_H
#define PRODUCT_TFSM_H
#include "tfsm.h"
#include <map>
#include <deque>
#include "productstate.h"
#include "../structs.h"
class Product_TFSM
{
private:
    bool isProductConnected();
public:
    TFSM * specification;
    TFSM * mutationMachine;
    std::map<std::string, ProductState*> states;
    ProductState * initialState;
    std::vector<GuardedProductTransition> transitions;
    Product_TFSM(TFSM * S, TFSM * M);
    bool hasNoSinkState;
    bool isConnected;
    void generateNext(ProductState * state);
    void insertState(ProductState * state, std::string i, Guard g, ProductState * newState, bool isTimeout, int id);
    std::vector<path> revealingPaths(sequence alpha);
    bool isPathDeterministic(const path p);
    void revealingPathsRecursive(ProductState * state, path currentPath, std::vector<path> & results, sequence alpha, int sequenceIndex, int timeBuffer, int timeLeftOver);
    void print();
    sequence inputSequenceFromAcceptedLanguage(std::set<std::string> beginningStates, sequence prefix);
    void reachableStates(ProductState * state, path currentPath, std::set<std::string> &results, sequence alpha, int sequenceIndex, int timeBuffer);
    std::string DijkstraFindMin(std::map<std::string, int> distances, std::set<std::string> Q);
    void DijkstraUpdateDistancesMin(std::map<std::string, int> & distances, std::map<std::string, GuardedProductTransition> & predecessors, std::string s1, std::string s2, GuardedProductTransition transition);
    std::deque<GuardedProductTransition> Dijkstra(std::string key);
    std::vector<path> revealingPathsPrefixed(std::string beginningStateKey, sequence alpha);
};

#endif // PRODUCT_TFSM_H
