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
    std::vector<IOTransition> transitions;
    std::map<int, std::vector<IOTransition> > transitionsPerState;
    std::map<int, IOTransition> transitionIdMap;
    FSM(std::set<int> S, int s0, std::set<std::string> I, std::set<std::string> O, std::vector<IOTransition> lambda);
    void addTransitions(std::vector<IOTransition> transitions);
    std::vector<IOTransition> getXi(int s, std::string i);
    IOTransition getTransitionFromId(int id);
    std::vector<IOTransition> lambda(int s);
    void print();
};

#endif // FSM_H
