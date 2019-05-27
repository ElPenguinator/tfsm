#ifndef TIMEDINPUTSEQUENCE_H
#define TIMEDINPUTSEQUENCE_H
#include "sequence.h"
#include <vector>

class TimedInputSequence : public Sequence
{
public:
    TimedInputSequence();
    std::vector<std::pair<std::string, double> > content;
    std::string toString();
    int getSize();
    std::pair<std::string, double> getElement(int index);
    void addElement(std::pair<std::string, double> element);
    void addElements(std::vector<std::pair<std::string, double> > elements);
};

#endif // TIMEDINPUTSEQUENCE_H
