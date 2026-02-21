#include <iostream>
#include "utils.h"
using namespace std;

// 静态全局变量
static int static_global_var = 200;

// 外部全局变量定义
int extern_var = 300;

int add(int x, int y) {
    int sum = x + y;  // 自动变量
    return sum;
}

void staticGlobalVarTest() {
    static_global_var++;
    cout << "Static Global Var: " << static_global_var << endl;
}

void arrayPointerTest(char* arr, int size) {
    char* p = arr;
    for (; p-arr<size; p++) {
        *p+=('A'-'a');
        cout << *p<< endl;
    }
}
void arrayPointerTest2(int array[5], int size) {
    int* p = array;
    for (; p - array < size; p++) {
        *p+=5;
        cout << *p << endl;
    }
    p += 10;
}
void arrayPointerTest3(char twoch2[3][4], int size) {
    char* p = &twoch2[2][0];
    for (; p - twoch2[2] < 4; p++) {
        *p += ('A' - 'a');
        cout << *p << endl;
    }
    p++;
}
//void arrayPointerTest4(char twoch2, int size) {
//    char* p = twoch2[0];
//    for (; p - twoch2[0] < size; p++) {
//        *p += ('A' - 'a');
//        cout << *p << endl;
//    }
//}
