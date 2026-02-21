/* 2350222 盛曦 计科 */
#pragma once

#include <iostream>
using namespace std;

/* 如果有其它全局函数需要声明，写于此处 */
#define YEAR_DEFAULT 1900
#define MONTH_DEFAULT 1
#define DAY_DEFAULT 1

#define YEAR_MAX 2099
#define YEAR_MIN 1900

#define COMMON_YEAR 0
#define LEAP_YEAR 1

#define DATE_PERIOD 73049

const int MONTH[2][13] = { {0,31,28,31,30,31,30,31,31,30,31,30,31},
							{0,31,29,31,30,31,30,31,31,30,31,30,31} };

/* Date类的声明 */ 
class Date {
protected:
	/* 除这三个以外，不允许再定义任何数据成员 */ 
	int year;
	int month;
	int day;
public:
	/* 允许需要的成员函数及友元函数的声明 */
	int type_of_year();
	int year_long();
	Date();
	Date(const int y, const int m, const int d);
	int set(const int y, const int m = 1, const int d = 1);
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
	/* 允许加入友元声明（如果有必要） */

};
