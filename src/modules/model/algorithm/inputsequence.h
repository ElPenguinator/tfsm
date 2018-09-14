#ifndef INPUTSEQUENCE_H
#define INPUTSEQUENCE_H
#include "sequence.h"
#include <vector>

class InputSequence : public Sequence
{
public:
    InputSequence();
    std::vector<std::string> content;
    std::string toString();
    int getSize();
    std::string getElement(int index);
    void addElement(std::string element);
    void addElements(std::vector<std::string> elements);
};

#endif // INPUTSEQUENCE_H
