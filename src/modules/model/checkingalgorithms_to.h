#ifndef CHECKINGALGORITHMS_TO_H
#define CHECKINGALGORITHMS_TO_H
#include <iostream>
#include "machine/tfsm_to.h"
#include "machine/fsm.h"
#include "structs.h"
#include "machine/product_tfsm_to.h"
#include "machine/product_fsm_full.h"
#include "cryptominisat5/cryptominisat.h"
#include "tools.h"
#include <ctime>
#include <fstream>
#include <math.h>

TFSM_TO * generateSubmachine(CMSat::SATSolver * &solver, TFSM_TO * M);

void computePhiP(CMSat::SATSolver * &solver, TFSM_TO * P);

void computePhiE(CMSat::SATSolver * &solver, std::vector<sequence> E, Product_TFSM_TO * D);

void computePhiM(CMSat::SATSolver * &solver, TFSM_TO * S, TFSM_TO * M);

sequence verifyCheckingExperiment(CMSat::SATSolver * &solver,std::vector<sequence> E, TFSM_TO * S, Product_TFSM_TO * D);

std::vector<sequence> generateCheckingExperimentTimeouted(std::vector<sequence> Einit, TFSM_TO * S, TFSM_TO * M);

std::vector<sequence> generateCheckingExperiment(std::vector<sequence> Einit, TFSM_TO * S, TFSM_TO * M);

sequence verifyCheckingSequence(CMSat::SATSolver * &solver,sequence CS, TFSM_TO * S, Product_TFSM_TO * D);

sequence generateCheckingSequenceTimeouted(TFSM_TO * S, TFSM_TO * M);

sequence generateCheckingSequence(TFSM_TO * S, TFSM_TO * M);

#endif // CHECKINGALGORITHMS_TO_H
