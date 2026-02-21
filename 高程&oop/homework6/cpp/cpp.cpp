#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <cstring>
using namespace std;
struct birth
{
public:
    int y;
    int m;
private:
    static int d;
};

struct stu
{
    int name;
    struct birth birthday;
};

class test
{
public:
    int pub;
    test(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    void display()
    {
        cout << x << y;
    }
private:
    int x;
    int y;
    //int z;
    void t();
};

class test2
{
public:
    test2(int a, int b, int c)
    {
        x = a;
        y = b;
        z = c;
    }
private:
    int x, y, z;
    friend void print(test2& t);
};

void print(test2& t)
{
    cout << t.x;
}
//int* point()
//{
//    int x = 0;
//    return x;
//}
int main()
{/*
    char str1[100] = "tongji\0\0HAO";
    char str2[] = "laji";
    cout << strcat(str1, str2);*/

   /* int a = 0;
    (++a)+= 1;
    cout << a;*/


    /*float f = 123.456;
    int* p = (int*)&f;
    cout << hex << *p << endl;
    
    short* q = (short*)&f;
    cout << hex << *(q + 1) << endl;*/

    struct stu one;
    struct birth b = { 1999,1 };
    one.birthday = b;

    test t1 = { 1,2 };
    /*test t2 = (1, 2);
    test t2(1, 2);*/
    //test t2;
    test t2(2, 3);
    //t2.display();

    test t3 = test(10, 11);
   // t3.display();

    test2 t[2] = { {1,2,3},{4,5,6} };
    test2* p = t;

    int arr[5] = { 0,1,2,3,4 };
    int* p1 = arr;
    cout << p1[4];
    (*arr)++;

    int* p2 = &t1.pub;

    int a;
    cin.unsetf(ios::skipws);
    cin >>a;

    char ch[10];
    cout << scanf("%s", ch);

    scanf("%c", ch[0]);
    cout << ch[0];
    return 0;
}

/*
类的初始化：
无构造函数：只能用结构体那种“={}”的形式，而且必须所有成员（除函数）均为共有
有构造函数：
    单个对象的初始化：
        test t1(1,2);
        test t2{1,2};
        test t3={1,2};
        不可以=(1,2)   逗号表达式
        不可以test t1; 与构造函数形参表不匹配
        可以test t4=test(10,11);  显式调用构造函数
    对象数组的初始化：
        构造函数只带一个参数/重载中有只带一个参数的/有只带一个不默认的参数的默认参数函数：用数组初始化的方式初始化：test t[5]={1,2,3,4,5};
        若初始化时多于一个参数：test t[5]={test(1,2),test(1,2,3),3};    后2个元素不初始化是因为有可以不带实参的构造函数，否则会报错
                                test t[]={{1,2},{1,2,3},3}; 默认t中有3个元素
                                不可以test t[5]={(1,2),(1,2,3),3,4,5}; 逗号表达式
*/
