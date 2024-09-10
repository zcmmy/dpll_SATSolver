#ifndef DPLL_H
#define DPLL_H
#include "Global.h"

class SAT_Solver {
public:
    Head *root = nullptr;
    int varNum = 0, clauseNum = 0;
    bool *value_list = nullptr;

    SAT_Solver(int varNum, int clauseNum);

    void DestroyList() const;

    void PrintList() const;

    void CreateClause();

    Head* DestroyClause(Head *tar, ClauseStack &cs);

    status Destroyliteral(int ord, ClauseStack &cs, LiteralStack &ls);

    void AddLiteral(Head *clause, int ord, bool is_neg);

    status UnitPropagation(ClauseStack &cs, LiteralStack &ls);

    status PureLiteralelimination(ClauseStack &cs);

    int ChooseVariable() const;

    void Backup(ClauseStack &cs, LiteralStack &ls);

    bool DPLL(int v = 0, bool is_pos = false);

    void printRes() const;

    void saveRes(const std::string &filename, bool res, double time) const;

    ~SAT_Solver();
};

#endif //DPLL_H
