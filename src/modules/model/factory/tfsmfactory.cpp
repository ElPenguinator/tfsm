#include "tfsmfactory.h"
#include "../algorithm/algorithms_tfsm.h"

using namespace std;

TFSMFactory::TFSMFactory() : MachineFactory()
{

}

Algorithms * TFSMFactory::getAlgorithms()
{
    return new Algorithms_TFSM();
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
    cout << "Guard : " << leftBracket << tmin << "," << tmax << rightBracket << endl;

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

    cout << "Hello" << endl;
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
