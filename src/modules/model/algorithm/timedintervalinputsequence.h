#ifndef TIMEDINTERVALINPUTSEQUENCE_H
#define TIMEDINTERVALINPUTSEQUENCE_H
#include "sequence.h"
#include <vector>
#include "../machine/guard.h"

class TimedIntervalInputSequence : public Sequence
{
public:
    TimedIntervalInputSequence();
    std::vector<std::pair<std::string, Guard> > content;
    std::string toString();
    int getSize();
    std::pair<std::string, Guard> getElement(int index);
    void addElement(std::pair<std::string, Guard>  element);
    void addElements(std::vector<std::pair<std::string, Guard> > elements);
};

#endif // TIMEDINTERVALINPUTSEQUENCE_H
