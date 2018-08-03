//#include "checkingalgorithms.h"
//using namespace std;
//using namespace CMSat;

//TFSM * generateSubmachine(SATSolver * &solver, TFSM * M)
//{
//    lbool ret = solver->solve();
//    if (ret == l_True) {
//        set<int> S(M->states);
//        int s0 = M->initialState;
//        set<string> I(M->inputs);
//        set<string> O(M->inputs);
//        vector<IOTransition *> lambda;
//        vector<TimeoutTransition *> delta;
//        for (int i=0; i<M->transitions.size() + M->timeouts.size(); i++) {
//            if (solver->get_model()[i] == l_True) {
//                int id = i;
//                if (M->isIdTimeout(id)) {
//                    TimeoutTransition * corr = M->getTimeoutFromId(id);
//                    delta.push_back(new TimeoutTransition(corr->src, corr->t, corr->tgt, corr->id));
//                }
//                else {
//                    IOTransition * corr = M->getTransitionFromId(id);
//                    lambda.push_back(new GuardedTransition(corr->src, corr->i, corr->getGuard(), corr->o, corr->tgt, corr->id));
//                }
//            }
//        }

//        return new TFSM(S, s0, I, O, lambda, delta);
//    }
//    else {
//        return NULL;
//    }
//}

//void computePhiP(SATSolver * &solver, TFSM * P)
//{
//    vector<Lit> clause;
//    for (auto transition : P->transitions) {
//        clause.push_back(Lit(transition->id, true));
//    }
//    for (auto timeout : P->timeouts) {
//        clause.push_back(Lit(timeout->id, true));
//    }
//    solver->add_clause(clause);
//}

//void computePhiE(SATSolver * &solver, vector<sequence> E, Product_TFSM * D)
//{
//    for (auto alpha : E) {
//        //cout << "Begin computing Paths" << endl;
//        vector<path> rev = D->revealingPaths(alpha);
//        //cout << "Alpha To Eliminate : " << endl;
//        //printSequence(alpha);
//        //cout << "Paths : " << endl;
//        for (auto path : rev) {
//            //printPath(path);
//            vector<Lit> clause;
//            for (auto id : path) {
//                if (D->mutationMachine->isIdTimeout(id)) {
//                    TimeoutTransition * corresponding = D->mutationMachine->getTimeoutFromId(id);
//                    vector<TimeoutTransition *> correspondingSuspicious = D->mutationMachine->getXi(corresponding->src);
//                    if (correspondingSuspicious.size() > 1) {
//                        clause.push_back(Lit(id, true));
//                    }
//                }
//                else {
//                    IOTransition * corresponding = D->mutationMachine->getTransitionFromId(id);
//                    vector<IOTransition *> correspondingSuspicious = D->mutationMachine->getXi(corresponding->src, corresponding->i);
//                    if (correspondingSuspicious.size() > 1) {
//                        clause.push_back(Lit(id, true));
//                    }
//                }
//            }
//            solver->add_clause(clause);
//        }
//    }
//}

//void computePhiMRecur(SATSolver * &solver, const vector<IOTransition *> &xi, const set<set<int>>::iterator &current, const set<set<int>>::iterator &end, vector<int> currentClause) {
//    set<int> combi = (*current);
//    for (int elt : combi) {
//        set<set<int>>::iterator copyCurrent(current);
//        copyCurrent++;
//        vector<int> copyCurrentClause(currentClause);
//        copyCurrentClause.push_back(elt);
//        if (copyCurrent != end) {
//            computePhiMRecur(solver, xi, copyCurrent, end, copyCurrentClause);
//        }
//        else {
//            vector<Lit> clause;
//            for (int li : copyCurrentClause) {;
//                if (li >= 0) {
//                    clause.push_back(Lit(li, false));
//                }
//                else {
//                    clause.push_back(Lit(-li, true));
//                }
//            }
//            solver->add_clause(clause);
//        }
//    }
//}


//void computePhiM(SATSolver * &solver, TFSM * S, TFSM * M)
//{
//    //Chose one subset of guarded transitions per input per state
//    for (auto s : M->states) {
//        for (auto i : M->inputs) {
//            vector<IOTransition *> xi = M->getXi(s, i);
//            set<set<int>> eta = M->getEta(s, i);
//            vector<int> emptyClause;
//            set<set<int>> newEta;

