#include "model.h"
using namespace std;
#include "algorithm/algorithms_fsm.h"
#include "algorithm/algorithms_tfsm_to.h"
#include "algorithm/algorithms_tfsm.h"
#include <graphviz/gvc.h>

Model::Model() : QObject()
{
showSpecification = true;
}

Model::~Model()
{

}

void Model::bindEvents()
{
    //connect(_generationCurrentDatabaseContent, &MetricTable::valueChanged, this, &Model::onCurrentDatabaseContentChanged);
}

//Everything there is temporary

void exampleFSM(FSM *& S, FSM *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<IOTransition *> lambda = {new IOTransition(1, "a", "0", 1, 0),
                                     new IOTransition(1, "b", "0", 2, 1),
                                     new IOTransition(2, "a", "0", 3, 2),
                                     new IOTransition(2, "b", "0", 2, 3),
                                     new IOTransition(3, "a", "0", 3, 4),
                                     new IOTransition(3, "b", "0", 4, 5),
                                     new IOTransition(4, "a", "1", 1, 6),
                                     new IOTransition(4, "b", "0", 4, 7)
                                    };
    S = new FSM(S2, s0, I, O, lambda);
    M = new FSM(S2, s0, I, O, lambda);
    M->addTransitions({new IOTransition(3, "a", "1", 3, 8),
                       new IOTransition(3, "b", "0", 3, 9),
                       new IOTransition(4, "a", "1", 2, 10)
                      }, true);

    E = {sequence({ts("b", 0),
                   ts("a", 0),
                   ts("b", 0),
                   ts("b", 0),
                   ts("a", 0)
         }
         )
        };
}

void exampleTFSM_TO(TFSM_TO *& S, TFSM_TO *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<IOTransition *> lambda = {new IOTransition(1, "a", "0", 1, 0),
                                     new IOTransition(1, "b", "0", 2, 1),
                                     new IOTransition(2, "b", "0", 2, 3),
                                     new IOTransition(2, "a", "0", 3, 4),
                                     new IOTransition(3, "a", "0", 3, 6),
                                     new IOTransition(3, "b", "0", 4, 7),
                                     new IOTransition(4, "b", "0", 4, 10),
                                     new IOTransition(4, "a", "1", 1, 9)
                                    };
    vector<TimeoutTransition *> delta = {new TimeoutTransition(1, 4, 4, 2),
                                         new TimeoutTransition(3, 5, 2, 8),
                                         new TimeoutTransition(2, inf, 2, 5),
                                         new TimeoutTransition(4, inf, 4, 11)
                                        };
    S = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M->addTransitions({new IOTransition(3, "a", "1", 3, 13),
                       new IOTransition(3, "b", "0", 3, 14),
                       new IOTransition(4, "a", "1", 2, 12)
                      }, true);
    M->addTimeouts({new TimeoutTransition(1, 3, 4, 15),
                    new TimeoutTransition(3, 8, 1, 16)
                   }, true);

    E = {sequence({ts("a", 0),
                   ts("a", 2),
                   ts("a", 5),
                   ts("a", 14),
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("b", 24),
                   ts("a", 24)
         }
         )
        };
}

