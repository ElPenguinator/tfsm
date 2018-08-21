#ifndef MACHINELOADER_H
#define MACHINELOADER_H
#include "../machine/fsm.h"

class MachineLoader
{
public:
    MachineLoader();
    virtual void readLine(std::string line, bool createSpecification) = 0;
    virtual FSM * getResult() = 0;

};

#endif // MACHINELOADER_H
