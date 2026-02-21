/* 2350222 盛曦 计科 */

#pragma once

#include <iostream>
using namespace std;

/* 补全TString类的定义，所有成员函数均体外实现，不要在此处体内实现 */
class TString {
	private:
		char *content;
		int   len;
		/* 根据需要定义所需的数据成员、成员函数、友元函数等 */
	public:
		/* 根据需要定义所需的数据成员、成员函数、友元函数等 */
		TString();
		TString(const char* s);
		TString(const TString& s);
		~TString();

		TString& operator=(const TString& s);

		friend istream& operator>>(istream& in, TString& s);
		friend ostream& operator<<(ostream& out, const TString& s);

		const char* c_str() const;

		TString& operator+=(const char* s) &;
		TString& operator+=(const TString& s) &;
		TString& operator+=(const char c) &;

		TString& append(const char* s);
		TString& append(const TString& s);
		TString& append(const char c);

		TString operator+(const char* s) const;
		TString operator+(const TString& s) const;
		TString operator+(const char c) const;
		friend TString operator+(const char* s, const TString& ts);
		friend TString operator+(const char c, const TString& ts);

		TString operator-(const char* s) const;
		TString operator-(const TString& s) const;
		TString operator-(const char c) const;

		TString& operator-=(const char* s) &;
		TString& operator-=(const TString& s) &;
		TString& operator-=(const char c)&;

		TString operator*(const int n) const;
		TString& operator*=(const int n)&;

		TString operator!() const;

		bool operator==(const TString& s) const;
		bool operator==(const char* s) const;
		bool operator!=(const TString& s) const;
		bool operator!=(const char* s) const;
		bool operator>(const TString& s) const;
		bool operator>(const char* s) const;
		bool operator>=(const TString& s) const;
		bool operator>=(const char* s) const;
		bool operator<(const TString& s) const;
		bool operator<(const char* s) const;
		bool operator<=(const TString& s) const;
		bool operator<=(const char* s) const;

		int length() const;
		friend int TStringLen(const TString& s);

		char& operator[](int i);
		const char& operator[](int i) const;
};

/* 如果有其它全局函数需要声明，写于此处 */

/* 如果有需要的宏定义、只读全局变量等，写于此处 */
