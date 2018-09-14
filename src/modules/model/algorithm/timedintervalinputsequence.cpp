#include "timedintervalinputsequence.h"

using namespace std;

TimedIntervalInputSequence::TimedIntervalInputSequence() : Sequence()
{

}

string TimedIntervalInputSequence::toString()
{
    return "!";
}

int TimedIntervalInputSequence::getSize()
{
    return this->content.size();
}

std::pair<std::string, Guard> TimedIntervalInputSequence::getElement(int index)
{
    return this->content[index];
}

void TimedIntervalInputSequence::addElement(std::pair<std::string, Guard>  element)
{
    this->content.push_back(element);
}

void TimedIntervalInputSequence::addElements(std::vector<std::pair<std::string, Guard> > elements)
{
    this->content.insert(this->content.end(), elements.begin(), elements.end());
}
