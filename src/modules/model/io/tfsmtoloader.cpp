#include "tfsmtoloader.h"
#include <QObject>
#include <iostream>
using namespace std;
TFSMTOLoader::TFSMTOLoader()
{
    set<int> emptyStates;
    set<string> emptyI;
    set<string> emptyO;
    vector<IOTransition *> emptyLambda;
    vector<TimeoutTransition *> emptyDelta;
    this->machine = new TFSM_TO(emptyStates, 1, emptyI, emptyO, emptyLambda, emptyDelta);
}

void TFSMTOLoader::readLine(string line, bool createSpecification)
{
    int src;
    int tgt;
    string i;
    string o;
    int id;
    int t;

    QString patternTS("([0-9]+)\\s*->\\s*([0-9]+)");
    //label="([^"]+)"
    QRegExp rxTS(patternTS);
    int index = rxTS.indexIn(QString::fromStdString(line));
    bool createTransition = false;
    if (index != -1) {
        src = rxTS.cap(1).toInt();
        tgt = rxTS.cap(2).toInt();
        createTransition = true;
    }

    QString patternIO("label\=\"([^\\s]+)\\s*\\/\\s*([^\\s]+)\\s*\\[([0-9]+)\\]\"");
    QRegExp rxIO(patternIO);

    index = rxIO.indexIn(QString::fromStdString(line));

    if (index != -1) {
        i = rxIO.cap(1).toStdString();
        o = rxIO.cap(2).toStdString();
        id = rxIO.cap(3).toInt();
        createTransition = createTransition && true;
        bool mutant = false;
        if (line.find("dashed") != string::npos) {
            mutant = true;
        }

        if ((!mutant || !createSpecification) && createTransition) {
            machine->states.insert(src);
            machine->states.insert(tgt);
            machine->inputs.insert(i);
            machine->outputs.insert(o);

            vector<IOTransition *> newLambda = {new IOTransition(src, i, o, tgt, id)};
            machine->addTransitions(newLambda, mutant);
        }
    }
    else {
        QString patternTO("label\=\"([^\\s]+)\\s*\\[([0-9]+)\\]\"");
        QRegExp rxTO(patternTO);
        index = rxTO.indexIn(QString::fromStdString(line));
        if (index != -1) {
            if (rxTO.cap(1).toStdString() == "∞") {
                t = inf;
            }
            else {
                t = rxTO.cap(1).toInt();
            }
            id = rxTO.cap(2).toInt();
            createTransition = createTransition && true;
            bool mutant = false;
            if (line.find("dashed") != string::npos) {
                mutant = true;
            }

            if ((!mutant || !createSpecification) && createTransition) {
                machine->states.insert(src);
                machine->states.insert(tgt);

                vector<TimeoutTransition *> newDelta = {new TimeoutTransition(src, t, tgt, id)};
                machine->addTimeouts(newDelta, mutant);
            }
        }
    }
}

FSM * TFSMTOLoader::getResult()
{
    return this->machine;
}
