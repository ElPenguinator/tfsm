#include "algorithms_tfsm.h"
#include "../machine/distinguishingautomaton_tfsm.h"
#include "../algorithm/timedinputsequence.h"
using namespace std;
using namespace CMSat;

Algorithms_TFSM::Algorithms_TFSM(bool generateLogs, bool onlyDot) : Algorithms(generateLogs, onlyDot)
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

void Algorithms_TFSM::computePhiE(SATSolver * &solver, vector<Sequence *> E, DistinguishingAutomaton_FSM * D)
{
    int cpt = 1;
    for (auto alpha : E) {
        vector<executingPath> rev = D->revealingPaths(alpha);
        if (generateLogs) {
            savePath(logPath + "paths" + to_string(nbVerifying) + "_" + to_string(cpt) +".paths", rev);
        }
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
        cpt++;
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
            if (eta.size() == 0) {
                cout << "Cluster empty for state " << s << " and entry " << i << endl;
                vector<Lit> clause1;
                clause1.push_back(Lit(1, false));
                solver->add_clause(clause1);
                vector<Lit> clause2;
                clause2.push_back(Lit(1, true));
                solver->add_clause(clause2);
            }
            else {
                set<set<int>>::iterator begin = newEta.begin();
                set<set<int>>::iterator end = newEta.end();

                computePhiMRecur(solver, xi, begin, end, emptyClause);
            }
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

Sequence * Algorithms_TFSM::verifyCheckingExperiment(SATSolver * &solver,vector<Sequence *> E, FSM * S, DistinguishingAutomaton_FSM * D)
{

    computePhiE(solver, E, D);
    Sequence * alpha = new TimedInputSequence();
    FSM * P = D->mutationMachine;
    while (alpha->getSize() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            nbPassedMutants++;
            DistinguishingAutomaton_TFSM * DP = new DistinguishingAutomaton_TFSM(S, P);
            DP->initialize();
            if (generateLogs) {
                saveSVG(logPath + "mutant" + to_string(nbPassedMutants) +".dot", logPath + "mutant" + to_string(nbPassedMutants) +".svg", P->generateDot());
                saveSVG(logPath + "productMutant" + to_string(nbPassedMutants) + ".dot", logPath + "productMutant" + to_string(nbPassedMutants) + ".svg", DP->generateDot());
            }
            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                Sequence * nullPrefix = new TimedInputSequence();
                set<string> beginningStates;
                beginningStates.insert(DP->initialState->getKey());
                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, nullPrefix);
                //No deterministic input sequence
                if (alpha->getSize() == 0) {
                    computePhiP(solver, P);
                }
            }
            delete DP;
        }
    }
    nbVerifying++;
    return alpha;
}

