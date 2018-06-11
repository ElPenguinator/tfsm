#include "checkingalgorithms.h"
using namespace std;
using namespace CMSat;

TFSM * generateSubmachine(SATSolver * &solver, TFSM * M)
{
    lbool ret = solver->solve();
    if (ret == l_True) {
        set<int> S(M->states);
        int s0 = M->initialState;
        set<string> I(M->inputs);
        set<string> O(M->inputs);
        vector<GuardedTransition> lambda;
        vector<Timeout> delta;
        for (int i=0; i<M->transitions.size() + M->timeouts.size(); i++) {
            if (solver->get_model()[i] == l_True) {
                int id = i;
                if (M->isIdTimeout(id)) {
                    Timeout corr = M->getTimeoutFromId(id);
                    delta.push_back(Timeout(corr.src, corr.t, corr.tgt, corr.id));
                }
                else {
                    GuardedTransition corr = M->getTransitionFromId(id);
                    lambda.push_back(GuardedTransition(corr.src, corr.i, corr.g, corr.o, corr.tgt, corr.id));
                }
            }
        }

        return new TFSM(S, s0, I, O, lambda, delta);
    }
    else {
        return NULL;
    }
}

void computePhiP(SATSolver * &solver, TFSM * P)
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

void computePhiE(SATSolver * &solver, vector<sequence> E, Product_TFSM * D)
{
    for (auto alpha : E) {
        cout << "Begin computing Paths" << endl;
        vector<path> rev = D->revealingPaths(alpha);
        cout << "Alpha To Eliminate : " << endl;
        printSequence(alpha);
        cout << "Paths : " << endl;
        for (auto path : rev) {
            printPath(path);
            vector<Lit> clause;
            for (auto id : path) {
                if (D->mutationMachine->isIdTimeout(id)) {
                    Timeout corresponding = D->mutationMachine->getTimeoutFromId(id);
                    vector<Timeout> correspondingSuspicious = D->mutationMachine->getXi(corresponding.src);
                    if (correspondingSuspicious.size() > 1) {
                        clause.push_back(Lit(id, true));
                    }
                }
                else {
                    GuardedTransition corresponding = D->mutationMachine->getTransitionFromId(id);
                    vector<GuardedTransition> correspondingSuspicious = D->mutationMachine->getXi(corresponding.src, corresponding.i);
                    cout << "CSS : " << correspondingSuspicious.size() << endl;
                    if (correspondingSuspicious.size() > 1) {
                        clause.push_back(Lit(id, true));
                    }
                }
            }
            solver->add_clause(clause);
        }
    }
}

