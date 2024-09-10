#include "Sudoku_to_cnf.h"
using namespace std;
int table[10][10];
extern bool *value_list;
extern int varNum;

void Generate_table(char *input) {
    int col = 1, row = 1;
    for(int i = 0; i < strlen(input); i++) {
        char c = input[i];
        if(c >= '0' && c <= '9') {
            table[col][row] = c - '0';
            row++;
            if(row == 10) {
                col++;
                row = 1;
            }
        }
    }
}

void PrintSoduku() {
    for(int i = 1; i <= 9; i++) {
        cout << "| ";
        for(int j = 1; j <= 9; j++) {
            cout << table[i][j] << " | ";
        }
        cout << endl;
    }
}


void Sudoku_to_cnf() {
    ofstream out("../src/cnffile/sudoku.cnf");

    int numClause = 0;

    // 写入CNF文件头部，子句数为五位数，进行占位
    out << "p cnf 729 00000" << endl;

    // 1. 固定已经填入的数字
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            if (table[i][j] != 0) {
                out << (i - 1) * 81 + (j - 1) * 9 + table[i][j] << " 0" << endl;
                numClause++;
            }
        }
    }

    // 2. 每个格子里必须有一个数字
    for (int i = 1; i <= 9; i++) {
        for (int j = 1; j <= 9; j++) {
            for (int k = 1; k <= 9; k++) {
                out << (i - 1) * 81 + (j - 1) * 9 + k << " ";
            }
            out << "0" << endl;
            numClause++;
        }
    }

    // 3. 行约束：每行每个数字只能出现一次
    for (int i = 1; i <= 9; i++) {
        for (int k = 1; k <= 9; k++) {
            for (int j1 = 1; j1 <= 8; j1++) {
                for (int j2 = j1 + 1; j2 <= 9; j2++) {
                    out << -((i - 1) * 81 + (j1 - 1) * 9 + k) << " "
                        << -((i - 1) * 81 + (j2 - 1) * 9 + k) << " 0" << endl;
                    numClause++;
                }
            }
        }
    }

    // 4. 列约束：每列每个数字只能出现一次
    for (int j = 1; j <= 9; j++) {
        for (int k = 1; k <= 9; k++) {
            for (int i1 = 1; i1 <= 8; i1++) {
                for (int i2 = i1 + 1; i2 <= 9; i2++) {
                    out << -((i1 - 1) * 81 + (j - 1) * 9 + k) << " "
                        << -((i2 - 1) * 81 + (j - 1) * 9 + k) << " 0" << endl;
                    numClause++;
                }
            }
        }
    }

    // 5. 宫内约束：每个3x3宫内每个数字只能出现一次
    for (int blockRow = 0; blockRow <= 2; blockRow++) {
        for (int blockCol = 0; blockCol <= 2; blockCol++) {
            for (int k = 1; k <= 9; k++) {
                for (int i1 = 1; i1 <= 3; i1++) {
                    for (int j1 = 1; j1 <= 3; j1++) {
                        for (int i2 = 1; i2 <= 3; i2++) {
                            for (int j2 = 1; j2 <= 3; j2++) {
                                if (i1 != i2 || j1 != j2) {
                                    out << -((blockRow * 3 + i1 - 1) * 81 + (blockCol * 3 + j1 - 1) * 9 + k) << " "
                                        << -((blockRow * 3 + i2 - 1) * 81 + (blockCol * 3 + j2 - 1) * 9 + k) << " 0" << endl;
                                    numClause++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 6. 对角线约束（可选）：在每条对角线中每个数字只能出现一次
    for (int k = 1; k <= 9; k++) {
        // 主对角线
        for (int i = 1; i <= 9; i++) {
            out << (i - 1) * 81 + (i - 1) * 9 + k << " ";
        }
        out << "0" << endl;
        numClause++;

        // 副对角线
        for (int i = 1; i <= 9; i++) {
            out << (i - 1) * 81 + (9 - i) * 9 + k << " ";
        }
        out << "0" << endl;
        numClause++;
    }

    // 修正文件头部的子句数量
    out.seekp(0, ios::beg);
    out << "p cnf 729 " << numClause << endl;

    // 关闭文件
    out.close();
}

void getAnswer() {
    Sudoku_to_cnf();
    CNFParser("sudoku.cnf");
    bool s = DPLL();
    for(int i = 1; i <= 9; i++) {
        for(int j = 1; j <= 9; j++) {
            for(int k = 1; k <= 9; k++) {
                if(value_list[(i-1)*81 + (j-1)*9 + k]) {
                    table[i][j] = k;
                }
            }
        }
    }
    DestroyList();
    saveRes("sudoku", s, 0);
    PrintSoduku();
}