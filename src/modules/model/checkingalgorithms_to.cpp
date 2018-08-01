#include "checkingalgorithms_to.h"
using namespace std;
using namespace CMSat;

TFSM_TO * generateSubmachine(SATSolver * &solver, TFSM_TO * M)
{
    lbool ret = solver->solve();
    if (ret == l_True) {
        set<int> S(M->states);
        int s0 = M->initialState;
        set<string> I(M->inputs);
        set<string> O(M->inputs);
        vector<IOTransition> lambda;
        vector<TimeoutTransition> delta;
        for (int i=0; i<M->transitions.size() + M->timeouts.size(); i++) {
            if (solver->get_model()[i] == l_True) {
                int id = i;
                if (M->isIdTimeout(id)) {
                    TimeoutTransition corr = M->getTimeoutFromId(id);
                    delta.push_back(TimeoutTransition(corr.src, corr.t, corr.tgt, corr.id));
                }
                else {
                    IOTransition corr = M->getTransitionFromId(id);
                    lambda.push_back(IOTransition(corr.src, corr.i, corr.o, corr.tgt, corr.id));
                }
            }
        }

        return new TFSM_TO(S, s0, I, O, lambda, delta);
    }
    else {
        return NULL;
    }
}

void computePhiP(SATSolver * &solver, TFSM_TO * P)
{
    vector<Lit> clause;
    for (auto transition : P->transitions) {
        clause.push_back(Lit(transition.id, true));
    }
    for (auto timeout : P->timeouts) {
        clause.push_back(Lit(timeout.id, true));
    }
    solver->add_clause(clause);
}

void computePhiE(SATSolver * &solver, vector<sequence> E, Product_TFSM_TO * D)
{
    for (auto alpha : E) {
        vector<path> rev = D->revealingPaths(alpha);
        for (auto path : rev) {
            vector<Lit> clause;
            for (auto id : path) {
                if (D->mutationMachine->isIdTimeout(id)) {
                    TimeoutTransition corresponding = D->mutationMachine->getTimeoutFromId(id);
                    vector<TimeoutTransition> correspondingSuspicious = D->mutationMachine->getXi(corresponding.src);
                    if (correspondingSuspicious.size() > 1) {
                        clause.push_back(Lit(id, true));
                    }
                }
                else {
                    IOTransition corresponding = D->mutationMachine->getTransitionFromId(id);
                    vector<IOTransition> correspondingSuspicious = D->mutationMachine->getXi(corresponding.src, corresponding.i);
                    if (correspondingSuspicious.size() > 1) {
                        clause.push_back(Lit(id, true));
                    }
                }
            }
            solver->add_clause(clause);
        }
    }
}

void computePhiM(SATSolver * &solver, TFSM_TO * S, TFSM_TO * M)
{
    for (auto s : M->states) {
        for (auto i : M->inputs) {
            vector<IOTransition> res = M->getXi(s, i);
            for (int k=0; k<res.size(); k++) {
                for (int l=k+1; l<res.size(); l++) {
                    vector<Lit> clause;
                    clause.push_back(Lit(res[k].id, true));
                    clause.push_back(Lit(res[l].id, true));
                    solver->add_clause(clause);
                }
            }
            vector<Lit> clause;
            for (int k=0; k<res.size(); k++) {
                clause.push_back(Lit(res[k].id, false));
            }
            solver->add_clause(clause);
        }
    }
    for (auto s : M->states) {
        vector<TimeoutTransition> res = M->getXi(s);
        for (int k=0; k<res.size(); k++) {
            for (int l=k+1; l<res.size(); l++) {
                vector<Lit> clause;
                clause.push_back(Lit(res[k].id, true));
                clause.push_back(Lit(res[l].id, true));
                solver->add_clause(clause);
            }
        }
        vector<Lit> clause;
        for (int k=0; k<res.size(); k++) {
            clause.push_back(Lit(res[k].id, false));
        }
        solver->add_clause(clause);
    }
    vector<Lit> clause;
    for (auto k : S->transitions) {
        clause.push_back(Lit(k.id, true));
    }
    for (auto k : S->timeouts) {
        clause.push_back(Lit(k.id, true));
    }
    solver->add_clause(clause);
}

sequence verifyCheckingExperiment(SATSolver * &solver,vector<sequence> E, TFSM_TO * S, Product_TFSM_TO * D)
{
    computePhiE(solver, E, D);
    sequence alpha;
    TFSM_TO * P = D->mutationMachine;
    while (alpha.size() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            Product_TFSM_TO * DP = new Product_TFSM_TO(S, P);
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

vector<sequence> generateCheckingExperimentTimeouted(vector<sequence> Einit, TFSM_TO * S, TFSM_TO * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    computePhiM(solver, S, M);
    Product_TFSM_TO * D = new Product_TFSM_TO(S, M);
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

vector<sequence> generateCheckingExperiment(vector<sequence> Einit, TFSM_TO * S, TFSM_TO * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    Product_TFSM_TO * D = new Product_TFSM_TO(S, M);
    vector<sequence> E;
    vector<sequence> Ecurr = Einit;
    //double elapsed_secs = 0;
    sequence alpha;
    do {
        //clock_t begin = clock();
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        //printSequence(alpha);
        Ecurr.clear();
        Ecurr.push_back(alpha);
        //clock_t end = clock();
        //elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
        //cout << elapsed_secs << endl;
    }
    while (alpha.size() != 0);
    return E;
}

sequence verifyCheckingSequence(SATSolver * &solver,sequence CS, TFSM_TO * S, Product_TFSM_TO * D)
{
    vector<sequence> E;
    E.push_back(CS);
    set<string> beginningStates;
    computePhiE(solver, E, D);
    sequence alpha;
    TFSM_TO * P = D->mutationMachine;
    while (alpha.size() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            Product_TFSM_TO * DP = new Product_TFSM_TO(S, P);
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

sequence generateCheckingSequenceTimeouted(TFSM_TO * S, TFSM_TO * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    Product_TFSM_TO * D = new Product_TFSM_TO(S, M);
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

sequence generateCheckingSequence(TFSM_TO * S, TFSM_TO * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    Product_TFSM_TO * D = new Product_TFSM_TO(S, M);
    sequence CS;
    sequence alpha;
    do {
        CS.insert(CS.end(), alpha.begin(), alpha.end());
        alpha = verifyCheckingSequence(solver, CS, S, D);
    }
    while (alpha.size() != 0);
    return CS;
}
