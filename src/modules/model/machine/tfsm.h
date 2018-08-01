#ifndef TFSM_H
#define TFSM_H
#include <string>
#include <set>
#include <vector>
#include <map>
#include "tfsm_to.h"

enum Bracket { Square, Curly };

class Guard {
public:
    Bracket left;
    int tmin;
    int tmax;
    Bracket right;
    Guard();
    Guard(std::string left, int tmin, int tmax, std::string right);
    Guard(Bracket left, int tmin, int tmax, Bracket right);
    std::string toString();
    bool isIntersectionEmpty(Guard other);
    Guard intersect(Guard other);
    bool contains(int x);
    bool equals(Guard other);
};

class GuardedTransition {
public:
    int src;
    std::string i;
    Guard g;
    std::string o;
    int tgt;
    int id;
    GuardedTransition(int src, std::string i, Guard g, std::string o, int tgt, int id);
};

class TFSM
{
private:
    void computeMaps();
public:
    std::set<int> states;
    int initialState;
    std::set<std::string> inputs;
    std::set<std::string> outputs;
    std::vector<GuardedTransition> transitions;
    std::vector<TimeoutTransition> timeouts;
    std::map<int, std::vector<GuardedTransition> > transitionsPerState;
    std::map<int, std::vector<TimeoutTransition> > timeoutsPerState;
    std::map<int, GuardedTransition> transitionIdMap;
    std::map<int, TimeoutTransition> timeoutIdMap;
    std::map<int, std::map<std::string, std::set<std::set<int>>>> combinationsMaps;
    TFSM(std::set<int> S, int s0, std::set<std::string> I, std::set<std::string> O, std::vector<GuardedTransition> lambda, std::vector<TimeoutTransition> delta);
    void addTransitions(std::vector<GuardedTransition> transitions);
    void addTimeouts(std::vector<TimeoutTransition> timeouts);
    std::vector<GuardedTransition> getXi(int s, std::string i);
    std::vector<TimeoutTransition> getXi(int s);
    bool isIdTimeout(int id);
    GuardedTransition getTransitionFromId(int id);
    TimeoutTransition getTimeoutFromId(int id);
    int getMaxDelta(int s);
    std::vector<GuardedTransition> lambda(int s);
    std::vector<TimeoutTransition> delta(int s);
    void print();
    std::set<std::set<int>> getEta(int s, std::string i);
};

#endif // TFSM_H
