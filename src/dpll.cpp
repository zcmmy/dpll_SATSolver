#include "dpll.h"
using namespace std;

SAT_Solver::SAT_Solver(int varNum, int clauseNum):varNum(varNum), clauseNum(clauseNum) {
    root = new Head;
    root->next = nullptr;
    root->first = nullptr;

    value_list = new bool[varNum + 1];
    memset(value_list, 0, sizeof(bool) * (varNum + 1));
}

SAT_Solver::~SAT_Solver() {
    DestroyList();
    delete [] value_list;
}


void SAT_Solver::PrintList() const {
    for(auto p = root->next; p; p = p->next) {
        for(auto q = p->first; q; q = q->next) {
            if(q->is_neg) printf("-");
            printf("%d ", q->ord);
        }
        printf("0\n");
    }
}

void SAT_Solver::DestroyList() const {
    // ��������
    auto p = root->next;
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
    delete root;
}

void SAT_Solver::CreateClause() {
    auto *new_Clause = new Head;
    new_Clause->next = root->next;
    new_Clause->first = nullptr;
    root->next = new_Clause;
}

Head* SAT_Solver::DestroyClause(Head *tar, ClauseStack &cs) {
    // �����Ӿ䣬������һ�Ӿ�
    auto p = root;
    while (p->next != tar && p->next != nullptr) {
        p = p->next;
    }
    p->next = tar->next;
    cs.push(tar);   // �������ջ
    return p->next;
}

status SAT_Solver::Destroyliteral(const int ord, ClauseStack &cs, LiteralStack &ls) {
    Head *p = root->next;
    while(p) {
        bool flag = true;
        for(auto q = p->first; q; q = q->next) {
            if(q->ord == ord) {
                if(q->is_neg != value_list[ord]) {
                    // ����Ϊ�棬ɾ���Ӿ�
                    p = DestroyClause(p, cs);
                    flag = false;
                }
                else {
                    // ����Ϊ�٣�ɾ����Ԫ
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
                    if(p->first == nullptr)    // �Ӿ䱻��գ�˵�����Ӿ��Ϊ�٣�����ì��
                        return CONFLICT;
                }
                break;
            }
        }
        if(p && flag) p = p->next;
    }
    return NOTHING;
}

void SAT_Solver::AddLiteral(Head *clause, const int ord, const bool is_neg) {
    auto *new_literal = new Node;
    new_literal->ord = ord;
    new_literal->is_neg = is_neg;
    new_literal->next = clause->first;
    new_literal->belong = clause;
    clause->first = new_literal;
}

status SAT_Solver::UnitPropagation(ClauseStack &cs, LiteralStack &ls) {
    // ���Ӿ䴫���������Ƿ��ҵ����Ӿ�
    auto p = root->next;
    while(p) {
        if(p->first->next == nullptr) {
            // �Ӿ�ֻ��һ����Ԫ
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

status SAT_Solver::PureLiteralelimination(ClauseStack &cs) {
    // �������������������Ƿ��ҵ���������
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

int SAT_Solver::ChooseVariable() const {
    // ѡ���Ԫ����������
    return root->next->first->ord;
}

void SAT_Solver::Backup(ClauseStack &cs, LiteralStack &ls) {
    // �ȶ��Ӿ�ջ��ջ
    while(!cs.is_empty()) {
        auto p = cs.pop();
        p->next = root->next;
        root->next = p;
    }
    // �ٶ�����ջ��ջ
    while(!ls.is_empty()) {
        auto p = ls.pop();
        auto q = (Head*)p->belong;
        p->next = q->first;
        q->first = p;
    }
}

bool SAT_Solver::DPLL(int v, bool is_pos) {
    // if(nums%100000) cout << flush;
    // printf("Current status: %d\n", ++nums);
    // cout << "set variable " << v << " to " << is_pos << endl;
    bool *new_value_list = new bool[varNum + 1];    // ���ݱ�Ԫֵ�б�
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
    // ���Ӿ䴫��
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
    if(!root->next) return true; // ������
    else {
        int var = ChooseVariable();
        // �����ԪΪ��
        if(DPLL(var, true)) {
            delete cs;
            delete ls;
            delete [] new_value_list;
            // printf("recall %d\n", ++nums2);
            return true;
        }
        // cout << "end of true" << endl;
        // �����ԪΪ��
        if(DPLL(var, false)) {
            delete cs;
            delete ls;
            delete [] new_value_list;
            // printf("recall %d\n", ++nums2);
            return true;
        }
        // cout << "end of false" << endl;
    }
    // �޽⣬����
    Backup(*cs, *ls);
    memcpy(value_list, new_value_list, sizeof(bool) * (varNum + 1));
    delete cs;
    delete ls;
    delete[] new_value_list;
    // printf("recall %d\n", ++nums2);
    return false;
}

void SAT_Solver::printRes() const {
    for(int i = 1; i <= varNum; i++) {
        if(value_list[i]) cout << 'x' << i << '=' << 1 << endl;
        else cout << 'x' << i << '=' << 0 << endl;
    }
}

void SAT_Solver::saveRes(const string &filename, bool res, double time) const {
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