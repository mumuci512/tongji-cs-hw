/* 2350222 盛曦 计科 */
#include <iostream>
#include <iomanip>
#include "17-b2-time.h"
using namespace std;

/* --- 给出Time类的成员函数的体外实现(含友元及其它必要的公共函数)  --- */ 
Time::Time()
{
	hour = HOUR_DEFAULT;
	minute = MIN_DEFAULT;
	second = SEC_DEFAULT;
}

Time::Time(const int h, const int m, const int s)
{
	if ((h >= HOUR_MIN && h <= HOUR_MAX) && (m >= MIN_MIN && m <= MIN_MAX) && (s >= SEC_MIN && s <= SEC_MAX))
	{
		hour = h;
		minute = m;
		second = s;
	}
	else
	{
		hour = HOUR_DEFAULT;
		minute = MIN_DEFAULT;
		second = SEC_DEFAULT;
	}
}

int Time::set(const int h, const int m, const int s)
{
	if ((h >= HOUR_MIN && h <= HOUR_MAX) && (m >= MIN_MIN && m <= MIN_MAX) && (s >= SEC_MIN && s <= SEC_MAX))
	{
		hour = h;
		minute = m;
		second = s;
		return 0;
	}
	else
	{
		hour = HOUR_DEFAULT;
		minute = MIN_DEFAULT;
		second = SEC_DEFAULT;
		return 1;
	}
}

int Time::get(int& h, int& m, int& s) const
{
	h = hour;
	m = minute;
	s = second;
	return 0;
}

int Time::show() const
{
	cout << setfill('0') << setw(2) << hour << ":" << setw(2) << minute << ":" << setw(2) << second << setfill(' ') << endl;
	return 0;
}

Time& Time::operator++()//前缀
{
	if (hour == HOUR_MAX && minute == MIN_MAX && second == SEC_MAX)
	{
		hour = HOUR_MIN;
		minute = MIN_MIN;
		second = SEC_MIN;
		return *this;
	}
	if (second < SEC_MAX)
		second++;
	else
	{
		second = SEC_MIN;
		if (minute < MIN_MAX)
			minute++;
		else
		{
			minute = MIN_MIN;
			if (hour < HOUR_MAX)
				hour++;
			else
				hour = HOUR_MIN;
		}
	}
	return *this;
}

Time Time::operator++(int)//后缀
{
	Time temp = *this;
	if (hour == HOUR_MAX && minute == MIN_MAX && second == SEC_MAX)
	{
		hour = HOUR_MIN;
		minute = MIN_MIN;
		second = SEC_MIN;
		return temp;
	}
	if (second < SEC_MAX)
		second++;
	else
	{
		second = SEC_MIN;
		if (minute < MIN_MAX)
			minute++;
		else
		{
			minute = MIN_MIN;
			if (hour < HOUR_MAX)
				hour++;
			else
				hour = HOUR_MIN;
		}
	}
	return temp;
}

Time& Time::operator--()//前缀
{
	if (hour == HOUR_MIN && minute == MIN_MIN && second == SEC_MIN)
	{
		hour = HOUR_MAX;
		minute = MIN_MAX;
		second = SEC_MAX;
		return *this;
	}
	if (second > SEC_MIN)
		second--;
	else
	{
		second = SEC_MAX;
		if (minute > MIN_MIN)
			minute--;
		else
		{
			minute = MIN_MAX;
			if (hour > HOUR_MIN)
				hour--;
			else
				hour = HOUR_MAX;
		}
	}
	return *this;
}

Time Time::operator--(int)//后缀
{
	Time temp = *this;
	if (hour == HOUR_MIN && minute == MIN_MIN && second == SEC_MIN)
	{
		hour = HOUR_MAX;
		minute = MIN_MAX;
		second = SEC_MAX;
		return temp;
	}
	if (second > SEC_MIN)
		second--;
	else
	{
		second = SEC_MAX;
		if (minute > MIN_MIN)
			minute--;
		else
		{
			minute = MIN_MAX;
			if (hour > HOUR_MIN)
				hour--;
			else
				hour = HOUR_MAX;
		}
	}
	return temp;
}

Time Time::operator+(int n)
{
	if (n < -TIME_PERIOD || n > TIME_PERIOD)
		n %= TIME_PERIOD;
	Time temp = *this;
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

Time operator+(int n, Time& t)
{
	if (n < -TIME_PERIOD || n > TIME_PERIOD)
		n %= TIME_PERIOD;
	Time temp = t;
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

Time Time::operator-(int n)
{
	if (n < -TIME_PERIOD || n > TIME_PERIOD)
		n %= TIME_PERIOD;
	Time temp = *this;
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

Time::Time(int n)
{
	hour = HOUR_MIN;
	minute = MIN_MIN;
	second = SEC_MIN;
	*this = *this + n;
}

Time::operator int()
{
	int n = 0;
	Time temp = *this;
	while (temp.hour != HOUR_MIN || temp.minute != MIN_MIN || temp.second != SEC_MIN)
	{
		--temp;
		n++;
	}
	return n;
}

int Time::operator-(Time& t)
{
	int temp1 = *this;
	int temp2 = t;
	int temp = temp1 - temp2;
	return temp;
}

ostream& operator<<(ostream& out, const Time& t)
{
	out << setfill('0') << setw(2) << t.hour << ":" << setw(2) << t.minute << ":" << setw(2) << t.second << setfill(' ') << endl;
	return out;
}

istream& operator>>(istream& in, Time& t)
{
	if (in.fail() == 1)
		return in;
	Time temp = t;
	in >> t.hour >> t.minute >> t.second;
	if (in.fail() == 1)
	{
		t = temp;
		return in;
	}
	if (t.hour < HOUR_MIN || t.hour>HOUR_MAX || t.minute < MIN_MIN || t.minute>MIN_MAX || t.second < SEC_MIN || t.second>SEC_MAX)
	{
		t.hour = HOUR_DEFAULT;
		t.minute = MIN_DEFAULT;
		t.second = SEC_DEFAULT;
	}
	
	return in;
}

bool Time::operator==(const Time& t)
{
	if (hour == t.hour && minute == t.minute && second == t.second)
		return true;
	else
		return false;
}

bool Time::operator!=(const Time& t)
{
	if (hour != t.hour || minute != t.minute || second != t.second)
		return true;
	else
		return false;
}

bool Time::operator>(const Time& t)
{
	if (hour > t.hour)
		return true;
	else if (hour == t.hour && minute > t.minute)
		return true;
	else if (hour == t.hour && minute == t.minute && second > t.second)
		return true;
	else
		return false;
}

bool Time::operator>=(const Time& t)
{
	if (hour > t.hour)
		return true;
	else if (hour == t.hour && minute > t.minute)
		return true;
	else if (hour == t.hour && minute == t.minute && second >= t.second)
		return true;
	else
		return false;
}

bool Time::operator<(const Time& t)
{
	if (hour < t.hour)
		return true;
	else if (hour == t.hour && minute < t.minute)
		return true;
	else if (hour == t.hour && minute == t.minute && second < t.second)
		return true;
	else
		return false;
}

bool Time::operator<=(const Time& t)
{
	if (hour < t.hour)
		return true;
	else if (hour == t.hour && minute < t.minute)
		return true;
	else if (hour == t.hour && minute == t.minute && second <= t.second)
		return true;
	else
		return false;
}
