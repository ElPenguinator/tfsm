#ifndef TIMEDINPUTSEQUENCE_H
#define TIMEDINPUTSEQUENCE_H
#include "sequence.h"
#include <vector>

class TimedInputSequence : public Sequence
{
public:
    TimedInputSequence();
    std::vector<std::pair<std::string, int> > content;
    std::string toString();
    int getSize();
    std::pair<std::string, int> getElement(int index);
    void addElement(std::pair<std::string, int> element);
    void addElements(std::vector<std::pair<std::string, int> > elements);
};

#endif // TIMEDINPUTSEQUENCE_H
