/* 2350222 盛曦 计科 */
#include <iostream>
#include <iomanip>
#include "17-b2-datetime.h"
using namespace std;

/* --- 给出DateTime类的成员函数的体外实现(含友元及其它必要的公共函数)  --- */ 

int year_long(int year)
{
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
		return 366;
	else
		return 365;
}

DateTime::DateTime() :Date(), Time()
{
}

DateTime::DateTime(const int y, const int m, const int d, const int h, const int min, const int s)
{
	if (Date::set(y, m, d) !=0 || Time::set(h, min, s) !=0)//对于Date类，有0此时不算默认值，而算非法
	{
		year = YEAR_DEFAULT;
		month = MONTH_DEFAULT;
		day = DAY_DEFAULT;
		hour = HOUR_DEFAULT;
		minute = MIN_DEFAULT;
		second = SEC_DEFAULT;
	}
}

int DateTime::set(const int y, const int m, const int d, const int h, const int min, const int s)
{
	int flag = 0;//0:正常 1：非法 2：有0
	int date = Date::set(y, m, d);
	int time = Time::set(h, min, s);
	if (date==2||time==2)
		flag = 2;
	else if (date == 1 || time == 1)
		flag = 1;
	else
		flag = 0;

	if (flag)
	{
		year = YEAR_DEFAULT;
		month = MONTH_DEFAULT;
		day = DAY_DEFAULT;
		hour = HOUR_DEFAULT;
		minute = MIN_DEFAULT;
		second = SEC_DEFAULT;
		return flag;
	}
	return 0;
}

int DateTime::get(int& y, int& m, int& d, int& h, int& min, int& s) const
{
	Date::get(y, m, d);
	Time::get(h, min, s);
	return 0;
}

int DateTime::show() const
{
	cout << setfill('0') <<setw(4)<<year << "-" << setw(2) << month << "-" << setw(2) << day << ' ';
	cout<< setw(2) << hour << ":" << setw(2) << minute << ":" << setw(2) << second << setfill(' ') << endl;
	return 0;
}

DateTime& DateTime::operator++()//前缀
{
	Time::operator++();
	if (hour == 0 && minute == 0 && second == 0)
		Date::operator++();
	return *this;
}

DateTime DateTime::operator++(int)//后缀
{
	DateTime temp = *this;
	++(*this);
	return temp;
}

DateTime& DateTime::operator--()//前缀
{
	Time::operator--();
	if (hour == 23 && minute == 59 && second == 59)
		Date::operator--();
	return *this;
}

DateTime DateTime::operator--(int)//后缀
{
	DateTime temp = *this;
	--(*this);
	return temp;
}

DateTime DateTime::operator+(long long int n)
{
	if (n < -DT_PERIOD || n > DT_PERIOD)
		n %= DT_PERIOD;
	if (n < 0)
		n += DT_PERIOD;
	DateTime temp = *this;
	
	while (n >= temp.year_long()*TIME_PERIOD)
	{
		n -= temp.year_long() * TIME_PERIOD;
		if (temp.year == YEAR_MAX)
			temp.year = YEAR_MIN;
		else
			temp.year++;
	}
	while (n >= MONTH[temp.type_of_year()][temp.month] * TIME_PERIOD)
	{
		n -= MONTH[temp.type_of_year()][temp.month] * TIME_PERIOD;
		if(temp.month==12)
		{
			temp.month = 1;
			temp.year++;
			if (temp.year > YEAR_MAX)
				temp.year = YEAR_MIN;
		}
		else
			temp.month++;
	}

	temp.day += (int)(n / TIME_PERIOD);
	n %= TIME_PERIOD;

	temp.hour += (int)(n / 3600);
	n %= 3600;

	temp.minute += (int)(n / 60);
	n %= 60;

	temp.second += (int)n;

	if (temp.second >= 60)
	{
		temp.minute += temp.second/60;
		temp.second%=60;
	}

	if (temp.minute >= 60)
	{
		temp.hour += temp.minute / 60;
		temp.minute %= 60;
	}

	if (temp.hour >= 24)
	{
		temp.day += temp.hour / 24;
		temp.hour %= 24;
	}

	while (temp.day > MONTH[temp.type_of_year()][temp.month])
	{
		temp.day -= MONTH[temp.type_of_year()][temp.month];
		temp.day++;
		temp.month++;
		if (temp.month > 12) 
		{
			temp.month = 1;
			temp.year++;
			if (temp.year > YEAR_MAX)
				temp.year = YEAR_MIN;
		}
	}

	//while (n > 0)
	//{
	//	temp++;
	//	n--;
	//}
	return temp;
}
DateTime DateTime::operator+(int n)
{
	return *this+(long long int)n;
}
DateTime DateTime::operator+(long int n)
{
	return *this + (long long int)n;
}

