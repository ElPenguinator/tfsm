#include "model.h"
using namespace std;
#include "algorithm/algorithms_fsm.h"
#include "algorithm/algorithms_tfsm_to.h"
#include "algorithm/algorithms_tfsm.h"
#include <graphviz/gvc.h>
#include "factory/fsmfactory.h"
#include "factory/tfsmfactory.h"
#include "factory/tfsmtofactory.h"

//TMP
#include "algorithm/timedintervalinputsequence.h"

void testOmer()
{
    set<int> S = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"x", "y"};
    vector<IOTransition *> lambda = {new GuardedTransition(1, "a", Guard("[", 0, 3, "]"), "x", 2, 1),
                                     new GuardedTransition(2, "a", Guard("[", 2, 4, "]"), "x", 3, 2),
                                     new GuardedTransition(2, "b", Guard("[", 0, 3, "]"), "x", 3, 3),
                                     new GuardedTransition(3, "a", Guard("[", 1, 4, "]"), "x", 4, 4),
                                     new GuardedTransition(3, "b", Guard("[", 1, 4, "]"), "x", 3, 5),
                                     new GuardedTransition(4, "a", Guard("[", 1, 5, "]"), "y", 1, 6)
                                    };
    vector<TimeoutTransition *> delta = {new TimeoutTransition(1, 5, 2, 7),
                                         new TimeoutTransition(2, 5, 1, 8),
                                         new TimeoutTransition(3, 5, 1, 9),
                                         new TimeoutTransition(4, 6, 1, 10),
                                        };

    TFSM * Spec = new TFSM(S, s0, I, O, lambda, delta);
    TFSM * Muta = new TFSM(S, s0, I, O, lambda, delta);
    vector<IOTransition *> newLambda = {new GuardedTransition(3, "a", Guard("[", 3, 4, "]"), "y", 3, 11),
                                        new GuardedTransition(3, "a", Guard("[", 1, 3, ")"), "x", 4, 12),
                                        new GuardedTransition(4, "a", Guard("[", 1, 4, ")"), "y", 2, 13),
                                        new GuardedTransition(4, "a", Guard("[", 2, 5, "]"), "y", 2, 14),
                                        new GuardedTransition(4, "a", Guard("[", 4, 5, "]"), "y", 1, 15)
                                       };
    Muta->addTransitions(newLambda, true);
    vector<TimeoutTransition *> newDelta = {new TimeoutTransition(4, 5, 2, 16),
                                        new TimeoutTransition(3, 4, 1, 17)
                                       };
    //Muta->addTransitions(newLambda, true);
    Muta->addTimeouts(newDelta, true);
    Spec->print();
    Muta->print();

    Algorithms * transformator = new Algorithms_TFSM(false, false);
    FSM * fullMuta = transformator->completeMutation(Muta);
    fullMuta->print();

    DistinguishingAutomaton_TFSM * product = new DistinguishingAutomaton_TFSM(Spec, Muta);
    product->initialize();
    product->print();
    {
        //save .dot
        string dotPath = "tmp/specOmer.dot";
        string svgPath = "tmp/specOmer.svg";
        ofstream dotFile;
        dotFile.open(dotPath);
        dotFile << Spec->generateDot();
        dotFile.close();
        string o_arg = "-o tmp/specOmer.svg";
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
        gvFreeContext(gvc);
    }
    {
        //save .dot
        string dotPath = "tmp/mutaOmer.dot";
        string svgPath = "tmp/mutaOmer.svg";
        ofstream dotFile;
        dotFile.open(dotPath);
        dotFile << Muta->generateDot();
        dotFile.close();
        string o_arg = "-o tmp/mutaOmer.svg";
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
        gvFreeContext(gvc);
    }
    {
        //save .dot
        string dotPath = "tmp/prodOmer.dot";
        string svgPath = "tmp/prodOmer.svg";
        ofstream dotFile;
        dotFile.open(dotPath);
        dotFile << product->generateDot();
        dotFile.close();
        string o_arg = "-o tmp/prodOmer.svg";
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
        gvFreeContext(gvc);
    }
}

