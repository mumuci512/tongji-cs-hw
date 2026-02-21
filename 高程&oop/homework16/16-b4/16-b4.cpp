/* 2350222 盛曦 计科 */
#include <iostream>
#include "16-b4.h"
using namespace std;

/* 给出 Date 类的所有成员函数的体外实现 */
int Date::type_of_year()
{
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
		return LEAP_YEAR;
	else
		return COMMON_YEAR;
}

Date::Date()
{
	year = YEAR_DEFAULT;
	month = MONTH_DEFAULT;
	day = DAY_DEFAULT;
}

Date::Date(const int y, const int m, const int d)
{
	if (y >= YEAR_MIN && y <= YEAR_MAX)
		year = y;
	else
		year = YEAR_DEFAULT;
	
	if (m >= 1 && m <= 12)
		month = m;
	else
		month = MONTH_DEFAULT;

	if (d >= 1 && d <= MONTH[type_of_year()][month])
		day = d;
	else
		day = DAY_DEFAULT;
}

int Date::set(const int y, const int m, const int d)
{
	if (y != 0)
	{
		if (y >= YEAR_MIN && y <= YEAR_MAX)
			year = y;
		else
			year = YEAR_DEFAULT;
	}
	
	if (m != 0)
	{
		if (m >= 1 && m <= 12)
			month = m;
		else
			month = MONTH_DEFAULT;
	}
	
	if (d != 0)
	{
		if (d >= 1 && d <= MONTH[type_of_year()][month])
			day = d;
		else
			day = DAY_DEFAULT;
	}
	else
	{
		if (day < 1 || day > MONTH[type_of_year()][month])//若不要求改，但是不合法，还是要置1
			day = DAY_DEFAULT;
	}

	return 0;
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
	cout << year << "年" << month << "月" << day << "日"<<endl;
	return 0;
}

Date& Date::operator++()//前缀
{
	if (year == YEAR_MAX && month == 12 && day == 31)
		return *this;
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
	if (year == YEAR_MAX && month == 12 && day == 31)
		return *this;
	Date temp = *this;
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
		return *this;
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
	if (year == YEAR_MIN && month == 1 && day == 1)
		return *this;
	Date temp = *this;
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
	if (n >= 73049)
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
			*this = *this + n-1;
	}
}

Date::operator int()
{
	int n = 1;
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
	out << d.year << "年" << d.month << "月" << d.day << "日";
	return out;
}

istream& operator>>(istream& in,Date& d)
{
	if (in.fail() == 1)
		return in;
	Date temp=d;
	in >> d.year >> d.month >> d.day;
	if (in.fail() == 1)
	{
		d = temp;
		return in;
	}
	if (d.year < YEAR_MIN || d.year>YEAR_MAX)
	{
		d.year = YEAR_DEFAULT;
	}
	if (d.month < 1 || d.month>12)
	{
		d.month = MONTH_DEFAULT;
	}
	if (d.day < 1 || d.day>MONTH[d.type_of_year()][d.month])
	{
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
		return year>d.year;
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
/* 如果有需要的其它全局函数的实现，可以写于此处 */


//思考题
/*
可以省略成员函数Date operator+(int n);友元函数friend Date operator+(int n, Date& d);成员函数Date operator-(int n);
因为删去那四行cout后，所有Date类和int型数字的加减法的结果都会通过=号被直接赋值给一个Date类对象，所以在计算时，可以通过Date转int的类型转换
函数现将加减法中的Date类转为int型，再做int的加减法，把得到的int型结果再转为Date类对象，赋值给=号的左值，不需要上面省略的三个函数
但是假如加上了那四行cout，Date类和int型做完加减法后的结果没有被直接赋给一个Date对象，而是作为一个临时变量被cout输出，这时就不会把临时结果
int转Date，而是直接输出int型结果，因为这样更加直接。因此需要上面三个函数，用重载函数的形参列表绝对匹配的方式，让编译器知道应该把计算结果转
为Date类对象，并用<<重载输出。
*/