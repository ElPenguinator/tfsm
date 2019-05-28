#include "algorithms_tfsm_to.h"
#include "../machine/distinguishingautomaton_tfsm_to.h"
#include "../algorithm/timedinputsequence.h"
using namespace std;
using namespace CMSat;

Algorithms_TFSM_TO::Algorithms_TFSM_TO(bool generateLogs, bool onlyDot) : Algorithms(generateLogs, onlyDot)
{

}

FSM * Algorithms_TFSM_TO::generateSubmachine(SATSolver * &solver, FSM * M)
{
    lbool ret = solver->solve();
    if (ret == l_True) {
        set<int> S(M->states);
        int s0 = M->initialState;
        set<string> I(M->inputs);
        set<string> O(M->inputs);
        vector<IOTransition *> lambda;
        vector<TimeoutTransition *> delta;
        for (int i=0; i<M->getTransitions().size() + M->getTimeouts().size(); i++) {
            if (solver->get_model()[i] == l_True) {
                int id = i;
                if (M->isIdTimeout(id)) {
                    TimeoutTransition * corr = M->getTimeoutFromId(id);
                    delta.push_back(new TimeoutTransition(corr->src, corr->t, corr->tgt, corr->id));
                }
                else {
                    IOTransition * corr = M->getTransitionFromId(id);
                    lambda.push_back(new IOTransition(corr->src, corr->i, corr->o, corr->tgt, corr->id));
                }
            }
        }

        return new TFSM_TO(S, s0, I, O, lambda, delta);
    }
    else {
        return NULL;
    }
}

void Algorithms_TFSM_TO::computePhiP(SATSolver * &solver, FSM * P)
{
    vector<Lit> clause;
    for (auto transition : P->getTransitions()) {
        clause.push_back(Lit(transition->id, true));
    }
    for (auto timeout : P->getTimeouts()) {
        clause.push_back(Lit(timeout->id, true));
    }
    solver->add_clause(clause);
}

void Algorithms_TFSM_TO::computePhiE(SATSolver * &solver, vector<Sequence *> E, DistinguishingAutomaton_FSM * D)
{
    int cpt = 1;
    for (auto alpha : E) {
        vector<executingPath> rev = D->revealingPaths(alpha);
        if (generateLogs) {
            savePath(logPath + "paths" + to_string(nbVerifying) + "_" + to_string(cpt) +".paths", rev);
        }
        for (auto path : rev) {
            vector<Lit> clause;
            for (auto id : path) {
                if (D->mutationMachine->isIdTimeout(id)) {
                    TimeoutTransition * corresponding = D->mutationMachine->getTimeoutFromId(id);
                    vector<TimeoutTransition *> correspondingSuspicious = D->mutationMachine->getXi(corresponding->src);
                    if (correspondingSuspicious.size() > 1) {
                        clause.push_back(Lit(id, true));
                    }
                }
                else {
                    IOTransition * corresponding = D->mutationMachine->getTransitionFromId(id);
                    vector<IOTransition *> correspondingSuspicious = D->mutationMachine->getXi(corresponding->src, corresponding->i);
                    if (correspondingSuspicious.size() > 1) {
                        clause.push_back(Lit(id, true));
                    }
                }
            }
            solver->add_clause(clause);
        }
        cpt++;
    }
}

void Algorithms_TFSM_TO::computePhiM(SATSolver * &solver, FSM * S, FSM * M)
{

    for (auto s : M->states) {
        for (auto i : M->inputs) {
            vector<IOTransition *> res = M->getXi(s, i);
            for (int k=0; k<res.size(); k++) {
                for (int l=k+1; l<res.size(); l++) {
                    vector<Lit> clause;
                    clause.push_back(Lit(res[k]->id, true));
                    clause.push_back(Lit(res[l]->id, true));
                    solver->add_clause(clause);
                }
            }
            vector<Lit> clause;
            for (int k=0; k<res.size(); k++) {
                clause.push_back(Lit(res[k]->id, false));
            }
            solver->add_clause(clause);
        }
    }
    for (auto s : M->states) {
        vector<TimeoutTransition *> res = M->getXi(s);
        for (int k=0; k<res.size(); k++) {
            for (int l=k+1; l<res.size(); l++) {
                vector<Lit> clause;
                clause.push_back(Lit(res[k]->id, true));
                clause.push_back(Lit(res[l]->id, true));
                solver->add_clause(clause);
            }
        }
        vector<Lit> clause;
        for (int k=0; k<res.size(); k++) {
            clause.push_back(Lit(res[k]->id, false));
        }
        solver->add_clause(clause);
    }
    vector<Lit> clause;
    for (auto k : S->getTransitions()) {
        clause.push_back(Lit(k->id, true));
    }
    for (auto k : S->getTimeouts()) {
        clause.push_back(Lit(k->id, true));
    }
    solver->add_clause(clause);
}

