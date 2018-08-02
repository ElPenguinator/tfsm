//#include <iostream>
//#include "machine/tfsm_to.h"
//#include "machine/tfsm.h"
//#include "machine/fsm.h"
//#include "structs.h"
//#include "machine/product_tfsm_to.h"
//#include "machine/product_fsm_full.h"
//#include "cryptominisat5/cryptominisat.h"
//#include "tools.h"
//#include <ctime>
//#include <fstream>
//#include <math.h>
//#include "checkingalgorithms_to.h"
//using namespace std;
//using namespace CMSat;

//void example2FSM(TFSM_TO *& S, TFSM_TO *& M, vector<sequence> & E)
//{
//    set<int> S2 = {1, 2, 3, 4};
//    int s0 = 1;
//    set<string> I = {"a", "b"};
//    set<string> O = {"0", "1"};
//    vector<IOTransition *> lambda = {IOTransition(1, "a", "0", 1, 0),
//                                 IOTransition(1, "b", "0", 2, 1),
//                                 IOTransition(2, "a", "0", 3, 2),
//                                 IOTransition(2, "b", "0", 2, 3),
//                                 IOTransition(3, "a", "0", 3, 4),
//                                 IOTransition(3, "b", "0", 4, 5),
//                                 IOTransition(4, "a", "1", 1, 6),
//                                 IOTransition(4, "b", "0", 4, 7)
//                                };
//    vector<TimeoutTransition> delta = {TimeoutTransition(1, inf, 1, 8),
//                             TimeoutTransition(2, inf, 2, 9),
//                             TimeoutTransition(3, inf, 3, 10),
//                             TimeoutTransition(4, inf, 4, 11)
//                            };
//    S = new TFSM_TO(S2, s0, I, O, lambda, delta);
//    M = new TFSM_TO(S2, s0, I, O, lambda, delta);
//    M->addTransitions({IOTransition(3, "a", "1", 3, 12),
//                       IOTransition(3, "b", "0", 3, 13),
//                       IOTransition(4, "a", "1", 2, 14)
//                      });

//    E = {sequence({ts("b", 0),
//                   ts("a", 0),
//                   ts("b", 0),
//                   ts("b", 0),
//                   ts("a", 0)
//         }
//         )
//        };
//}

//void example1FSM(TFSM_TO *& S, TFSM_TO *& M, vector<sequence> & E)
//{
//    set<int> S2 = {1, 2};
//    int s0 = 1;
//    set<string> I = {"a", "b"};
//    set<string> O = {"0", "1"};
//    vector<IOTransition> lambda = {IOTransition(1, "a", "0", 1, 0),
//                                 IOTransition(1, "b", "0", 2, 1),
//                                 IOTransition(2, "a", "1", 1, 2),
//                                 IOTransition(2, "b", "1", 1, 3)
//                                };
//    vector<TimeoutTransition> delta = {TimeoutTransition(1, inf, 1, 4),
//                             TimeoutTransition(2, inf, 2, 5)
//                            };
//    S = new TFSM_TO(S2, s0, I, O, lambda, delta);
//    M = new TFSM_TO(S2, s0, I, O, lambda, delta);
//    M->addTransitions({IOTransition(2, "a", "1", 2, 6),
//                       IOTransition(2, "b", "1", 2, 7)
//                      });

//    /*
//    M->addTransitions({IOTransition(1, "a", "0", 2, 8),
//                       IOTransition(1, "b", "0", 1, 9)
//                      });
//    */

//    E = {sequence({ts("b", 0),
//                   ts("a", 0),
//                   ts("b", 0),
//                   ts("b", 0),
//                   ts("a", 0)
//         }
//         )
//        };
//}

//void checkExample1FSM()
//{
//    TFSM_TO * S;
//    TFSM_TO * M;
//    vector<sequence> E;
//    example1FSM(S, M, E);

//    Product_TFSM_TO * P = new Product_TFSM_TO(S, M);
//    P->print();
//    SATSolver * solver = new SATSolver();
//    solver->log_to_file("/tmp/test.txt");
//    solver->new_vars(M->timeouts.size() + M->transitions.size());

