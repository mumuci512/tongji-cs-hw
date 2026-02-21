#include <iostream>
#include "utils.h"
using namespace std;

// 静态全局变量
static int static_global_var = 400;

// 全局变量
int global_var = 100;

// 外部全局变量声明
extern int extern_var;

void function() {
    int localVar = 5;  // 自动变量
    static int staticLocalVar = 10;  // 静态局部变量

    localVar++;
    staticLocalVar++;

    cout << "LocalVar: " << localVar << ", StaticLocalVar: " << staticLocalVar << endl;
}

int main() {
    int a = 10;
    int b = 20;
    int c = 0;
    int d = 0;
    int e = 0;
    int f = 0;
    int x = 100;
    int& X = x;
    X++;
    
    int two[3][3] = { {1,2,3},{4,5,6},{7,8,9} };
    const char* point = "china";
    // 调用自定义函数
    int result = add(a, b);
    cout << "Result: " << result << endl;

    function();

    // 静态全局变量测试
    staticGlobalVarTest();

    // 外部全局变量
    cout << "Extern Var: " << extern_var << endl;

    // 调用函数演示指针和数组调试
    
    char arr[5] = { 'a', 'b','c','d','\0'};
    int size = 5;
    int array[5] = { 1,2,3 ,4,5 };
    int erwei[3][3] = { {1,2,3},{1,2,3},{1,2,3} };
    int* p2 = erwei[1];
    char twoch[3][4] = { {'a','b','c','a'},{'d','e','f','a'},{'g','h','i','a'}};
    char twoch2[3][4] = { {'a','b','c'},{'d','e','f'},{'g','h','i'} };
    char twoch3[3][4] = { {'a','b','c','\0'},{'d','e','f','\0'},{'g','h','i','\0'}};
    arrayPointerTest(arr, size);
    arrayPointerTest2(array, size);
    arrayPointerTest3(twoch, 3);
    arrayPointerTest3(twoch2, 3);

    arrayPointerTest(twoch2[0], size);
    add(a, b);
    return 0;
}