Sequence * Algorithms_TFSM_TO::verifyCheckingExperiment(SATSolver * &solver,vector<Sequence *> E, FSM * S, DistinguishingAutomaton_FSM * D)
{

    computePhiE(solver, E, D);
    Sequence * alpha = new TimedInputSequence();
    FSM * P = D->mutationMachine;
    while (alpha->getSize() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            nbPassedMutants++;
            DistinguishingAutomaton_TFSM_TO * DP = new DistinguishingAutomaton_TFSM_TO(S, P);
            DP->initialize();
            if (generateLogs) {
                saveSVG(logPath + "mutant" + to_string(nbPassedMutants) +".dot", logPath + "mutant" + to_string(nbPassedMutants) +".svg", P->generateDot());
                saveSVG(logPath + "productMutant" + to_string(nbPassedMutants) + ".dot", logPath + "productMutant" + to_string(nbPassedMutants) + ".svg", DP->generateDot());
            }
            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                Sequence * nullPrefix = new TimedInputSequence();
                set<string> beginningStates;
                beginningStates.insert(DP->initialState->getKey());
                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, nullPrefix);
            }
            delete DP;
        }
    }
    nbVerifying++;
    return alpha;
}

vector<Sequence *> Algorithms_TFSM_TO::generateCheckingExperimentTimeouted(vector<Sequence *> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM_TO * D = new DistinguishingAutomaton_TFSM_TO(S, M);
    D->initialize();
    vector<Sequence *> E;
    vector<Sequence *> Ecurr = Einit;

    double elapsed_secs = 0;
    do {
        clock_t begin = clock();
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        Sequence * alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        if (alpha->getSize() > 0)
            Ecurr.push_back(alpha);
        clock_t end = clock();
        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
    }
    while (Ecurr.size() != 0 && elapsed_secs < 3600);
    if (elapsed_secs > 3600) {
        E.clear();
    }
    return removePrefixes(E);
}

vector<Sequence *> Algorithms_TFSM_TO::generateCheckingExperiment(vector<Sequence *> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();
    solver->log_to_file("/tmp/test.txt");
    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM_TO * D = new DistinguishingAutomaton_TFSM_TO(S, M);
    D->initialize();
    if (generateLogs) {
        saveSVG(logPath + "specification.dot", logPath + "specification.svg", S->generateDot());
        saveSVG(logPath + "mutation.dot", logPath + "mutation.svg", M->generateDot());
        saveSVG(logPath + "distinguishing.dot", logPath + "distinguishing.svg", D->generateDot());
    }
    vector<Sequence *> E;
    vector<Sequence *> Ecurr = Einit;

    do {
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        Sequence * alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        if (alpha->getSize() > 0)
            Ecurr.push_back(alpha);
    }
    while (Ecurr.size() != 0);
    return removePrefixes(E);
}

Sequence * Algorithms_TFSM_TO::verifyCheckingSequence(SATSolver * &solver,Sequence * CS, FSM * S, DistinguishingAutomaton_FSM * D)
{
    vector<Sequence *> E;
    E.push_back(CS);
    set<string> beginningStates;
    computePhiE(solver, E, D);
    Sequence * alpha = new TimedInputSequence();
    FSM * P = D->mutationMachine;
    while (alpha->getSize() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            DistinguishingAutomaton_TFSM_TO * DP = new DistinguishingAutomaton_TFSM_TO(S, P);
            DP->initialize();
            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, CS);
                if (alpha->getSize() == 0) {
                    computePhiP(solver, P);
                }
            }
            delete DP;
        }
    }
    return alpha;
}

Sequence * Algorithms_TFSM_TO::generateCheckingSequenceTimeouted(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM_TO * D = new DistinguishingAutomaton_TFSM_TO(S, M);
    D->initialize();
    Sequence * CS = new TimedInputSequence();
    Sequence * alpha = new TimedInputSequence();
    double elapsed_secs = 0;
    do {
        clock_t begin = clock();
        dynamic_cast<TimedInputSequence *>(CS)->addElements(dynamic_cast<TimedInputSequence *>(alpha)->content);
        alpha = verifyCheckingSequence(solver, CS, S, D);
        clock_t end = clock();
        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
    }
    while (alpha->getSize() != 0 && elapsed_secs < 3600);
    return CS;
}

