#include "algorithms.h"
#include <sstream>
using namespace std;
Algorithms::Algorithms(bool generateLogs)
{
    this->generateLogs = generateLogs;
    this->nbPassedMutants = 0;
}

void Algorithms::printDOT(string content, string path)
{
    ofstream dotFile;
    dotFile.open(path);
    dotFile << content;
    dotFile.close();
}
