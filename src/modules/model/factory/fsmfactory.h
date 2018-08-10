#ifndef FSMFACTORY_H
#define FSMFACTORY_H
#include "machinefactory.h"

class FSMFactory : public MachineFactory
{
public:
    FSMFactory();
    Algorithms * getAlgorithms(bool generateLogs);
    FSM * generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates);
    FSM * generateMutation(FSM *specification, QMap<QString, QTableWidget *> map, int nbOfStates);
};

#endif // FSMFACTORY_H
