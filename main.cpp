#include <iostream>
#include "tfsm_to.h"
#include <iostream>
#include "structs.h"
#include "product_tfsm_to.h"
#include "cryptominisat5/cryptominisat.h"
#include "tools.h"
#include <ctime>
#include <fstream>
using namespace std;
using namespace CMSat;


void example1(TFSM_TO *& S, TFSM_TO *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<Transition> lambda = {Transition(1, "a", "0", 1, 0),
                                 Transition(1, "b", "0", 2, 1),
                                 Transition(2, "b", "0", 2, 3),
                                 Transition(2, "a", "0", 3, 4),
                                 Transition(3, "a", "0", 3, 6),
                                 Transition(3, "b", "0", 4, 7),
                                 Transition(4, "b", "0", 4, 10),
                                 Transition(4, "a", "1", 1, 9)
                                };
    vector<Timeout> delta = {Timeout(1, 4, 4, 2),
                             Timeout(3, 5, 2, 8),
                             Timeout(2, inf, 2, 5),
                             Timeout(4, inf, 4, 11)
                            };
    S = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M->addTransitions({Transition(3, "a", "1", 3, 13),
                       Transition(3, "b", "0", 3, 14),
                       Transition(4, "a", "1", 2, 12)
                      });
    M->addTimeouts({Timeout(1, 3, 4, 15),
                    Timeout(3, 8, 1, 16)
                   });
    E = {sequence({ts("a", 0),
                   ts("a", 2),
                   ts("a", 5),
                   ts("a", 14),
                   ts("a", 19)
         }
         ),
         sequence({ts("a", 0),
                   ts("a", 2),
                   ts("a", 6),
                   ts("a", 14),
                   ts("a", 19)
         }
         ),
         sequence({ts("a", 0),
                   ts("a", 2),
                   ts("a", 6),
                   ts("a", 14),
                   ts("a", 19)
         }
         ),
        };
}

