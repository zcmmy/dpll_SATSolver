#include "cnfparser.h"
using namespace std;

SAT_Solver* CNFParser(const string &filename) {
    string path = "../src/cnffile/" + filename;
    if(filename.find(".cnf") == string::npos) path += ".cnf";
    ifstream file(path, ios::in);   // 打开文件
    if (!file.is_open()) {
        cout << "File not found!" << endl;
        exit(1);
    }
    char buf[100];
    char ch;
    file.get(ch);
    while(ch != 'p') {  // 跳过注释
        file.getline(buf, 100);
        file.get(ch);
    }
    int varNum, clauseNum;
    file >> buf >> varNum >> clauseNum;   // 读取变元数和子句数
    auto solver = new SAT_Solver(varNum, clauseNum);
    for(int i = 0; i < clauseNum; i++) {
        // 读取子句
        solver->CreateClause();
        int literal;
        file >> literal;
        while(literal) {
            if(literal < 0) solver->AddLiteral(solver->root->next, -literal, true);
            else solver->AddLiteral(solver->root->next, literal, false);
            file >> literal;
        }
    }
    file.close();
    return solver;
}