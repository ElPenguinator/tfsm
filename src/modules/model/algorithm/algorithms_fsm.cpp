#include "algorithms_fsm.h"
using namespace std;
using namespace CMSat;
#include "../algorithm/inputsequence.h"

Algorithms_FSM::Algorithms_FSM(bool generateLogs, bool onlyDot) : Algorithms(generateLogs, onlyDot)
{

}

FSM * Algorithms_FSM::generateSubmachine(SATSolver * &solver, FSM * M)
{
    lbool ret = solver->solve();
    if (ret == l_True) {
        set<int> S(M->states);
        int s0 = M->initialState;
        set<string> I(M->inputs);
        set<string> O(M->inputs);
        vector<IOTransition *> lambda;
        for (int i=0; i<M->transitions.size(); i++) {
            if (solver->get_model()[i] == l_True) {
                int id = i;
                IOTransition * corr = M->getTransitionFromId(id);
                lambda.push_back(new IOTransition(corr->src, corr->i, corr->o, corr->tgt, corr->id));
            }
        }

        return new FSM(S, s0, I, O, lambda);
    }
    else {
        return NULL;
    }
}

void Algorithms_FSM::computePhiP(SATSolver * &solver, FSM * P)
{
    vector<Lit> clause;
    for (auto transition : P->transitions) {
        clause.push_back(Lit(transition->id, true));
    }
    solver->add_clause(clause);
}

void Algorithms_FSM::computePhiE(SATSolver * &solver, std::vector<Sequence *> E, DistinguishingAutomaton_FSM * D)
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
                IOTransition * corresponding = D->mutationMachine->getTransitionFromId(id);
                vector<IOTransition *> correspondingSuspicious = D->mutationMachine->getXi(corresponding->src, corresponding->i);
                if (correspondingSuspicious.size() > 1) {
                    clause.push_back(Lit(id, true));
                }
            }
            solver->add_clause(clause);
        }
        cpt++;
    }
}

void Algorithms_FSM::computePhiM(SATSolver * &solver, FSM * S, FSM * M)
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
    vector<Lit> clause;
    for (auto k : S->transitions) {
        clause.push_back(Lit(k->id, true));
    }
    solver->add_clause(clause);
}

Sequence * Algorithms_FSM::verifyCheckingExperiment(SATSolver * &solver,vector<Sequence *> E, FSM * S, DistinguishingAutomaton_FSM * D)
{
    computePhiE(solver, E, D);
    Sequence * alpha = new InputSequence();
    FSM * P = D->mutationMachine;
    while (alpha->getSize() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            nbPassedMutants++;
            DistinguishingAutomaton_FSM * DP = new DistinguishingAutomaton_FSM(S, P);
            DP->initialize();
            if (generateLogs) {
                saveSVG(logPath + "mutant" + to_string(nbPassedMutants) +".dot", logPath + "mutant" + to_string(nbPassedMutants) +".svg", P->generateDot());
                saveSVG(logPath + "productMutant" + to_string(nbPassedMutants) + ".dot", logPath + "productMutant" + to_string(nbPassedMutants) + ".svg", DP->generateDot());
            }
            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                Sequence * nullPrefix = new InputSequence();
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

vector<Sequence *> Algorithms_FSM::generateCheckingExperimentTimeouted(vector<Sequence *> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->transitions.size());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_FSM * D = new DistinguishingAutomaton_FSM(S, M);
    D->initialize();
    vector<Sequence *> E;
    vector<Sequence *> Ecurr = Einit;
    nbPassedMutants = 0;
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

vector<Sequence *> Algorithms_FSM::generateCheckingExperiment(vector<Sequence *> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->transitions.size());
    solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);

    DistinguishingAutomaton_FSM * D = new DistinguishingAutomaton_FSM(S, M);
    D->initialize();
    if (generateLogs) {
        saveSVG(logPath + "specification.dot", logPath + "specification.svg", S->generateDot());
        saveSVG(logPath + "mutation.dot", logPath + "mutation.svg", M->generateDot());
        saveSVG(logPath + "distinguishing.dot", logPath + "distinguishing.svg", D->generateDot());
    }
    vector<Sequence *> E;
    vector<Sequence *> Ecurr = Einit;
    Sequence * alpha = new InputSequence();
    do {
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        Ecurr.push_back(alpha);
    }
    while (alpha->getSize() != 0);
    return removePrefixes(E);
}

