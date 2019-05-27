#include "timedinputsequence.h"
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace std;

TimedInputSequence::TimedInputSequence() : Sequence()
{

}

string TimedInputSequence::toString()
{
    ostringstream res;
    for (pair<string, double> timeState : this->content) {
        res << "(" << timeState.first << "," << timeState.second << ")";
    }
    return res.str();
}

int TimedInputSequence::getSize()
{
    return this->content.size();
}

std::pair<string, double> TimedInputSequence::getElement(int index)
{
    return this->content[index];
}

void TimedInputSequence::addElement(std::pair<std::string, double> element)
{
    this->content.push_back(element);
}

void TimedInputSequence::addElements(std::vector<std::pair<std::string, double> > elements)
{
    this->content.insert(this->content.end(), elements.begin(), elements.end());
}
