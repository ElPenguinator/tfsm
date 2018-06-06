#include <iostream>
#include "tfsm_to.h"
#include "fsm.h"
#include "structs.h"
#include "product_tfsm_to.h"
#include "product_fsm_full.h"
#include "cryptominisat5/cryptominisat.h"
#include "tools.h"
#include <ctime>
#include <fstream>
#include <math.h>
#include "checkingalgorithms.h"
using namespace std;
using namespace CMSat;

void example2FSM(TFSM_TO *& S, TFSM_TO *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<Transition> lambda = {Transition(1, "a", "0", 1, 0),
                                 Transition(1, "b", "0", 2, 1),
                                 Transition(2, "a", "0", 3, 2),
                                 Transition(2, "b", "0", 2, 3),
                                 Transition(3, "a", "0", 3, 4),
                                 Transition(3, "b", "0", 4, 5),
                                 Transition(4, "a", "1", 1, 6),
                                 Transition(4, "b", "0", 4, 7)
                                };
    vector<Timeout> delta = {Timeout(1, inf, 1, 8),
                             Timeout(2, inf, 2, 9),
                             Timeout(3, inf, 3, 10),
                             Timeout(4, inf, 4, 11)
                            };
    S = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M->addTransitions({Transition(3, "a", "1", 3, 12),
                       Transition(3, "b", "0", 3, 13),
                       Transition(4, "a", "1", 2, 14)
                      });

    E = {sequence({ts("b", 0),
                   ts("a", 0),
                   ts("b", 0),
                   ts("b", 0),
                   ts("a", 0)
         }
         )
        };
}

void example1FSM(TFSM_TO *& S, TFSM_TO *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<Transition> lambda = {Transition(1, "a", "0", 1, 0),
                                 Transition(1, "b", "0", 2, 1),
                                 Transition(2, "a", "1", 1, 2),
                                 Transition(2, "b", "1", 1, 3)
                                };
    vector<Timeout> delta = {Timeout(1, inf, 1, 4),
                             Timeout(2, inf, 2, 5)
                            };
    S = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M->addTransitions({Transition(2, "a", "1", 2, 6),
                       Transition(2, "b", "1", 2, 7)
                      });

    /*
    M->addTransitions({Transition(1, "a", "0", 2, 8),
                       Transition(1, "b", "0", 1, 9)
                      });
    */

    E = {sequence({ts("b", 0),
                   ts("a", 0),
                   ts("b", 0),
                   ts("b", 0),
                   ts("a", 0)
         }
         )
        };
}


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
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("b", 19),
                   ts("a", 19),
                   ts("b", 24),
                   ts("a", 24)
         }
         )
        };
    /*
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
        */
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
    example1FSM(S, M, E);

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

void checkExample1FSM()
{
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    example1FSM(S, M, E);

    Product_TFSM_TO * P = new Product_TFSM_TO(S, M);
    P->print();
    SATSolver * solver = new SATSolver();
    solver->log_to_file("/tmp/test.txt");
    solver->new_vars(M->timeouts.size() + M->transitions.size());

    computePhiM(solver, S, M);
    sequence res = verifyCheckingExperiment(solver, E, S, P);
    printSequence(res);
    if (res.empty())
        cout << " Test suite complete." << endl;
    else
        cout << " Test suite incomplete." << endl;
}

void checkExample1FSMSequence()
{
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    example1(S, M, E);

    Product_TFSM_TO * P = new Product_TFSM_TO(S, M);
    P->print();
    SATSolver * solver = new SATSolver();
    solver->log_to_file("/tmp/test.txt");
    solver->new_vars(M->timeouts.size() + M->transitions.size());

    computePhiM(solver, S, M);
    sequence CS = E.front();
    printSequence(CS);
    sequence res = verifyCheckingSequence(solver, CS, S, P);
    printSequence(res);
    if (res.empty())
        cout << " Test suite complete." << endl;
    else
        cout << " Test suite incomplete." << endl;
}

void checkEverythingExampleFSM()
{
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    example1FSM(S, M, E);
    Product_TFSM_TO * P = new Product_TFSM_TO(S, M);
    P->print();

    for (int exp=0; exp<10; exp++) {
        cout << "Exp : " << exp << " " << ( pow(2, exp)) << endl;
        bool found = false;
        for (int i=0; i<pow(2, exp); i++) {
            //cout << i << endl;
            vector<sequence> E;
            sequence seq;
            for (int j = 0; j < exp; ++j) {
                bool value = (i >> j) & 1;
                if (value)
                    seq.push_back(ts("a", 0));
                else
                    seq.push_back(ts("b", 0));
            }
            E.push_back(seq);
            SATSolver * solver = new SATSolver();
            solver->new_vars(M->timeouts.size() + M->transitions.size());

            computePhiM(solver, S, M);
            sequence res = verifyCheckingExperiment(solver, E, S, P);
            if (res.empty()) {
                cout << " Test suite complete." << endl;
                printSequence(seq);
                found = true;
            }
        }
        if (found)
            return;
    }
    /*
    SATSolver * solver = new SATSolver();
    solver->new_vars(M->timeouts.size() + M->transitions.size());

    computePhiM(solver, S, M);
    sequence res = verifyCheckingExperiment(solver, E, S, P);
    printSequence(res);
    if (res.empty())
        cout << " Test suite complete." << endl;
    else
        cout << " Test suite incomplete." << endl;
        */
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

void minimalCheckingSequenceExampleFSM()
{
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    example1FSM(S, M, E);


    Product_FSM_Full * P = new Product_FSM_Full(S, M);
    P->print();
    P->minimalCheckingSequence();
    /*
    for (int exp=0; exp<10; exp++) {
        cout << "Exp : " << exp << " " << ( pow(2, exp)) << endl;
        bool found = false;
        for (int i=0; i<pow(2, exp); i++) {
            //cout << i << endl;
            vector<sequence> E;
            sequence seq;
            for (int j = 0; j < exp; ++j) {
                bool value = (i >> j) & 1;
                if (value)
                    seq.push_back(ts("a", 0));
                else
                    seq.push_back(ts("b", 0));
            }
            E.push_back(seq);
            SATSolver * solver = new SATSolver();
            solver->new_vars(M->timeouts.size() + M->transitions.size());

            computePhiM(solver, S, M);
            sequence res = verifyCheckingExperiment(solver, E, S, P);
            if (res.empty()) {
                cout << " Test suite complete." << endl;
                printSequence(seq);
                found = true;
            }
        }
        if (found)
            return;
    }
    */
}

int main()
{
    //minimalCheckingSequenceExampleFSM();
    //checkEverythingExampleFSM();
    /*
    checkExample1FSM();
    checkingExperimentExample1();
    */
    //checkExample1FSMSequence();
    checkingSequenceExample1();

    checkingSequenceBenchmarks();
    //testChaosMachine();
    return 0;
}
