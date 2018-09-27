#include "tfsmfactory.h"
#include "../algorithm/algorithms_tfsm.h"
#include "../io/tfsmloader.h"

using namespace std;

TFSMFactory::TFSMFactory() : MachineFactory()
{

}

Algorithms * TFSMFactory::getAlgorithms(bool generateLogs, bool onlyDot)
{
    return new Algorithms_TFSM(generateLogs, onlyDot);
}

Guard generateGuard(QString cap1, QString cap2, QString cap3, QString cap4)
{
    string leftBracket = cap1.toStdString();
    int tmin = cap2.toInt();
    int tmax;
    if (cap3 == "inf")
        tmax = inf;
    else
        tmax = cap3.toInt();
    string rightBracket = cap4.toStdString();
    //cout << "Guard : " << leftBracket << tmin << "," << tmax << rightBracket << endl;

    return Guard(leftBracket, tmin, tmax, rightBracket);
}

FSM * TFSMFactory::generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates)
{
    set<int> S = generateStates(nbOfStates);
    int s0 = 1;

    QTableWidget * tableInputs = (*map.find(QString("inputs")));
    QTableWidget * tableOutputs = (*map.find(QString("outputs")));
    QTableWidget * tableTransitions = (*map.find(QString("transitions")));
    QTableWidget * tableTimeouts = (*map.find(QString("timeouts")));

    set<string> I = generateAlphabet(tableInputs);
    set<string> O = generateAlphabet(tableOutputs);

    vector<IOTransition *> lambda;
    int id=0;
    for (int row=0; row<tableTransitions->rowCount()-1; row++) {
        int src = -1;
        string i = "";
        string o = "";
        Guard g;
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
            else if (header == "g") {
                QString reason;
                QString pattern = "([\\[\\(])\\s*([0-9]+)\\s*,\\s*([0-9]+|inf)\\s*([\\)\\]])";
                QRegExp rx(pattern);
                int index = rx.indexIn(item->text());
                g = generateGuard(rx.cap(1), rx.cap(2), rx.cap(3), rx.cap(4));
            }
            else if (header == "o") {
                o = item->text().toStdString();
            }
            else if (header == "tgt") {
                tgt = item->text().toInt();
            }
        }
        lambda.push_back(new GuardedTransition(src, i, g, o, tgt, id));
        id++;
    }

    vector<TimeoutTransition *> delta;
    for (int row=0; row<tableTimeouts->rowCount()-1; row++) {
        int src = -1;
        int t = -1;
        int tgt = -1;
        for (int column=0; column<tableTimeouts->columnCount(); column++) {
            QTableWidgetItem * item = tableTimeouts->item(row, column);
            QString header = tableTimeouts->horizontalHeaderItem(column)->text();
            if (header == "src") {
                src = item->text().toInt();
            }
            else if (header == "t") {
                if (item->text() == "inf")
                    t = inf;
                else
                    t = item->text().toInt();
            }
            else if (header == "tgt") {
                tgt = item->text().toInt();
            }
        }
        delta.push_back(new TimeoutTransition(src, t, tgt, id));
        id++;
    }

    return new TFSM(S, s0, I, O, lambda, delta);
}

FSM * TFSMFactory::generateMutation(FSM *specification, QMap<QString, QTableWidget *> map, int nbOfStates)
{
    set<int> S = generateStates(nbOfStates);
    int s0 = 1;

    QTableWidget * tableInputs = (*map.find(QString("inputs")));
    QTableWidget * tableOutputs = (*map.find(QString("outputs")));
    QTableWidget * tableTransitions = (*map.find(QString("transitions")));
    QTableWidget * tableTimeouts = (*map.find(QString("timeouts")));

    set<string> I = generateAlphabet(tableInputs);
    set<string> O = generateAlphabet(tableOutputs);

    vector<IOTransition *> lambda;
    vector<TimeoutTransition *> delta;
    if (specification) {
        lambda = specification->getTransitions();
        delta = specification->getTimeouts();
    }
    vector<IOTransition *> newLambda;
    int id = specification->getTransitionSize();
    for (int row=0; row<tableTransitions->rowCount()-1; row++) {
        int src = -1;
        string i = "";
        Guard g;
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
            else if (header == "g") {
                QString reason;
                QString pattern = "([\\[\\(])\\s*([0-9]+)\\s*,\\s*([0-9]+|inf)\\s*([\\)\\]])";
                QRegExp rx(pattern);
                int index = rx.indexIn(item->text());
                g = generateGuard(rx.cap(1), rx.cap(2), rx.cap(3), rx.cap(4));
            }
            else if (header == "o") {
                o = item->text().toStdString();
            }
            else if (header == "tgt") {
                tgt = item->text().toInt();
            }
        }
        newLambda.push_back(new GuardedTransition(src, i, g, o, tgt, id));
        id++;
    }

    vector<TimeoutTransition *> newDelta;
    for (int row=0; row<tableTimeouts->rowCount()-1; row++) {
        int src = -1;
        int t = -1;
        int tgt = -1;
        for (int column=0; column<tableTimeouts->columnCount(); column++) {
            QTableWidgetItem * item = tableTimeouts->item(row, column);
            QString header = tableTimeouts->horizontalHeaderItem(column)->text();
            if (header == "src") {
                src = item->text().toInt();
            }
            else if (header == "t") {
                if (item->text() == "inf")
                    t = inf;
                else
                    t = item->text().toInt();
            }
            else if (header == "tgt") {
                tgt = item->text().toInt();
            }
        }
        newDelta.push_back(new TimeoutTransition(src, t, tgt, id));
        id++;
    }

    TFSM * res =  new TFSM(S, s0, I, O, lambda, delta);
    res->addTransitions(newLambda, true);
    res->addTimeouts(newDelta, true);
    return res;
}