void exampleTFSM(TFSM*& S, TFSM *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<IOTransition *> lambda = {new GuardedTransition(1, "a", Guard("[", 0, inf, ")"), "0", 1, 0),
                                     new GuardedTransition(1, "b", Guard("[", 0, inf, ")"), "0", 2, 1),
                                     new GuardedTransition(2, "b", Guard("[", 0, inf, ")"), "0", 2, 3),
                                     new GuardedTransition(2, "a", Guard("[", 0, 2, ")"), "0", 3, 4),
                                     new GuardedTransition(2, "a", Guard("[", 2, inf, ")"), "0", 4, 5),
                                     new GuardedTransition(3, "a", Guard("[", 0, inf, ")"), "0", 3, 7),
                                     new GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "0", 4, 8),
                                     new GuardedTransition(4, "b", Guard("[", 0, inf, ")"), "0", 4, 12),
                                     new GuardedTransition(4, "a", Guard("[", 0, 3, ")"), "1", 1, 10),
                                     new GuardedTransition(4, "a", Guard("[", 3, inf, ")"), "1", 3, 11)
                                    };
    vector<TimeoutTransition *> delta = {new TimeoutTransition(1, 4, 4, 2),
                                         new TimeoutTransition(3, 5, 2, 9),
                                         new TimeoutTransition(2, inf, 2, 6),
                                         new TimeoutTransition(4, inf, 4, 13)
                                        };
    S = new TFSM(S2, s0, I, O, lambda, delta);
    M = new TFSM(S2, s0, I, O, lambda, delta);
    M->addTransitions({new GuardedTransition(2, "a", Guard("[", 0, 5, ")"), "0", 3, 15),
                       new GuardedTransition(2, "a", Guard("[", 5, inf, ")"), "0", 4, 16),
                       new GuardedTransition(3, "a", Guard("[", 0, inf, ")"), "1", 3, 17),
                       new GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "0", 3, 18),
                       new GuardedTransition(4, "a", Guard("[", 0, inf, ")"), "1", 2, 20),
                       new GuardedTransition(4, "a", Guard("[", 3, inf, ")"), "1", 1, 21),
                       new GuardedTransition(4, "a", Guard("[", 0, 3, ")"), "1", 3, 22)
                      }, true);

    M->addTimeouts({new TimeoutTransition(1, 3, 4, 14),
                    new TimeoutTransition(3, 8, 1, 19)
                   }, true);

    E = {sequence({ts("a", 0),
                   ts("a", 2),
                   ts("a", 5),
                   ts("a", 14),
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("b", 24),
                   ts("a", 24)
         }
         )
        };
}

void checkFSM() {
    FSM * S;
    FSM * M;
    vector<sequence> E;
    exampleFSM(S, M, E);
    S->print();
    M->print();

    cout << S->generateDot();
    /*
    DistinguishingAutomaton_FSM * P = new DistinguishingAutomaton_FSM(S, M);
    P->initialize();
    P->print();

    Algorithms * algo = new Algorithms_FSM();
    vector<sequence> Einit;
    E = algo->generateCheckingExperiment(Einit, S, M);
    cout << "E : " << endl;
    for (auto s : E) {
        printSequence(s);
    }
    */
}

void checkTFSM_TO() {
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    exampleTFSM_TO(S, M, E);
    S->print();
    M->print();

    cout << S->generateDot();

    //    DistinguishingAutomaton_TFSM_TO * P = new DistinguishingAutomaton_TFSM_TO(S, M);
    //    P->initialize();
    //    P->print();

    //    Algorithms * algo = new Algorithms_TFSM_TO();
    //    vector<sequence> Einit;
    //    E = algo->generateCheckingExperiment(Einit, S, M);
    //    cout << "E : " << endl;
    //    for (auto s : E) {
    //        printSequence(s);
    //    }
}

void checkTFSM() {
    TFSM * S;
    TFSM * M;
    vector<sequence> E;
    exampleTFSM(S, M, E);
    S->print();
    M->print();

    cout << S->generateDot();
    /*
    DistinguishingAutomaton_TFSM * P = new DistinguishingAutomaton_TFSM(S, M);
    P->initialize();
    P->print();
    */
    //    Algorithms * algo = new Algorithms_TFSM();

    /*
    vector<sequence> Einit = {sequence({ts("a", 7),
                                        ts("a", 14),
                                        ts("a", 14)
                              }
                              )
                             };
    vector<path> test = P->revealingPaths(sequence({ts("a", 7),
                                                    ts("a", 14),
                                                    ts("a", 14)
                                                   }
                                                   ));
    cout << "Paths : " << endl;
    for (auto path : test) {
        printPath(path);
    }
    */

    //    vector<sequence> Einit;
    //    E = algo->generateCheckingExperiment(Einit, S, M);
    //    cout << "E : " << endl;
    //    for (auto s : E) {
    //        printSequence(s);
    //    }

}

void Model::modelCheck()
{
    checkFSM();
    checkTFSM_TO();
    checkTFSM();
}

void Model::importFile(QString fileName)
{
    cout << "Import : " << fileName.toStdString() << endl;
}

void Model::exportFile(QString fileName)
{
    cout << "Export : " << fileName.toStdString() << endl;
}