//    computePhiM(solver, S, M);
//    sequence res = verifyCheckingExperiment(solver, E, S, P);
//    printSequence(res);
//    if (res.empty())
//        cout << " Test suite complete." << endl;
//    else
//        cout << " Test suite incomplete." << endl;
//}

//void checkExample1FSMSequence()
//{
//    TFSM_TO * S;
//    TFSM_TO * M;
//    vector<sequence> E;
//    example1FSM(S, M, E);

//    Product_TFSM_TO * P = new Product_TFSM_TO(S, M);
//    P->print();
//    SATSolver * solver = new SATSolver();
//    solver->log_to_file("/tmp/test.txt");
//    solver->new_vars(M->timeouts.size() + M->transitions.size());

//    computePhiM(solver, S, M);
//    sequence CS = E.front();
//    printSequence(CS);
//    sequence res = verifyCheckingSequence(solver, CS, S, P);
//    printSequence(res);
//    if (res.empty())
//        cout << " Test suite complete." << endl;
//    else
//        cout << " Test suite incomplete." << endl;
//}

//void checkEverythingExampleFSM()
//{
//    TFSM_TO * S;
//    TFSM_TO * M;
//    vector<sequence> E;
//    example1FSM(S, M, E);
//    Product_TFSM_TO * P = new Product_TFSM_TO(S, M);
//    P->print();

//    for (int exp=0; exp<10; exp++) {
//        cout << "Exp : " << exp << " " << ( pow(2, exp)) << endl;
//        bool found = false;
//        for (int i=0; i<pow(2, exp); i++) {
//            //cout << i << endl;
//            vector<sequence> E;
//            sequence seq;
//            for (int j = 0; j < exp; ++j) {
//                bool value = (i >> j) & 1;
//                if (value)
//                    seq.push_back(ts("a", 0));
//                else
//                    seq.push_back(ts("b", 0));
//            }
//            E.push_back(seq);
//            SATSolver * solver = new SATSolver();
//            solver->new_vars(M->timeouts.size() + M->transitions.size());

//            computePhiM(solver, S, M);
//            sequence res = verifyCheckingExperiment(solver, E, S, P);
//            if (res.empty()) {
//                cout << " Test suite complete." << endl;
//                printSequence(seq);
//                found = true;
//            }
//        }
//        if (found)
//            return;
//    }
//    /*
//    SATSolver * solver = new SATSolver();
//    solver->new_vars(M->timeouts.size() + M->transitions.size());

//    computePhiM(solver, S, M);
//    sequence res = verifyCheckingExperiment(solver, E, S, P);
//    printSequence(res);
//    if (res.empty())
//        cout << " Test suite complete." << endl;
//    else
//        cout << " Test suite incomplete." << endl;
//        */
//}

///*
//void minimalCheckingSequenceExampleFSM()
//{
//    TFSM_TO * S;
//    TFSM_TO * M;
//    vector<sequence> E;
//    example1FSM(S, M, E);


//    Product_FSM_Full * P = new Product_FSM_Full(S, M);
//    P->print();
//    P->minimalCheckingSequence();

//    for (int exp=0; exp<10; exp++) {
//        cout << "Exp : " << exp << " " << ( pow(2, exp)) << endl;
//        bool found = false;
//        for (int i=0; i<pow(2, exp); i++) {
//            //cout << i << endl;
//            vector<sequence> E;
//            sequence seq;
//            for (int j = 0; j < exp; ++j) {
//                bool value = (i >> j) & 1;
//                if (value)
//                    seq.push_back(ts("a", 0));
//                else
//                    seq.push_back(ts("b", 0));
//            }
//            E.push_back(seq);
//            SATSolver * solver = new SATSolver();
//            solver->new_vars(M->timeouts.size() + M->transitions.size());

//            computePhiM(solver, S, M);
//            sequence res = verifyCheckingExperiment(solver, E, S, P);
//            if (res.empty()) {
//                cout << " Test suite complete." << endl;
//                printSequence(seq);
//                found = true;
//            }
//        }
//        if (found)
//            return;
//    }

//}
//*/
