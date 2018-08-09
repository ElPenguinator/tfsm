#ifndef TFSMFACTORY_H
#define TFSMFACTORY_H
#include "machinefactory.h"

class TFSMFactory : public MachineFactory
{
public:
    TFSMFactory();
    Algorithms * getAlgorithms();
    FSM * generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates);
    FSM * generateMutation(FSM *specification, QMap<QString, QTableWidget *> map, int nbOfStates);
};

#endif // TFSMFACTORY_H
