#include <iostream>
#include "tfsm_to.h"
#include "tfsm.h"
#include "fsm.h"
#include "structs.h"
#include "product_tfsm_to.h"
#include "product_tfsm.h"
#include "product_fsm_full.h"
#include "cryptominisat5/cryptominisat.h"
#include "tools.h"
#include <ctime>
#include <fstream>
#include <math.h>
//#include "checkingalgorithms.h"
#include "checkingalgorithms_to.h"
#include "infint.h"
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
}

InfInt computeNumberOfMutants(TFSM_TO * M) {
    InfInt res = 1;
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


void checkingExperimentBenchmarks(int nbOfBench)
{
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};


    int nbStates [5] = {4, 8, 10, 12, 15};
    int nbMutants [6] = {20, 50, 100, 200, 300, 400};
    int maxTime = 5;
    ofstream benchFile;
    for (int j=0; j<5; j++) {
        //benchFile.open("bench_CS_" + to_string(nbStates[j]) + '_' + to_string(nbOfBench) + ".txt");
        for (int i=0; i < 4; i++) {
            benchFile.open("bench_CE_" + to_string(nbStates[j]) + "_" + to_string(nbMutants[i]) + '_' + to_string(nbOfBench) + ".txt");
            TFSM_TO * randomSpec = generateRandomSpecification_TO(nbStates[j], maxTime, I, O);
            TFSM_TO * randomMuta = generateRandomMutationMachine_TO(randomSpec, maxTime*2, nbMutants[i]);
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


void checkingSequenceBenchmarks()
{
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};


    int nbStates [5] = {4, 8, 10, 12, 15};
    int nbMutants [6] = {20, 50, 100, 200, 300, 400};
    int maxTime = 5;

    for (int nbOfBench=0; nbOfBench < 5; nbOfBench++) {
        ofstream benchFile;

        for (int j=0; j<3; j++) {
            for (int i=0; i < 6; i++) {
                benchFile.open("bench_CS_" + to_string(nbStates[j]) + "_" + to_string(nbMutants[i]) + '_' + to_string(nbOfBench) + ".txt");
                TFSM_TO * randomSpec = generateRandomSpecification_TO(nbStates[j], maxTime, I, O);
                TFSM_TO * randomMuta = generateRandomMutationMachine_TO(randomSpec, maxTime*2, nbMutants[i]);
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

    TFSM_TO * randomSpec = generateRandomSpecification_TO(4, 4, I, O);
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




/////

TFSM_TO * generateRandomMutationMachineTFTP(TFSM_TO * S, int maxTime, int numberOfMutations)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<Transition> lambda(S->transitions);
    vector<Timeout> delta(S->timeouts);
    TFSM_TO * M = new TFSM_TO(States, s0, I, O, lambda, delta);

    srand (time(NULL));
    vector<Transition> newLambda;
    vector<Timeout> newDelta;
    for (int i=0; i<numberOfMutations; i++) {
        bool alreadyExisting = true;
        while (alreadyExisting) {
            bool createTimeout = rand()%2;
            int randomSrc = floor(rand() % S->states.size());
            int randomTgt = floor(rand() % S->states.size());
            int newId = i + (S->transitions.size() + S->timeouts.size());
            if (createTimeout) {
                if (randomSrc != 0) {
                    int randomT = floor(rand() % maxTime);
                    if (randomT == 0) {
                        randomT = inf;
                        randomTgt = randomSrc;
                    }
                    Timeout newTimeout(randomSrc,randomT, randomTgt, newId);
                    if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                        newDelta.push_back(newTimeout);
                        alreadyExisting = false;
                    }
                }
            }
            else {
                string randomI = getRandomStringFromSet(I);
                bool isOkay = true;
                for (Transition t : S->transitionsPerState.find(randomSrc)->second) {
                    if (t.i == randomI) {
                        if (t.o == "Not_Defined") {
                            isOkay = false;
                        }
                    }
                }
                if (isOkay) {
                    string randomO = getRandomStringFromSet(O);
                    Transition newTransition(randomSrc,randomI, randomO, randomTgt, newId);
                    if (!transitionAlreadyExist(newTransition, lambda, newLambda)) {
                        newLambda.push_back(newTransition);
                        alreadyExisting = false;
                    }
                }
            }
        }
    }
    M->addTransitions(newLambda);
    M->addTimeouts(newDelta);
    return M;
}

TFSM_TO * generateCompleteMutationMachineTFTP(TFSM_TO * S)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<Transition> lambda(S->transitions);
    vector<Timeout> delta(S->timeouts);
    TFSM_TO * M = new TFSM_TO(States, s0, I, O, lambda, delta);

    srand (time(NULL));
    vector<Transition> newLambda;
    vector<Timeout> newDelta;

    int newId = (S->transitions.size() + S->timeouts.size());

    for (int src : States) {
        for (int tgt : States) {
            for (string i : I) {
                bool isOkay = true;
                for (Transition t : S->transitionsPerState.find(src)->second) {
                    if (t.i == i) {
                        if (t.o == "Not_Defined") {
                            isOkay = false;
                        }
                    }
                }
                if (!isOkay) {
                    for (string o : O) {
                        Transition newTransition(src,i, o, tgt, newId);
                        if (!transitionAlreadyExist(newTransition, lambda, newLambda)) {
                            newLambda.push_back(newTransition);
                            newId++;
                        }
                    }
                }
            }
        }
        if (src != 0) {
            vector<int> possibleTimeouts = {0, 1, 5};
            for (int t : possibleTimeouts) {
                int newT = t;
                if (newT == 0) {
                    newT = inf;
                }
                Timeout newTimeout(src,newT, 0, newId);
                if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                    newDelta.push_back(newTimeout);
                    newId++;
                }
            }
        }
    }
    M->addTransitions(newLambda);
    M->addTimeouts(newDelta);
    return M;
}

void exampleTFTP(TFSM_TO *& S, TFSM_TO *& M, vector<sequence> & E)
{
    set<int> S2 = {0, 1, 2, 3};
    int s0 = 0;
    set<string> I = {"RRQ", "ACK_1", "ACK_2", "ACK_3", "ERROR"};
    set<string> O = {"DATA_1", "DATA_2", "DATA_3", "Ignore", "Empty", "ERROR", "Not_Defined"};
    vector<Transition> lambda = {Transition(0, "RRQ", "DATA_1", 1, 0),
                                 Transition(0, "ACK_1", "Not_Defined", 0, 1),
                                 Transition(0, "ACK_2", "Not_Defined", 0, 2),
                                 Transition(0, "ACK_3", "Not_Defined", 0, 3),
                                 Transition(0, "ERROR", "Not_Defined", 0, 4),
                                 Transition(1, "RRQ", "Not_Defined", 1, 6),
                                 Transition(1, "ACK_1", "DATA_2", 2, 7),
                                 Transition(1, "ACK_2", "ERROR", 0, 8),
                                 Transition(1, "ACK_3", "ERROR", 0, 9),
                                 Transition(1, "ERROR", "Empty", 0, 10),
                                 Transition(2, "RRQ", "Not_Defined", 2, 12),
                                 Transition(2, "ACK_1", "Ignore", 2, 13),
                                 Transition(2, "ACK_2", "DATA_3", 3, 14),
                                 Transition(2, "ACK_3", "ERROR", 0, 15),
                                 Transition(2, "ERROR", "Empty", 0, 16),
                                 Transition(3, "RRQ", "Not_Defined", 3, 18),
                                 Transition(3, "ACK_1", "Ignore", 3, 19),
                                 Transition(3, "ACK_2", "Ignore", 3, 20),
                                 Transition(3, "ACK_3", "Empty", 0, 21),
                                 Transition(3, "ERROR", "Empty", 0, 22)

                                };
    vector<Timeout> delta = {Timeout(0, inf, 0, 5),
                             Timeout(1, 3, 0, 11),
                             Timeout(2, 3, 0, 17),
                             Timeout(3, 3, 0, 23)
                            };
    S = new TFSM_TO(S2, s0, I, O, lambda, delta);
    //M = new TFSM_TO(S2, s0, I, O, lambda, delta);

    //M = generateRandomMutationMachineTFTP(S, 5, 100);
    //M = generateCompleteMutationMachineTFTP(S);
    M = generateChaosMachine(S, 5);
    cout << "Number of mutants : " << computeNumberOfMutants(M) << endl;;
    /*
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
        */
    E = {};
}

void example4(TFSM_TO *& S, TFSM_TO *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2};
    int s0 = 1;
    set<string> I = {"a"};
    set<string> O = {"0", "1"};
    vector<Transition> lambda = {Transition(1, "a", "0", 1, 0),
                                 Transition(2, "a", "1", 2, 2),
                                };
    vector<Timeout> delta = {Timeout(1, 3, 2, 1),
                             Timeout(2, 3, 1, 3)
                            };
    S = new TFSM_TO(S2, s0, I, O, lambda, delta);
    M = new TFSM_TO(S2, s0, I, O, lambda, delta);

    M->addTimeouts({Timeout(1, 1, 2, 4),
                    Timeout(1, 5, 2, 5),
                    Timeout(2, 1, 1, 6),
                    Timeout(2, 5, 1, 7)
                   });
    E = {};
}



int main(int argc, char ** argv)
{
    /*
    TFSM * S;
    TFSM * M;
    vector<sequence> E;
    example3FULL(S, M, E);
    //S->print();
    M->print();
    Product_TFSM * P = new Product_TFSM(S, M);
    P->print();
    vector<sequence> Einit;
    E = generateCheckingExperiment(Einit, S, M);
    cout << "Boum" << endl;
    for (auto s : E) {
        printSequence(s);
    }
    */
    //checkingExperimentBenchmarks();
    /*
    TFSM_TO * S;
    TFSM_TO * M;
    vector<sequence> E;
    exampleTFTP(S, M, E);
    //example4(S, M , E);
    //example1(S, M, E);
    S->print();
    M->print();
    Product_TFSM_TO * P = new Product_TFSM_TO(S, M);
    //P->print();

    vector<sequence> Einit;
    clock_t begin = clock();
    E = generateCheckingExperiment(Einit, S, M);
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    cout << elapsed_secs << endl;
    cout << "Boum" << endl;
    for (auto s : E) {
        printSequence(s);
    }

    */
    checkingExperimentBenchmarks(2);

    return 0;
}
