#include "Sudoku_to_cnf.h"
using namespace std;

Sudoku_Solver::Sudoku_Solver(string &input) {
    int col = 1, row = 1;
    memset(table, 0, sizeof(table));
    for(char c : input) {
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

void Sudoku_Solver::PrintSoduku() const{
    for(int i = 1; i <= 9; i++) {
        cout << "| ";
        for(int j = 1; j <= 9; j++) {
            cout << table[i][j] << " | ";
        }
        cout << endl;
    }
}


void Sudoku_Solver::Sudoku_to_cnf() {
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

void Sudoku_Solver::getAnswer() {
    Sudoku_to_cnf();
    auto solver = CNFParser("sudoku.cnf");
    bool s = solver->DPLL();
    for(int i = 1; i <= 9; i++) {
        for(int j = 1; j <= 9; j++) {
            for(int k = 1; k <= 9; k++) {
                if(solver->value_list[(i-1)*81 + (j-1)*9 + k]) {
                    table[i][j] = k;
                }
            }
        }
    }
    solver->saveRes("sudoku", s, 0);
}

Sudoku_Solver Random_Generate(int level) {
    random_device rd;

    // 使用随机设备创建一个引擎
    mt19937 gen(rd());

    // 创建一个均匀分布范围为1到100的整数分布
    uniform_int_distribution<> dis(0, 3);
    int index = dis(gen);
    string input;
    switch(index) {
        case 0:
            input = "921564387758329461346187259195738642837642915462951738273415896619873524584296173";
            break;
        case 1:
            input = "731856924462931578958472631129368745384517296675249183813725469296184357547693812";
            break;
        case 2:
            input = "546129378237485691189673452925831746413267985768594213892756134671348529354912867";
            break;
        case 3:
            input = "742865319518973246396412587831296754274581693965734821653148972427659138189327465";
            break;
        default:
            input = "123456789456789123789123456234567891567891234891234567345678912678912345912345678";
    }
    int cnt = 0, tar[5] = {0, 30, 40, 50, 60};
    uniform_int_distribution<> dis2(1, 9);
    while(cnt < tar[level]) {
        int x = dis2(gen);
        int y = dis2(gen);
        if(input[(x-1)*9+y-1] != '0') {
            input[(x-1)*9+y-1] = '0';
            cnt++;
        }
    }
    return {input};
}