Model::Model() : QObject()
{
    showSpecification = true;
    currentFactory = new FSMFactory();
    //testOmer();
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
    cout << "Import : " << fileName.toStdString() << endl;

    ifstream fileFill(fileName.toStdString(), ios::in);
    if(fileFill) {
        string line;
        getImportedType(line);
        MachineLoader * specLoader = currentFactory->getLoader();
        MachineLoader * mutaLoader = currentFactory->getLoader();
        //First line
        getline(fileFill, line);
        cout << "Bidibip " << line << endl;


        while(getline(fileFill, line))
        {
            specLoader->readLine(line, true);
            mutaLoader->readLine(line, false);
        }

        SpecificationMachine = specLoader->getResult();
        MutationMachine = mutaLoader->getResult();
        cout << "Result Spec : " << endl;
        SpecificationMachine->print();
        cout << "Result Muta : " << endl;
        MutationMachine->print();
        fileFill.close();
        currentFactory->fillTabs(MutationMachine, map, edit);
        this->saveSVG(MutationMachine->generateDot());
        emit machineSVGGenerated(true);
    }
    else {
        cout << "Non" << endl;
    }
}

void Model::exportFile(QString fileName)
{
    cout << "Export : " << fileName.toStdString() << endl;
    string dotPath = fileName.toStdString();
    ofstream dotFile;
    dotFile.open(dotPath);
    dotFile << MutationMachine->generateDot();
    dotFile.close();
}

void Model::checkingExperiment()
{
    cout << ":D " << endl;
    Algorithms * algo = currentFactory->getAlgorithms(true, false);
    vector<Sequence *> E;
    vector<Sequence *> Einit;
    SpecificationMachine->print();
    MutationMachine->print();
    for (int s : MutationMachine->states) {
        for (string i : MutationMachine->inputs) {
            cout << "[" << s << " | " << i << "] : " << endl;
            for (auto eta : MutationMachine->getEta(s, i)) {
                cout << "{";
                for (int id : eta) {
                    cout << id << " " << endl;
                }
                cout << "}";
            }
        }
    }

    E = algo->generateCheckingExperiment(Einit, SpecificationMachine, MutationMachine);
    cout << "E : " << endl;
    for (auto s : E) {
        cout << s->toString() << endl;//printSequence(s);
    }

    emit checkingExperimentResults(E);
}

void Model::checkingSequence()
{
    /*
    TimedIntervalInputSequence *test = new TimedIntervalInputSequence();
    vector<pair<string, Guard>> elements = {make_pair<string, Guard>("b", Guard("[", 0, inf, ")")),
                                           make_pair<string, Guard>("a", Guard("[", 0, inf, ")")),
                                           make_pair<string, Guard>("a", Guard("[", 0, 3, ")")),
                                           make_pair<string, Guard>("a", Guard("[", 0, 3, ")"))};
    test->addElements(elements);
    cout << test->toString() << endl;
    DistinguishingAutomaton_TFSM * testD = new DistinguishingAutomaton_TFSM(this->SpecificationMachine, this->MutationMachine);
    testD->initialize();
    set<string> * results = new set<string>();
    executingPath currentPath;
    testD->reachableStates(testD->initialState, currentPath, results, test, 0, 0, 0);
    for (string key : (*results)) {
        cout << "Result : " << key << endl;
    }
    set<string> tmp;
    Sequence * newSeq = testD->inputSequenceFromAcceptedLanguage(tmp, test);

    cout << newSeq->toString() << endl;
    */

    cout << ":( " << endl;
    Algorithms * algo = currentFactory->getAlgorithms(true, true);
    Sequence * seq;
    seq = algo->generateCheckingSequence(SpecificationMachine, MutationMachine);
    cout << seq->toString() << endl;
    //printSequence(seq);
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
    cout << "Coucou !" << endl;
    Algorithms * algo = currentFactory->getAlgorithms(true, true);
    algo->checkingExperimentBenchmarks(folder, nbStates, nbMutations, nbMachines, timeoutedValue, maxTimeout);
}

void Model::checkingSequenceBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
{
cout << "Yo !" << endl;
}
