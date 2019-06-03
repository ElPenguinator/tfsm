#ifndef TFSM_H
#define TFSM_H
#include <string>
#include <set>
#include <vector>
#include <map>
#include "tfsm_to.h"
#include "guardedtransition.h"

class TFSM : public TFSM_TO
{
private:
    void computeMaps();
public:
    std::map<int, std::map<std::string, std::set<std::set<int>>>> combinationsMaps;
    TFSM(std::set<int> S, int s0, std::set<std::string> I, std::set<std::string> O, std::vector<IOTransition *> lambda, std::vector<TimeoutTransition *> delta);
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
    void print();
    int getTransitionSize();
    std::set<std::set<int>> getEta(int s, std::string i);
    std::vector<IOTransition *> getTransitions();
    std::vector<TimeoutTransition *> getTimeouts();
    int getInitialState();
    std::string generateFormalism();
    std::string generateDot();
};

#endif // TFSM_H