DateTime operator+(int n, DateTime& dt)
{
	return dt + n;
}
DateTime operator+(long int n, DateTime& dt)
{
	return dt + n;
}
DateTime operator+(long long int n, DateTime& dt)
{
	return dt + n;
}

DateTime DateTime::operator-(long long int n)
{
	return *this+(-n);
}
DateTime DateTime::operator-(int n)
{
	return *this-(long long int)n;
}
DateTime DateTime::operator-(long int n)
{
	return *this - (long long int)n;
}

DateTime::DateTime(long long int n)
{
	//会先调用基类构造，不用初始化
	/*year = YEAR_MIN;
	month = 1;
	day = 1;
	hour = HOUR_MIN;
	minute = MIN_MIN;
	second = SEC_MIN;*/
	*this = *this + n;
}

DateTime::operator long long int()
{
	long long int n = 0;
	for (int y = YEAR_MIN; y < year; ++y)
		n += ::year_long(y) * TIME_PERIOD;
	for (int m = 1; m < month; ++m)
		n += MONTH[type_of_year()][m] * TIME_PERIOD;
	n += (day - 1) * TIME_PERIOD;
	n += hour * 3600;
	n += minute * 60;
	n += second;
	
	return n;
}

long long int DateTime::operator-(DateTime& dt)
{
	long long int temp1 = *this;
	long long int temp2 = dt;
	long long int temp = temp1 - temp2;
	return temp;
}

ostream& operator<<(ostream& out, const DateTime& dt)
{
	out << setfill('0') << setw(4) << dt.year << "-" << setw(2) << dt.month << "-" << setw(2) << dt.day << ' ';
	out << setw(2) << dt.hour << ":" << setw(2) << dt.minute << ":" << setw(2) << dt.second << setfill(' ') << endl;
	return out;
}

istream& operator>>(istream& in, DateTime& dt)
{
	/*in >> static_cast<Date&>(dt);
	in >> static_cast<Time&>(dt);*/
	if (in.fail() == 1)
		return in;
	DateTime temp = dt;
	in >> dt.year >> dt.month >> dt.day >> dt.hour >> dt.minute >> dt.second;
	if (in.fail() == 1)
	{
		dt = temp;
		return in;
	}
	if (dt.year < YEAR_MIN || dt.year>YEAR_MAX|| dt.month < 1 || dt.month>12 || dt.day < 1 || dt.day>MONTH[dt.type_of_year()][dt.month]
		|| dt.hour < HOUR_MIN || dt.hour>HOUR_MAX || dt.minute < MIN_MIN || dt.minute>MIN_MAX || dt.second < SEC_MIN || dt.second>SEC_MAX)
	{
		dt.year = YEAR_DEFAULT;
		dt.month = MONTH_DEFAULT;
		dt.day = DAY_DEFAULT;
		dt.hour = HOUR_DEFAULT;
		dt.minute = MIN_DEFAULT;
		dt.second = SEC_DEFAULT;
	}
	return in;
}

bool DateTime::operator==(const DateTime& dt)
{
	if ((Date::operator==(dt)) == true && (Time::operator==(dt)) == true)
		return true;
	else
		return false;
}

bool DateTime::operator!=(const DateTime& dt)
{
	if ((Date::operator!=(dt)) == true || (Time::operator!=(dt)) == true)
		return true;
	else
		return false;
}

bool DateTime::operator>(const DateTime& dt)
{
	if ((Date::operator>(dt)) == true)
		return true;
	else if ((Date::operator==(dt)) == true && (Time::operator>(dt)) == true)
		return true;
	else
		return false;
}

bool DateTime::operator>=(const DateTime& dt)
{
	if ((Date::operator>(dt)) == true)
		return true;
	else if ((Date::operator==(dt)) == true && (Time::operator>=(dt)) == true)
		return true;
	else
		return false;
}

bool DateTime::operator<(const DateTime& dt)
{
	if ((Date::operator<(dt)) == true)
		return true;
	else if ((Date::operator==(dt)) == true && (Time::operator<(dt)) == true)
		return true;
	else
		return false;
}

bool DateTime::operator<=(const DateTime& dt)
{
	if ((Date::operator<(dt)) == true)
		return true;
	else if ((Date::operator==(dt)) == true && (Time::operator<=(dt)) == true)
		return true;
	else
		return false;
}
