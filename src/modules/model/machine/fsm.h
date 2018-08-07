#ifndef FSM_H
#define FSM_H
#include <string>
#include <set>
#include <vector>
#include <map>
#include "iotransition.h"
#include "timeouttransition.h"
class FSM
{
protected:
    void computeMaps();
public:
    std::set<int> states;
    int initialState;
    std::set<std::string> inputs;
    std::set<std::string> outputs;
    std::vector<IOTransition *> transitions;
    std::map<int, std::vector<IOTransition *> > transitionsPerState;
    std::map<int, IOTransition *> transitionIdMap;
    FSM(std::set<int> S, int s0, std::set<std::string> I, std::set<std::string> O, std::vector<IOTransition *> lambda);
    virtual void addTransitions(std::vector<IOTransition *> transitions);
    std::vector<IOTransition *> getXi(int s, std::string i);
    virtual std::vector<TimeoutTransition *> getXi(int s);

    IOTransition * getTransitionFromId(int id);
    virtual TimeoutTransition * getTimeoutFromId(int id);
    virtual bool isIdTimeout(int id);
    virtual int getMaxDelta(int s);
    virtual std::vector<IOTransition *> lambda(int s);
    virtual std::vector<TimeoutTransition *> delta(int s);
    virtual void print();
    virtual int getTransitionSize();
    virtual std::vector<IOTransition *> getTransitions();
    virtual std::vector<TimeoutTransition *> getTimeouts();
    virtual std::set<std::set<int>> getEta(int s, std::string i);
    virtual int getInitialState();

    virtual std::string generateDot();
};

#endif // FSM_H
