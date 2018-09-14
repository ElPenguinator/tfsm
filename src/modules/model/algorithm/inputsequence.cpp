#include "inputsequence.h"
#include <sstream>
using namespace std;

InputSequence::InputSequence() : Sequence()
{

}

string InputSequence::toString()
{
    ostringstream res;
    for (string elt : this->content) {
        res << elt;
    }
    return res.str();
}

int InputSequence::getSize()
{
    return this->content.size();
}

std::string InputSequence::getElement(int index)
{
    return this->content[index];
}

void InputSequence::addElement(std::string element)
{
    this->content.push_back(element);
}

void InputSequence::addElements(std::vector<std::string> elements)
{
    this->content.insert(this->content.end(), elements.begin(), elements.end());
}
