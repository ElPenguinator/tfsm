#include "fsmfactory.h"
#include "../algorithm/algorithms_fsm.h"
#include "../io/fsmloader.h"
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QtSvg>
#include <QMap>
using namespace std;
FSMFactory::FSMFactory() : MachineFactory()
{

}

Algorithms * FSMFactory::getAlgorithms(bool generateLogs, bool onlyDot)
{
    return new Algorithms_FSM(generateLogs, onlyDot);
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

MachineLoader * FSMFactory::getLoader()
{
    return new FSMLoader();
}

void FSMFactory::fillTabs(FSM * machine, QMap<QString, QTableWidget *> map, QLineEdit * edit)
{
    this->fillNbStates(machine, edit);
    this->fillInputs(machine, map);
    this->fillOutputs(machine, map);


    QTableWidget * tableSpecTransitions = (*map.find(QString("specificationTransitions")));
    QTableWidget * tableMutaTransitions = (*map.find(QString("mutationTransitions")));

    tableSpecTransitions->setRowCount(0);
    tableSpecTransitions->setRowCount(1);
    tableMutaTransitions->setRowCount(0);
    tableMutaTransitions->setRowCount(1);


    int rowSpec = 0;
    int rowMuta = 0;
    for (IOTransition * t : machine->transitions) {
        if ((*machine->mutatedTransitions.find(t->id)).second == false) {
            tableSpecTransitions->setRowCount(tableSpecTransitions->rowCount()+1);
            tableSpecTransitions->setItem(rowSpec, 0, new QTableWidgetItem(QString::number(t->src)));
            tableSpecTransitions->setItem(rowSpec, 1, new QTableWidgetItem(QString::fromStdString(t->i)));
            tableSpecTransitions->setItem(rowSpec, 2, new QTableWidgetItem(QString::fromStdString(t->o)));
            tableSpecTransitions->setItem(rowSpec, 3, new QTableWidgetItem(QString::number(t->tgt)));
            rowSpec++;
        }
        else {
            tableMutaTransitions->setRowCount(tableMutaTransitions->rowCount()+1);
            tableMutaTransitions->setItem(rowMuta, 0, new QTableWidgetItem(QString::number(t->src)));
            tableMutaTransitions->setItem(rowMuta, 1, new QTableWidgetItem(QString::fromStdString(t->i)));
            tableMutaTransitions->setItem(rowMuta, 2, new QTableWidgetItem(QString::fromStdString(t->o)));
            tableMutaTransitions->setItem(rowMuta, 3, new QTableWidgetItem(QString::number(t->tgt)));
            rowMuta++;
        }
    }
}
