#include "tfsmloader.h"
#include "../structs.h"
#include <QObject>
using namespace std;
TFSMLoader::TFSMLoader()
{
    set<int> emptyStates;
    set<string> emptyI;
    set<string> emptyO;
    vector<IOTransition *> emptyLambda;
    vector<TimeoutTransition *> emptyDelta;
    this->machine = new TFSM(emptyStates, 1, emptyI, emptyO, emptyLambda, emptyDelta);
}

void TFSMLoader::readLine(string line, bool createSpecification)
{
    int src;
    int tgt;
    string i;
    string leftBracket;
    int tmin;
    int tmax;
    string rightBracket;
    string o;
    int id;
    int t;

    QString patternTS("([0-9]+)\\s*->\\s*([0-9]+)");
    QRegExp rxTS(patternTS);
    int index = rxTS.indexIn(QString::fromStdString(line));
    bool createTransition = false;
    if (index != -1) {
        src = rxTS.cap(1).toInt();
        tgt = rxTS.cap(2).toInt();
        createTransition = true;
    }

    QString patternIO("label\=\"([\\[\\(])\\s*([0-9]+)\\s*,\\s*([0-9]+|∞)\\s*([\\]\\)])\\s*([^\\s]+)\\s*\\/\\s*([^\\s]+)\\s*\\[([0-9]+)\\]\"");
    QRegExp rxIO(patternIO);

    index = rxIO.indexIn(QString::fromStdString(line));

    if (index != -1) {
        leftBracket = rxIO.cap(1).toStdString();
        tmin = rxIO.cap(2).toInt();
        if (rxIO.cap(3).toStdString() == "∞") {
            tmax = inf;
        }
        else {
            tmax = rxIO.cap(3).toInt();
        }
        rightBracket = rxIO.cap(4).toStdString();
        i = rxIO.cap(5).toStdString();
        o = rxIO.cap(6).toStdString();
        id = rxIO.cap(7).toInt();
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

            vector<IOTransition *> newLambda = {new GuardedTransition(src, i, Guard(leftBracket, tmin, tmax, rightBracket), o, tgt, id)};
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

FSM * TFSMLoader::getResult()
{
    return this->machine;
}
