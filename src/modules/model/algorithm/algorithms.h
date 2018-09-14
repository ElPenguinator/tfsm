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
#include "../../../lib/infint.h"

class Algorithms
{
protected:
    bool generateLogs;
    int nbPassedMutants;
    int nbVerifying;
    std::string logPath;
    bool onlyDot;
public:
    Algorithms(bool generateLogs, bool onlyDot);

    void printDOT(std::string content, std::string path);

    virtual FSM * generateSubmachine(CMSat::SATSolver * &solver, FSM * M) = 0;

    virtual void computePhiP(CMSat::SATSolver * &solver, FSM * P) = 0;

    virtual void computePhiE(CMSat::SATSolver * &solver, std::vector<Sequence *> E, DistinguishingAutomaton_FSM * D) = 0;

    virtual void computePhiM(CMSat::SATSolver * &solver, FSM * S, FSM * M) = 0;

    virtual Sequence * verifyCheckingExperiment(CMSat::SATSolver * &solver,std::vector<Sequence *> E, FSM * S, DistinguishingAutomaton_FSM * D) = 0;

    virtual std::vector<Sequence *> generateCheckingExperimentTimeouted(std::vector<Sequence *> Einit, FSM * S, FSM * M) = 0;

    virtual std::vector<Sequence *> generateCheckingExperiment(std::vector<Sequence *> Einit, FSM * S, FSM * M) = 0;

    virtual Sequence * verifyCheckingSequence(CMSat::SATSolver * &solver,Sequence * CS, FSM * S, DistinguishingAutomaton_FSM * D) = 0;

    virtual Sequence * generateCheckingSequenceTimeouted(FSM * S, FSM * M) = 0;

    virtual Sequence * generateCheckingSequence(FSM * S, FSM * M) = 0;

    virtual FSM * generateRandomSpecification(int nbOfStates, int maxTime, std::set<std::string> I, std::set<std::string> O) = 0;

    virtual FSM * generateRandomMutation(FSM * S, int maxTime, int numberOfMutations) = 0;

    virtual void checkingExperimentBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout) = 0;

    virtual void checkingSequenceBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout) = 0;

    virtual std::vector<Sequence *> removePrefixes(std::vector<Sequence *> E) = 0;

    bool saveSVG(std::string dotPath, std::string svgPath, std::string dot);

    void savePath(std::string pathsPath, std::vector<executingPath> paths);
    virtual FSM * completeMutation(FSM * M) = 0;

    virtual InfInt computeNumberOfMutants(FSM * M) = 0;
};

#endif // ALGORITHMS_H
