#include "Global.h"
#include "Sudoku_to_cnf.h"
#include <opencv2/opencv.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"
#include <windows.h>
#define WINDOW_NAME "Sudoku"
#define CELL_SIZE 50 // ÿ����Ԫ��Ĵ�С
#define GRID_SIZE 9  // ������ 9x9 �ı��
using namespace std;

int selectedRow = -1;  // ��ǰѡ�еĵ�Ԫ����
int selectedCol = -1;  // ��ǰѡ�еĵ�Ԫ����
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
            // ���Ƶ�Ԫ�����
            cv::rectangle(frame, cv::Point(x, y), cv::Point(x + CELL_SIZE, y + CELL_SIZE), cv::Scalar(200, 200, 200), 1);

            // �ڵ�Ԫ������ʾ����
            if(table[i][j] != 0 && initial_table[i][j] == 0) {
                cvui::printf(frame, x + 20, y + 20, 0.5, 0xFFFFFF, "%d", table[i][j]);
            }
            else if(table[i][j] != 0 && initial_table[i][j] != 0) {
                cvui::printf(frame, x + 20, y + 20, 0.5, 0x00FFFF, "%d", table[i][j]);
            }
        }
    }
    // ���ƾŹ����Ĵ���
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
                // ��ӡ��
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
                    // ��մ�������
                    frame = cv::Scalar(49, 52, 49);
                    check = false;
                    showanswer = false;

                    // �����������
                    drawGrid(frame);

                    // ���鰴ť
                    if(cvui::button(frame, 151, 470, "Check Your Answer")) {
                        check = true;
                    }

                    // ��ʾ�𰸰�ť
                    if(cvui::button(frame, 157, 510, "Show The Answer")) {
                        showanswer = true;
                    }

                    // ����û�����¼�
                    if (cvui::mouse(cvui::LEFT_BUTTON, cvui::DOWN)) {
                        int x = cvui::mouse().x;
                        int y = cvui::mouse().y;
                        handleMouseClick(x, y); // ��ȡ��ѡ�еĵ�Ԫ��λ��
                    }

                    // ��ʾѡ�е�Ԫ��
                    if (selectedRow != -1 && selectedCol != -1) {
                        int x = (selectedCol-1) * CELL_SIZE;
                        int y = (selectedRow-1) * CELL_SIZE;
                        cv::rectangle(frame, cv::Point(x, y), cv::Point(x + CELL_SIZE, y + CELL_SIZE), 0x00FF00, 2);
                    }

                    // �����û��޸�ѡ�еĵ�Ԫ�����ݣ������ּ���
                    int key = cv::waitKey(1);
                    if (selectedRow != -1 && selectedCol != -1 && key >= '1' && key <= '9' && initial_table[selectedRow][selectedCol] == 0) {
                        table[selectedRow][selectedCol] = key - '0';  // �����������������
                    }

                    // �����û����ѡ�еĵ�Ԫ�񣨰� 0 ����
                    if (selectedRow != -1 && selectedCol != -1 && key == '0' && initial_table[selectedRow][selectedCol] == 0) {
                        table[selectedRow][selectedCol] = 0;  // ��յ�Ԫ��
                    }

                    // �����ҵĴ�
                    if(check) {
                        // ��������Ƿ���д���
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
                            MessageBox(GetForegroundWindow(),"����δ���","����",1);
                        }
                        else {
                            // ��������Ƿ���д��ȷ
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
                                MessageBox(GetForegroundWindow(),"������д��ȷ","��ϲ",1);
                            }
                            else {
                                MessageBox(GetForegroundWindow(),"������д����","��Ǹ",1);
                            }
                        }
                    }

                    // ��ʾ��
                    if(showanswer) {
                        memcpy(table, solver.table, sizeof(table));
                    }

                    // ���� cvui ����
                    cvui::update();

                    // ��ʾ����
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
                    // ��մ�������
                    frame = cv::Scalar(49, 52, 49);
                    check = false;
                    showanswer = false;

                    // �����������
                    drawGrid(frame);

                    // ���鰴ť
                    if(cvui::button(frame, 151, 470, "Check Your Answer")) {
                        check = true;
                    }

                    // ��ʾ�𰸰�ť
                    if(cvui::button(frame, 157, 510, "Show The Answer")) {
                        showanswer = true;
                    }

                    // ����û�����¼�
                    if (cvui::mouse(cvui::LEFT_BUTTON, cvui::DOWN)) {
                        int x = cvui::mouse().x;
                        int y = cvui::mouse().y;
                        handleMouseClick(x, y); // ��ȡ��ѡ�еĵ�Ԫ��λ��
                    }

                    // ��ʾѡ�е�Ԫ��
                    if (selectedRow != -1 && selectedCol != -1) {
                        int x = (selectedCol-1) * CELL_SIZE;
                        int y = (selectedRow-1) * CELL_SIZE;
                        cv::rectangle(frame, cv::Point(x, y), cv::Point(x + CELL_SIZE, y + CELL_SIZE), 0x00FF00, 2);
                    }

                    // �����û��޸�ѡ�еĵ�Ԫ�����ݣ������ּ���
                    int key = cv::waitKey(1);
                    if (selectedRow != -1 && selectedCol != -1 && key >= '1' && key <= '9' && initial_table[selectedRow][selectedCol] == 0) {
                        table[selectedRow][selectedCol] = key - '0';  // �����������������
                    }

                    // �����û����ѡ�еĵ�Ԫ�񣨰� 0 ����
                    if (selectedRow != -1 && selectedCol != -1 && key == '0' && initial_table[selectedRow][selectedCol] == 0) {
                        table[selectedRow][selectedCol] = 0;  // ��յ�Ԫ��
                    }

                    // �����ҵĴ�
                    if(check) {
                        // ��������Ƿ���д���
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
                            MessageBox(GetForegroundWindow(),"����δ���","����",1);
                        }
                        else {
                            // ��������Ƿ���д��ȷ
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
                                MessageBox(GetForegroundWindow(),"������д��ȷ","��ϲ",1);
                            }
                            else {
                                MessageBox(GetForegroundWindow(),"������д����","��Ǹ",1);
                            }
                        }
                    }

                    // ��ʾ��
                    if(showanswer) {
                        memcpy(table, solver.table, sizeof(table));
                    }

                    // ���� cvui ����
                    cvui::update();

                    // ��ʾ����
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
