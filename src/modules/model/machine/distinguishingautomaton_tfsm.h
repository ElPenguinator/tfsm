#ifndef DISTINGUISHINGAUTOMATON_TFSM_H
#define DISTINGUISHINGAUTOMATON_TFSM_H
#include "distinguishingautomaton_tfsm_to.h"

class DistinguishingAutomaton_TFSM : public DistinguishingAutomaton_TFSM_TO
{
private:
    bool isProductConnected();
public:
    DistinguishingAutomaton_TFSM(FSM * S, FSM * M);
    void generateNext(ProductState * state);
    void insertState(ProductState * state, std::string i, Guard g, ProductState * newState, bool isTimeout, int id);
    std::vector<executingPath> revealingPaths(Sequence * alpha);
    bool isPathDeterministic(const executingPath p);
    void revealingPathsRecursive(ProductState * state, executingPath currentPath, std::vector<executingPath> & results, Sequence * alpha, int sequenceIndex, double timeBuffer, double timeLeftOver);
    void print();
    Sequence * inputSequenceFromAcceptedLanguage(std::set<std::string> beginningStates, Sequence * prefix);
    void reachableStates(ProductState * state, executingPath currentPath, std::set<std::string> *results, Sequence * alpha, int sequenceIndex, int timeBuffer, int timeLeftOver);
    std::string DijkstraFindMin(std::map<std::string, int> distances, std::set<std::string> Q);
    void DijkstraUpdateDistancesMin(std::map<std::string, int> & distances, std::map<std::string, ProductTransition *> & predecessors, std::string s1, std::string s2, ProductTransition * transition);
    std::deque<ProductTransition *> Dijkstra(std::string key);
    virtual void initialize();
    std::string generateDot();
};

#endif // DISTINGUISHINGAUTOMATON_TFSM_H
