#include "model.h"
using namespace std;
#include "algorithm/algorithms_fsm.h"
#include "algorithm/algorithms_tfsm_to.h"
#include "algorithm/algorithms_tfsm.h"
#include <graphviz/gvc.h>
#include "factory/fsmfactory.h"
#include "factory/tfsmfactory.h"
#include "factory/tfsmtofactory.h"

Model::Model() : QObject()
{
showSpecification = true;
currentFactory = new FSMFactory();
}

Model::~Model()
{

}

void Model::bindEvents()
{
    //connect(_generationCurrentDatabaseContent, &MetricTable::valueChanged, this, &Model::onCurrentDatabaseContentChanged);
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
    cout << ":D " << endl;
    Algorithms * algo = currentFactory->getAlgorithms();
    vector<sequence> E;
    vector<sequence> Einit;
    SpecificationMachine->print();
    MutationMachine->print();
    E = algo->generateCheckingExperiment(Einit, SpecificationMachine, MutationMachine);
    cout << "E : " << endl;
    for (auto s : E) {
        printSequence(s);
    }
    emit checkingExperimentResults(E);
}

void Model::checkingSequence()
{
    cout << ":( " << endl;
    Algorithms * algo = currentFactory->getAlgorithms();
    sequence seq;
    seq = algo->generateCheckingSequence(SpecificationMachine, MutationMachine);
    printSequence(seq);
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
