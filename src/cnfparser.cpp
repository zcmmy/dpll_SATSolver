#include "cnfparser.h"
using namespace std;

extern Head *root;
extern int varNum, clauseNum;
extern bool *value_list;

void CNFParser(const string &filename) {
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
    file >> buf >> varNum >> clauseNum;   // ��ȡ��Ԫ�����Ӿ���
    value_list = new bool[varNum + 1];   // ��ʼ����Ԫֵ�б�
    memset(value_list, 0, sizeof(bool) * (varNum + 1));
    root = new Head;
    root->next = nullptr;
    root->first = nullptr;
    for(int i = 0; i < clauseNum; i++) {
        // ��ȡ�Ӿ�
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