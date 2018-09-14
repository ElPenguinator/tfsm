#ifndef FSMALGORITHMS_H
#define FSMALGORITHMS_H
#include "algorithms.h"

class Algorithms_FSM : public Algorithms
{
public:
    Algorithms_FSM(bool generateLogs, bool onlyDot);

    FSM * generateSubmachine(CMSat::SATSolver * &solver, FSM * M);

    void computePhiP(CMSat::SATSolver * &solver, FSM * P);

    void computePhiE(CMSat::SATSolver * &solver, std::vector<Sequence *> E, DistinguishingAutomaton_FSM * D);

    void computePhiM(CMSat::SATSolver * &solver, FSM * S, FSM * M);

    Sequence * verifyCheckingExperiment(CMSat::SATSolver * &solver,std::vector<Sequence *> E, FSM * S, DistinguishingAutomaton_FSM * D);

    std::vector<Sequence *> generateCheckingExperimentTimeouted(std::vector<Sequence *> Einit, FSM * S, FSM * M);

    std::vector<Sequence *> generateCheckingExperiment(std::vector<Sequence *> Einit, FSM * S, FSM * M);

    Sequence * verifyCheckingSequence(CMSat::SATSolver * &solver,Sequence * CS, FSM * S, DistinguishingAutomaton_FSM * D);

    Sequence * generateCheckingSequenceTimeouted(FSM * S, FSM * M);

    Sequence * generateCheckingSequence(FSM * S, FSM * M);

    FSM * generateRandomSpecification(int nbOfStates, int maxTime, std::set<std::string> I, std::set<std::string> O);

    FSM * generateRandomMutation(FSM * S, int maxTime, int numberOfMutations);

    void checkingExperimentBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout);

    void checkingSequenceBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout);

    std::vector<Sequence *> removePrefixes(std::vector<Sequence *> E);

    FSM * completeMutation(FSM * M);

    InfInt computeNumberOfMutants(FSM * M);
};

#endif // FSMALGORITHMS_H
