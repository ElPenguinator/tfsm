#ifndef FSM_H
#define FSM_H
#include <string>
#include <set>
#include <vector>
#include <map>

class Transition {
public:
    int src;
    std::string i;
    std::string o;
    int tgt;
    int id;
    Transition(int src, std::string i, std::string o, int tgt, int id);
};

class FSM
{
private:
    void computeMaps();
public:
    std::set<int> states;
    int initialState;
    std::set<std::string> inputs;
    std::set<std::string> outputs;
    std::vector<Transition> transitions;
    std::map<int, std::vector<Transition> > transitionsPerState;
    std::map<int, Transition> transitionIdMap;
    FSM(std::set<int> S, int s0, std::set<std::string> I, std::set<std::string> O, std::vector<Transition> lambda);
    void addTransitions(std::vector<Transition> transitions);
    std::vector<Transition> getXi(int s, std::string i);
    Transition getTransitionFromId(int id);
    std::vector<Transition> lambda(int s);
    void print();
};

#endif // FSM_H