void Model::checkingExperiment()
{
    /*
    FSM * S;
    FSM * M;
    vector<sequence> E;
    exampleFSM(S, M, E);

    Algorithms * algo = new Algorithms_FSM();
    vector<sequence> Einit;
    E = algo->generateCheckingExperiment(Einit, S, M);

    emit checkingExperimentResults(E);
    */
    SpecificationMachine->print();
    MutationMachine->print();
    Algorithms * algo = new Algorithms_FSM();
    vector<sequence> E;
    vector<sequence> Einit;
    E = algo->generateCheckingExperiment(Einit, SpecificationMachine, MutationMachine);
    emit checkingExperimentResults(E);
}

void Model::checkingSequence()
{
    /*
    FSM * S;
    FSM * M;
    vector<sequence> E;
    exampleFSM(S, M, E);

    Algorithms * algo = new Algorithms_FSM();
    sequence seq;
    seq = algo->generateCheckingSequence(S, M);

    emit checkingSequenceResults(seq);
    */
    Algorithms * algo = new Algorithms_FSM();
    sequence seq;
    seq = algo->generateCheckingSequence(SpecificationMachine, MutationMachine);
    emit checkingSequenceResults(seq);
}

bool Model::saveSVG(string dot)
{
    //save .dot
    string dotPath = "tmp/spec.dot";
    string svgPath = "tmp/spec.svg";
    ofstream dotFile;
    dotFile.open(dotPath);
    dotFile << dot;
    dotFile.close();
    string o_arg = "-o tmp/spec.svg";
    char* args[] = {const_cast<char*>("dot"), const_cast<char*>("-Tsvg"), const_cast<char*>(dotPath.c_str()), const_cast<char*>(o_arg.c_str()) };

    const int argc = sizeof(args)/sizeof(char*);
    GVC_t *gvc;
    Agraph_t * g;
    gvc = gvContext();
    gvParseArgs(gvc, argc, args);
    FILE * fpDot;
    FILE * fpSVG;
    fpDot = fopen(dotPath.c_str(), "r");
    fpSVG = fopen(svgPath.c_str(), "wb+");
    g = agread(fpDot, 0);
    gvLayout(gvc, g, "dot");
    gvRender(gvc, g, "svg", fpSVG);
    gvFreeLayout(gvc, g);
    agclose(g);
    fclose(fpDot);
    return (gvFreeContext(gvc));
}

void Model::generateSpecification(QTableWidget * tableTransitions, int nbOfStates, QTableWidget * tableInputs, QTableWidget * tableOutputs)
{

    set<int> S;
    for (int i=1; i<nbOfStates+1; i++) {
        S.insert(i);
    }
    int s0 = 1;

    set<string> I;
    for (int row=0; row<tableInputs->rowCount()-1; row++) {
        I.insert(tableInputs->item(row, 0)->text().toStdString());
    }
    set<string> O;
    for (int row=0; row<tableOutputs->rowCount()-1; row++) {
        O.insert(tableOutputs->item(row, 0)->text().toStdString());
    }
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

    SpecificationMachine = new FSM(S, s0, I, O, lambda);
    if (showSpecification) {
        this->saveSVG(SpecificationMachine->generateDot());

        emit machineSVGGenerated(true);
    }
}

void Model::generateMutation(QTableWidget * tableTransitions, int nbOfStates, QTableWidget * tableInputs, QTableWidget * tableOutputs)
{

    set<int> S;
    for (int i=1; i<nbOfStates+1; i++) {
        S.insert(i);
    }
    int s0 = 1;

    set<string> I;
    for (int row=0; row<tableInputs->rowCount()-1; row++) {
        I.insert(tableInputs->item(row, 0)->text().toStdString());
    }
    set<string> O;
    for (int row=0; row<tableOutputs->rowCount()-1; row++) {
        O.insert(tableOutputs->item(row, 0)->text().toStdString());
    }
    vector<IOTransition *> lambda;
    if (SpecificationMachine)
        lambda = SpecificationMachine->getTransitions();
    vector<IOTransition *> newLambda;
    int id = SpecificationMachine->getTransitionSize();
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
    showSpecification = false;
    MutationMachine = new FSM(S, s0, I, O, lambda);
    MutationMachine->addTransitions(newLambda, true);
    this->saveSVG(MutationMachine->generateDot());

    emit machineSVGGenerated(true);
}
