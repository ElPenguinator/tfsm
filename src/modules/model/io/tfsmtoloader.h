#ifndef TFSMTOLOADER_H
#define TFSMTOLOADER_H
#include "machineloader.h"
#include "../machine/tfsm_to.h"

class TFSMTOLoader : public MachineLoader
{
private:
    TFSM_TO * machine;
public:
    TFSMTOLoader();
    void readLine(std::string line, bool createSpecification);
    FSM * getResult();
};

#endif // TFSMTOLOADER_H
