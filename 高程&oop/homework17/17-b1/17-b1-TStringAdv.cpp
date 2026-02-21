/* 2350222 Ê¢êØ ¼Æ¿Æ */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
#if (defined(__linux) || defined(__linux__))
#include <climits>
#endif
#include "17-b1-TStringAdv.h"
using namespace std;

TStringAdv::TStringAdv():TString()
{
}
TStringAdv::TStringAdv(const char* s) :TString(s)
{
}
TStringAdv::TStringAdv(const TString& s) : TString(s)
{
}

TStringAdv& TStringAdv::assign(const TStringAdv& ts2)
{
	/*len = ts2.len;
	if (len != 0)
	{
		if (content != NULL)
			delete[] content;
		content = new(nothrow) char[ts2.len + 1];
		if (content == NULL)
		{
			cout << "OVERFLOW" << endl;
			return *this;
		}
		strcpy(content, ts2.content);
		len = ts2.len;
	}
	else
		content = NULL;*/
	*this = ts2;
	return *this;
}

TStringAdv& TStringAdv::assign(const char* s)
{
	/*if (s == nullptr || s == NULL || *s == '\0')
	{
		if (content != NULL)
			delete[] content;
		content = NULL;
		len = 0;
	}
	else
	{
		len = strlen(s);
		if (content != NULL)
			delete[] content;
		content = new(nothrow) char[len + 1];
		if (content == NULL)
		{
			cout << "OVERFLOW" << endl;
			return *this;
		}
		strcpy(content, s);
	}*/
	*this = s;
	return *this;
}

TStringAdv& TStringAdv::append(const TStringAdv& ts2)
{
	if (ts2.len == 0)
		return *this;
	char* t = new(nothrow) char[len + ts2.len + 1];
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
	memcpy(t + len, ts2.content, ts2.len);
	t[len + ts2.len] = '\0';
	content = t;
	len += ts2.len;
	return *this;
}

TStringAdv& TStringAdv::append(const char* s)
{
	if (s == nullptr || s == NULL || *s == '\0')
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

TStringAdv& TStringAdv::append(const char& c)
{
	if (c == '\0')
		return *this;
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
	return *this;
}

TStringAdv& TStringAdv::insert(const TStringAdv& ts2, int pos)
{
	if (ts2.content == NULL||(pos<1||pos>len+1))
		return *this;
	if (content == NULL)
	{
		*this = ts2;
		return *this;
	}

	int l = ts2.len;
	int nl = len + l;
	char* t = new(nothrow) char[nl+1];
	pos--;
	if (t == NULL)
	{
		cout << "OVERFLOW" << endl;
		return *this;
	}
	memcpy(t, content, pos);
	memcpy(t+pos, ts2.content, l);
	memcpy(t+pos+l, content+pos, len - pos+ 1);
	delete[] content;
	content = t;
	len = nl;
	return *this;
}

TStringAdv& TStringAdv::insert(const char* s, int pos)
{
	if (s == NULL || (pos<1 || pos>len + 1))
		return *this;
	int l = strlen(s);
	if (content == NULL)
	{
		content = new(nothrow) char[l + 1];
		if (content == NULL)
		{
			cout << "ONVERFLOW" << endl;
			return *this;//?????????????????????????????????????????????????
		}
		memcpy(content, s, l + 1);
		len = l;
		return *this;
	}

	int nl = len + l;
	char* t = new(nothrow) char[nl + 1];
	pos--;
	if (t == NULL)
	{
		cout << "OVERFLOW" << endl;
		return *this;
	}
	memcpy(t, content, pos);
	memcpy(t + pos, s, l);
	memcpy(t + pos + l, content + pos, len - pos + 1);
	delete[] content;
	content = t;
	len = nl;
	return *this;
}

TStringAdv& TStringAdv::insert(const char& c, int pos)
{
	if (pos<1 || pos>len + 1)
		return *this;
	if (content == NULL)
	{
		content = new(nothrow) char[1 + 1];
		if (content == NULL)
		{
			cout << "ONVERFLOW" << endl;
			return *this;//?????????????????????????????????????????????????
		}
		content[0] = c;
		content[1] = '\0';
		len = 1;
		return *this;
	}
	
	if (c == '\0')
	{
		if (pos == 1)
		{
			if (content != NULL)
			{
				delete[] content;
				content = NULL;
			}
			len = 0;
			return *this;
		}
		pos--;
		char* t = new(nothrow) char[pos+1];
		if (t == NULL)
		{
			cout << "OVERFLOW" << endl;
			return *this;
		}
		memcpy(t,content, pos);
		t[pos] = '\0';
		delete[] content;
		content = t;
		len = pos;
		return *this;
	}


	int l = 1;
	int nl = len + l;
	char* t = new(nothrow) char[nl + 1];
	pos--;
	if (t == NULL)
	{
		cout << "OVERFLOW" << endl;
		return *this;
	}
	memcpy(t, content, pos);
	t[pos]=c;
	memcpy(t + pos + l, content + pos, len - pos + 1);//°üº¬\0
	delete[] content;
	content = t;
	len = nl;
	return *this;
}

TStringAdv& TStringAdv::erase(const TStringAdv& ts2)
{
	*this -= ts2;
	return *this;
}

TStringAdv& TStringAdv::erase(const char* s)
{
	*this -= s;
	return *this;
}

TStringAdv& TStringAdv::erase(const char& c)
{
	*this -= c;
	return *this;
}

TStringAdv TStringAdv::substr(const int pos, const int len)
{
	if (pos<1 || pos>this->len)
		return TStringAdv();
	long long int l=len;
	if (len <= 0)
		return TStringAdv();
	else if (pos + l > this->len+1)
		l = this->len - pos + 1;

	TStringAdv t;
	t.content = new(nothrow) char[int(l) + 1];
	if (t.content == NULL)
	{
		cout << "OVERFLOW" << endl;
		return TStringAdv();
	}

	memcpy(t.content, content + pos - 1, int(l));
	t.content[l] = '\0';
	t.len = int(l);
	return t;
}

char& TStringAdv::at(const int n)
{
	return content[n];
}

int TStringAdv::TStringAdvLen()
{
	return len;
}
int TStringAdvLen(const TStringAdv& s)
{
	return s.len;
}