void computePhiM(SATSolver * &solver, TFSM * S, TFSM * M)
{
    for (auto s : M->states) {
        for (auto i : M->inputs) {
            vector<GuardedTransition> xi = M->getXi(s, i);
            set<set<int>> eta = M->getEta(s, i);
            for (set<int> combi : eta) {
                for (int i : combi) {
                    if (eta.size() == 1) {
                        vector<Lit> clause;
                        clause.push_back(Lit(i, false));
                        solver->add_clause(clause);
                    }
                    else {
                        for (set<int> otherCombi : eta) {
                            if (combi != otherCombi) {
                                for (int j : otherCombi) {
                                    vector<Lit> clause;
                                    clause.push_back(Lit(i, false));
                                    clause.push_back(Lit(j, false));
                                    solver->add_clause(clause);
                                }
                                for (GuardedTransition v : xi) {
                                    if (otherCombi.find(v.id) == otherCombi.end()) {
                                        vector<Lit> clause;
                                        clause.push_back(Lit(i, false));
                                        clause.push_back(Lit(v.id, true));
                                        solver->add_clause(clause);
                                    }
                                }

                            }
                        }
                    }
                }
                for (GuardedTransition t : xi) {
                    if (combi.find(t.id) == combi.end()) {
                        if (eta.size() == 1) {
                            vector<Lit> clause;
                            clause.push_back(Lit(t.id, false));
                            solver->add_clause(clause);
                        }
                        else {
                            for (set<int> otherCombi : eta) {
                                if (combi != otherCombi) {

                                    for (int j : otherCombi) {
                                        vector<Lit> clause;
                                        clause.push_back(Lit(t.id, true));
                                        clause.push_back(Lit(j, false));
                                        solver->add_clause(clause);
                                    }
                                    for (GuardedTransition v : xi) {
                                        if (otherCombi.find(v.id) == otherCombi.end()) {
                                            vector<Lit> clause;
                                            clause.push_back(Lit(t.id, true));
                                            clause.push_back(Lit(v.id, true));
                                            solver->add_clause(clause);
                                        }
                                    }

                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (auto s : M->states) {
        vector<Timeout> res = M->getXi(s);
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


sequence verifyCheckingExperiment(SATSolver * &solver,vector<sequence> E, TFSM * S, Product_TFSM * D)
{
    computePhiE(solver, E, D);
    sequence alpha;
    TFSM * P = D->mutationMachine;
    while (alpha.size() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            Product_TFSM * DP = new Product_TFSM(S, P);
            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                sequence nullPrefix;
                set<string> beginningStates;
                beginningStates.insert(DP->initialState->getKey());
                cout << "P : " << endl;
                P->print();
                cout << "DP : " << endl;
                DP->print();
                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, nullPrefix);
                //No deterministic input sequence
                if (alpha.size() == 0) {
                    cout << "Exclude this one" << endl;
                    computePhiP(solver, P);
                }
            }
            delete DP;
        }
    }
    return alpha;
}

/*
vector<sequence> generateCheckingExperimentTimeouted(vector<sequence> Einit, TFSM * S, TFSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    Product_TFSM * D = new Product_TFSM(S, M);
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
        cout << elapsed_secs << endl;
    }
    while (Ecurr.size() != 0 && elapsed_secs < 400);
    if (elapsed_secs > 400) {
        E.clear();
    }
    return E;
}
*/
vector<sequence> generateCheckingExperiment(vector<sequence> Einit, TFSM * S, TFSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    Product_TFSM * D = new Product_TFSM(S, M);
    vector<sequence> E;
    vector<sequence> Ecurr = Einit;
    double elapsed_secs = 0;
    sequence alpha;
    int cpt = 0;
    do {
        clock_t begin = clock();
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        printSequence(alpha);
        Ecurr.clear();
        Ecurr.push_back(alpha);
        clock_t end = clock();
        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
        cout << elapsed_secs << endl;
        cpt++;
    }
    while (alpha.size() != 0 && cpt < 4);
    return E;
}

/*
sequence verifyCheckingSequence(SATSolver * &solver,sequence CS, TFSM * S, Product_TFSM * D)
{
    vector<sequence> E;
    E.push_back(CS);
    set<string> beginningStates;
    computePhiE(solver, E, D);
    sequence alpha;
    TFSM * P = D->mutationMachine;
    while (alpha.size() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            Product_TFSM * DP = new Product_TFSM(S, P);
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

sequence generateCheckingSequenceTimeouted(TFSM * S, TFSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    Product_TFSM * D = new Product_TFSM(S, M);
    sequence CS;
    sequence alpha;
    double elapsed_secs = 0;
    do {
        clock_t begin = clock();
        CS.insert(CS.end(), alpha.begin(), alpha.end());
        alpha = verifyCheckingSequence(solver, CS, S, D);
        clock_t end = clock();
        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
        cout << elapsed_secs << endl;
    }
    while (alpha.size() != 0 && elapsed_secs < 400);
    return CS;
}

sequence generateCheckingSequence(TFSM * S, TFSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    Product_TFSM * D = new Product_TFSM(S, M);
    sequence CS;
    sequence alpha;
    do {
        CS.insert(CS.end(), alpha.begin(), alpha.end());
        alpha = verifyCheckingSequence(solver, CS, S, D);
    }
    while (alpha.size() != 0);
    return CS;
}
*/
