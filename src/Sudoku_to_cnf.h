#ifndef SUDOKU_TO_CNF_H
#define SUDOKU_TO_CNF_H
#include <iostream>
#include <fstream>
#include "dpll.h"
#include "cnfparser.h"

class Sudoku_Solver {
private:
    int table[10][10];

public:
    Sudoku_Solver(char *input);

    void PrintSoduku();

    void Sudoku_to_cnf();

    void getAnswer();
};

#endif //SUDOKU_TO_CNF_H
