/* 2350222 Ê¢êØ ¼Æ¿Æ */
#pragma once

#include <string>
using namespace std;
int tj_strcasecmp(const char* s1, const char* s2);
int strcmp_twoways(const char* const s1,const string s2, const bool is_case_sensitive);
int strcmp_twoways(const string s1, const string s2, const bool is_case_sensitive);
int iptonum(const string ip, unsigned int& c);
int tj_strchr(const char* str, const char ch);
int get_effective_comment(char* buf, int& start, int& end);
int divide_comment(string content, vector <string>& divided_content);