#ifndef TFSMFACTORY_H
#define TFSMFACTORY_H
#include "machinefactory.h"

class TFSMFactory : public MachineFactory
{
public:
    TFSMFactory();
    Algorithms * getAlgorithms(bool generateLogs, bool onlyDot);
    FSM * generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates);
    FSM * generateMutation(FSM *specification, QMap<QString, QTableWidget *> map, int nbOfStates);
    MachineLoader * getLoader();
    void fillTabs(FSM * machine, QMap<QString, QTableWidget *> map, QLineEdit * edit);
};

#endif // TFSMFACTORY_H
