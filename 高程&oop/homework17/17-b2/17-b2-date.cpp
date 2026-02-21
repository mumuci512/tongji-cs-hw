/* 2350222 盛曦 计科 */
#include <iostream>
#include <iomanip>
#include "17-b2-date.h"
using namespace std;

/* --- 给出Date类的成员函数的体外实现(含友元及其它必要的公共函数)  --- */ 

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/

int Date::type_of_year()
{
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
		return LEAP_YEAR;
	else
		return COMMON_YEAR;
}

int Date::year_long()
{
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
		return 366;
	else
		return 365;
}

Date::Date()
{
	year = YEAR_DEFAULT;
	month = MONTH_DEFAULT;
	day = DAY_DEFAULT;
}

Date::Date(const int y, const int m, const int d)
{
	bool illegal = false;
	if ((y >= YEAR_MIN && y <= YEAR_MAX) && (m >= 1 && m <= 12))
	{
		year = y;
		if (d >= 1 && d <= MONTH[type_of_year()][m])
		{
			month = m;
			day = d;
		}
		else
			illegal = true;
	}
	else
		illegal = true;
	if (illegal)
	{
		year = YEAR_DEFAULT;
		month = MONTH_DEFAULT;
		day = DAY_DEFAULT;
	}
}

int Date::set(const int y, const int m, const int d)
{
	int flag = 0;//0:正常 1：非法 2：有0
	if ((y != 0) && (m != 0) && (d != 0))//有0就默认值
	{
		if ((y >= YEAR_MIN && y <= YEAR_MAX) && (m >= 1 && m <= 12))
		{
			year = y;
			if (d >= 1 && d <= MONTH[type_of_year()][m])
			{
				month = m;
				day = d;
			}
			else
				flag = 1;
		}
		else
			flag = 1;
	}
	else
		flag = 2;

	if (flag!=0)
	{
		year = YEAR_DEFAULT;
		month = MONTH_DEFAULT;
		day = DAY_DEFAULT;
	}
	return flag;
}

int Date::get(int& y, int& m, int& d) const
{
	y = year;
	m = month;
	d = day;
	return 0;
}

int Date::show() const
{
	cout << setfill('0') << setw(4) <<year << "-" <<setw(2)<< month << "-" << setw(2)<<day <<setfill(' ') << endl;
	return 0;
}

Date& Date::operator++()//前缀
{
	if (year == YEAR_MAX && month == 12 && day == 31)
	{
		year = YEAR_MIN;
		month = 1;
		day = 1;
		return *this;
	}
	if (day < MONTH[type_of_year()][month])
		day++;
	else
	{
		day = 1;
		if (month < 12)
			month++;
		else
		{
			month = 1;
			year++;
		}
	}
	return *this;
}

Date Date::operator++(int)//后缀
{
	Date temp = *this;
	if (year == YEAR_MAX && month == 12 && day == 31)
	{
		year = YEAR_MIN;
		month = 1;
		day = 1;
		return temp;
	}
	if (day < MONTH[type_of_year()][month])
		day++;
	else
	{
		day = 1;
		if (month < 12)
			month++;
		else
		{
			month = 1;
			year++;
		}
	}
	return temp;
}

Date& Date::operator--()//前缀
{
	if (year == YEAR_MIN && month == 1 && day == 1)
	{
		year = YEAR_MAX;
		month = 12;
		day = 31;
		return *this;
	}
	if (day > 1)
		day--;
	else
	{
		if (month > 1)
			month--;
		else
		{
			month = 12;
			year--;
		}
		day = MONTH[type_of_year()][month];
	}
	return *this;
}

Date Date::operator--(int)//后缀
{
	Date temp = *this;
	if (year == YEAR_MIN && month == 1 && day == 1)
	{
		year = YEAR_MAX;
		month = 12;
		day = 31;
		return temp;
	}
	if (day > 1)
		day--;
	else
	{
		if (month > 1)
			month--;
		else
		{
			month = 12;
			year--;
		}
		day = MONTH[type_of_year()][month];
	}
	return temp;
}

