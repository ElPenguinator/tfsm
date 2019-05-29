#include "model.h"
using namespace std;
#include "algorithm/algorithms_fsm.h"
#include "algorithm/algorithms_tfsm_to.h"
#include "algorithm/algorithms_tfsm.h"
#include <graphviz/gvc.h>
#include "factory/fsmfactory.h"
#include "factory/tfsmfactory.h"
#include "factory/tfsmtofactory.h"
#include "algorithm/timedinputsequence.h"

//TMP
#include "algorithm/timedintervalinputsequence.h"


void testOmer1()
{
    MachineFactory * currentFactory = new TFSMFactory();
    set<int> S = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"x", "y"};

    vector<IOTransition *> lambda = {new GuardedTransition(1, "a", Guard("[", 0, inf, ")"), "x", 1, 0),
                                     new GuardedTransition(1, "b", Guard("[", 0, 6, ")"), "x", 2, 1),
                                     new GuardedTransition(1, "b", Guard("[", 6, inf, ")"), "x", 4, 2),
                                      new GuardedTransition(2, "a", Guard("[", 0, inf, ")"), "x", 3, 4),
                                      new GuardedTransition(2, "b", Guard("[", 0, 3, "]"), "x", 1, 5),
                                      new GuardedTransition(2, "b", Guard("(", 3, 5, ")"), "x", 2, 6),
                                      new GuardedTransition(2, "b", Guard("[", 5, inf, ")"), "x", 3, 7),
                                       new GuardedTransition(3, "a", Guard("[", 0, 3, ")"), "x", 3, 9),
                                       new GuardedTransition(3, "a", Guard("[", 3, inf, "]"), "x", 4, 10),
                                       new GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "x", 4, 11),
                                        new GuardedTransition(4, "a", Guard("[", 0, inf, "]"), "y", 1, 13),
                                        new GuardedTransition(4, "b", Guard("[", 0, inf, "]"), "x", 1, 14)
                                        };
    vector<TimeoutTransition *> delta = {new TimeoutTransition(1, inf, 1, 3),
                                         new TimeoutTransition(2, inf, 2, 8),
                                         new TimeoutTransition(3, inf, 3, 12),
                                         new TimeoutTransition(4, inf, 4, 15),
                                        };

    TFSM * Spec = new TFSM(S, s0, I, O, lambda, delta);
    TFSM * Muta = new TFSM(S, s0, I, O, lambda, delta);
    vector<IOTransition *> newLambda = {new GuardedTransition(1, "a", Guard("[", 0, 3, "]"), "x", 1, 16),
                                        new GuardedTransition(1, "a", Guard("(", 3, inf, ")"), "y", 2, 17),
                                        new GuardedTransition(3, "a", Guard("[", 3, inf, ")"), "x", 3, 19),
                                        new GuardedTransition(3, "b", Guard("[", 0, 7, "]"), "y", 3, 20),
                                        new GuardedTransition(3, "b", Guard("(", 7, 12, ")"), "x", 4, 21),
                                        new GuardedTransition(3, "b", Guard("[", 12, inf, ")"), "x", 1, 22),
                                        new GuardedTransition(4, "a", Guard("[", 0, 5, ")"), "y", 1, 23),
                                        new GuardedTransition(4, "a", Guard("[", 5, inf, ")"), "y", 2, 24),
                                        new GuardedTransition(4, "a", Guard("[", 5, inf, ")"), "x", 2, 25)
                                       };
    Muta->addTransitions(newLambda, true);
    vector<TimeoutTransition *> newDelta = {new TimeoutTransition(1, 5, 2, 18)
                                       };

    Muta->addTimeouts(newDelta, true);


    Spec->print();
    Muta->print();
    /*
    cout << "Test 1 : " << endl;

    vector<Sequence *> E;
    vector<Sequence *> Einit;
    Algorithms * algo = currentFactory->getAlgorithms(false, false);
    E = algo->generateCheckingExperiment(Einit, Spec, Muta);
    cout << "E : " << endl;
    for (auto s : E) {
        cout << s->toString() << endl;//printSequence(s);
    }
    */

    cout << "Test 2 : " << endl;

    vector<Sequence *> E2;
    vector<Sequence *> Einit2;

    TimedInputSequence * seq1 = new TimedInputSequence();
    seq1->addElements(vector<pair<string, double>>{make_pair("a", 3.5)});
    Einit2.push_back(seq1);

    TimedInputSequence * seq2 = new TimedInputSequence();
    seq2->addElements(vector<pair<string, double>>{make_pair("b", 10.5), make_pair("a", 11)});
    Einit2.push_back(seq2);

    TimedInputSequence * seq3 = new TimedInputSequence();
    seq3->addElements(vector<pair<string, double>>{make_pair("b", 6.5), make_pair("a", 12)});
    Einit2.push_back(seq3);

    TimedInputSequence * seq4 = new TimedInputSequence();
    seq4->addElements(vector<pair<string, double>>{make_pair("b", 0.5), make_pair("b", 6), make_pair("a", 9.5), make_pair("a", 10)});
    Einit2.push_back(seq4);

    TimedInputSequence * seq5 = new TimedInputSequence();
    seq5->addElements(vector<pair<string, double>>{make_pair("b", 6.5), make_pair("a", 12), make_pair("b", 18.5), make_pair("a", 19)});
    Einit2.push_back(seq5);

    Algorithms * algo2 = currentFactory->getAlgorithms(true, false);
    E2 = algo2->generateCheckingExperiment(Einit2, Spec, Muta);
    cout << "E2 : " << endl;
    for (auto s : E2) {
        cout << s->toString() << endl;//printSequence(s);
    }


}

