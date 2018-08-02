#ifndef DISTINGUISHINGAUTOMATON_H
#define DISTINGUISHINGAUTOMATON_H
#include "fsm.h"
#include "productstate.h"
#include <deque>

class DistinguishingAutomaton
{
private:
    bool isProductConnected();
public:
    FSM * specification;
    FSM * mutationMachine;
    std::map<std::string, ProductState*> states;
    ProductState * initialState;
    std::vector<ProductTransition> transitions;
    DistinguishingAutomaton(FSM * S, FSM * M);
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
    std::string DijkstraFindMin(std::map<std::string, int> distances, std::set<std::string> Q);
    void DijkstraUpdateDistancesMin(std::map<std::string, int> & distances, std::map<std::string, ProductTransition> & predecessors, std::string s1, std::string s2, ProductTransition transition);
    std::deque<ProductTransition> Dijkstra(std::string key);
};

#endif // DISTINGUISHINGAUTOMATON_H
