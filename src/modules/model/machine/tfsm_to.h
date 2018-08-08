#ifndef TFSM_TO_H
#define TFSM_TO_H
#include <string>
#include <set>
#include <vector>
#include <map>
#include "fsm.h"
#include "timeouttransition.h"


class TFSM_TO : public FSM
{
private:
    void computeMaps();
public:
    std::vector<TimeoutTransition *> timeouts;
    std::map<int, std::vector<TimeoutTransition *> > timeoutsPerState;
    std::map<int, TimeoutTransition *> timeoutIdMap;
    TFSM_TO(std::set<int> S, int s0, std::set<std::string> I, std::set<std::string> O, std::vector<IOTransition *> lambda, std::vector<TimeoutTransition *> delta);
    void addTransitions(std::vector<IOTransition *> transitions, bool isMutated);
    void addTimeouts(std::vector<TimeoutTransition *> timeouts, bool isMutated);
    std::vector<IOTransition *> getXi(int s, std::string i);
    std::vector<TimeoutTransition *> getXi(int s);
    bool isIdTimeout(int id);
    IOTransition * getTransitionFromId(int id);
    TimeoutTransition * getTimeoutFromId(int id);
    int getMaxDelta(int s);
    std::vector<IOTransition *> lambda(int s);
    std::vector<TimeoutTransition *> delta(int s);
    virtual void print() override;
    virtual int getTransitionSize();
    virtual std::set<std::set<int>> getEta(int s, std::string i);
    std::vector<IOTransition *> getTransitions();
    std::vector<TimeoutTransition *> getTimeouts();
    int getInitialState();

    virtual std::string generateDot();
};

#endif // TFSM_TO_H
