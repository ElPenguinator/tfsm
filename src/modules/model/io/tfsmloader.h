#ifndef TFSMLOADER_H
#define TFSMLOADER_H
#include "machineloader.h"
#include "../machine/tfsm.h"

class TFSMLoader : public MachineLoader
{
private:
    TFSM * machine;
public:
    TFSMLoader();
    void readLine(std::string line, bool createSpecification);
    FSM * getResult();
};

#endif // TFSMLOADER_H
