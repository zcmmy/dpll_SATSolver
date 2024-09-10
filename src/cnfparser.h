#ifndef CNFPARSER_H
#define CNFPARSER_H
#include "Global.h"
#include "dpll.h"

SAT_Solver* CNFParser(const std::string &filename);

#endif //CNFPARSER_H
