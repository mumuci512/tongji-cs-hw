/* 2350222 盛曦 计科 */

#pragma once

#include <iostream>
using namespace std;

/* 如果有其它全局函数需要声明，写于此处 */

/* 如果有需要的宏定义、只读全局变量等，写于此处 */
#define YEAR_DEFAULT 2000
#define MONTH_DEFAULT 1
#define DAY_DEFAULT 1

#define YEAR_MAX 2099
#define YEAR_MIN 1900

#define COMMON_YEAR 0
#define LEAP_YEAR 1
const int MONTH[2][13] = { {0,31,28,31,30,31,30,31,31,30,31,30,31},
							{0,31,29,31,30,31,30,31,31,30,31,30,31} };

/* 补全Date类的定义，所有成员函数均体外实现，不要在此处体内实现 */
class Date {
private:
	int year;
	int month;
	int day;
	/* 不允许添加数据成员 */
public:
	/* 根据需要定义所需的成员函数、友元函数等(不允许添加数据成员) */
	int type_of_year();
	Date();
	Date(const int y,const int m,const int d);
	int set(const int y, const int m=1, const int d=1);
	int get(int& y, int& m, int& d) const;
	int show() const;
	Date& operator++();//前缀
	Date operator++(int);//后缀
	Date& operator--();//前缀
	Date operator--(int);//后缀
	Date operator+(int n);
	friend Date operator+(int n, Date& d);
	Date operator-(int n);
	Date(int n);
	operator int();
	int operator-(Date& d);
	friend ostream& operator<<(ostream& out, const Date& d);
	friend istream& operator>>(istream& in, Date& d);
	bool operator==(const Date& d);
	bool operator!=(const Date& d);
	bool operator>(const Date& d);
	bool operator>=(const Date& d);
	bool operator<(const Date& d);
	bool operator<=(const Date& d);
};

