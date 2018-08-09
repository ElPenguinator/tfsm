#include "machinefactory.h"

using namespace std;
MachineFactory::MachineFactory()
{

}

set<int> MachineFactory::generateStates(int nbOfStates)
{
    set<int> S;
    for (int i=1; i<nbOfStates+1; i++) {
        S.insert(i);
    }
    return S;
}

std::set<std::string> MachineFactory::generateAlphabet(QTableWidget * table)
{
    set<string> A;
    for (int row=0; row<table->rowCount()-1; row++) {
        A.insert(table->item(row, 0)->text().toStdString());
    }
    return A;
}
