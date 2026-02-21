/* 2350222 盛曦 计科 */

/* 允许添加需要的头文件、宏定义等 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#include "17-b1-TString.h"
using namespace std;

/* 给出 TString 类的所有成员函数的体外实现 */
TString::TString()
{
	content = NULL;
	len = 0;
}

TString::TString(const char* s)
{
	if (s == nullptr||s==NULL||*s=='\0')
	{
		content = NULL;
		len = 0;
	}
	else
	{
		len = strlen(s);
		content = new(nothrow) char[len + 1];
		if (content == NULL)
		{
			cout << "内存申请失败" << endl;
			return;
		}
		strcpy(content, s);
	}	
}

TString::TString(const TString& s)
{
	/*if (content != NULL)
		delete[] content;*/
	if (s.len != 0)
	{
		content = new(nothrow) char[s.len + 1];
		if (content == NULL)
		{
			cout << "OVERFLOW" << endl;
			return;
		}
		strcpy(content, s.content);
		len = s.len;
	}
	else
	{
		content = NULL;
		len = 0;
	}
}

TString::~TString()
{
	if (content != NULL)
		delete[] content;
}

TString& TString::operator=(const TString& s)
{
	if (this != &s)
	{
		if (content != NULL)
			delete[] content;
		if (s.len != 0)
		{
			content = new(nothrow) char[s.len + 1];
			if (content == NULL)
			{
				cout << "OVERFLOW" << endl;
				return *this;
			}
			strcpy(content, s.content);
			len = s.len;
		}
		else
		{
			content = NULL;
			len = 0;
		}
	}
	return *this;
}

istream& operator>>(istream& in, TString& s)
{
	if (s.content != NULL)
		delete[] s.content;
	s.len = 0;

	char c = getchar();
	char* t;
	while (c != ' ' && c != '\n' && c != '\r')
	{
		t = new(nothrow) char[s.len + 1];
		if (t == NULL)
		{
			cout << "OVERFLOW" << endl;
			return in;
		}
		if(s.content!=NULL)
			memcpy(t, s.content, s.len);
		t[s.len] = c;
		s.len++;

		s.content = new(nothrow) char[s.len+1];
		if (s.content == NULL)
		{
			cout << "OVERFLOW" << endl;
			return in;
		}

		memcpy(s.content, t, s.len);
		delete[] t;

		c = getchar();
	}
	if (s.content != NULL)
		s.content[s.len] = '\0';
	return in;
}

ostream& operator<<(ostream& out, const TString& s)
{
	if (s.content != NULL&&s.content!=nullptr)
		out << s.content;
	else
		out << "<EMPTY>";
	return out;
}

const char* TString::c_str() const
{
	if (content != NULL)
		return content;
	else
		return "<empty>";
}


TString& TString::operator+=(const char* s) &
{
	if (s == NULL || *s == '\0')
		return *this;
	int slen = strlen(s);
	char* t = new(nothrow) char[len + slen + 1];
	if (t == NULL)
	{
		cout << "OVERFLOW" << endl;
		return *this;
	}
	if (content != NULL)
	{
		memcpy(t, content, len);
		delete[] content;
	}
	memcpy(t + len, s, slen);
	t[len + slen] = '\0';
	content = t;
	len += slen;
	return *this;
}

TString& TString::operator+=(const TString& s) &
{
	if (s.len==0)
		return *this;
	char* t = new(nothrow) char[len + s.len + 1];
	if (t == NULL)
	{
		cout << "OVERFLOW" << endl;
		return *this;
	}
	if (content != NULL)
	{
		memcpy(t, content, len);
		delete[] content;
	}
	memcpy(t + len, s.content, s.len);
	t[len + s.len] = '\0';
	content = t;
	len += s.len;
	return *this;
}

TString& TString::operator+=(const char c) &
{
	if (c != '\0')
	{
		char* t = new(nothrow) char[len + 1 + 1];
		if (t == NULL)
		{
			cout << "OVERFLOW" << endl;
			return *this;
		}
		if (content != NULL)
		{
			memcpy(t, content, len);
			delete[] content;
		}
		t[len] = c;
		t[len + 1] = '\0';
		content = t;
		len += 1;
	}
	return *this;
}