Sequence * Algorithms_TFSM_TO::generateCheckingSequence(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM_TO * D = new DistinguishingAutomaton_TFSM_TO(S, M);
    D->initialize();
    Sequence * CS = new TimedInputSequence();
    Sequence * alpha = new TimedInputSequence();
    do {
        dynamic_cast<TimedInputSequence *>(CS)->addElements(dynamic_cast<TimedInputSequence *>(alpha)->content);
        alpha = verifyCheckingSequence(solver, CS, S, D);
    }
    while (alpha->getSize() != 0);
    return CS;
}

void Algorithms_TFSM_TO::checkingExperimentBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
{
    //    set<string> I = {"a", "b"};
    //    set<string> O = {"0", "1"};


    //    //int nbStates [5] = {4, 8, 10, 12, 15};
    //    //int nbMutants [6] = {16, 32, 64, 128, 256, 512};

    //    int nbStates [1] = {15};
    //    int nbMutants [1] = {96};
    //    int maxTimeSpec = 3;
    //    int maxTimeMuta = 5;
    //    ofstream benchFile;
    //    ->getTimeouts() << "Num of Bench : " << nbOfBench << endl;
    //    //for (int j=4; j<5; j++) {
    //    for (int j=0; j<1; j++) {
    //        ->getTimeouts() << nbStates[j] << " states" << endl;
    //        int maximumTransitions = nbStates[j] * I.size() * O.size() * nbStates[j] + nbStates[j] * (maxTimeMuta+1) * nbStates[j];
    //        int transitionsInSpec = nbStates[j] * I.size() + nbStates[j];
    //        ->getTimeouts() << "Maximum : " << maximumTransitions << " In Spec : " << transitionsInSpec << " available : " << maximumTransitions - transitionsInSpec << endl;
    //        //benchFile.open("bench_CS_" + to_string(nbStates[j]) + '_' + to_string(nbOfBench) + ".txt");
    //        //for (int i=0; i < 6; i++) {
    //        for (int i=0; i < 1; i++) {
    //            ->getTimeouts() << nbMutants[i] << " mutated transitions/timeouts" << endl;
    //            if (nbMutants[i] < maximumTransitions - transitionsInSpec) {
    //                benchFile.open("bench_CE_Less_" + to_string(nbStates[j]) + "_" + to_string(nbMutants[i]) + '_' + to_string(nbOfBench) + ".txt");
    //                TFSM_TO * randomSpec = generateRandomSpecification_TO(nbStates[j], maxTimeSpec, I, O);
    //                TFSM_TO * randomMuta = generateRandomMutationMachine_TO(randomSpec, maxTimeMuta, nbMutants[i]);
    //                vector<Sequence *> E;
    //                vector<Sequence *> Einit;
    //                ->getTimeouts() << "Begin" << endl;
    //                clock_t begin = clock();
    //                E = generateCheckingExperimentTimeouted(Einit, randomSpec, randomMuta);
    //                clock_t end = clock();
    //                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    //                ->getTimeouts() << "End" << endl;
    //                benchFile << nbStates[j] << " " << nbMutants[i] << " " << maxTimeSpec << " " << maxTimeMuta << " " << elapsed_secs << "s "<< computeNumberOfMutants(randomMuta) << " " << E.size() << "\n";
    //                delete randomSpec;
    //                delete randomMuta;
    //                benchFile.close();
    //            }
    //        }

    //    }
}


void Algorithms_TFSM_TO::checkingSequenceBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
{
    //    set<string> I = {"a", "b"};
    //    set<string> O = {"0", "1"};


    //    int nbStates [5] = {4, 8, 10, 12, 15};
    //    int nbMutants [6] = {16, 32, 64, 128, 300, 400};
    //    int maxTime = 5;

    //    for (int nbOfBench=0; nbOfBench < 5; nbOfBench++) {
    //        ofstream benchFile;

    //        for (int j=0; j<3; j++) {
    //            for (int i=0; i < 6; i++) {
    //                benchFile.open("bench_CS_" + to_string(nbStates[j]) + "_" + to_string(nbMutants[i]) + '_' + to_string(nbOfBench) + ".txt");
    //                TFSM_TO * randomSpec = generateRandomSpecification_TO(nbStates[j], maxTime, I, O);
    //                TFSM_TO * randomMuta = generateRandomMutationMachine_TO(randomSpec, maxTime*2, nbMutants[i]);
    //                Sequence * CS;
    //                clock_t begin = clock();
    //                CS = generateCheckingSequenceTimeouted(randomSpec, randomMuta);
    //                clock_t end = clock();
    //                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    //                benchFile << nbStates[j] << " " << nbMutants[i] << " " << maxTime << " " << elapsed_secs << "s "<< computeNumberOfMutants(randomMuta) << " " << CS.size() << "\n";
    //                delete randomSpec;
    //                delete randomMuta;
    //                benchFile.close();
    //            }
    //        }

    //    }

}