Sequence * Algorithms_FSM::verifyCheckingSequence(SATSolver * &solver,Sequence * CS, FSM * S, DistinguishingAutomaton_FSM * D)
{
    vector<Sequence *> E;
    E.push_back(CS);
    set<string> beginningStates;
    computePhiE(solver, E, D);
    Sequence * alpha = new InputSequence();
    FSM * P = D->mutationMachine;
    while (alpha->getSize() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            DistinguishingAutomaton_FSM * DP = new DistinguishingAutomaton_FSM(S, P);
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

Sequence * Algorithms_FSM::generateCheckingSequenceTimeouted(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->transitions.size());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    DistinguishingAutomaton_FSM * D = new DistinguishingAutomaton_FSM(S, M);
    D->initialize();
    Sequence * CS;
    Sequence * alpha = new InputSequence();
    double elapsed_secs = 0;
    do {
        clock_t begin = clock();
        //CS.insert(CS.end(), alpha.begin(), alpha.end());
        dynamic_cast<InputSequence *>(CS)->addElements(dynamic_cast<InputSequence *>(alpha)->content);
        alpha = verifyCheckingSequence(solver, CS, S, D);
        clock_t end = clock();
        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
    }
    while (alpha->getSize() != 0 && elapsed_secs < 3600);
    return CS;
}

Sequence * Algorithms_FSM::generateCheckingSequence(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->transitions.size());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    DistinguishingAutomaton_FSM * D = new DistinguishingAutomaton_FSM(S, M);
    D->initialize();
    Sequence * CS = new InputSequence();
    Sequence * alpha = new InputSequence();
    do {
        //CS.insert(CS.end(), alpha.begin(), alpha.end());
        dynamic_cast<InputSequence *>(CS)->addElements(dynamic_cast<InputSequence *>(alpha)->content);
        alpha = verifyCheckingSequence(solver, CS, S, D);
    }
    while (alpha->getSize() != 0);
    return CS;
}

void Algorithms_FSM::checkingExperimentBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
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
    //    cout << "Num of Bench : " << nbOfBench << endl;
    //    //for (int j=4; j<5; j++) {
    //    for (int j=0; j<1; j++) {
    //        cout << nbStates[j] << " states" << endl;
    //        int maximumTransitions = nbStates[j] * I.size() * O.size() * nbStates[j] + nbStates[j] * (maxTimeMuta+1) * nbStates[j];
    //        int transitionsInSpec = nbStates[j] * I.size() + nbStates[j];
    //        cout << "Maximum : " << maximumTransitions << " In Spec : " << transitionsInSpec << " available : " << maximumTransitions - transitionsInSpec << endl;
    //        //benchFile.open("bench_CS_" + to_string(nbStates[j]) + '_' + to_string(nbOfBench) + ".txt");
    //        //for (int i=0; i < 6; i++) {
    //        for (int i=0; i < 1; i++) {
    //            cout << nbMutants[i] << " mutated transitions/timeouts" << endl;
    //            if (nbMutants[i] < maximumTransitions - transitionsInSpec) {
    //                benchFile.open("bench_CE_Less_" + to_string(nbStates[j]) + "_" + to_string(nbMutants[i]) + '_' + to_string(nbOfBench) + ".txt");
    //                TFSM_TO * randomSpec = generateRandomSpecification_TO(nbStates[j], maxTimeSpec, I, O);
    //                TFSM_TO * randomMuta = generateRandomMutationMachine_TO(randomSpec, maxTimeMuta, nbMutants[i]);
    //                vector<Sequence *> E;
    //                vector<Sequence *> Einit;
    //                cout << "Begin" << endl;
    //                clock_t begin = clock();
    //                E = generateCheckingExperimentTimeouted(Einit, randomSpec, randomMuta);
    //                clock_t end = clock();
    //                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    //                cout << "End" << endl;
    //                benchFile << nbStates[j] << " " << nbMutants[i] << " " << maxTimeSpec << " " << maxTimeMuta << " " << elapsed_secs << "s "<< computeNumberOfMutants(randomMuta) << " " << E.size() << "\n";
    //                delete randomSpec;
    //                delete randomMuta;
    //                benchFile.close();
    //            }
    //        }

    //    }

    cout << folder << " | " << nbMachines << " | " << timeoutedValue << " | " << maxTimeout << " | " << " | " << endl;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};

    ofstream benchFile;
    for (int states : nbStates) {
            cout << "States : " << states << endl;
        for (int i = 0; i < nbMachines; i++) {
            cout << "Machines : " << i << endl;
            FSM * randomSpec = generateRandomSpecification(states, maxTimeout, I, O);
            int transitionsInSpec = states * I.size();
            for (int mutations : nbMutations) {
                cout << "Mutations : " << mutations << endl;
                int maximumTransitions = states * I.size() * O.size() * states;
                if (mutations < maximumTransitions - transitionsInSpec) {
                    randomSpec->print();
                    FSM * randomMuta = generateRandomMutation(randomSpec, maxTimeout * 2, mutations);
                    string pathName = folder + "/bench_CE_" + to_string(states) + "_" + to_string(mutations) + '_' + to_string(i);
                    benchFile.open(pathName + "_results.txt");

                    vector<Sequence *> E;
                    vector<Sequence *> Einit;
                    cout << "Begin" << endl;
                    clock_t begin = clock();
                    this->logPath = pathName + "_";
                    E = generateCheckingExperimentTimeouted(Einit, randomSpec, randomMuta);
                    clock_t end = clock();
                    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
                    cout << "End" << endl;
                    benchFile << states << " " << mutations << " " << maxTimeout << " " << maxTimeout*2 << " " << elapsed_secs << "s "<< computeNumberOfMutants(randomMuta) << " " << E.size() << "\n";
                    delete randomMuta;
                    benchFile.close();
                }
            }
            delete randomSpec;
        }
    }
}


