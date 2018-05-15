#ifndef TFSM_TO_H
#define TFSM_TO_H
#include <string>
#include <set>
#include <vector>

class Transition {
public:
    int src;
    std::string i;
    std::string o;
    int tgt;
    int id;
    Transition(int src, std::string i, std::string o, int tgt, int id);
};

class Timeout {
public:
    int src;
    int t;
    int tgt;
    int id;
    Timeout(int src, int t, int tgt, int id);
};

class TFSM_TO
{
public:
    std::set<int> states;
    int initialState;
    std::set<std::string> inputs;
    std::set<std::string> outputs;
    std::vector<Transition> transitions;
    std::vector<Timeout> timeouts;
    TFSM_TO(std::set<int> S, int s0, std::set<std::string> I, std::set<std::string> O, std::vector<Transition> lambda, std::vector<Timeout> delta);
    void addTransitions(std::vector<Transition> transitions);
    void addTimeouts(std::vector<Timeout> timeouts);
    std::vector<Transition> getXi(int s, std::string i);
    std::vector<Timeout> getXi(int s);
    bool isIdTimeout(int id);
    Transition getTransitionFromId(int id);
    Timeout getTimeoutFromId(int id);
    int getMaxDelta(int s);
    void print();
};

#endif // TFSM_TO_H