//            for (set<int> combi : eta) {
//                set<int> newSet;
//                for (int elt : combi) {
//                    newSet.insert(elt);
//                }
//                for (IOTransition * elt : xi) {
//                    if (combi.find(elt->id) == combi.end()) {
//                        newSet.insert(-elt->id);
//                    }
//                }
//                newEta.insert(newSet);
//            }

//            set<set<int>>::iterator begin = newEta.begin();
//            set<set<int>>::iterator end = newEta.end();

//            computePhiMRecur(solver, xi, begin, end, emptyClause);
//        }
//    }
//    //Choose one timeout per state
//    for (auto s : M->states) {
//        vector<TimeoutTransition *> res = M->getXi(s);
//        for (int k=0; k<res.size(); k++) {
//            for (int l=k+1; l<res.size(); l++) {
//                vector<Lit> clause;
//                clause.push_back(Lit(res[k]->id, true));
//                clause.push_back(Lit(res[l]->id, true));
//                solver->add_clause(clause);
//            }
//        }
//        vector<Lit> clause;
//        for (int k=0; k<res.size(); k++) {
//            clause.push_back(Lit(res[k]->id, false));
//        }
//        solver->add_clause(clause);
//    }
//    //Eliminate the specification
//    vector<Lit> clause;
//    for (auto k : S->transitions) {
//        clause.push_back(Lit(k->id, true));
//    }
//    for (auto k : S->timeouts) {
//        clause.push_back(Lit(k->id, true));
//    }
//    solver->add_clause(clause);
//}


//sequence verifyCheckingExperiment(SATSolver * &solver,vector<sequence> E, TFSM * S, Product_TFSM * D)
//{
//    computePhiE(solver, E, D);
//    sequence alpha;
//    TFSM * P = D->mutationMachine;
//    while (alpha.size() == 0 && P != NULL) {
//        P = generateSubmachine(solver, D->mutationMachine);
//        if (P != NULL) {
//            Product_TFSM * DP = new Product_TFSM(S, P);
//            if (DP->hasNoSinkState || !DP->isConnected) {
//                computePhiP(solver, P);
//            }
//            else {
//                sequence nullPrefix;
//                set<string> beginningStates;
//                beginningStates.insert(DP->initialState->getKey());
//                /*
//                cout << "P : " << endl;
//                P->print();
//                cout << "DP : " << endl;
//                DP->print();
//                */
//                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, nullPrefix);
//                //No deterministic input sequence
//                if (alpha.size() == 0) {
//                    computePhiP(solver, P);
//                }
//            }
//            delete DP;
//        }
//    }
//    return alpha;
//}

///*
//vector<sequence> generateCheckingExperimentTimeouted(vector<sequence> Einit, TFSM * S, TFSM * M)
//{
//    SATSolver * solver = new SATSolver();

//    solver->new_vars(M->timeouts.size() + M->transitions.size());
//    solver->log_to_file("/tmp/test.txt");
//    computePhiM(solver, S, M);
//    Product_TFSM * D = new Product_TFSM(S, M);
//    vector<sequence> E;
//    vector<sequence> Ecurr = Einit;

//    double elapsed_secs = 0;
//    do {
//        clock_t begin = clock();
//        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
//        sequence alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
//        Ecurr.clear();
//        if (alpha.size() > 0)
//            Ecurr.push_back(alpha);
//        clock_t end = clock();
//        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
//        cout << elapsed_secs << endl;
//    }
//    while (Ecurr.size() != 0 && elapsed_secs < 400);
//    if (elapsed_secs > 400) {
//        E.clear();
//    }
//    return E;
//}
//*/
//vector<sequence> generateCheckingExperiment(vector<sequence> Einit, TFSM * S, TFSM * M)
//{
//    SATSolver * solver = new SATSolver();

//    solver->new_vars(M->timeouts.size() + M->transitions.size());
//    solver->log_to_file("/tmp/test.txt");
//    computePhiM(solver, S, M);

//    Product_TFSM * D = new Product_TFSM(S, M);
//    vector<sequence> E;

//    vector<sequence> Ecurr = Einit;
//    double elapsed_secs = 0;
//    sequence alpha;
//    int cpt = 0;
//    do {
//        clock_t begin = clock();
//        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
//        alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
//        cout << "BIP" << endl;
//        printSequence(alpha);
//        Ecurr.clear();
//        Ecurr.push_back(alpha);
//        clock_t end = clock();
//        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
//        cout << elapsed_secs << endl;
//        cpt++;
//    }
//    while (alpha.size() != 0 && cpt < 50);

