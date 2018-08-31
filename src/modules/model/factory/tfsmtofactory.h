#ifndef TFSMTOFACTORY_H
#define TFSMTOFACTORY_H
#include "machinefactory.h"

class TFSMTOFactory : public MachineFactory
{
public:
    TFSMTOFactory();
    Algorithms * getAlgorithms(bool generateLogs, bool onlyDot);
    FSM * generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates);
    FSM * generateMutation(FSM *specification, QMap<QString, QTableWidget *> map, int nbOfStates);
    MachineLoader * getLoader();
    void fillTabs(FSM * machine, QMap<QString, QTableWidget *> map, QLineEdit * edit);
};

#endif // TFSMTOFACTORY_H