void testOmer2()
{
    MachineFactory * currentFactory = new TFSMFactory();
    set<int> S = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"x", "y"};

    vector<IOTransition *> lambda = {new GuardedTransition(1, "a", Guard("[", 0, inf, ")"), "x", 1, 0),
                                     new GuardedTransition(1, "b", Guard("[", 0, 3, ")"), "x", 2, 1),
                                     new GuardedTransition(1, "b", Guard("[", 3, inf, ")"), "x", 4, 2),
                                      new GuardedTransition(2, "a", Guard("[", 0, inf, ")"), "x", 3, 4),
                                      new GuardedTransition(2, "b", Guard("[", 0, 3, "]"), "x", 1, 5),
                                      new GuardedTransition(2, "b", Guard("(", 3, 5, ")"), "x", 2, 6),
                                      new GuardedTransition(2, "b", Guard("[", 5, inf, ")"), "x", 3, 7),
                                       new GuardedTransition(3, "a", Guard("[", 0, inf, ")"), "x", 3, 9),
                                       new GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "x", 4, 10),
                                        new GuardedTransition(4, "a", Guard("[", 0, inf, "]"), "y", 1, 12),
                                        new GuardedTransition(4, "b", Guard("[", 0, inf, "]"), "x", 1, 13)
                                        };
    vector<TimeoutTransition *> delta = {new TimeoutTransition(1, 4, 1, 3),
                                         new TimeoutTransition(2, inf, 2, 8),
                                         new TimeoutTransition(3, 5, 2, 11),
                                         new TimeoutTransition(4, inf, 4, 14),
                                        };

    TFSM * Spec = new TFSM(S, s0, I, O, lambda, delta);
    TFSM * Muta = new TFSM(S, s0, I, O, lambda, delta);
    vector<IOTransition *> newLambda = {new GuardedTransition(1, "a", Guard("[", 0, 3, "]"), "x", 1, 15),
                                        new GuardedTransition(1, "a", Guard("(", 3, inf, ")"), "y", 2, 16),
                                        new GuardedTransition(3, "a", Guard("[", 3, inf, ")"), "x", 3, 18),
                                        new GuardedTransition(3, "a", Guard("[", 0, 3, ")"), "y", 3, 19),
                                        new GuardedTransition(3, "b", Guard("[", 0, 7, "]"), "y", 3, 20),
                                        new GuardedTransition(3, "b", Guard("[", 0, 7, "]"), "x", 4, 21),
                                        new GuardedTransition(3, "b", Guard("(", 7, 12, ")"), "y", 4, 22),
                                        new GuardedTransition(3, "b", Guard("[", 12, inf, ")"), "x", 1, 23),
                                        new GuardedTransition(4, "a", Guard("[", 0, 5, ")"), "y", 1, 26),
                                        new GuardedTransition(4, "a", Guard("[", 5, inf, ")"), "y", 2, 27),
                                        new GuardedTransition(4, "a", Guard("[", 5, inf, ")"), "x", 2, 28)
                                       };
    Muta->addTransitions(newLambda, true);
    vector<TimeoutTransition *> newDelta = {new TimeoutTransition(1, 3, 2, 17),
                                            new TimeoutTransition(3, 5, 2, 24),
                                            new TimeoutTransition(3, 8, 1, 25),
                                            new TimeoutTransition(4, 4, 1, 29),
                                       };
    Muta->addTimeouts(newDelta, true);


    Spec->print();
    Muta->print();
