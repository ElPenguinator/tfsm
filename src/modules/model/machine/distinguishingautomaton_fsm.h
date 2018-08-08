#ifndef DISTINGUISHINGAUTOMATON_H
#define DISTINGUISHINGAUTOMATON_H
#include "fsm.h"
#include "productstate.h"
#include <deque>

class DistinguishingAutomaton_FSM
{
private:
    bool isProductConnected();
public:
    FSM * specification;
    FSM * mutationMachine;
    std::map<std::string, ProductState*> states;
    ProductState * initialState;
    std::vector<ProductTransition *> transitions;
    DistinguishingAutomaton_FSM(FSM * S, FSM * M);
    bool hasNoSinkState;
    bool isConnected;
    virtual void generateNext(ProductState * state);
    virtual void insertState(ProductState * state, std::string i, ProductState * newState, bool isTimeout, int id);
    virtual std::vector<executingPath> revealingPaths(sequence alpha);
    virtual bool isPathDeterministic(const executingPath p);
    virtual void revealingPathsRecursive(ProductState * state, executingPath currentPath, std::vector<executingPath> & results, sequence alpha, int sequenceIndex, int timeBuffer);
    virtual void print();
    virtual sequence inputSequenceFromAcceptedLanguage(std::set<std::string> beginningStates, sequence prefix);
    virtual void reachableStates(ProductState * state, executingPath currentPath, std::set<std::string> &results, sequence alpha, int sequenceIndex, int timeBuffer);
    virtual std::string DijkstraFindMin(std::map<std::string, int> distances, std::set<std::string> Q);
    virtual void DijkstraUpdateDistancesMin(std::map<std::string, int> & distances, std::map<std::string, ProductTransition *> & predecessors, std::string s1, std::string s2, ProductTransition * transition);
    virtual std::deque<ProductTransition *> Dijkstra(std::string key);
    virtual void initialize();
};

#endif // DISTINGUISHINGAUTOMATON_H
