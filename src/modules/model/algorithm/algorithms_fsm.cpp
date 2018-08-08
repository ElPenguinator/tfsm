#include "algorithms_fsm.h"
using namespace std;
using namespace CMSat;

Algorithms_FSM::Algorithms_FSM()
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

void Algorithms_FSM::computePhiE(SATSolver * &solver, vector<sequence> E, DistinguishingAutomaton_FSM * D)
{
    for (auto alpha : E) {
        vector<executingPath> rev = D->revealingPaths(alpha);
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

sequence Algorithms_FSM::verifyCheckingExperiment(SATSolver * &solver,vector<sequence> E, FSM * S, DistinguishingAutomaton_FSM * D)
{
    computePhiE(solver, E, D);
    sequence alpha;
    FSM * P = D->mutationMachine;
    while (alpha.size() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            DistinguishingAutomaton_FSM * DP = new DistinguishingAutomaton_FSM(S, P);
            DP->initialize();
            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                sequence nullPrefix;
                set<string> beginningStates;
                beginningStates.insert(DP->initialState->getKey());
                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, nullPrefix);
            }
            delete DP;
        }
    }
    return alpha;
}

vector<sequence> Algorithms_FSM::generateCheckingExperimentTimeouted(vector<sequence> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->transitions.size());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_FSM * D = new DistinguishingAutomaton_FSM(S, M);
    D->initialize();
    vector<sequence> E;
    vector<sequence> Ecurr = Einit;

    double elapsed_secs = 0;
    do {
        clock_t begin = clock();
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        sequence alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        if (alpha.size() > 0)
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

vector<sequence> Algorithms_FSM::generateCheckingExperiment(vector<sequence> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->transitions.size());
    solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    DistinguishingAutomaton_FSM * D = new DistinguishingAutomaton_FSM(S, M);
    D->initialize();
    vector<sequence> E;
    vector<sequence> Ecurr = Einit;
    sequence alpha;
    do {
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        Ecurr.push_back(alpha);
    }
    while (alpha.size() != 0);
    return removePrefixes(E);
}

sequence Algorithms_FSM::verifyCheckingSequence(SATSolver * &solver,sequence CS, FSM * S, DistinguishingAutomaton_FSM * D)
{
    vector<sequence> E;
    E.push_back(CS);
    set<string> beginningStates;
    computePhiE(solver, E, D);
    sequence alpha;
    FSM * P = D->mutationMachine;
    while (alpha.size() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            DistinguishingAutomaton_FSM * DP = new DistinguishingAutomaton_FSM(S, P);
            DP->initialize();
            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, CS);
                if (alpha.size() == 0) {
                    computePhiP(solver, P);
                }
            }
            delete DP;
        }
    }
    return alpha;
}

sequence Algorithms_FSM::generateCheckingSequenceTimeouted(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->transitions.size());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    DistinguishingAutomaton_FSM * D = new DistinguishingAutomaton_FSM(S, M);
    D->initialize();
    sequence CS;
    sequence alpha;
    double elapsed_secs = 0;
    do {
        clock_t begin = clock();
        CS.insert(CS.end(), alpha.begin(), alpha.end());
        alpha = verifyCheckingSequence(solver, CS, S, D);
        clock_t end = clock();
        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
    }
    while (alpha.size() != 0 && elapsed_secs < 3600);
    return CS;
}

sequence Algorithms_FSM::generateCheckingSequence(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->transitions.size());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    DistinguishingAutomaton_FSM * D = new DistinguishingAutomaton_FSM(S, M);
    D->initialize();
    sequence CS;
    sequence alpha;
    do {
        CS.insert(CS.end(), alpha.begin(), alpha.end());
        alpha = verifyCheckingSequence(solver, CS, S, D);
    }
    while (alpha.size() != 0);
    return CS;
}

void Algorithms_FSM::checkingExperimentBenchmarks()
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
    //                vector<sequence> E;
    //                vector<sequence> Einit;
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
}


void Algorithms_FSM::checkingSequenceBenchmarks()
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
    //                sequence CS;
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
vector<sequence> Algorithms_FSM::removePrefixes(vector<sequence> E)
{
    vector<sequence> newE;
    for (sequence s1 : E) {
        bool found = false;
        cout << "S1 : ";
        printSequence(s1);

        vector<sequence> copyNewE(newE);
        int j = 0;
        for (sequence s2 : copyNewE) {
            cout << "S2 : ";
            printSequence(s2);
            int i=0;
            while (i < min(s1.size(), s2.size()) && s1[i].first == s2[i].first && s1[i].second == s2[i].second) {
                i++;
            }
            if (i == min(s1.size(), s2.size())) {
                if (s1.size() <= s2.size()) {
                    printSequence(s1);
                    cout << "Is prefix of" << endl;
                    printSequence(s2);
                    found = true;
                }
                else {
                    printSequence(s2);
                    cout << "Is prefix of" << endl;
                    printSequence(s1);
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
