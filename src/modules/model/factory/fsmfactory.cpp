#include "fsmfactory.h"
#include "../algorithm/algorithms_fsm.h"

using namespace std;
FSMFactory::FSMFactory() : MachineFactory()
{

}

Algorithms * FSMFactory::getAlgorithms(bool generateLogs)
{
    return new Algorithms_FSM(generateLogs);
}

FSM * FSMFactory::generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates)
{
    set<int> S = generateStates(nbOfStates);
    int s0 = 1;

    QTableWidget * tableInputs = (*map.find(QString("inputs")));
    QTableWidget * tableOutputs = (*map.find(QString("outputs")));
    QTableWidget * tableTransitions = (*map.find(QString("transitions")));

    set<string> I = generateAlphabet(tableInputs);
    set<string> O = generateAlphabet(tableOutputs);

    vector<IOTransition *> lambda;
    int id=0;
    for (int row=0; row<tableTransitions->rowCount()-1; row++) {
        int src = -1;
        string i = "";
        string o = "";
        int tgt = -1;
        for (int column=0; column<tableTransitions->columnCount(); column++) {
            QTableWidgetItem * item = tableTransitions->item(row, column);
            QString header = tableTransitions->horizontalHeaderItem(column)->text();
            if (header == "src") {
                src = item->text().toInt();
            }
            else if (header == "i") {
                i = item->text().toStdString();
            }
            else if (header == "o") {
                o = item->text().toStdString();
            }
            else if (header == "tgt") {
                tgt = item->text().toInt();
            }
        }
        lambda.push_back(new IOTransition(src, i, o, tgt, id));
        id++;
    }

    return new FSM(S, s0, I, O, lambda);
}

FSM * FSMFactory::generateMutation(FSM * specification, QMap<QString, QTableWidget *> map, int nbOfStates)
{
    set<int> S = generateStates(nbOfStates);
    int s0 = 1;

    QTableWidget * tableInputs = (*map.find(QString("inputs")));
    QTableWidget * tableOutputs = (*map.find(QString("outputs")));
    QTableWidget * tableTransitions = (*map.find(QString("transitions")));

    set<string> I = generateAlphabet(tableInputs);
    set<string> O = generateAlphabet(tableOutputs);

    vector<IOTransition *> lambda;
    if (specification)
        lambda = specification->getTransitions();
    vector<IOTransition *> newLambda;
    int id = specification->getTransitionSize();
    for (int row=0; row<tableTransitions->rowCount()-1; row++) {
        int src = -1;
        string i = "";
        string o = "";
        int tgt = -1;
        for (int column=0; column<tableTransitions->columnCount(); column++) {
            QTableWidgetItem * item = tableTransitions->item(row, column);
            QString header = tableTransitions->horizontalHeaderItem(column)->text();
            if (header == "src") {
                src = item->text().toInt();
            }
            else if (header == "i") {
                i = item->text().toStdString();
            }
            else if (header == "o") {
                o = item->text().toStdString();
            }
            else if (header == "tgt") {
                tgt = item->text().toInt();
            }
        }
        newLambda.push_back(new IOTransition(src, i, o, tgt, id));
        id++;
    }

    FSM * res =  new FSM(S, s0, I, O, lambda);
    res->addTransitions(newLambda, true);
    return res;
}
