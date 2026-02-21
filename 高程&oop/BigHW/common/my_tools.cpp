/* 2350222 盛曦 计科 */
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "../include/my_tools.h"
using namespace std;

int tj_strcasecmp(const char* s1, const char* s2)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (s1 == NULL && s2 != NULL)
        return -1;
    if (s1 != NULL && s2 == NULL)
        return 1;
    if (s1 == NULL && s2 == NULL)
        return 0;
    const char* p1 = s1;
    const char* p2 = s2;
    char m1, m2;
    for (; !(*p1 == '\0' && *p2 == '\0'); p1++, p2++)//end+1
    {
        if (*p1 != *p2)
        {
            if (*p1 >= 'A' && *p1 <= 'Z')
            {
                m1 = *p1 - 'A' + 'a';
            }
            else
            {
                m1 = *p1;
            }
            if (*p2 >= 'A' && *p2 <= 'Z')
            {
                m2 = *p2 - 'A' + 'a';
            }
            else
            {
                m2 = *p2;
            }
            if (m1 != m2)
                return int(m1 - m2);
        }
    }
    return 0;
}

int strcmp_twoways(const char* const s1, const string s2, const bool is_case_sensitive)
{
	if (is_case_sensitive)
		return strcmp(s1, s2.c_str());
	else
        return tj_strcasecmp(s1, s2.c_str());
}

int strcmp_twoways(const string s1, const string s2, const bool is_case_sensitive)
{
    if (is_case_sensitive)
        return strcmp(s1.c_str(), s2.c_str());
    else
        return tj_strcasecmp(s1.c_str(), s2.c_str());
}

int iptonum(const string ip, unsigned int& c)
{
    int a[4] = { 0 };
    int flag = 0;
    c = 0;

    for (int j = 0; ip[j] != '\0'; j++)
    {
        if (ip[j] == '.')
        {
            flag++;
            continue;
        }
        if (ip[j] < '0' || ip[j]>'9')//有不是数字的部分
            return 0;
        if (flag > 3)//超过了4段（超过3个点）
            return 1;

        a[flag] = a[flag] * 10 + ip[j] - '0';
    }
    for (int j = 0; j < 4; j++)
    {
        if (a[j] > 255)//数字超上限
            return 2;
        c = c * 256 + a[j];
    }
    return 3;//成功转换
}

int tj_strchr(const char* str, const char ch)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str == NULL)
        return 0;
    const char* p1 = str;
    for (; *p1 != '\0'; p1++)//没有算\0
    {
        if (*p1 == ch)
        {
            return p1 - str + 1;
        }
    }
    return 0;
}


int get_effective_comment(char* buf, int& start, int& end)
{
    int i = 0;
    while (buf[i] == ' ' || buf[i] == '\t')
        i++;

    //检查多行注释的开始
    if (buf[i] == '/' && i + 1 < 1024 && buf[i + 1] == '*')
    {
        if (i + 2 == 1024 || buf[i - 2] == '\0')
            return -2;//首行多行注释格式不正确
        start = i + 2;
        while (i + 1 < 1024 && (buf[i] != '*' || buf[i + 1] != '/'))
            i++;
        if (i + 1 == 1024)
            return -2;//首行多行注释格式不正确
        else
            end = i - 1;

        while (buf[start] == ' ' || buf[start] == '\t')
            start++;
        while (buf[end] == ' ' || buf[end] == '\t')
            end--;
        if (end <= start)
            return -4;//首行不是三项
    }
    //检查单行注释的开始
    else if (buf[i] == '/' && i + 1 < 1024 && buf[i + 1] == '/')
    {
        //注释没有内容
        if (i + 2 == 1024 || buf[i - 2] == '\0')
            return -4;//首行不是三项

        start = i + 2;
        while (buf[start] == ' ' || buf[start] == '\t')
            start++;

        //跳过尾部空格和制表符
        end = start;
        while (buf[end] != '\0')
            end++;//先让end指向字符串末尾
        while (buf[end] == ' ' || buf[end] == '\t' || buf[end] == '\0')
            end--;

        if (end <= start)
            return -4;

    }
    else
        return -7;//首行多行注释格式不正确
    return 0;
}

int divide_comment(string content, vector <string>& divided_content)
{
    //按空格拆分内容并计数
    bool in_space = false;
    int space_count = 0;
    vector <int> space_start, space_end;
    for (unsigned int k = 0; k < content.length(); k++)
    {
        if (content[k] == ' ' || content[k] == '\t')
        {
            if (!in_space)
            {
				space_start.push_back(k);
                space_count++;
                in_space = true;
            }
        }
        else
        {
            if (in_space)
            {
				space_end.push_back(k-1);
                in_space = false;
            }
        }
    }

	if (space_count == 0)
	{
		divided_content.push_back(content);
	}
	else
    {
        int i;
        divided_content.push_back(content.substr(0, space_start[0]));//第一段单独处理
        for (i = 0; i < space_count - 1; i++)//space_count+1表示有几段,中间有space_count-1段
            divided_content.push_back(content.substr(space_end[i] + 1, space_start[i + 1] - space_end[i] - 1));
        divided_content.push_back(content.substr(space_end[i] + 1));//最后一段单独处理
    }

    return space_count;
}