#ifndef TOOL_H
#define TOOL_H
#include "structs.h"
#include "cryptominisat5/cryptominisat.h"

void printPath(path p);
void printSequence(sequence s);
void printClause(std::vector<CMSat::Lit> clause);
TFSM_TO * generateRandomSpecification(int nbOfStates, int maxTime, std::set<std::string> I, std::set<std::string> O);
TFSM_TO * generateRandomMutationMachine(TFSM_TO * S, int maxTime, int numberOfMutations);
#endif // TOOL_H
