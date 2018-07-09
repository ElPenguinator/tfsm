#ifndef TOOL_H
#define TOOL_H
#include "structs.h"
#include "cryptominisat5/cryptominisat.h"

void printPath(path p);
void printSequence(sequence s);
void printClause(std::vector<CMSat::Lit> clause);

TFSM * generateRandomSpecification(int nbOfStates, int maxTime, std::set<std::string> I, std::set<std::string> O);
TFSM * generateRandomMutationMachine(TFSM * S, int maxTime, int numberOfMutations);


bool transitionAlreadyExist(Transition newTransition, std::vector<Transition> lambda, std::vector<Transition> newLambda);
bool timeoutAlreadyExist(Timeout newTimeout, std::vector<Timeout> delta, std::vector<Timeout> newDelta);
std::string getRandomStringFromSet(std::set<std::string> samples);
TFSM_TO * generateRandomSpecification_TO(int nbOfStates, int maxTime, std::set<std::string> I, std::set<std::string> O);
TFSM_TO * generateRandomMutationMachine_TO(TFSM_TO * S, int maxTime, int numberOfMutations);
TFSM_TO * generateChaosMachine(TFSM_TO * S, int maxTime);
#endif // TOOL_H