//    return E;
//}

///*
//sequence verifyCheckingSequence(SATSolver * &solver,sequence CS, TFSM * S, Product_TFSM * D)
//{
//    vector<sequence> E;
//    E.push_back(CS);
//    set<string> beginningStates;
//    computePhiE(solver, E, D);
//    sequence alpha;
//    TFSM * P = D->mutationMachine;
//    while (alpha.size() == 0 && P != NULL) {
//        P = generateSubmachine(solver, D->mutationMachine);
//        if (P != NULL) {
//            Product_TFSM * DP = new Product_TFSM(S, P);
//            if (DP->hasNoSinkState || !DP->isConnected) {
//                computePhiP(solver, P);
//            }
//            else {
//                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, CS);
//                if (alpha.size() == 0) {
//                    computePhiP(solver, P);
//                }
//            }
//            delete DP;
//        }
//    }
//    return alpha;
//}

//sequence generateCheckingSequenceTimeouted(TFSM * S, TFSM * M)
//{
//    SATSolver * solver = new SATSolver();

//    solver->new_vars(M->timeouts.size() + M->transitions.size());
//    //solver->log_to_file("/tmp/test.txt");
//    computePhiM(solver, S, M);
//    Product_TFSM * D = new Product_TFSM(S, M);
//    sequence CS;
//    sequence alpha;
//    double elapsed_secs = 0;
//    do {
//        clock_t begin = clock();
//        CS.insert(CS.end(), alpha.begin(), alpha.end());
//        alpha = verifyCheckingSequence(solver, CS, S, D);
//        clock_t end = clock();
//        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
//        cout << elapsed_secs << endl;
//    }
//    while (alpha.size() != 0 && elapsed_secs < 400);
//    return CS;
//}

//sequence generateCheckingSequence(TFSM * S, TFSM * M)
//{
//    SATSolver * solver = new SATSolver();

//    solver->new_vars(M->timeouts.size() + M->transitions.size());
//    //solver->log_to_file("/tmp/test.txt");
//    computePhiM(solver, S, M);
//    Product_TFSM * D = new Product_TFSM(S, M);
//    sequence CS;
//    sequence alpha;
//    do {
//        CS.insert(CS.end(), alpha.begin(), alpha.end());
//        alpha = verifyCheckingSequence(solver, CS, S, D);
//    }
//    while (alpha.size() != 0);
//    return CS;
//}
//*/

//unsigned long long int computeNumberOfMutants(TFSM * M) {
//    unsigned long long int res = 1;
//    for (int s : M->states) {
//        for (string i : M->inputs) {
//            if (M->getXi(s).size() > 1)
//                res *= M->getEta(s, i).size();
//        }
//    }
//    for (int s : M->states) {
//        if (M->getXi(s).size() > 1)
//            res *= M->getXi(s).size();
//    }
//    return res;
//}
///*
//void checkingExperimentBenchmarks()
//{
//    set<string> I = {"a", "b"};
//    set<string> O = {"0", "1"};


//    int nbStates [5] = {4, 8, 10, 12, 15};
//    int nbMutants [6] = {20, 50, 100, 200, 300, 400};
//    int maxTime = 5;

//    for (int nbOfBench=0; nbOfBench < 5; nbOfBench++) {
//        ofstream benchFile;

//        for (int j=2; j<3; j++) {
//            //benchFile.open("bench_CS_" + to_string(nbStates[j]) + '_' + to_string(nbOfBench) + ".txt");
//            for (int i=0; i < 3; i++) {
//                benchFile.open("bench_CE_FULL_" + to_string(nbStates[j]) + "_" + to_string(nbMutants[i]) + '_' + to_string(nbOfBench) + ".txt");
//                TFSM * randomSpec = generateRandomSpecification(nbStates[j], maxTime, I, O);
//                TFSM * randomMuta = generateRandomMutationMachine(randomSpec, maxTime*2, nbMutants[i]);
//                vector<sequence> E;
//                vector<sequence> Einit;
//                clock_t begin = clock();
//                E = generateCheckingExperiment(Einit, randomSpec, randomMuta);
//                clock_t end = clock();
//                double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//                benchFile << nbStates[j] << " " << nbMutants[i] << " " << maxTime << " " << elapsed_secs << "s "<< computeNumberOfMutants(randomMuta) << " " << E.size() << "\n";
//                delete randomSpec;
//                delete randomMuta;
//                benchFile.close();
//            }

//        }
//    }
//}
//*/