TString TString::operator+(const char* s) const
{
	TString r = *this;
	r += s;
	return r;
}

TString TString::operator+(const TString& s) const
{
	TString r = *this;
	r += s.content;
	return r;
}

TString TString::operator+(const char c) const
{
	TString r = *this;
	r += c;
	return r;
}

TString operator+(const char* s, const TString& ts)
{
	TString r = s;
	r += ts;
	return r;
}

TString operator+(const char c, const TString& ts)
{
	TString r;
	if (c != '\0')
	{
		r.content = new(nothrow) char[2];
		r.content[0] = c;
		r.content[1] = '\0';
		r.len = 1;
	}
	r += ts;
	return r;
}

TString TString::operator-(const char* s) const
{
	if (s == NULL || *s == '\0'||s==nullptr)
		return *this;
	int i;
	int slen = strlen(s);
	for (i = 0; i < len; i++)
	{
		if (content[i] == *s)
		{
			if (strncmp(content + i, s, slen) == 0)
				break;
		}
	}

	if (i == len)
		return *this;
	else
	{
		TString r;
		r.content = new(nothrow) char[len - slen + 1];
		if (r.content == NULL)
		{
			cout << "OVERFLOW<" << endl;
			return *this;
		}
		r.len = len - slen;
		memcpy(r.content, content, i);
		memcpy(r.content+i, content + i + slen, len - slen-i);
		r.content[len - slen] = '\0';
		return r;
	}
}

TString TString::operator-(const TString& s) const
{
	if(s.len==0)
		return *this;
	int i;
	for (i = 0; i < len; i++)
	{
		if (content[i] == s.content[0])
		{
			if (strncmp(content + i, s.content, s.len) == 0)
				break;
		}
	}
	if (i == len)
		return *this;
	else
	{
		TString r;
		r.content = new(nothrow) char[len - s.len + 1];
		if (r.content == NULL)
		{
			cout << "OVERFLOW" << endl;
			return *this;
		}
		r.len = len - s.len;
		memcpy(r.content, content, i);
		memcpy(r.content+i, content + i + s.len, len - s.len - i);
		r.content[len - s.len] = '\0';
		return r;
	}
}

TString TString::operator-(const char c) const
{
	if(c=='\0')
		return *this;
	int i;
	for (i = 0; i < len; i++)
	{
		if (content[i] == c)
			break;
	}
	if (i == len)
		return *this;
	else
	{
		TString r;
		r.content = new(nothrow) char[len - 1 + 1];
		if (r.content == NULL)
		{
			cout << "OVERFLOW<" << endl;
			return *this;
		}
		r.len = len - 1;
		memcpy(r.content, content, i);
		memcpy(r.content+i, content + i + 1, len - 1 - i);
		r.content[len - 1] = '\0';
		return r;
	}
}

TString& TString::operator-=(const char* s) &
{
	*this = *this - s;
	return *this;
}

TString& TString::operator-=(const TString& s) &
{
	*this = *this - s;
	return *this;
}

TString& TString::operator-=(const char c) &
{
	*this = *this - c;
	return *this;
}

TString TString::operator*(const int n) const
{
	TString r;
	r.content = new(nothrow) char[len * n + 1];
	if (r.content == NULL)
	{
		cout << "OVERFLOW" << endl;
		return *this;
	}
	r.len = len * n;
	for (int i = 0; i < n; i++)
		memcpy(r.content + i * len, content, len);
	r.content[len * n] = '\0';
	return r;
}

TString& TString::operator*=(const int n) &
{
	*this = *this * n;
	return *this;
}

TString TString::operator!() const
{
	TString r;
	r.content = new(nothrow) char[len + 1];
	if (r.content == NULL)
	{
		cout << "OVERFLOW" << endl;
		return *this;
	}
	r.len = len;
	for (int i = 0; i < len; i++)
		r.content[i] = content[len - i-1];
	r.content[len] = '\0';
	return r;
}

