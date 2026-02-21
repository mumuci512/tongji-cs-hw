/* 2350222 盛曦 计科 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cmath>
using namespace std;

/* 从此处到标记替换行之间，给出各种类的定义及实现
	1、不允许定义全局变量（不含const和#define）
	2、不允许添加其它系统头文件
*/
class integral
{
protected:
	char name[7];
	double high, low;
	int n;
public:
	friend istream& operator>>(istream& in, integral& fRef);
	virtual double value() = 0;
};

class integral_sin : public integral
{
private:
public:
	integral_sin();
	friend istream& operator>>(istream& in, integral& fRef);
	virtual double value();
};
integral_sin::integral_sin()
{
	//strcpy(name, "sinxdx");
	name[0] = 's';
	name[1] = 'i';
	name[2] = 'n';
	name[3] = 'x';
	name[4] = 'd';
	name[5] = 'x';
	name[6] = '\0';
	high = 0;
	low = 0;
	n = 0;
}
double integral_sin::value()
{
	double step = (high - low) / n;
	double value = 0;
	int i;
	for (i = 0; i < n; i++)
		value += sin(high - step * i);
	return value * step;
}

class integral_cos : public integral
{
private:
public:
	integral_cos();
	friend istream& operator>>(istream& in, integral& fRef);
	virtual double value();
};
integral_cos::integral_cos()
{
	//strcpy(name, "cosxdx");
	name[0] = 'c';
	name[1] = 'o';
	name[2] = 's';
	name[3] = 'x';
	name[4] = 'd';
	name[5] = 'x';
	name[6] = '\0';
	high = 0;
	low = 0;
	n = 0;
}
double integral_cos::value()
{
	double step = (high - low) / n;
	double value = 0;
	int i;
	for (i = 0; i < n; i++)
		value += cos(high - step * i);
	return value * step;
}

class integral_exp : public integral
{
private:
public:
	integral_exp();
	friend istream& operator>>(istream& in, integral& fRef);
	virtual double value();
};
integral_exp::integral_exp()
{
	//strcpy(name, "e^xdx");
	name[0] = 'e';
	name[1] = '^';
	name[2] = 'x';
	name[3] = 'd';
	name[4] = 'x';
	name[5] = '\0';
	high = 0;
	low = 0;
	n = 0;
}
double integral_exp::value()
{
	double step = (high - low) / n;
	double value = 0;
	int i;
	for (i = 0; i < n; i++)
		value += exp(high - step * i);
	return value * step;
}

istream& operator>>(istream& in, integral& fRef)
{
	cout << "请输入"<< fRef.name<<"的下限、上限及区间划分数量" << endl;
	in >> fRef.low >> fRef.high >> fRef.n;
	cout << fRef.name << "[" << fRef.low << '~' << fRef.high << "/n=" << fRef.n << "] : " << fRef.value() << endl;
	return in;
}
/* -- 替换标记行 -- 本行不要做任何改动 -- 本行不要删除 -- 在本行的下面不要加入任何自己的语句，作业提交后从本行开始会被替换 -- 替换标记行 -- */

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：fun_integral不准动，思考一下，integral应如何定义
***************************************************************************/
void fun_integral(integral& fRef)
{
	cin >> fRef;	//输入上下限、划分数
	cout << fRef.value() << endl;
	return;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：main函数不准动
***************************************************************************/
int main()
{
	integral_sin s1;
	integral_cos s2;
	integral_exp s3;

	fun_integral(s1); //计算sinxdx的值
	fun_integral(s2); //计算cosxdx的值
	fun_integral(s3); //计算expxdx的值

	return 0;
}

//注：矩形计算取右值，输出为正常double格式
