#include "fsmloader.h"
#include <QObject>
#include <iostream>
using namespace std;

FSMLoader::FSMLoader()
{
    set<int> emptyStates;
    set<string> emptyI;
    set<string> emptyO;
    vector<IOTransition *> emptyLambda;
    this->machine = new FSM(emptyStates, 1, emptyI, emptyO, emptyLambda);
}

void FSMLoader::readLine(string line, bool createSpecification)
{
    int src;
    int tgt;
    string i;
    string o;
    int id;

    QString patternTS("([0-9]+)\\s*->\\s*([0-9]+)");
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

}

FSM * FSMLoader::getResult()
{
    return this->machine;
}