void Algorithms_FSM::checkingSequenceBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
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
vector<Sequence *> Algorithms_FSM::removePrefixes(vector<Sequence *> E)
{
    vector<Sequence *> newE;
    for (Sequence * s1 : E) {
        bool found = false;

        vector<Sequence *> copyNewE(newE);
        int j = 0;
        for (Sequence * s2 : copyNewE) {
            int i=0;
            while (i < min(s1->getSize(), s2->getSize())
                   && dynamic_cast<InputSequence *>(s1)->getElement(i) == dynamic_cast<InputSequence *>(s2)->getElement(i)) {
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

FSM * Algorithms_FSM::completeMutation(FSM * M)
{

}

FSM * Algorithms_FSM::generateRandomSpecification(int nbOfStates, int maxTime, set<string> I, set<string> O)
{
    set<int> S;
    int s0 = 0;
    vector<IOTransition *> lambda;
    int transitionId;
    srand (time(NULL));
    bool isConnected = false;

    FSM * res;

    //while (!isConnected) {
        transitionId = 0;
        S.clear();
        lambda.clear();
        for (int s=0; s<nbOfStates; s++) {
            S.insert(s);
            for (string i : I) {
                string randomO = getRandomStringFromSet(O);
                int randomTgt = floor(rand() % nbOfStates);

                lambda.push_back(new IOTransition(s, i, randomO, randomTgt, transitionId));
                transitionId++;
            }
        }
        res = new FSM(S, s0, I, O, lambda);
        /*
         * isConnected = isTFSMConnected(res);
        if (!isConnected)
            delete res;
        */
    //}
    return res;
}

FSM * Algorithms_FSM::generateRandomMutation(FSM * S, int maxTime, int numberOfMutations)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<IOTransition *> lambda(S->transitions);
    FSM * M = new FSM(States, s0, I, O, lambda);

    srand (time(NULL));
    vector<IOTransition *> newLambda;
    for (int i=0; i<numberOfMutations; i++) {
        bool alreadyExisting = true;
        while (alreadyExisting) {
            int randomSrc = floor(rand() % S->states.size());
            int randomTgt = floor(rand() % S->states.size());
            int newId = i + (S->transitions.size() + S->getTimeouts().size());
            string randomI = getRandomStringFromSet(I);
            string randomO = getRandomStringFromSet(O);
            IOTransition * newTransition = new IOTransition(randomSrc,randomI, randomO, randomTgt, newId);
            if (!transitionAlreadyExist(newTransition, lambda, newLambda)) {
                newLambda.push_back(newTransition);
                alreadyExisting = false;
            }
        }
    }
    M->addTransitions(newLambda, true);
    return M;
}

InfInt Algorithms_FSM::computeNumberOfMutants(FSM * M) {
    InfInt res = 1;
    for (int s : M->states) {
        for (string i : M->inputs) {
            if (M->getXi(s, i).size() > 1)
                res *= M->getXi(s, i).size();
        }
    }
    return res;
}
