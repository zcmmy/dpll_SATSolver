#ifndef GLOBAL_H
#define GLOBAL_H
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstring>

typedef int status;
#define FOUND 1
#define NOTHING 0
#define CONFLICT -1

typedef struct Node {   // 变元节点
    int ord;    // 变元序号
    bool is_neg;    // 是否为负元
    Node *next;    // 子句中下一变元
    void *belong;   // 所属子句
}Node;

typedef struct Head {   // 子句头节点
    Node *first; // 第一变元
    Head *next; // 下一子句
}Head;

class ClauseStack {
public:
    Head *top;

    ClauseStack() {
        top = nullptr;
    }
    void push(Head *p) {
        // 浅拷贝的方式入栈
        p->next = top;
        top = p;
    }
    Head* pop() {
        // 浅拷贝的方式出栈
        Head *p = top;
        top = top->next;
        p->next = nullptr;
        return p;
    }
    bool is_empty() {
        return top == nullptr;
    }
    void clear() {
        while(!is_empty()) {
            pop();
        }
    }
};

class LiteralStack {
public:
    Node *top;

    LiteralStack() {
        top = nullptr;
    }
    void push(Node *p) {
        // 浅拷贝的方式入栈
        p->next = top;
        top = p;
    }
    Node* pop() {
        // 浅拷贝的方式出栈
        Node *p = top;
        top = top->next;
        p->next = nullptr;
        return p;
    }
    bool is_empty() {
        return top == nullptr;
    }
    void clear() {
        while(!is_empty()) {
            pop();
        }
    }
};

#endif //GLOBAL_H
