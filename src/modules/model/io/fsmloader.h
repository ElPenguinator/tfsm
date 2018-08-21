#ifndef FSMLOADER_H
#define FSMLOADER_H
#include "machineloader.h"

class FSMLoader : public MachineLoader
{
private:
    FSM * machine;
public:
    FSMLoader();
    void readLine(std::string line, bool createSpecification);
    FSM * getResult();
};

#endif // FSMLOADER_H