bool TString::operator==(const TString& s) const
{
	if (content == NULL || s.content == NULL)
	{
		if (content == NULL && s.content == NULL)
			return true;
		else
			return false;
	}
	if (strcmp(content, s.content) == 0)
		return true;
	else
		return false;
}

bool TString::operator==(const char* s) const
{
	if (content == NULL || (s == NULL || *s == '\0'))
	{
		if (content == NULL && (s == NULL||*s=='\0'))
			return true;
		else
			return false;
	}
	if (strcmp(content, s) == 0)
		return true;
	else
		return false;
}

bool TString::operator!=(const TString& s) const
{
	if (content == NULL || s.content == NULL)
	{
		if (content == NULL && s.content == NULL)
			return false;
		else
			return true;
	}
	if (strcmp(content, s.content) == 0)
		return false;
	else
		return true;
}

bool TString::operator!=(const char* s) const
{
	if (content == NULL || (s == NULL || *s == '\0'))
	{
		if (content == NULL && (s == NULL || *s == '\0'))
			return false;
		else
			return true;
	}
	if (strcmp(content, s) == 0)
		return false;
	else
		return true;
}

bool TString::operator>(const TString& s) const
{
	if (content == NULL || s.content == NULL)
	{
		if (content != NULL && s.content == NULL)
			return true;
		else
			return false;
	}
	if (strcmp(content, s.content) > 0)
		return true;
	else
		return false;
}

bool TString::operator>(const char* s) const
{
	if (content == NULL || (s == NULL || *s == '\0'))
	{
		if (content != NULL && (s == NULL || *s == '\0'))
			return true;
		else
			return false;
	}
	if (strcmp(content, s) > 0)
		return true;
	else
		return false;
}

bool TString::operator>=(const TString& s) const
{
	if (content == NULL || s.content == NULL)
	{
		if (s.content==NULL)
			return true;
		else
			return false;
	}
	if (strcmp(content, s.content) >= 0)
		return true;
	else
		return false;
}

bool TString::operator>=(const char* s) const
{
	if (content == NULL || (s == NULL || *s == '\0'))
	{
		if ((s == NULL || *s == '\0'))
			return true;
		else
			return false;
	}
	if (strcmp(content, s) >= 0)
		return true;
	else
		return false;
}

bool TString::operator<(const TString& s) const
{
	if (content == NULL || s.content == NULL)
	{
		if (content == NULL && s.content != NULL)
			return true;
		else
			return false;
	}
	if (strcmp(content, s.content) < 0)
		return true;
	else
		return false;
}

bool TString::operator<(const char* s) const
{
	if (content == NULL || (s == NULL || *s == '\0'))
	{
		if (content == NULL && (s != NULL&&*s!='\0'))
			return true;
		else
			return false;
	}
	if (strcmp(content, s) < 0)
		return true;
	else
		return false;
}

bool TString::operator<=(const TString& s) const
{
	if (content == NULL || s.content == NULL)
	{
		if (content == NULL)
			return true;
		else
			return false;
	}
	if (strcmp(content, s.content) <= 0)
		return true;
	else
		return false;
}

bool TString::operator<=(const char* s) const
{
	if (content == NULL || (s == NULL || *s == '\0'))
	{
		if (content == NULL)
			return true;
		else
			return false;
	}
	if (strcmp(content, s) <= 0)
		return true;
	else
		return false;
}

int TString::length() const
{
	return len;
}

int TStringLen(const TString& s)
{
	return s.len;
}

char& TString::operator[](int i)
{
	/*if (i >= 0 && i < len)
		return content[i];
	else if(i<0)
	{
		cout << "下标超过范围，已返回下限" << endl;
		return content[0];
	}
	else
	{
		cout << "下标超过范围，已返回上限" << endl;
		return content[len - 1];
	}*/
	return content[i];
}

const char& TString::operator[](int i) const
{
	if (i >= 0 && i < len)
		return content[i];
	else if (i < 0)
	{
		cout << "下标超过范围，已返回下限" << endl;
		return content[0];
	}
	else
	{
		cout << "下标超过范围，已返回上限" << endl;
		return content[len - 1];
	}
}
/* 如果有需要的其它全局函数的实现，可以写于此处 */

