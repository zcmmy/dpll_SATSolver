#include "dpll.h"
using namespace std;

extern Head *root;
extern int varNum, clauseNum, nums, nums2;
extern bool *value_list;

void PrintList() {
    for(auto p = root->next; p; p = p->next) {
        for(auto q = p->first; q; q = q->next) {
            if(q->is_neg) printf("-");
            printf("%d ", q->ord);
        }
        printf("0\n");
    }
}

void DestroyList() {
    // 销毁链表
    auto p = root;
    while(p) {
        auto q = p->first;
        while(q) {
            auto temp = q;
            q = q->next;
            delete temp;
        }
        auto temp = p;
        p = p->next;
        delete temp;
    }
}

void CreateClause() {
    auto *new_Clause = new Head;
    new_Clause->next = root->next;
    new_Clause->first = nullptr;
    root->next = new_Clause;
}

Head* DestroyClause(Head *tar, ClauseStack &cs) {
    // 销毁子句，返回下一子句
    auto p = root;
    while (p->next != tar && p->next != nullptr) {
        p = p->next;
    }
    p->next = tar->next;
    cs.push(tar);   // 存入回收栈
    return p->next;
}

status Destroyliteral(const int ord, ClauseStack &cs, LiteralStack &ls) {
    Head *p = root->next;
    while(p) {
        bool flag = true;
        for(auto q = p->first; q; q = q->next) {
            if(q->ord == ord) {
                if(q->is_neg != value_list[ord]) {
                    // 文字为真，删除子句
                    p = DestroyClause(p, cs);
                    flag = false;
                }
                else {
                    // 文字为假，删除变元
                    auto r = p->first;
                    if(r == q) {
                        p->first = q->next;
                        ls.push(q);
                    }
                    else {
                        while(r->next != q) r = r->next;
                        r->next = q->next;
                        ls.push(q);
                    }
                    if(p->first == nullptr)    // 子句被清空，说明该子句恒为假，返回矛盾
                        return CONFLICT;
                }
                break;
            }
        }
        if(p && flag) p = p->next;
    }
    return NOTHING;
}

void AddLiteral(Head *clause, const int ord, const bool is_neg) {
    auto *new_literal = new Node;
    new_literal->ord = ord;
    new_literal->is_neg = is_neg;
    new_literal->next = clause->first;
    new_literal->belong = clause;
    clause->first = new_literal;
}

status UnitPropagation(ClauseStack &cs, LiteralStack &ls) {
    // 单子句传播，返回是否找到单子句
    auto p = root->next;
    while(p) {
        if(p->first->next == nullptr) {
            // 子句只有一个变元
            int tar = p->first->ord;
            if(p->first->is_neg) value_list[tar] = false;
            else value_list[tar] = true;
            p = DestroyClause(p, cs);
            if(Destroyliteral(tar, cs, ls) == CONFLICT) {
                // cout << "Conflict occured!" << endl;
                return CONFLICT;
            }
            // cout << "UnitPropagation found, " << "target literal is: " << tar << endl;
            // PrintList();
            return FOUND;
        }
        p = p->next;
    }
    // cout << "UnitProgation not found" << endl;
    // PrintList();
    return NOTHING;
}

status PureLiteralelimination(ClauseStack &cs) {
    // 孤立文字消除，返回是否找到孤立文字
    int count[varNum+1] = {0};
    for(auto *p = root->next; p; p = p->next) {
        for(auto *q = p->first; q; q = q->next)
            count[q->ord]++;
    }
    status s = NOTHING;
    auto p = root->next;
    while(p) {
        bool flag = true;
        for(auto *q = p->first; q; q = q->next) {
            if(count[q->ord] == 1) {
                if(q->is_neg) value_list[q->ord] = false;
                else value_list[q->ord] = true;
                p = DestroyClause(p, cs);
                s = FOUND;
                // cout << "PureLiteralElimination found, " << "target literal is: " << q->ord << endl;
                // PrintList();
                flag = false;
                break;
            }
        }
        if(flag) p = p->next;
    }
    // if(s == NOTHING) cout << "PureLiteralElimination not found" << endl;
    return s;
}

int ChooseVariable() {
    // 选择变元，启发函数
    return root->next->first->ord;
}

void Backup(ClauseStack &cs, LiteralStack &ls) {
    // 先对子句栈退栈
    while(!cs.is_empty()) {
        auto p = cs.pop();
        p->next = root->next;
        root->next = p;
    }
    // 再对文字栈退栈
    while(!ls.is_empty()) {
        auto p = ls.pop();
        auto q = (Head*)p->belong;
        p->next = q->first;
        q->first = p;
    }
}

bool DPLL(int v, bool is_pos) {
    // if(nums%100000) cout << flush;
    // printf("Current status: %d\n", ++nums);
    // cout << "set variable " << v << " to " << is_pos << endl;
    bool *new_value_list = new bool[varNum + 1];    // 备份变元值列表
    memcpy(new_value_list, value_list, sizeof(bool) * (varNum + 1));
    auto *cs = new ClauseStack();
    auto *ls = new LiteralStack();
    if(v) {
        value_list[v] = is_pos;
        if(Destroyliteral(v, *cs, *ls) == CONFLICT) {
            // cout << "Conflict occured!" << endl;
            Backup(*cs, *ls);
            delete cs;
            delete ls;
            delete [] new_value_list;
            // printf("recall %d\n", ++nums2);
            return false;
        }
    }
    // 单子句传播
    while(true) {
        status s = UnitPropagation(*cs, *ls);
        if(s == CONFLICT) {
            Backup(*cs, *ls);
            memcpy(value_list, new_value_list, sizeof(bool) * (varNum + 1));
            delete cs;
            delete ls;
            delete [] new_value_list;
            // printf("recall %d\n", ++nums2);
            return false;
        }
        else if(s == NOTHING) break;
    }
    while(PureLiteralelimination(*cs) == FOUND);
    if(!root->next) return true; // 问题解决
    else {
        int var = ChooseVariable();
        // 假设变元为真
        if(DPLL(var, true)) {
            delete cs;
            delete ls;
            delete [] new_value_list;
            // printf("recall %d\n", ++nums2);
            return true;
        }
        // cout << "end of true" << endl;
        // 假设变元为假
        if(DPLL(var, false)) {
            delete cs;
            delete ls;
            delete [] new_value_list;
            // printf("recall %d\n", ++nums2);
            return true;
        }
        // cout << "end of false" << endl;
    }
    // 无解，回溯
    Backup(*cs, *ls);
    memcpy(value_list, new_value_list, sizeof(bool) * (varNum + 1));
    delete cs;
    delete ls;
    delete[] new_value_list;
    // printf("recall %d\n", ++nums2);
    return false;
}

void printRes() {
    for(int i = 1; i <= varNum; i++) {
        if(value_list[i]) cout << 'x' << i << '=' << 1 << endl;
        else cout << 'x' << i << '=' << 0 << endl;
    }
}

void saveRes(const string &filename, bool res, double time) {
    string path = "../src/resfile/" + filename;
    if(filename.find(".res") == string::npos) path += ".res";
    ofstream file(path, ios::out);
    file << 's' << ' ' << res << endl;
    file << 'v' << ' ';
    for(int i = 1; i <= varNum; i++) {
        if(value_list[i] == 0) file << '-';
        file << i << ' ';
    }
    file << endl;
    file << 't' << ' ' << time << endl;
}