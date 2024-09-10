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

typedef struct Node {   // ��Ԫ�ڵ�
    int ord;    // ��Ԫ���
    bool is_neg;    // �Ƿ�Ϊ��Ԫ
    Node *next;    // �Ӿ�����һ��Ԫ
    void *belong;   // �����Ӿ�
}Node;

typedef struct Head {   // �Ӿ�ͷ�ڵ�
    Node *first; // ��һ��Ԫ
    Head *next; // ��һ�Ӿ�
}Head;

class ClauseStack {
public:
    Head *top;

    ClauseStack() {
        top = nullptr;
    }
    void push(Head *p) {
        // ǳ�����ķ�ʽ��ջ
        p->next = top;
        top = p;
    }
    Head* pop() {
        // ǳ�����ķ�ʽ��ջ
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
        // ǳ�����ķ�ʽ��ջ
        p->next = top;
        top = p;
    }
    Node* pop() {
        // ǳ�����ķ�ʽ��ջ
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
