/* 2350222 盛曦 计科 */
#include <iostream>
#include <iomanip>
using namespace std;

#define STUDENT_NUM	4
#define SCORE_NUM	5

/* --- 不允许定义任何形式的全局变量 --- */

/***************************************************************************
  函数名称：
  功    能：求第一门课的平均分
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void average(int (*score)[STUDENT_NUM])
{
    /* 函数定义语句部分：
       1、本函数中仅允许定义 1个简单变量 + 1个指针变量 */
    double average = 0;
    int *p;
       /* 函数执行语句部分：
          1、不允许再定义任何类型的变量，包括 for (int i=0;...）等形式定义变量
          2、循环变量必须是指针变量，后续语句中不允许出现[]形式访问数组
             不允许：int a[10], i;
                     for(i=0; i<10; i++)
                         cout << a[i];
             允许  ：int a[10], p;
                     for(p=a; p<a+10; p++)
                         cout << *p;          */
    for (p = *score; p - *score < STUDENT_NUM; p++)
        average += *p;
    average /= STUDENT_NUM;
    cout << "第1门课平均分：" << average << endl;
}

/***************************************************************************
  函数名称：
  功    能：找出有两门以上课程不及格的学生
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void fail(int (*score)[STUDENT_NUM])
{
    /* 函数定义语句部分：
       1、本函数中仅允许定义 3个简单变量 + 1个行指针变量 + 1个简单指针变量 */
    int (*p_sco)[STUDENT_NUM];
    int* p_stu;
    int num_of_fail;
    double average;
       /* 函数执行语句部分（要求同average）*/
    cout << "2门以上不及格的学生：" << endl;
    for (p_stu = *score; p_stu - *score < STUDENT_NUM; p_stu++)
    {
        num_of_fail = 0;
        for (p_sco = score; p_sco - score < SCORE_NUM; p_sco++)
        {
            if (*(*p_sco + (p_stu-*score)) < 60)
                num_of_fail++;
        }
        if (num_of_fail >= 2)
        {
            cout << "No：" << p_stu - *score + 1 << " ";
            average = 0;
            for (p_sco = score; p_sco - score < SCORE_NUM; p_sco++)
            {
                cout << *(*p_sco + (p_stu - *score)) << " ";
                average += *(*p_sco + (p_stu - *score));
            }
            average /= SCORE_NUM;
            cout << "平均：" << average << endl;
        }
    }
}

/***************************************************************************
  函数名称：
  功    能：找出平均成绩在90分以上或全部成绩在85分以上的学生
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
void good(int (*score)[STUDENT_NUM])
{
    /* 函数定义语句部分：
       1、本函数中仅允许定义 3个简单变量 + 1个行指针变量 + 1个简单指针变量 */
    int (*p_sco)[STUDENT_NUM];
    int* p_stu;
    int flag_a85;
    double average;
       /* 函数执行语句部分（要求同average）*/
    cout << "平均90以上或全部85以上的学生：" << endl;
    for (p_stu = *score; p_stu - *score < STUDENT_NUM; p_stu++)
    {
        flag_a85 = 1;
        average = 0;
        for (p_sco = score; p_sco - score < SCORE_NUM; p_sco++)
        {
            if (*(*p_sco + (p_stu - *score)) < 85)
                flag_a85 = 0;
            average += *(*p_sco + (p_stu - *score));
        }
        average /= SCORE_NUM;
        if (flag_a85 == 1 || average >= 90)
        {
            cout << "No：" << p_stu - *score + 1 << " ";
            for (p_sco = score; p_sco - score < SCORE_NUM; p_sco++)
            {
                cout << *(*p_sco + (p_stu - *score)) << " ";
            }
            cout << "平均：" << average << endl;
        }
    }
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int main()
{
    int a[SCORE_NUM][STUDENT_NUM] = {
        {91,92,93,97},  //第1-4个学生的第1门课成绩
        {81,82,83,85},  //第1-4个学生的第2门课成绩
        {71,72,99,87},  //第1-4个学生的第3门课成绩
        {61,32,80,91},  //第1-4个学生的第4门课成绩
        {51,52,95,88} };//第1-4个学生的第5门课成绩
    /* 除上面的预置数组外，本函数中仅允许定义 1个行指针变量 + 1个简单指针变量 */
    int (*p1)[STUDENT_NUM];
    int* p2;
    /* 函数执行语句部分（要求同average）*/
    cout << "初始信息：" << endl;
    for (p1 = a; p1 - a < SCORE_NUM; p1++)
    {
        cout << "No.1-4学生的第" << p1 - a + 1 << "门课的成绩：";
        for (p2 = *p1; p2 - *p1 < STUDENT_NUM; p2++)
            cout << *p2 << " ";
        cout << endl;
    }
    cout << endl;

    p1 = a;
    average(p1);
    cout << endl;

    fail(p1);
    cout << endl;

    good(p1);

    return 0;
}