Date Date::operator+(int n)
{
	//while (n >= 366)
	//{
	//	year++;
	//	if (type_of_year() == LEAP_YEAR)//闰年
	//		n -= 366;
	//	else
	//		n -= 365;
	//}
	//if (n == 365)
	//{
	//	if (type_of_year() == COMMON_YEAR)//平年
	//		year++;
	//	n -= 365;
	//}

	//while (n >= MONTH[type_of_year()][month])
	//{
	//	n -= MONTH[type_of_year()][month];
	//	if (month < 12)
	//		month++;
	//	else
	//	{
	//		month = 1;
	//		year++;
	//	}
	//}

	//while (n >= 0)
	//{
	//	n--;
	//	if (day < MONTH[type_of_year()][month])
	//		day++;
	//	else
	//	{
	//		day = 1;
	//		if (month < 12)
	//			month++;
	//		else
	//		{
	//			month = 1;
	//			year++;
	//		}
	//	}
	//}

	if (n < -DATE_PERIOD || n > DATE_PERIOD)
		n %= DATE_PERIOD;
	Date temp = *this;
	while (n > 0)
	{
		++temp;
		n--;
	}
	while (n < 0)
	{
		--temp;
		n++;
	}
	return temp;
}

Date operator+(int n, Date& d)
{
	if (n < -DATE_PERIOD || n > DATE_PERIOD)
		n %= DATE_PERIOD;
	Date temp = d;
	while (n > 0)
	{
		++temp;
		n--;
	}
	while (n < 0)
	{
		--temp;
		n++;
	}
	return temp;
}

Date Date::operator-(int n)
{
	if (n < -DATE_PERIOD || n > DATE_PERIOD)
		n %= DATE_PERIOD;
	Date temp = *this;
	while (n > 0)
	{
		--temp;
		n--;
	}
	while (n < 0)
	{
		++temp;
		n++;
	}
	return temp;
}

Date::Date(int n)
{
	/*if (n >= 73049)
	{
		year = YEAR_MAX;
		month = 12;
		day = 31;
	}
	else
	{
		year = YEAR_MIN;
		month = 1;
		day = 1;
		if (n > 1)
			*this = *this + n - 1;
	}*/
	year = YEAR_MIN;
	month = 1;
	day = 1;
	/*if (n > 1)
		*this = *this + n - 1;*/
	*this = *this + n;//包含n<0
}

Date::operator int()
{
	int n = 0;
	Date temp = *this;
	while (temp.year != YEAR_MIN || temp.month != 1 || temp.day != 1)
	{
		--temp;
		n++;
	}
	return n;
}

int Date::operator-(Date& d)
{
	int temp1 = *this;
	int temp2 = d;
	int temp = temp1 - temp2;
	return temp;
}

ostream& operator<<(ostream& out, const Date& d)
{
	out << setfill('0') << setw(4) << d.year << "-" << setw(2) << d.month << "-" << setw(2) << d.day << setfill(' ') << endl;;
	return out;
}

istream& operator>>(istream& in, Date& d)
{
	if (in.fail() == 1)
		return in;
	Date temp = d;
	in >> d.year >> d.month >> d.day;
	if (in.fail() == 1)
	{
		d = temp;
		return in;
	}
	if (d.year < YEAR_MIN || d.year>YEAR_MAX || d.month < 1 || d.month>12 || d.day < 1 || d.day>MONTH[d.type_of_year()][d.month])
	{
		d.year = YEAR_DEFAULT;
		d.month = MONTH_DEFAULT;
		d.day = DAY_DEFAULT;
	}
	return in;
}

bool Date::operator==(const Date& d)
{
	if (year == d.year && month == d.month && day == d.day)
		return true;
	else
		return false;
}

bool Date::operator!=(const Date& d)
{
	if (year == d.year && month == d.month && day == d.day)
		return false;
	else
		return true;
}

bool Date::operator>(const Date& d)
{
	if (year != d.year)
		return year > d.year;
	else if (month != d.month)
		return month > d.month;
	else if (day != d.day)
		return day > d.day;
	else//两者相等
		return false;
}

bool Date::operator>=(const Date& d)
{
	if (year != d.year)
		return year > d.year;
	else if (month != d.month)
		return month > d.month;
	else if (day != d.day)
		return day > d.day;
	else//两者相等
		return true;
}

bool Date::operator<(const Date& d)
{
	if (year != d.year)
		return year < d.year;
	else if (month != d.month)
		return month < d.month;
	else if (day != d.day)
		return day < d.day;
	else//两者相等
		return false;
}

bool Date::operator<=(const Date& d)
{
	if (year != d.year)
		return year < d.year;
	else if (month != d.month)
		return month < d.month;
	else if (day != d.day)
		return day < d.day;
	else//两者相等
		return true;
}