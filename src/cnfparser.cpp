#include "cnfparser.h"
using namespace std;

extern Head *root;
extern int varNum, clauseNum;
extern bool *value_list;

void CNFParser(const string &filename) {
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
    file >> buf >> varNum >> clauseNum;   // 读取变元数和子句数
    value_list = new bool[varNum + 1];   // 初始化变元值列表
    memset(value_list, 0, sizeof(bool) * (varNum + 1));
    root = new Head;
    root->next = nullptr;
    root->first = nullptr;
    for(int i = 0; i < clauseNum; i++) {
        // 读取子句
        CreateClause();
        int literal;
        file >> literal;
        while(literal) {
            if(literal < 0) AddLiteral(root->next, -literal, true);
            else AddLiteral(root->next, literal, false);
            file >> literal;
        }
    }
    file.close();
}