MachineLoader * TFSMFactory::getLoader()
{
    return new TFSMLoader();
}

void TFSMFactory::fillTabs(FSM * machine, QMap<QString, QTableWidget *> map, QLineEdit * edit)
{
    this->fillNbStates(machine, edit);
    this->fillInputs(machine, map);
    this->fillOutputs(machine, map);


    QTableWidget * tableSpecTransitions = (*map.find(QString("specificationTransitions")));
    QTableWidget * tableMutaTransitions = (*map.find(QString("mutationTransitions")));

    QTableWidget * tableSpecTimeouts = (*map.find(QString("specificationTimeouts")));
    QTableWidget * tableMutaTimeouts = (*map.find(QString("mutationTimeouts")));

    tableSpecTransitions->setRowCount(0);
    tableSpecTransitions->setRowCount(1);
    tableMutaTransitions->setRowCount(0);
    tableMutaTransitions->setRowCount(1);

    tableSpecTimeouts->setRowCount(0);
    tableSpecTimeouts->setRowCount(1);
    tableMutaTimeouts->setRowCount(0);
    tableMutaTimeouts->setRowCount(1);

    int rowSpec = 0;
    int rowMuta = 0;
    for (IOTransition * t : machine->transitions) {
        if ((*machine->mutatedTransitions.find(t->id)).second == false) {
            tableSpecTransitions->setRowCount(tableSpecTransitions->rowCount()+1);
            tableSpecTransitions->setItem(rowSpec, 0, new QTableWidgetItem(QString::number(t->src)));
            tableSpecTransitions->setItem(rowSpec, 1, new QTableWidgetItem(QString::fromStdString(t->i)));
            string g = t->getGuard().toString();
            if (g.find("∞") != string::npos)
                g.replace(g.find("∞"), 3, "inf");
            tableSpecTransitions->setItem(rowSpec, 2, new QTableWidgetItem(QString::fromStdString(g)));
            tableSpecTransitions->setItem(rowSpec, 3, new QTableWidgetItem(QString::fromStdString(t->o)));
            tableSpecTransitions->setItem(rowSpec, 4, new QTableWidgetItem(QString::number(t->tgt)));
            rowSpec++;
        }
        else {
            tableMutaTransitions->setRowCount(tableMutaTransitions->rowCount()+1);
            tableMutaTransitions->setItem(rowMuta, 0, new QTableWidgetItem(QString::number(t->src)));
            tableMutaTransitions->setItem(rowMuta, 1, new QTableWidgetItem(QString::fromStdString(t->i)));
            string g = t->getGuard().toString();
            if (g.find("∞") != string::npos)
                g.replace(g.find("∞"), 3, "inf");
            tableMutaTransitions->setItem(rowMuta, 2, new QTableWidgetItem(QString::fromStdString(g)));
            tableMutaTransitions->setItem(rowMuta, 3, new QTableWidgetItem(QString::fromStdString(t->o)));
            tableMutaTransitions->setItem(rowMuta, 4, new QTableWidgetItem(QString::number(t->tgt)));
            rowMuta++;
        }
    }

    rowSpec = 0;
    rowMuta = 0;
    for (TimeoutTransition * t : machine->getTimeouts()) {
        if ((*machine->mutatedTransitions.find(t->id)).second == false) {
            tableSpecTimeouts->setRowCount(tableSpecTimeouts->rowCount()+1);
            tableSpecTimeouts->setItem(rowSpec, 0, new QTableWidgetItem(QString::number(t->src)));
            if (t->t == inf)
                tableSpecTimeouts->setItem(rowSpec, 1, new QTableWidgetItem(QString("inf")));
            else
                tableSpecTimeouts->setItem(rowSpec, 1, new QTableWidgetItem(QString::number(t->t)));
            tableSpecTimeouts->setItem(rowSpec, 2, new QTableWidgetItem(QString::number(t->tgt)));
            rowSpec++;
        }
        else {
            tableMutaTimeouts->setRowCount(tableMutaTimeouts->rowCount()+1);
            tableMutaTimeouts->setItem(rowMuta, 0, new QTableWidgetItem(QString::number(t->src)));
            if (t->t == inf)
                tableMutaTimeouts->setItem(rowMuta, 1, new QTableWidgetItem(QString("inf")));
            else
                tableMutaTimeouts->setItem(rowMuta, 1, new QTableWidgetItem(QString::number(t->t)));
            tableMutaTimeouts->setItem(rowMuta, 2, new QTableWidgetItem(QString::number(t->tgt)));
            rowMuta++;
        }
    }
}
