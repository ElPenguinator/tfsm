#ifndef STRUCTS_H
#define STRUCTS_H
#include <vector>
#include <utility>
#include <limits>
#include <string>
#include "machine/guard.h"

typedef std::pair<std::string, double> ts;
typedef std::pair<std::string, Guard> its;
//typedef std::vector<ts> sequence;
typedef std::vector<int> executingPath;
const int inf = std::numeric_limits<double>::infinity();
#endif // STRUCTS_H
