/* 2350222 盛曦 计科 */
#include <iostream>
#if (defined(__linux) || defined(__linux__))
#include <cstring>
#endif
#include "16-b6.h"
using namespace std;

int low_to_up(char* t)
{
	int i;
	for (i = 0; t[i] != '\0'; i++)
	{
		if (t[i] >= 'a' && t[i] <= 'z')
			t[i] = t[i] +'A' - 'a';
	}
	return 0;
}

ostream& operator<<(ostream& out, const week& w)
{
	switch (w)
	{
		case week::sun:
			out << "星期日";
			break;
		case week::mon:
			out << "星期一";
			break;
		case week::tue:
			out << "星期二";
			break;
		case week::wed:
			out << "星期三";
			break;
		case week::thu:
			out << "星期四";
			break;
		case week::fri:
			out << "星期五";
			break;
		case week::sat:
			out << "星期六";
			break;
		default:
			out << "错误";
			break;
	}
	return out;
}

istream& operator>>(istream& in, week& w)
{
	char temp[4] = { '\0' };
	char c = getchar();
	int i = 0;
	while (c != ' ' && c != '\n' && c != '\r')
	{
		if(i<3)
			temp[i] = c;
		i++;
		c = getchar();
	}
	if (i > 3)
	{
		w = week(-1);
		return in;
	}
	low_to_up(temp);

	if (strcmp(temp, "SUN") == 0)
		w = week::sun;
	else if (strcmp(temp, "MON") == 0)
		w = week::mon;
	else if (strcmp(temp, "TUE") == 0)
		w = week::tue;
	else if (strcmp(temp, "WED") == 0)
		w = week::wed;
	else if (strcmp(temp, "THU") == 0)
		w = week::thu;
	else if (strcmp(temp, "FRI") == 0)
		w = week::fri;
	else if (strcmp(temp, "SAT") == 0)
		w = week::sat;
	else
		w = week(-1);
	
	return in;
}

week& operator++(week& w)
{
	if (w != week::sat)
		w = week(int(w) + 1);
	else
		w = week::sun;

	return w;
}

week operator++(week& w, int)
{
	week temp = w;
	if (w != week::sat)
		w = week(int(w) + 1);
	else
		w = week::sun;
	return temp;
}

week& operator--(week& w)
{
	if (w != week::sun)
		w = week(int(w) - 1);
	else
		w = week::sat;
	return w;
}

week operator--(week& w, int)
{
	week temp = w;
	if (w != week::sun)
		w = week(int(w) - 1);
	else
		w = week::sat;
	return temp;
}

week operator+(week w, int n)
{
	n %= 7;
	if (n < 0)
		n += 7;
	week temp=w;
	for(int i=0;i<n;i++)
		++temp;
	return temp;
}

week operator-(week w, int n)
{
	n %= 7;
	if (n < 0)
		n += 7;
	week temp = w;
	for (int i = 0; i < n; i++)
		--temp;
	return temp;
}

week& operator+=(week& w, int n)
{
	w = w + n;
	return w;
}

week& operator-=(week& w, int n)
{
	w = w - n;
	return w;
}