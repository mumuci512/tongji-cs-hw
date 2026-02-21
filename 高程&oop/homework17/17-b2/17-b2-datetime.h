/* 2350222 盛曦 计科 */
#pragma once

#include "17-b2-date.h"
#include "17-b2-time.h"

/* 如果有其它全局函数需要声明，写于此处 */
const long long int DT_PERIOD = ((long long int)DATE_PERIOD * (long long int)TIME_PERIOD);
/* DateTime类的基本要求：
	1、不允许定义任何数据成员
	2、尽量少定义成员函数 
*/

class DateTime:public Date, public Time {
protected:
	/* 不允许再定义任何数据成员 */ 

public:
	/* 不允许再定义任何数据成员，允许需要的成员函数及友元函数的声明 */
	DateTime();
	DateTime(const int y, const int m, const int d, const int h, const int min, const int s);
	int set(const int y= YEAR_DEFAULT, const int m=MONTH_DEFAULT, const int d=DAY_DEFAULT, const int h=HOUR_DEFAULT, const int min=MIN_DEFAULT, const int s=SEC_DEFAULT);
	int get(int& y, int& m, int& d, int& h, int& min, int& s) const;
	int show() const;
	DateTime& operator++();//前缀
	DateTime operator++(int);//后缀
	DateTime& operator--();//前缀
	DateTime operator--(int);//后缀
	DateTime operator+(int n);
	DateTime operator+(long int n);
	DateTime operator+(long long int n);
	friend DateTime operator+(int n, DateTime& dt);
	friend DateTime operator+(long int n, DateTime& dt);
	friend DateTime operator+(long long int n, DateTime& dt);
	DateTime operator-(int n);
	DateTime operator-(long int n);
	DateTime operator-(long long int n);
	DateTime(long long int n);
	operator long long int();
	long long int operator-(DateTime& dt);
	friend ostream& operator<<(ostream& out, const DateTime& dt);
	friend istream& operator>>(istream& in, DateTime& dt);
	bool operator==(const DateTime& dt);
	bool operator!=(const DateTime& dt);
	bool operator>(const DateTime& dt);
	bool operator>=(const DateTime& dt);
	bool operator<(const DateTime& dt);
	bool operator<=(const DateTime& dt);
	/* 允许加入友元声明（如果有必要） */

};