vector<Sequence *> Algorithms_TFSM::generateCheckingExperimentTimeouted(vector<Sequence *> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM * D = new DistinguishingAutomaton_TFSM(S, M);
    D->initialize();
    vector<Sequence *> E;
    vector<Sequence *> Ecurr = Einit;

    double elapsed_secs = 0;
    do {
        clock_t begin = clock();
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        Sequence * alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        if (alpha->getSize() > 0)
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

vector<Sequence *> Algorithms_TFSM::generateCheckingExperiment(vector<Sequence *> Einit, FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();
    solver->log_to_file("/tmp/test.txt");
    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM * D = new DistinguishingAutomaton_TFSM(S, M);
    D->initialize();
    if (generateLogs) {
        saveSVG(logPath + "specification.dot", logPath + "specification.svg", S->generateDot());
        saveSVG(logPath + "mutation.dot", logPath + "mutation.svg", M->generateDot());
        saveSVG(logPath + "distinguishing.dot", logPath + "distinguishing.svg", D->generateDot());
    }
    vector<Sequence *> E;
    vector<Sequence *> Ecurr = Einit;

    do {
        //cout << "?? " <<  Ecurr.size() << endl;
        E.insert(E.end(), Ecurr.begin(), Ecurr.end());
        Sequence * alpha = verifyCheckingExperiment(solver, Ecurr, S, D);
        Ecurr.clear();
        //cout << "Alpha : " << endl;
        //printSequence(alpha);
        if (alpha->getSize() > 0)
            Ecurr.push_back(alpha);
    }
    while (Ecurr.size() != 0);
    return removePrefixes(E);
}

Sequence * Algorithms_TFSM::verifyCheckingSequence(SATSolver * &solver,Sequence * CS, FSM * S, DistinguishingAutomaton_FSM * D)
{
    vector<Sequence *> E;
    E.push_back(CS);
    set<string> beginningStates;
    computePhiE(solver, E, D);
    Sequence * alpha = new TimedInputSequence();
    FSM * P = D->mutationMachine;
    while (alpha->getSize() == 0 && P != NULL) {
        P = generateSubmachine(solver, D->mutationMachine);
        if (P != NULL) {
            DistinguishingAutomaton_TFSM * DP = new DistinguishingAutomaton_TFSM(S, P);
            DP->initialize();
            if (DP->hasNoSinkState || !DP->isConnected) {
                computePhiP(solver, P);
            }
            else {
                alpha = DP->inputSequenceFromAcceptedLanguage(beginningStates, CS);
                if (alpha->getSize() == 0) {
                    computePhiP(solver, P);
                }
            }
            delete DP;
        }
    }
    return alpha;
}

Sequence * Algorithms_TFSM::generateCheckingSequenceTimeouted(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    //solver->log_to_file("/tmp/test.txt");
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM * D = new DistinguishingAutomaton_TFSM(S, M);
    D->initialize();
    Sequence * CS = new TimedInputSequence();
    Sequence * alpha = new TimedInputSequence();
    double elapsed_secs = 0;
    do {
        clock_t begin = clock();
        //CS.insert(CS.end(), alpha.begin(), alpha.end());
        dynamic_cast<TimedInputSequence *>(CS)->addElements(dynamic_cast<TimedInputSequence *>(alpha)->content);
        alpha = verifyCheckingSequence(solver, CS, S, D);
        clock_t end = clock();
        elapsed_secs += double(end - begin) / CLOCKS_PER_SEC;
    }
    while (alpha->getSize() != 0 && elapsed_secs < 3600);
    return CS;
}

Sequence * Algorithms_TFSM::generateCheckingSequence(FSM * S, FSM * M)
{
    SATSolver * solver = new SATSolver();

    solver->new_vars(M->getTransitionSize());
    computePhiM(solver, S, M);
    DistinguishingAutomaton_TFSM * D = new DistinguishingAutomaton_TFSM(S, M);
    D->initialize();
    Sequence * CS = new TimedInputSequence();
    Sequence * alpha = new TimedInputSequence();
    do {
        //CS.insert(CS.end(), alpha.begin(), alpha.end());
        dynamic_cast<TimedInputSequence *>(CS)->addElements(dynamic_cast<TimedInputSequence *>(alpha)->content);
        alpha = verifyCheckingSequence(solver, CS, S, D);
    }
    while (alpha->getSize() != 0);
    return CS;
}

void Algorithms_TFSM::checkingExperimentBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
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
    //                vector<Sequence *> E;
    //                vector<Sequence *> Einit;
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


void Algorithms_TFSM::checkingSequenceBenchmarks(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout)
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
    //                Sequence * CS;
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
vector<Sequence *> Algorithms_TFSM::removePrefixes(vector<Sequence *> E)
{
    vector<Sequence *> newE;
    for (Sequence * s1 : E) {
        bool found = false;
        //cout << "S1 : ";
        //printSequence(s1);

        vector<Sequence *> copyNewE(newE);
        int j = 0;
        for (Sequence * s2 : copyNewE) {
            //cout << "S2 : ";
            //printSequence(s2);
            int i=0;
            while (i < min(s1->getSize(), s2->getSize())
                   && dynamic_cast<TimedInputSequence *>(s1)->getElement(i).first == dynamic_cast<TimedInputSequence *>(s2)->getElement(i).first
                   && dynamic_cast<TimedInputSequence *>(s1)->getElement(i).second == dynamic_cast<TimedInputSequence *>(s2)->getElement(i).second) {
                i++;
            }
            if (i == min(s1->getSize(), s2->getSize())) {
                if (s1->getSize() <= s2->getSize()) {
                    //printSequence(s1);
                    //cout << "Is prefix of" << endl;
                    //printSequence(s2);
                    found = true;
                }
                else {
                    //printSequence(s2);
                    //cout << "Is prefix of" << endl;
                    //printSequence(s1);
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

FSM * Algorithms_TFSM::completeMutation(FSM * M)
{
    TFSM * newM = new TFSM(M->states, M->initialState, M->inputs, M->outputs, M->getTransitions(), M->getTimeouts());
    int lastID = newM->getTransitionSize();
    for (int src : newM->states) {
        for (string i : newM->inputs) {
            Bracket newLeft = Bracket::Square;
            int newtMin = 0;
            int newtMax = 0;
            Bracket newRight = Bracket::Curly;
            vector<Guard> availableGuards;
            for (IOTransition * t : newM->getXi(src, i)) {
                availableGuards.push_back(t->getGuard());
            }
            int nbGuards = availableGuards.size();
            Guard lastGuard("(", inf, inf, ")");
            for (int index=0; index<nbGuards+1; index++) {
                Guard minGuard("(", inf, inf, ")");
                bool isOneAvailable = false;
                for (Guard g : availableGuards) {
                    if (g.tmin < minGuard.tmin && !g.equals(lastGuard)) {
                        minGuard = g;
                        isOneAvailable = true;
                    }
                }
                if (!isOneAvailable && lastGuard.tmax != inf) {
                    if (lastGuard.right == Bracket::Curly)
                        newLeft == Bracket::Square;
                    else
                        newLeft == Bracket::Curly;
                    Guard newGuard(newLeft, lastGuard.tmax, inf, Bracket::Curly);
                    vector<IOTransition *> newLambda;
                    for (string o : newM->outputs) {
                        for (int tgt : newM->states) {
                            newLambda.push_back(new GuardedTransition(src, i, newGuard, o, tgt, lastID++));
                        }
                    }
                    newM->addTransitions(newLambda, true);
                }
                else {
                    lastGuard = minGuard;
                    if (minGuard.tmin > newtMax || (minGuard.tmin == newtMax && newRight == minGuard.left == Bracket::Curly)) {
                        if (minGuard.left == Bracket::Curly)
                            newRight == Bracket::Square;
                        else
                            newRight == Bracket::Curly;

                        Guard newGuard(newLeft, newtMin, minGuard.tmin, newRight);
                        vector<IOTransition *> newLambda;
                        for (string o : newM->outputs) {
                            for (int tgt : newM->states) {
                                newLambda.push_back(new GuardedTransition(src, i, newGuard, o, tgt, lastID++));
                            }
                        }
                        newM->addTransitions(newLambda, true);
                    }
                    if (minGuard.right == Bracket::Curly)
                        newLeft == Bracket::Square;
                    else
                        newLeft == Bracket::Curly;
                    newtMin = minGuard.tmax;
                    newtMax = minGuard.tmax;
                    newRight = Bracket::Curly;
                }
            }
        }
    }
    return newM;
}

FSM * Algorithms_TFSM::generateRandomSpecification(int nbOfStates, int maxTime, set<string> I, set<string> O)
{
    set<int> S;
    int s0 = 0;
    vector<IOTransition *> lambda;
    vector<TimeoutTransition *> delta;
    int transitionId;
    srand (time(NULL));
    bool isConnected = false;

    TFSM * res;

    while (!isConnected) {
        transitionId = 0;
        S.clear();
        lambda.clear();
        delta.clear();
        for (int s=0; s<nbOfStates; s++) {
            S.insert(s);

            int randomT = floor(rand() % maxTime);
            int randomTgt = floor(rand() % nbOfStates);
            if (randomT == 0) {
                randomT = inf;
                randomTgt = s;
            }
            delta.push_back(new TimeoutTransition(s, randomT, randomTgt, transitionId));
            transitionId++;
            if (randomT == inf)
                randomT = maxTime;
            for (string i : I) {
                int nbPartition = 1;
                if (randomT > 1)
                    nbPartition = 1 + floor(rand() % (randomT-1));
                int lastMax = 0;
                cout << "nbPartition : " << nbPartition << " of : " << randomT << endl;
                for (int j=0; j<nbPartition; j++) {
                    cout << "!!" << endl;
                    int tMin = round(lastMax);
                    int tMax = round((randomT/nbPartition)*(j+1));
                    if (tMax == tMin)
                        tMax++;
                    if (j == nbPartition-1)
                        tMax = inf;
                    lastMax = tMax;
                    string randomO = getRandomStringFromSet(O);
                    int randomTgt = floor(rand() % nbOfStates);
                    Guard g("[", tMin, tMax, ")");
                    cout << "- " << g.toString() << endl;
                    lambda.push_back(new GuardedTransition(s, i, g, randomO, randomTgt, transitionId));
                    transitionId++;
                }
            }
        }
        res = new TFSM(S, s0, I, O, lambda, delta);
        /*
        isConnected = isTFSMConnected(res);
        if (!isConnected)
            delete res;
            */
    }
    return res;
}

FSM * Algorithms_TFSM::generateRandomMutation(FSM * S, int maxTime, int numberOfMutations)
{
    set<int> States(S->states);
    int s0 = S->initialState;
    set<string> I(S->inputs);
    set<string> O(S->outputs);
    vector<IOTransition *> lambda(S->transitions);
    vector<TimeoutTransition *> delta(S->getTimeouts());
    TFSM * M = new TFSM(States, s0, I, O, lambda, delta);

    srand (time(NULL));
    vector<IOTransition *> newLambda;
    vector<TimeoutTransition *> newDelta;
    int i = 0 + (S->transitions.size() + S->getTimeouts().size());

    while (i<numberOfMutations + (S->transitions.size() + S->getTimeouts().size())) {
        bool alreadyExisting = true;
        bool createTimeout = rand()%2;
        int randomSrc = floor(rand() % S->states.size());
        int randomTgt = floor(rand() % S->states.size());
        if (createTimeout) {
            int randomT = floor(rand() % maxTime);
            if (randomT == 0) {
                randomT = inf;
                randomTgt = randomSrc;
            }
            TimeoutTransition * newTimeout = new TimeoutTransition(randomSrc,randomT, randomTgt, i);
            if (!timeoutAlreadyExist(newTimeout, delta, newDelta)) {
                newDelta.push_back(newTimeout);
                i++;
            }
        }
        else {
            int nbPartition = 1 + floor(rand() % (maxTime-1));
            int lastMax = 0;
            cout << "nbPartition : " << nbPartition << " of : " << maxTime << endl;
            string randomI = getRandomStringFromSet(I);
            for (int j=0; j<nbPartition; j++) {
                cout << "!!" << endl;
                int tMin = round(lastMax);
                int tMax = round((maxTime/nbPartition)*(j+1));
                if (tMax == tMin)
                    tMax++;
                if (j == nbPartition-1)
                    tMax = inf;
                lastMax = tMax;
                int randomTgt = floor(rand() % S->states.size());
                string randomO = getRandomStringFromSet(O);
                Guard g("[", tMin, tMax, ")");
                cout << "- " << g.toString() << endl;
                GuardedTransition * newTransition = new GuardedTransition(randomSrc, randomI, g, randomO, randomTgt, i);
                lambda.push_back(newTransition);

                if (!guardedTransitionAlreadyExist(newTransition, lambda, newLambda)) {
                    newLambda.push_back(newTransition);
                    i++;
                }

            }
        }
    }
    cout << "New Transitions : {";
    for (IOTransition * t : newLambda) {
        cout << "(" << t->src << "," << t->i << "," << t->getGuard().toString() <<"," << t->o << "," << t->tgt << ") : " << t->id << "," << endl;
    }
    cout << "}" << endl;
    cout << "New Timeouts : {";
    for (auto t : newDelta) {
        if (t->t != inf) {
            cout << "(" << t->src << "," << t->t << "," << t->tgt << ") : " << t->id << "," << endl;
        }
        else {
            cout << "(" << t->src << "," << "INF" << "," << t->tgt << ") : " << t->id << "," << endl;
        }
    }
    cout << "}" << endl;
    M->addTransitions(newLambda, true);
    M->addTimeouts(newDelta, true);
    return M;
}

//Wrong
InfInt Algorithms_TFSM::computeNumberOfMutants(FSM * M) {
    InfInt res = 1;
    for (int s : M->states) {
        for (string i : M->inputs) {
            if (M->getXi(s, i).size() > 1)
                res *= M->getXi(s, i).size();
        }
    }
    for (int s : M->states) {
        if (M->getXi(s).size() > 1)
            res *= M->getXi(s).size();
    }
    return res;
}
