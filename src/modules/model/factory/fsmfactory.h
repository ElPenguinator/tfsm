#ifndef FSMFACTORY_H
#define FSMFACTORY_H
#include "machinefactory.h"

class FSMFactory : public MachineFactory
{
public:
    FSMFactory();
    Algorithms * getAlgorithms(bool generateLogs, bool onlyDot);
    FSM * generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates);
    FSM * generateMutation(FSM *specification, QMap<QString, QTableWidget *> map, int nbOfStates);
    MachineLoader * getLoader();
    void fillTabs(FSM * machine, QMap<QString, QTableWidget *> map, QLineEdit * edit);
};

#endif // FSMFACTORY_H
