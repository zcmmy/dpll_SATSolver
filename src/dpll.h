#ifndef DPLL_H
#define DPLL_H
#include "Global.h"

void DestroyList();
void PrintList();
void CreateClause();
Head* DestroyClause(Head *tar, ClauseStack &cs);
status Destroyliteral(const int ord, ClauseStack &cs, LiteralStack &ls);
void AddLiteral(Head *clause, int ord, const bool is_neg);
bool DPLL(int v = 0, bool is_pos = false);
void printRes();
void saveRes(const std::string &filename, bool res, double time);

#endif //DPLL_H
