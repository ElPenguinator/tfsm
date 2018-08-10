#ifndef DISTINGUISHINGAUTOMATON_TO_H
#define DISTINGUISHINGAUTOMATON_TO_H
#include "distinguishingautomaton_fsm.h"

class DistinguishingAutomaton_TFSM_TO : public DistinguishingAutomaton_FSM
{
private:
    bool isProductConnected();
public:
    DistinguishingAutomaton_TFSM_TO(FSM * S, FSM * M);
    void generateNext(ProductState * state);
    void insertState(ProductState * state, std::string i, ProductState * newState, bool isTimeout, int id);
    std::vector<executingPath> revealingPaths(sequence alpha);
    bool isPathDeterministic(const executingPath p);
    void revealingPathsRecursive(ProductState * state, executingPath currentPath, std::vector<executingPath> & results, sequence alpha, int sequenceIndex, int timeBuffer);
    void print();
    sequence inputSequenceFromAcceptedLanguage(std::set<std::string> beginningStates, sequence prefix);
    void reachableStates(ProductState * state, executingPath currentPath, std::set<std::string> &results, sequence alpha, int sequenceIndex, int timeBuffer);
    std::string DijkstraFindMin(std::map<std::string, int> distances, std::set<std::string> Q);
    void DijkstraUpdateDistancesMin(std::map<std::string, int> & distances, std::map<std::string, ProductTransition *> & predecessors, std::string s1, std::string s2, ProductTransition * transition);
    std::deque<ProductTransition *> Dijkstra(std::string key);
    virtual void initialize();
    virtual std::string generateDot();
};

#endif // DISTINGUISHINGAUTOMATON_TO_H