/*
    cout << "Test 1 : " << endl;

    vector<Sequence *> E;
    vector<Sequence *> Einit;
    Algorithms * algo = currentFactory->getAlgorithms(false, false);
    E = algo->generateCheckingExperiment(Einit, Spec, Muta);
    cout << "E : " << endl;
    for (auto s : E) {
        cout << s->toString() << endl;//printSequence(s);
    }
*/

    cout << "Test 2 : " << endl;

    vector<Sequence *> E2;
    vector<Sequence *> Einit2;

    TimedInputSequence * seq1 = new TimedInputSequence();
    seq1->addElements(vector<pair<string, double>>{make_pair("a", 3.5)});
    Einit2.push_back(seq1);

    TimedInputSequence * seq2 = new TimedInputSequence();
    seq2->addElements(vector<pair<string, double>>{make_pair("b", 3.5), make_pair("a", 4)});
    Einit2.push_back(seq2);

    TimedInputSequence * seq3 = new TimedInputSequence();
    seq3->addElements(vector<pair<string, double>>{make_pair("b", 3.5), make_pair("a", 9)});
    Einit2.push_back(seq3);

    TimedInputSequence * seq4 = new TimedInputSequence();
    seq4->addElements(vector<pair<string, double>>{make_pair("b", 3.5), make_pair("a", 9), make_pair("b", 12.5), make_pair("a", 13)});
    Einit2.push_back(seq4);

    TimedInputSequence * seq5 = new TimedInputSequence();
    seq5->addElements(vector<pair<string, double>>{make_pair("b", 0.5), make_pair("b", 6), make_pair("b", 11.5), make_pair("a", 12)});
    Einit2.push_back(seq5);

    Algorithms * algo2 = currentFactory->getAlgorithms(true, false);
    E2 = algo2->generateCheckingExperiment(Einit2, Spec, Muta);
    cout << "E2 : " << endl;
    for (auto s : E2) {
        cout << s->toString() << endl;//printSequence(s);
    }
}

void testOmer3()
{
    MachineFactory * currentFactory = new TFSMFactory();
    set<int> S = {1, 2, 3};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"x", "y"};

    vector<IOTransition *> lambda = {new GuardedTransition(1, "a", Guard("[", 0, inf, ")"), "x", 1, 0),
                                     new GuardedTransition(1, "b", Guard("[", 0, inf, ")"), "x", 2, 1),
                                      new GuardedTransition(2, "a", Guard("[", 0, 2, ")"), "y", 3, 3),
                                      new GuardedTransition(2, "a", Guard("[", 2, 5, ")"), "x", 3, 4),
                                      new GuardedTransition(2, "a", Guard("[", 5, inf, ")"), "x", 1, 5),
                                      new GuardedTransition(2, "b", Guard("[", 0, inf, ")"), "x", 2, 6),
                                       new GuardedTransition(3, "a", Guard("[", 0, inf, ")"), "y", 3, 8),
                                       new GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "x", 1, 9)
                                        };
    vector<TimeoutTransition *> delta = {new TimeoutTransition(1, inf, 1, 2),
                                         new TimeoutTransition(2, 8, 1, 7),
                                         new TimeoutTransition(3, 5, 1, 10)
                                        };

    TFSM * Spec = new TFSM(S, s0, I, O, lambda, delta);
    TFSM * Muta = new TFSM(S, s0, I, O, lambda, delta);
    vector<IOTransition *> newLambda = {new GuardedTransition(2, "b", Guard("[", 0, 3, "]"), "y", 2, 11),
                                        new GuardedTransition(2, "b", Guard("(", 3, inf, ")"), "x", 2, 12),
                                        new GuardedTransition(3, "a", Guard("[", 0, 3, ")"), "y", 3, 14),
                                        new GuardedTransition(3, "a", Guard("[", 3, 5, ")"), "y", 1, 15),
                                        new GuardedTransition(3, "a", Guard("[", 5, inf, "]"), "y", 1, 16)
                                       };
    Muta->addTransitions(newLambda, true);
    vector<TimeoutTransition *> newDelta = {new TimeoutTransition(2, 7, 3, 13)
                                       };
    Muta->addTimeouts(newDelta, true);


    Spec->print();
    Muta->print();

    cout << "Test 1 : " << endl;

    vector<Sequence *> E;
    vector<Sequence *> Einit;
    Algorithms * algo = currentFactory->getAlgorithms(false, false);
    E = algo->generateCheckingExperiment(Einit, Spec, Muta);
    cout << "E : " << endl;
    for (auto s : E) {
        cout << s->toString() << endl;//printSequence(s);
    }