//Will be factorized later with a class
vector<Sequence *> Algorithms_TFSM_TO::removePrefixes(vector<Sequence *> E)
{
    vector<Sequence *> newE;
    for (Sequence * s1 : E) {
        bool found = false;

        vector<Sequence *> copyNewE(newE);
        int j = 0;
        for (Sequence * s2 : copyNewE) {
            int i=0;
            while (i < min(s1->getSize(), s2->getSize())
                   && dynamic_cast<TimedInputSequence *>(s1)->getElement(i).first == dynamic_cast<TimedInputSequence *>(s2)->getElement(i).first
                   && dynamic_cast<TimedInputSequence *>(s1)->getElement(i).second == dynamic_cast<TimedInputSequence *>(s2)->getElement(i).second) {
                i++;
            }
            if (i == min(s1->getSize(), s2->getSize())) {
                if (s1->getSize() <= s2->getSize()) {
                    found = true;
                }
                else {
                    newE.erase(newE.begin() + j);
                }
            }
            j++;
        }
        if (!found)
            newE.push_back(s1);
    }
    return newE;
}

FSM * Algorithms_TFSM_TO::completeMutation(FSM * M)
{

}

FSM * Algorithms_TFSM_TO::generateRandomSpecification(int nbOfStates, int maxTime, set<string> I, set<string> O)
{
    set<int> S;
    int s0 = 0;
    vector<IOTransition *> lambda;
    vector<TimeoutTransition *> delta;
    int transitionId;
    srand (time(NULL));
    bool isConnected = false;

    TFSM_TO * res;

    while (!isConnected) {
        transitionId = 0;
        S.clear();
        lambda.clear();
        delta.clear();
        for (int s=0; s<nbOfStates; s++) {
            S.insert(s);
            for (string i : I) {
                string randomO = getRandomStringFromSet(O);
                int randomTgt = floor(rand() % nbOfStates);

                lambda.push_back(new IOTransition(s, i, randomO, randomTgt, transitionId));
                transitionId++;
            }

            int randomT = floor(rand() % maxTime);
            int randomTgt = floor(rand() % nbOfStates);
            if (randomT == 0) {
                randomT = inf;
                randomTgt = s;
            }
            delta.push_back(new TimeoutTransition(s, randomT, randomTgt, transitionId));
            transitionId++;
        }
        res = new TFSM_TO(S, s0, I, O, lambda, delta);
        /*
        isConnected = isTFSMConnected(res);
        if (!isConnected)
            delete res;
            */
    }
    return res;
}

FSM * Algorithms_TFSM_TO::generateRandomMutation(FSM * S, int maxTime, int numberOfMutations)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<IOTransition *> lambda(S->transitions);
    vector<TimeoutTransition *> delta(S->getTimeouts());
    TFSM_TO * M = new TFSM_TO(States, s0, I, O, lambda, delta);

    srand (time(NULL));
    vector<IOTransition *> newLambda;
    vector<TimeoutTransition *> newDelta;
    for (int i=0; i<numberOfMutations; i++) {
        bool alreadyExisting = true;
        while (alreadyExisting) {
            bool createTimeout = rand()%2;
            int randomSrc = floor(rand() % S->states.size());
            int randomTgt = floor(rand() % S->states.size());
            int newId = i + (S->transitions.size() + S->getTimeouts().size());
            if (createTimeout) {
                int randomT = floor(rand() % (maxTime+1));
                if (randomT == 0) {
                    randomT = inf;
                    randomTgt = randomSrc;
                }
                TimeoutTransition  * newTimeout = new TimeoutTransition(randomSrc,randomT, randomTgt, newId);
                if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                    newDelta.push_back(newTimeout);
                    alreadyExisting = false;
                }
            }
            else {
                string randomI = getRandomStringFromSet(I);
                string randomO = getRandomStringFromSet(O);
                IOTransition * newTransition = new IOTransition(randomSrc,randomI, randomO, randomTgt, newId);
                if (!transitionAlreadyExist(newTransition, lambda, newLambda)) {
                    newLambda.push_back(newTransition);
                    alreadyExisting = false;
                }
            }
        }
    }
    M->addTransitions(newLambda, true);
    M->addTimeouts(newDelta, true);
    return M;
}

InfInt Algorithms_TFSM_TO::computeNumberOfMutants(FSM * M) {
    InfInt res = 1;
    for (int s : M->states) {
        for (string i : M->inputs) {
            if (M->getXi(s, i).size() > 1)
                res *= M->getXi(s, i).size();
        }
    }
    for (int s : M->states) {
        if (M->getXi(s).size() > 1)
            res *= M->getXi(s).size();
    }
    return res;
}
