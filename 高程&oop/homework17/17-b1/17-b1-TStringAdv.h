/* 2350222 Ê¢êØ ¼Æ¿Æ */
#pragma once

#include "17-b1-TString.h"
class TStringAdv : public TString {
public:
	TStringAdv();
	TStringAdv(const char* s);
	TStringAdv(const TString& s);

	TStringAdv& assign(const TStringAdv& ts2);
	TStringAdv& assign(const char* s);

	TStringAdv& append(const TStringAdv& ts2);
	TStringAdv& append(const char* s);
	TStringAdv& append(const char& c);	

	TStringAdv& insert(const TStringAdv& ts2, int pos);
	TStringAdv& insert(const char* s, int pos);
	TStringAdv& insert(const char& c, int pos);

	TStringAdv& erase(const TStringAdv& ts2);
	TStringAdv& erase(const char* s);
	TStringAdv& erase(const char& c);

	TStringAdv substr(const int pos, const int len = INT_MAX);

	char& at(const int n);

	int TStringAdvLen();
	friend int TStringAdvLen(const TStringAdv& s);
};
int TStringAdvLen(const TStringAdv& s);