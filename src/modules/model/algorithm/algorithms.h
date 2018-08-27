#ifndef ALGORITHMS_H
#define ALGORITHMS_H
#include "../machine/fsm.h"
#include "../machine/distinguishingautomaton_fsm.h"
#include <iostream>
#include "../structs.h"
#include "cryptominisat5/cryptominisat.h"
#include "../tools.h"
#include <ctime>
#include <fstream>
#include <math.h>


class Algorithms
{
protected:
    bool generateLogs;
    int nbPassedMutants;
    int nbVerifying;
public:
    Algorithms(bool generateLogs);

    void printDOT(std::string content, std::string path);

    virtual FSM * generateSubmachine(CMSat::SATSolver * &solver, FSM * M) = 0;

    virtual void computePhiP(CMSat::SATSolver * &solver, FSM * P) = 0;

    virtual void computePhiE(CMSat::SATSolver * &solver, std::vector<sequence> E, DistinguishingAutomaton_FSM * D) = 0;

    virtual void computePhiM(CMSat::SATSolver * &solver, FSM * S, FSM * M) = 0;

    virtual sequence verifyCheckingExperiment(CMSat::SATSolver * &solver,std::vector<sequence> E, FSM * S, DistinguishingAutomaton_FSM * D) = 0;

    virtual std::vector<sequence> generateCheckingExperimentTimeouted(std::vector<sequence> Einit, FSM * S, FSM * M) = 0;

    virtual std::vector<sequence> generateCheckingExperiment(std::vector<sequence> Einit, FSM * S, FSM * M) = 0;

    virtual sequence verifyCheckingSequence(CMSat::SATSolver * &solver,sequence CS, FSM * S, DistinguishingAutomaton_FSM * D) = 0;

    virtual sequence generateCheckingSequenceTimeouted(FSM * S, FSM * M) = 0;

    virtual sequence generateCheckingSequence(FSM * S, FSM * M) = 0;

    virtual void checkingExperimentBenchmarks() = 0;

    virtual void checkingSequenceBenchmarks() = 0;

    virtual std::vector<sequence> removePrefixes(std::vector<sequence> E) = 0;

    bool saveSVG(std::string dotPath, std::string svgPath, std::string dot);

    void savePath(std::string pathsPath, std::vector<executingPath> paths);
    virtual FSM * completeMutation(FSM * M) = 0;
};

#endif // ALGORITHMS_H
