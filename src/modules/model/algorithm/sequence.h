#ifndef SEQUENCE_H
#define SEQUENCE_H
#include <string>

class Sequence
{
public:
    Sequence();
    virtual std::string toString() = 0;
    virtual int getSize() = 0;
};

#endif // SEQUENCE_H