/*
    cout << "Test 2 : " << endl;

    vector<Sequence *> E2;
    vector<Sequence *> Einit2;

    TimedInputSequence * seq1 = new TimedInputSequence();
    seq1->addElements(vector<pair<string, double>>{make_pair("a", 3.5)});
    Einit2.push_back(seq1);

    TimedInputSequence * seq2 = new TimedInputSequence();
    seq2->addElements(vector<pair<string, double>>{make_pair("b", 3.5), make_pair("a", 4)});
    Einit2.push_back(seq2);

    TimedInputSequence * seq3 = new TimedInputSequence();
    seq3->addElements(vector<pair<string, double>>{make_pair("b", 3.5), make_pair("a", 9)});
    Einit2.push_back(seq3);

    TimedInputSequence * seq4 = new TimedInputSequence();
    seq4->addElements(vector<pair<string, double>>{make_pair("b", 3.5), make_pair("a", 9), make_pair("b", 12.5), make_pair("a", 13)});
    Einit2.push_back(seq4);

    TimedInputSequence * seq5 = new TimedInputSequence();
    seq5->addElements(vector<pair<string, double>>{make_pair("b", 0.5), make_pair("b", 6), make_pair("b", 11.5), make_pair("a", 12)});
    Einit2.push_back(seq5);

    Algorithms * algo2 = currentFactory->getAlgorithms(true, false);
    E2 = algo2->generateCheckingExperiment(Einit2, Spec, Muta);
    cout << "E2 : " << endl;
    for (auto s : E2) {
        cout << s->toString() << endl;//printSequence(s);
    }
    */
}

Model::Model() : QObject()
{
    showSpecification = true;
    currentFactory = new FSMFactory();
    testOmer3();
}

Model::~Model()
{

}

void Model::bindEvents()
{
    //connect(_generationCurrentDatabaseContent, &MetricTable::valueChanged, this, &Model::onCurrentDatabaseContentChanged);
}

void Model::importFile(QString fileName, QMap<QString, QTableWidget *> map, QLineEdit *edit)
{
    ifstream fileFill(fileName.toStdString(), ios::in);
    if(fileFill) {
        string line;
        getImportedType(line);
        MachineLoader * specLoader = currentFactory->getLoader();
        MachineLoader * mutaLoader = currentFactory->getLoader();
        //First line
        getline(fileFill, line);


        while(getline(fileFill, line))
        {
            specLoader->readLine(line, true);
            mutaLoader->readLine(line, false);
        }

        SpecificationMachine = specLoader->getResult();
        MutationMachine = mutaLoader->getResult();
        fileFill.close();
        currentFactory->fillTabs(MutationMachine, map, edit);
        this->saveSVG(MutationMachine->generateDot());
        emit machineSVGGenerated(true);
    }
}

void Model::exportFile(QString fileName)
{
    string dotPath = fileName.toStdString();
    ofstream dotFile;
    dotFile.open(dotPath);
    dotFile << MutationMachine->generateDot();
    dotFile.close();
}

void Model::checkingExperiment()
{
    Algorithms * algo = currentFactory->getAlgorithms(true, false);
    vector<Sequence *> E;
    vector<Sequence *> Einit;

    E = algo->generateCheckingExperiment(Einit, SpecificationMachine, MutationMachine);

    emit checkingExperimentResults(E);
}

void Model::checkingSequence()
{
    Algorithms * algo = currentFactory->getAlgorithms(true, true);
    Sequence * seq;
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

void Model::generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates)
{
    SpecificationMachine = currentFactory->generateSpecification(map, nbOfStates);
    if (showSpecification) {
        this->saveSVG(SpecificationMachine->generateDot());

        emit machineSVGGenerated(true);
    }
}

void Model::generateMutation(QMap<QString, QTableWidget *> map, int nbOfStates)
{
    showSpecification = false;
    MutationMachine = currentFactory->generateMutation(SpecificationMachine, map, nbOfStates);
    this->saveSVG(MutationMachine->generateDot());

    emit machineSVGGenerated(true);
}

void Model::changeMachineType(const QString &type)
{
    if (type == "FSM") {
        currentFactory = new FSMFactory();
    } else if (type == "TFSM_TO") {
        currentFactory = new TFSMTOFactory();
    } else if (type == "TFSM") {
        currentFactory = new TFSMFactory();
    } else {
        //Not implemented
    }
}

void Model::getImportedType(std::string header)
{
    QString typePattern = "digraph\\s*([^\\s]+)\\s*\\{";
    QRegExp rx(typePattern);
    int index = rx.indexIn(QString::fromStdString(header));
    if (index != -1) {
        changeMachineType(rx.cap(2));
    }
}

void Model::checkingExperimentBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
{
    Algorithms * algo = currentFactory->getAlgorithms(true, true);
    algo->checkingExperimentBenchmarks(folder, nbStates, nbMutations, nbMachines, timeoutedValue, maxTimeout);
}

void Model::checkingSequenceBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
{
    //TODO
}
