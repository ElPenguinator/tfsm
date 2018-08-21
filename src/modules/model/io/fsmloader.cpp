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
    //label="([^"]+)"
    QRegExp rxTS(patternTS);
    int index = rxTS.indexIn(QString::fromStdString(line));
    bool createTransition = false;
    if (index != -1) {
        src = rxTS.cap(1).toInt();
        tgt = rxTS.cap(2).toInt();
        cout << "TS: " << rxTS.cap(0).toStdString() << " " << rxTS.cap(1).toStdString() << " " << rxTS.cap(2).toStdString() << endl;
        createTransition = true;
    }

    QString patternIO("label\=\"([^\\s]+)\\s*\\/\\s*([^\\s]+)\\s*\\[([0-9]+)\\]\"");
    QRegExp rxIO(patternIO);
    index = rxIO.indexIn(QString::fromStdString(line));

    cout << line << endl << "label\=\"([^\\s]+)\\s*\\/\\s*([^\\s]+)\\s*\\[([0-9]+)\\]\"" << endl;
    if (index != -1) {
        i = rxIO.cap(1).toStdString();
        o = rxIO.cap(2).toStdString();
        id = rxIO.cap(3).toInt();
        cout << "IO: " << rxIO.cap(0).toStdString() << " " << rxIO.cap(1).toStdString() << " " << rxIO.cap(2).toStdString() << " " << rxIO.cap(3).toStdString() << endl;
        createTransition = createTransition && true;
    }
    bool mutant = false;
    if (line.find("dashed") != string::npos) {
        cout << "Is a mutant" << endl;
        mutant = true;
    }

    cout << mutant << " " << createSpecification << " " << createTransition << endl;

    if ((!mutant || !createSpecification) && createTransition) {
        cout << "Add transition ! " << endl;
        machine->states.insert(src);
        machine->states.insert(tgt);
        machine->inputs.insert(i);
        machine->outputs.insert(o);

        vector<IOTransition *> newLambda = {new IOTransition(src, i, o, tgt, id)};
        machine->addTransitions(newLambda, mutant);
    }
}

FSM * FSMLoader::getResult()
{
    return this->machine;
}
