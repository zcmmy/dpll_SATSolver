#include "cnfparser.h"
using namespace std;

SAT_Solver* CNFParser(const string &filename) {
    string path = "../src/cnffile/" + filename;
    if(filename.find(".cnf") == string::npos) path += ".cnf";
    ifstream file(path, ios::in);   // ���ļ�
    if (!file.is_open()) {
        cout << "File not found!" << endl;
        exit(1);
    }
    char buf[100];
    char ch;
    file.get(ch);
    while(ch != 'p') {  // ����ע��
        file.getline(buf, 100);
        file.get(ch);
    }
    int varNum, clauseNum;
    file >> buf >> varNum >> clauseNum;   // ��ȡ��Ԫ�����Ӿ���
    auto solver = new SAT_Solver(varNum, clauseNum);
    for(int i = 0; i < clauseNum; i++) {
        // ��ȡ�Ӿ�
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