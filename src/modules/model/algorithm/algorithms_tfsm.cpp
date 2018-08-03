#include "algorithms_tfsm.h"
#include "../machine/distinguishingautomaton_tfsm.h"
using namespace std;
using namespace CMSat;

Algorithms_TFSM::Algorithms_TFSM()
{

}

FSM * Algorithms_TFSM::generateSubmachine(SATSolver * &solver, FSM * M)
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
                    lambda.push_back(new GuardedTransition(corr->src, corr->i, corr->getGuard(), corr->o, corr->tgt, corr->id));
                }
            }
        }

        return new TFSM(S, s0, I, O, lambda, delta);
    }
    else {
        return NULL;
    }
}

void Algorithms_TFSM::computePhiP(SATSolver * &solver, FSM * P)
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

void Algorithms_TFSM::computePhiE(SATSolver * &solver, vector<sequence> E, DistinguishingAutomaton_FSM * D)
{
    for (auto alpha : E) {
        vector<path> rev = D->revealingPaths(alpha);
        cout << "Paths : " << endl;
        for (auto path : rev) {
            printPath(path);
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
    }
}

void computePhiMRecur(SATSolver * &solver, const vector<IOTransition *> &xi, const set<set<int>>::iterator &current, const set<set<int>>::iterator &end, vector<int> currentClause) {
    set<int> combi = (*current);
    for (int elt : combi) {
        set<set<int>>::iterator copyCurrent(current);
        copyCurrent++;
        vector<int> copyCurrentClause(currentClause);
        copyCurrentClause.push_back(elt);
        if (copyCurrent != end) {
            computePhiMRecur(solver, xi, copyCurrent, end, copyCurrentClause);
        }
        else {
            vector<Lit> clause;
            for (int li : copyCurrentClause) {;
                if (li >= 0) {
                    clause.push_back(Lit(li, false));
                }
                else {
                    clause.push_back(Lit(-li, true));
                }
            }
            solver->add_clause(clause);
        }
    }
}

void Algorithms_TFSM::computePhiM(SATSolver * &solver, FSM * S, FSM * M)
{

    for (auto s : M->states) {
        for (auto i : M->inputs) {
            vector<IOTransition *> xi = M->getXi(s, i);
            set<set<int>> eta = M->getEta(s, i);
            vector<int> emptyClause;
            set<set<int>> newEta;

            for (set<int> combi : eta) {
                set<int> newSet;
                for (int elt : combi) {
                    newSet.insert(elt);
                }
                for (IOTransition * elt : xi) {
                    if (combi.find(elt->id) == combi.end()) {
                        newSet.insert(-elt->id);
                    }
                }
                newEta.insert(newSet);
            }

            set<set<int>>::iterator begin = newEta.begin();
            set<set<int>>::iterator end = newEta.end();

            computePhiMRecur(solver, xi, begin, end, emptyClause);
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

sequence Algorithms_TFSM::verifyCheckingExperiment(SATSolver * &solver,vector<sequence> E, FSM * S, DistinguishingAutomaton_FSM * D)
{

    computePhiE(solver, E, D);
    sequence alpha;
    FSM * P = D->mutationMachine;
    while (alpha.size() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            DistinguishingAutomaton_TFSM * DP = new DistinguishingAutomaton_TFSM(S, P);
            DP->initialize();

            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                sequence nullPrefix;
                set<string> beginningStates;
                beginningStates.insert(DP->initialState->getKey());
                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, nullPrefix);
                //No deterministic input sequence
                if (alpha.size() == 0) {
                    computePhiP(solver, P);
                }
            }
            delete DP;
        }
    }
    return alpha;
}

vector<sequence> Algorithms_TFSM::generateCheckingExperimentTimeouted(vector<sequence> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM * D = new DistinguishingAutomaton_TFSM(S, M);
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
    return E;
}

vector<sequence> Algorithms_TFSM::generateCheckingExperiment(vector<sequence> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();
solver->log_to_file("/tmp/test.txt");
    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM * D = new DistinguishingAutomaton_TFSM(S, M);
    D->initialize();
    vector<sequence> E;
    vector<sequence> Ecurr = Einit;

    do {
        cout << "?? " <<  Ecurr.size() << endl;
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        sequence alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        cout << "Alpha : " << endl;
        printSequence(alpha);
        if (alpha.size() > 0)
            Ecurr.push_back(alpha);
    }
    while (Ecurr.size() != 0);
    return E;
}

sequence Algorithms_TFSM::verifyCheckingSequence(SATSolver * &solver,sequence CS, FSM * S, DistinguishingAutomaton_FSM * D)
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
            DistinguishingAutomaton_TFSM * DP = new DistinguishingAutomaton_TFSM(S, P);
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

sequence Algorithms_TFSM::generateCheckingSequenceTimeouted(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM * D = new DistinguishingAutomaton_TFSM(S, M);
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

sequence Algorithms_TFSM::generateCheckingSequence(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM * D = new DistinguishingAutomaton_TFSM(S, M);
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

void Algorithms_TFSM::checkingExperimentBenchmarks()
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
//                vector<sequence> E;
//                vector<sequence> Einit;
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


void Algorithms_TFSM::checkingSequenceBenchmarks()
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

