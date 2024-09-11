#include "Global.h"
#include "Sudoku_to_cnf.h"
#include <opencv2/opencv.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#include <windows.h>
#define WINDOW_NAME "Sudoku"
#define CELL_SIZE 50 // 每个单元格的大小
#define GRID_SIZE 9  // 数独是 9x9 的表格
using namespace std;

int selectedRow = -1;  // 当前选中的单元格行
int selectedCol = -1;  // 当前选中的单元格列
int table[10][10];
int initial_table[10][10];

void PrintMenu() {
    printf("------------Welcome to SAT Solver------------\n");
    printf("       1. SAT                2. Soduku       \n");
    printf("       3. Cls                4. Exit         \n");
    printf("---------------------------------------------\n");
}

void drawGrid(cv::Mat &frame) {
    for (int i = 1; i <= GRID_SIZE; ++i) {
        for (int j = 1; j <= GRID_SIZE; ++j) {
            int x = (j-1) * CELL_SIZE;
            int y = (i-1) * CELL_SIZE;
            // 绘制单元格矩形
            cv::rectangle(frame, cv::Point(x, y), cv::Point(x + CELL_SIZE, y + CELL_SIZE), cv::Scalar(200, 200, 200), 1);

            // 在单元格中显示数字
            if(table[i][j] != 0 && initial_table[i][j] == 0) {
                cvui::printf(frame, x + 20, y + 20, 0.5, 0xFFFFFF, "%d", table[i][j]);
            }
            else if(table[i][j] != 0 && initial_table[i][j] != 0) {
                cvui::printf(frame, x + 20, y + 20, 0.5, 0x00FFFF, "%d", table[i][j]);
            }
        }
    }
    // 绘制九宫格间的粗线
    for(int i = 1; i <= 2; i++) {
        cv::line(frame, cv::Point(i * CELL_SIZE * 3, 0), cv::Point(i * CELL_SIZE * 3, CELL_SIZE * 9), cv::Scalar(180, 255, 0), 2);
        cv::line(frame, cv::Point(0, i * CELL_SIZE * 3), cv::Point(CELL_SIZE * 9, i * CELL_SIZE * 3), cv::Scalar(180, 255, 0), 2);
    }
}

void handleMouseClick(int x, int y) {
    if(y < GRID_SIZE * CELL_SIZE && x < GRID_SIZE * CELL_SIZE) {
        selectedRow = y / CELL_SIZE + 1;
        selectedCol = x / CELL_SIZE + 1;
    }
}

