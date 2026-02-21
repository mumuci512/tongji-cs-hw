/* 2350222 盛曦 计科 */
#pragma once

#include <iostream>
using namespace std;

/* 如果有其它全局函数需要声明，写于此处 */
#define HOUR_DEFAULT 0
#define MIN_DEFAULT 0
#define SEC_DEFAULT 0

#define HOUR_MAX 23
#define MIN_MAX 59
#define SEC_MAX 59

#define HOUR_MIN 0
#define MIN_MIN 0
#define SEC_MIN 0

#define TIME_PERIOD 86400

/* Time类的声明 */ 
class Time {
protected:
	/* 除这三个以外，不允许再定义任何数据成员 */ 
	int hour;
	int minute;
	int second;
public:
	/* 允许需要的成员函数及友元函数的声明 */
	Time();
	Time(const int h, const int m, const int s);
	int set(const int h, const int m = 0, const int s = 0);
	int get(int& h, int& m, int& s) const;
	int show() const;
	Time& operator++();//前缀
	Time operator++(int);//后缀
	Time& operator--();//前缀
	Time operator--(int);//后缀
	Time operator+(int n);
	friend Time operator+(int n, Time& t);
	Time operator-(int n);
	Time(int n);
	operator int();
	int operator-(Time& t);
	friend ostream& operator<<(ostream& out, const Time& t);
	friend istream& operator>>(istream& in, Time& t);
	bool operator==(const Time& t);
	bool operator!=(const Time& t);
	bool operator>(const Time& t);
	bool operator>=(const Time& t);
	bool operator<(const Time& t);
	bool operator<=(const Time& t);
	/* 允许加入友元声明（如果有必要） */

};
