#ifndef SUDOKU_TO_CNF_H
#define SUDOKU_TO_CNF_H
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include "dpll.h"
#include "cnfparser.h"

class Sudoku_Solver {
public:
    int table[10][10]{};

    Sudoku_Solver(std::string &input);

    void PrintSoduku() const;

    void Sudoku_to_cnf();

    void getAnswer();
};

Sudoku_Solver Random_Generate(int level);

#endif //SUDOKU_TO_CNF_H