int main() {
    while(true) {
        int opt = -1;
        PrintMenu();
        cout << "Please input your option: ";
        cin >> opt;
        while(opt == -1) {
            cout <<  "Invalid input!" << endl;
            opt = -1;
            cout << "Please input your option: ";
            cin >> opt;
        }
        if(opt == 4) break;
        if(opt == 3) system("cls");
        else if(opt == 1) {
            string filename;
            cout << "Please input the filename of the CNF file: ";
            cin >> filename;
            auto solver = CNFParser(filename);
            clock_t start_time = clock();
            status s = solver->DPLL();
            clock_t end_time = clock();
            double duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;
            if(s) {
                // 打印解
                cout << "Satified!" << endl;
                // printRes();
                solver->saveRes(filename, true, 1000*duration);
            }
            else {
                cout << "Unsatisfied!" << endl;
                solver->saveRes(filename, false, duration);
            }
            delete solver;
        }
        else if(opt == 2) {
            cout << "1: Input your soduku" << endl;
            cout << "2: Random generate" << endl;
            cout << "Your option: ";
            cin >> opt;
            if(opt == 1) {
                string input;
                cout << "Please input the soduku string: ";
                cin.get();
                getline(cin, input);
                auto solver = Sudoku_Solver(input);
                memcpy(table, solver.table, sizeof(table));
                memcpy(initial_table, solver.table, sizeof(table));
                solver.getAnswer();
                cv::Mat frame = cv::Mat(GRID_SIZE * CELL_SIZE + 100, GRID_SIZE * CELL_SIZE, CV_8UC3);
                cvui::init(WINDOW_NAME);
                bool check;
                bool showanswer;

                while(true) {
                    // 清空窗口内容
                    frame = cv::Scalar(49, 52, 49);
                    check = false;
                    showanswer = false;

                    // 绘制数独表格
                    drawGrid(frame);

                    // 检验按钮
                    if(cvui::button(frame, 151, 470, "Check Your Answer")) {
                        check = true;
                    }

                    // 显示答案按钮
                    if(cvui::button(frame, 157, 510, "Show The Answer")) {
                        showanswer = true;
                    }

                    // 检测用户点击事件
                    if (cvui::mouse(cvui::LEFT_BUTTON, cvui::DOWN)) {
                        int x = cvui::mouse().x;
                        int y = cvui::mouse().y;
                        handleMouseClick(x, y); // 获取被选中的单元格位置
                    }

                    // 显示选中单元格
                    if (selectedRow != -1 && selectedCol != -1) {
                        int x = (selectedCol-1) * CELL_SIZE;
                        int y = (selectedRow-1) * CELL_SIZE;
                        cv::rectangle(frame, cv::Point(x, y), cv::Point(x + CELL_SIZE, y + CELL_SIZE), 0x00FF00, 2);
                    }

                    // 允许用户修改选中的单元格内容（按数字键）
                    int key = cv::waitKey(1);
                    if (selectedRow != -1 && selectedCol != -1 && key >= '1' && key <= '9' && initial_table[selectedRow][selectedCol] == 0) {
                        table[selectedRow][selectedCol] = key - '0';  // 将输入的数字填入表格
                    }

                    // 允许用户清空选中的单元格（按 0 键）
                    if (selectedRow != -1 && selectedCol != -1 && key == '0' && initial_table[selectedRow][selectedCol] == 0) {
                        table[selectedRow][selectedCol] = 0;  // 清空单元格
                    }

                    // 检查玩家的答案
                    if(check) {
                        // 检查数独是否填写完毕
                        bool isComplete = true;
                        for (int i = 1; i <= GRID_SIZE; ++i) {
                            for (int j = 1; j <= GRID_SIZE; ++j) {
                                if (table[i][j] == 0) {
                                    isComplete = false;
                                    break;
                                }
                            }
                        }
                        if(!isComplete) {
                            MessageBox(GetForegroundWindow(),"数独未完成","警告",1);
                        }
                        else {
                            // 检查数独是否填写正确
                            bool isCorrect = true;
                            for (int i = 1; i <= GRID_SIZE; ++i) {
                                for (int j = 1; j <= GRID_SIZE; ++j) {
                                    if (table[i][j] != solver.table[i][j]) {
                                        isCorrect = false;
                                        break;
                                    }
                                }
                            }
                            if(isCorrect) {
                                MessageBox(GetForegroundWindow(),"数独填写正确","恭喜",1);
                            }
                            else {
                                MessageBox(GetForegroundWindow(),"数独填写错误","抱歉",1);
                            }
                        }
                    }

                    // 显示答案
                    if(showanswer) {
                        memcpy(table, solver.table, sizeof(table));
                    }

                    // 更新 cvui 界面
                    cvui::update();

                    // 显示窗口
                    cv::imshow(WINDOW_NAME, frame);

                    if (cv::waitKey(20) == 27) {
                        break;
                    }
                }

                selectedCol = selectedRow = -1;
            }
            else if(opt == 2) {
                cout << "Now generate a random soduku for you!" << endl;
                cout << "1: Easy" << endl;
                cout << "2: Medium" << endl;
                cout << "3: Hard" << endl;
                cout << "4: Evil" << endl;
                cout << "Your expected level of difficulty: ";
                cin >> opt;
                auto solver = Random_Generate(opt);
                memcpy(table, solver.table, sizeof(table));
                memcpy(initial_table, solver.table, sizeof(table));
                solver.getAnswer();
                cv::Mat frame = cv::Mat(GRID_SIZE * CELL_SIZE + 100, GRID_SIZE * CELL_SIZE, CV_8UC3);
                cvui::init(WINDOW_NAME);
                bool check;
                bool showanswer;

                while(true) {
                    // 清空窗口内容
                    frame = cv::Scalar(49, 52, 49);
                    check = false;
                    showanswer = false;

                    // 绘制数独表格
                    drawGrid(frame);

                    // 检验按钮
                    if(cvui::button(frame, 151, 470, "Check Your Answer")) {
                        check = true;
                    }

                    // 显示答案按钮
                    if(cvui::button(frame, 157, 510, "Show The Answer")) {
                        showanswer = true;
                    }

                    // 检测用户点击事件
                    if (cvui::mouse(cvui::LEFT_BUTTON, cvui::DOWN)) {
                        int x = cvui::mouse().x;
                        int y = cvui::mouse().y;
                        handleMouseClick(x, y); // 获取被选中的单元格位置
                    }

                    // 显示选中单元格
                    if (selectedRow != -1 && selectedCol != -1) {
                        int x = (selectedCol-1) * CELL_SIZE;
                        int y = (selectedRow-1) * CELL_SIZE;
                        cv::rectangle(frame, cv::Point(x, y), cv::Point(x + CELL_SIZE, y + CELL_SIZE), 0x00FF00, 2);
                    }

                    // 允许用户修改选中的单元格内容（按数字键）
                    int key = cv::waitKey(1);
                    if (selectedRow != -1 && selectedCol != -1 && key >= '1' && key <= '9' && initial_table[selectedRow][selectedCol] == 0) {
                        table[selectedRow][selectedCol] = key - '0';  // 将输入的数字填入表格
                    }

                    // 允许用户清空选中的单元格（按 0 键）
                    if (selectedRow != -1 && selectedCol != -1 && key == '0' && initial_table[selectedRow][selectedCol] == 0) {
                        table[selectedRow][selectedCol] = 0;  // 清空单元格
                    }

                    // 检查玩家的答案
                    if(check) {
                        // 检查数独是否填写完毕
                        bool isComplete = true;
                        for (int i = 1; i <= GRID_SIZE; ++i) {
                            for (int j = 1; j <= GRID_SIZE; ++j) {
                                if (table[i][j] == 0) {
                                    isComplete = false;
                                    break;
                                }
                            }
                        }
                        if(!isComplete) {
                            MessageBox(GetForegroundWindow(),"数独未完成","警告",1);
                        }
                        else {
                            // 检查数独是否填写正确
                            bool isCorrect = true;
                            for (int i = 1; i <= GRID_SIZE; ++i) {
                                for (int j = 1; j <= GRID_SIZE; ++j) {
                                    if (table[i][j] != solver.table[i][j]) {
                                        isCorrect = false;
                                        break;
                                    }
                                }
                            }
                            if(isCorrect) {
                                MessageBox(GetForegroundWindow(),"数独填写正确","恭喜",1);
                            }
                            else {
                                MessageBox(GetForegroundWindow(),"数独填写错误","抱歉",1);
                            }
                        }
                    }

                    // 显示答案
                    if(showanswer) {
                        memcpy(table, solver.table, sizeof(table));
                    }

                    // 更新 cvui 界面
                    cvui::update();

                    // 显示窗口
                    cv::imshow(WINDOW_NAME, frame);

                    if (cv::waitKey(20) == 27) {
                        break;
                    }
                }

                selectedCol = selectedRow = -1;
            }
        }
    }
    system("pause");
    return 0;
}
