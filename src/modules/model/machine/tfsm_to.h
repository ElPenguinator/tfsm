#ifndef TFSM_TO_H
#define TFSM_TO_H
#include <string>
#include <set>
#include <vector>
#include <map>
#include "fsm.h"



class TFSM_TO : public FSM
{
private:
    void computeMaps();
public:
    std::vector<TimeoutTransition *> timeouts;
    std::map<int, std::vector<TimeoutTransition *> > timeoutsPerState;
    std::map<int, TimeoutTransition *> timeoutIdMap;
    TFSM_TO(std::set<int> S, int s0, std::set<std::string> I, std::set<std::string> O, std::vector<IOTransition *> lambda, std::vector<TimeoutTransition *> delta);
    void addTransitions(std::vector<IOTransition *> transitions);
    void addTimeouts(std::vector<TimeoutTransition *> timeouts);
    std::vector<IOTransition *> getXi(int s, std::string i);
    std::vector<TimeoutTransition *> getXi(int s);
    bool isIdTimeout(int id);
    IOTransition * getTransitionFromId(int id);
    TimeoutTransition * getTimeoutFromId(int id);
    int getMaxDelta(int s);
    std::vector<IOTransition *> lambda(int s);
    std::vector<TimeoutTransition *> delta(int s);
    void print();
};

#endif // TFSM_TO_H
