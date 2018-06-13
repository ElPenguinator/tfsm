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
#include "checkingalgorithms.h"
using namespace std;
using namespace CMSat;




void example1FULL(TFSM *& S, TFSM *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<GuardedTransition> lambda = {GuardedTransition(1, "a", Guard("[", 0, inf, ")"), "0", 1, 0),
                                 GuardedTransition(1, "b", Guard("[", 0, inf, ")"), "0", 2, 1),
                                 GuardedTransition(2, "b", Guard("[", 0, inf, ")"), "0", 2, 3),
                                 GuardedTransition(2, "a", Guard("[", 0, inf, ")"), "0", 3, 4),
                                 GuardedTransition(3, "a", Guard("[", 0, inf, ")"), "0", 3, 6),
                                 GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "0", 4, 7),
                                 GuardedTransition(4, "b", Guard("[", 0, inf, ")"), "0", 4, 10),
                                 GuardedTransition(4, "a", Guard("[", 0, inf, ")"), "1", 1, 9)
                                };
    vector<Timeout> delta = {Timeout(1, 4, 4, 2),
                             Timeout(3, 5, 2, 8),
                             Timeout(2, inf, 2, 5),
                             Timeout(4, inf, 4, 11)
                            };
    S = new TFSM(S2, s0, I, O, lambda, delta);
    M = new TFSM(S2, s0, I, O, lambda, delta);
    M->addTransitions({GuardedTransition(3, "a", Guard("[", 0, inf, ")"), "1", 3, 13),
                       GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "0", 3, 14),
                       GuardedTransition(4, "a", Guard("[", 0, inf, ")"), "1", 2, 12)
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

void example2FULL(TFSM *& S, TFSM *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<GuardedTransition> lambda = {GuardedTransition(1, "a", Guard("[", 0, inf, ")"), "0", 1, 0),
                                        GuardedTransition(1, "b", Guard("[", 0, 2, ")"), "0", 2, 1),
                                        GuardedTransition(1, "b", Guard("[", 2, inf, ")"), "1", 1, 2),
                                        GuardedTransition(2, "a", Guard("[", 0, 5, ")"), "0", 1, 4),
                                        GuardedTransition(2, "a", Guard("[", 5, 8, ")"), "1", 1, 5),
                                        GuardedTransition(2, "a", Guard("[", 8, inf, ")"), "1", 2, 6),
                                        GuardedTransition(2, "b", Guard("[", 0, inf, ")"), "0", 2, 7),
                                };
    vector<Timeout> delta = {Timeout(1, 4, 2, 3),
                             Timeout(2, inf, 2, 8)
                            };
    S = new TFSM(S2, s0, I, O, lambda, delta);
    M = new TFSM(S2, s0, I, O, lambda, delta);
    M->addTransitions({GuardedTransition(2, "a", Guard("[", 0, 3, ")"), "0", 1, 10),
                       GuardedTransition(2, "a", Guard("[", 3, 6, ")"), "1", 1, 11),
                       GuardedTransition(2, "a", Guard("[", 6, inf, ")"), "1", 1, 12)
                      });

    M->addTimeouts({Timeout(1, 3, 1, 9)
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

void example3FULL(TFSM *& S, TFSM *& M, vector<sequence> & E)
{
    set<int> S2 = {1, 2, 3, 4};
    int s0 = 1;
    set<string> I = {"a", "b"};
    set<string> O = {"0", "1"};
    vector<GuardedTransition> lambda = {GuardedTransition(1, "a", Guard("[", 0, inf, ")"), "0", 1, 0),
                                 GuardedTransition(1, "b", Guard("[", 0, inf, ")"), "0", 2, 1),
                                 GuardedTransition(2, "b", Guard("[", 0, inf, ")"), "0", 2, 3),
                                 GuardedTransition(2, "a", Guard("[", 0, 2, ")"), "0", 3, 4),
                                 GuardedTransition(2, "a", Guard("[", 2, inf, ")"), "0", 4, 5),
                                 GuardedTransition(3, "a", Guard("[", 0, inf, ")"), "0", 3, 7),
                                 GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "0", 4, 8),
                                 GuardedTransition(4, "b", Guard("[", 0, inf, ")"), "0", 4, 12),
                                 GuardedTransition(4, "a", Guard("[", 0, 3, ")"), "1", 1, 10),
                                 GuardedTransition(4, "a", Guard("[", 3, inf, ")"), "1", 3, 11)
                                };
    vector<Timeout> delta = {Timeout(1, 4, 4, 2),
                             Timeout(3, 5, 2, 9),
                             Timeout(2, inf, 2, 6),
                             Timeout(4, inf, 4, 13)
                            };
    S = new TFSM(S2, s0, I, O, lambda, delta);
    M = new TFSM(S2, s0, I, O, lambda, delta);
    M->addTransitions({/*GuardedTransition(2, "a", Guard("[", 0, 5, ")"), "0", 3, 15),
                       GuardedTransition(2, "a", Guard("[", 5, inf, ")"), "0", 4, 16),
                       GuardedTransition(3, "a", Guard("[", 0, inf, ")"), "1", 3, 17),
                       GuardedTransition(3, "b", Guard("[", 0, inf, ")"), "0", 3, 18),
                       GuardedTransition(4, "a", Guard("[", 0, inf, ")"), "1", 2, 20),
                       GuardedTransition(4, "a", Guard("[", 3, inf, ")"), "1", 1, 21),
                       GuardedTransition(4, "a", Guard("[", 0, 3, ")"), "1", 3, 22)
                      */});

    M->addTimeouts({/*Timeout(1, 3, 4, 14),*/
                    Timeout(3, 8, 1, 14)
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



int main()
{
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

    checkingExperimentBenchmarks();
    return 0;
}
