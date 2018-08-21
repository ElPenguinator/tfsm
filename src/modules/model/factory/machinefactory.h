#ifndef MACHINEFACTORY_H
#define MACHINEFACTORY_H
#include "../machine/fsm.h"
#include "../algorithm/algorithms.h"
#include "../io/machineloader.h"
#include <QMap>
#include <QTableWidget>

class MachineFactory
{
public:
    MachineFactory();
    std::set<int> generateStates(int nbOfStates);
    std::set<std::string> generateAlphabet(QTableWidget * table);
    virtual Algorithms * getAlgorithms(bool generateLogs) = 0;
    virtual FSM * generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates) = 0;
    virtual FSM * generateMutation(FSM *specification, QMap<QString, QTableWidget *> map, int nbOfStates) = 0;
    virtual MachineLoader * getLoader() = 0;
    virtual void fillTabs(FSM * machine, QMap<QString, QTableWidget *> map, QLineEdit * edit) = 0;
};

#endif // MACHINEFACTORY_H
