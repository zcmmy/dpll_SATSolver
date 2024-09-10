#include "Global.h"
#include "Sudoku_to_cnf.h"

using namespace std;

Head *root = nullptr;
int varNum = 0, clauseNum = 0, nums = 0, nums2 = 0;
bool *value_list = nullptr;

void PrintMenu() {
    cout << "1. SAT" << endl;
    cout << "2. Soduku" << endl;
    cout << "3. Exit" << endl;
}

void PrintSubMenu() {
    cout << "1. 求解" << endl;
}

int main() {
    PrintMenu();
    int opt;
    cin >> opt;
    if(opt == 1) {
        string filename;
        cout << "Please input the filename of the CNF file: ";
        cin >> filename;
        CNFParser(filename);
        clock_t start_time = clock();
        status s = DPLL();
        clock_t end_time = clock();
        double duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        if(s) {
            // 打印解
            cout << "Satified!" << endl;
            // printRes();
            saveRes(filename, true, 1000*duration);
        }
        else {
            cout << "Unsatisfied!" << endl;
            saveRes(filename, false, duration);
        }
        DestroyList();
    }
    else if(opt == 2) {
        char input[100];
        cout << "Please input the soduku string: ";
        cin.get();
        cin.getline(input, 100);
        Generate_table(input);
        cout << "Your Soduku is: " << endl;
        PrintSoduku();
        system("pause");
        system("cls");
        PrintSubMenu();
        cin >> opt;
        if(opt == 1) getAnswer();
    }
    system("pause");
    return 0;
}
