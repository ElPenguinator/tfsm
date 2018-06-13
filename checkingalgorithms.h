#ifndef CHECKINGALGORITHMS_H
#define CHECKINGALGORITHMS_H
#include <iostream>
#include "tfsm.h"
#include "fsm.h"
#include "structs.h"
#include "product_tfsm.h"
#include "product_fsm_full.h"
#include "cryptominisat5/cryptominisat.h"
#include "tools.h"
#include <ctime>
#include <fstream>
#include <math.h>

TFSM * generateSubmachine(CMSat::SATSolver * &solver, TFSM * M);

void computePhiP(CMSat::SATSolver * &solver, TFSM * P);

void computePhiE(CMSat::SATSolver * &solver, std::vector<sequence> E, Product_TFSM * D);

void computePhiM(CMSat::SATSolver * &solver, TFSM * S, TFSM * M);

sequence verifyCheckingExperiment(CMSat::SATSolver * &solver,std::vector<sequence> E, TFSM * S, Product_TFSM * D);

std::vector<sequence> generateCheckingExperimentTimeouted(std::vector<sequence> Einit, TFSM * S, TFSM * M);

std::vector<sequence> generateCheckingExperiment(std::vector<sequence> Einit, TFSM * S, TFSM * M);

sequence verifyCheckingSequence(CMSat::SATSolver * &solver,sequence CS, TFSM * S, Product_TFSM * D);

sequence generateCheckingSequenceTimeouted(TFSM * S, TFSM * M);

sequence generateCheckingSequence(TFSM * S, TFSM * M);

void checkingExperimentBenchmarks();

#endif // CHECKINGALGORITHMS_H
