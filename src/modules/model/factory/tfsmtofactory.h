#ifndef TFSMTOFACTORY_H
#define TFSMTOFACTORY_H
#include "machinefactory.h"

class TFSMTOFactory : public MachineFactory
{
public:
    TFSMTOFactory();
    Algorithms * getAlgorithms(bool generateLogs);
    FSM * generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates);
    FSM * generateMutation(FSM *specification, QMap<QString, QTableWidget *> map, int nbOfStates);
};

#endif // TFSMTOFACTORY_H