TFSM_TO * generateSubmachine(SATSolver * &solver, TFSM_TO * M)
{
    lbool ret = solver->solve();
    if (ret == l_True) {
        set<int> S(M->states);
        int s0 = M->initialState;
        set<string> I(M->inputs);
        set<string> O(M->inputs);
        vector<Transition> lambda;
        vector<Timeout> delta;
        for (int i=0; i<M->transitions.size() + M->timeouts.size(); i++) {
            if (solver->get_model()[i] == l_True) {
                int id = i;
                if (M->isIdTimeout(id)) {
                    Timeout corr = M->getTimeoutFromId(id);
                    delta.push_back(Timeout(corr.src, corr.t, corr.tgt, corr.id));
                }
                else {
                    Transition corr = M->getTransitionFromId(id);
                    lambda.push_back(Transition(corr.src, corr.i, corr.o, corr.tgt, corr.id));
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
                    Timeout corresponding = D->mutationMachine->getTimeoutFromId(id);
                    vector<Timeout> correspondingSuspicious = D->mutationMachine->getXi(corresponding.src);
                    if (correspondingSuspicious.size() > 1) {
                        clause.push_back(Lit(id, true));
                    }
                }
                else {
                    Transition corresponding = D->mutationMachine->getTransitionFromId(id);
                    vector<Transition> correspondingSuspicious = D->mutationMachine->getXi(corresponding.src, corresponding.i);
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
            vector<Transition> res = M->getXi(s, i);
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
    solver->log_to_file("/tmp/test.txt");
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
        cout << elapsed_secs << endl;
    }
    while (Ecurr.size() != 0 && elapsed_secs < 400);
    if (elapsed_secs > 400) {
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
    double elapsed_secs = 0;
    sequence alpha;
    do {
        clock_t begin = clock();
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        Ecurr.push_back(alpha);
        clock_t end = clock();
        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
        cout << elapsed_secs << endl;
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
                if (alpha.size() == 0)
                    computePhiP(solver, P);
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
        cout << elapsed_secs << endl;
    }
    while (alpha.size() != 0 && elapsed_secs < 400);
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

unsigned long long int computeNumberOfMutants(TFSM_TO * M) {
    unsigned long long int res = 1;
    for (int s : M->states) {
        for (string i : M->inputs) {
            if (M->getXi(s).size() > 1)
                res *= M->getXi(s, i).size();
        }
    }
    for (int s : M->states) {
        if (M->getXi(s).size() > 1)
            res *= M->getXi(s).size();
    }
    return res;
}

void checkExample1()
{
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    example1(S, M, E);

    Product_TFSM_TO * P = new Product_TFSM_TO(S, M);
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->timeouts.size() + M->transitions.size());
    sequence res = verifyCheckingExperiment(solver, E, S, P);
    if (res.empty())
        cout << " Test suite complete." << endl;
    else
        cout << " Test suite incomplete." << endl;
}

void checkingExperimentExample1()
{
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    example1(S, M, E);

    vector<sequence> Einit;
    E = generateCheckingExperiment(Einit, S, M);
    cout << "Result : " << endl;
    for (auto s : E) {
        printSequence(s);
    }

}

void checkingSequenceExample1()
{
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    sequence CS;
    example1(S, M, E);

    CS = generateCheckingSequence(S, M);
    printSequence(CS);
}

void checkingExperimentBenchmarks()
{
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};


    int nbStates [5] = {4, 8, 10, 12, 15};
    int nbMutants [6] = {20, 50, 100, 200, 300, 400};
    int maxTime = 5;

    for (int nbOfBench=0; nbOfBench < 5; nbOfBench++) {
        ofstream benchFile;

        for (int j=2; j<3; j++) {
            //benchFile.open("bench_CS_" + to_string(nbStates[j]) + '_' + to_string(nbOfBench) + ".txt");
            for (int i=0; i < 3; i++) {
                benchFile.open("bench_CE_" + to_string(nbStates[j]) + "_" + to_string(nbMutants[i]) + '_' + to_string(nbOfBench) + ".txt");
                TFSM_TO * randomSpec = generateRandomSpecification(nbStates[j], maxTime, I, O);
                TFSM_TO * randomMuta = generateRandomMutationMachine(randomSpec, maxTime*2, nbMutants[i]);
                vector<sequence> E;
                vector<sequence> Einit;
                clock_t begin = clock();
                E = generateCheckingExperimentTimeouted(Einit, randomSpec, randomMuta);
                clock_t end = clock();
                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
                benchFile << nbStates[j] << " " << nbMutants[i] << " " << maxTime << " " << elapsed_secs << "s "<< computeNumberOfMutants(randomMuta) << " " << E.size() << "\n";
                delete randomSpec;
                delete randomMuta;
                benchFile.close();
            }

        }
    }
}


void checkingSequenceBenchmarks()
{
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};


    int nbStates [5] = {4, 8, 10, 12, 15};
    int nbMutants [6] = {20, 50, 100, 200, 300, 400};
    int maxTime = 5;

    for (int nbOfBench=0; nbOfBench < 5; nbOfBench++) {
        ofstream benchFile;

        for (int j=2; j<3; j++) {
            for (int i=0; i < 3; i++) {
                benchFile.open("bench_CS_" + to_string(nbStates[j]) + "_" + to_string(nbMutants[i]) + '_' + to_string(nbOfBench) + ".txt");
                TFSM_TO * randomSpec = generateRandomSpecification(nbStates[j], maxTime, I, O);
                TFSM_TO * randomMuta = generateRandomMutationMachine(randomSpec, maxTime*2, nbMutants[i]);
                sequence CS;
                clock_t begin = clock();
                CS = generateCheckingSequenceTimeouted(randomSpec, randomMuta);
                clock_t end = clock();
                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
                benchFile << nbStates[j] << " " << nbMutants[i] << " " << maxTime << " " << elapsed_secs << "s "<< computeNumberOfMutants(randomMuta) << " " << CS.size() << "\n";
                delete randomSpec;
                delete randomMuta;
                benchFile.close();
            }
        }

    }
}
void testChaosMachine()
{
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};

    TFSM_TO * randomSpec = generateRandomSpecification(4, 4, I, O);
    randomSpec->print();
    TFSM_TO * chaosMuta = generateChaosMachine(randomSpec, 10);
    chaosMuta->print();

    Product_TFSM_TO * P = new Product_TFSM_TO(randomSpec, chaosMuta);
    P->print();
    cout << P->states.size() << " " << P->transitions.size() << endl;

    vector<sequence> E;
    vector<sequence> Einit;
    clock_t begin = clock();
    E = generateCheckingExperiment(Einit, randomSpec, chaosMuta);
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << elapsed_secs << " " << computeNumberOfMutants(chaosMuta) << endl;
    cout << "Result : " << endl;
    for (auto s : E) {
        printSequence(s);
    }

}

int main()
{
    checkingExperimentExample1();
    checkingSequenceExample1();
    //checkingSequenceBenchmarks();
    //testChaosMachine();
    return 0;
}
