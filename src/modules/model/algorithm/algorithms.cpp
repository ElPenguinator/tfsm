#include "algorithms.h"
#include <sstream>
#include <graphviz/gvc.h>
using namespace std;
Algorithms::Algorithms(bool generateLogs, bool onlyDot)
{
    this->generateLogs = generateLogs;
    this->nbPassedMutants = 0;
    this->nbVerifying = 0;
    this->logPath = "test/";
    this->onlyDot = onlyDot;
}

void Algorithms::printDOT(string content, string path)
{
    ofstream dotFile;
    dotFile.open(path);
    dotFile << content;
    dotFile.close();
}

bool Algorithms::saveSVG(string dotPath, string svgPath, string dot)
{
    //save .dot
    ofstream dotFile;
    dotFile.open(dotPath);
    dotFile << dot;
    dotFile.close();
    if (!onlyDot) {

        string o_arg = "-o " + svgPath;
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
        //return false;
    }
    else {
        return false;
    }
}

void Algorithms::savePath(std::string pathsPath, std::vector<executingPath> paths)
{
    ofstream pathsFile;
    pathsFile.open(pathsPath);
    for (auto path : paths) {
        for (auto id : path) {
            pathsFile << id << " -> ";
        }
        pathsFile << endl;
    }
    pathsFile.close();
}

void Algorithms::saveSequence(std::string sequencePath, Sequence * seq)
{
    ofstream seqFile;
    seqFile.open(sequencePath);
    seqFile << seq->toString();
    seqFile.close();
}
