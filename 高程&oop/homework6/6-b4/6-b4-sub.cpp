/* 2350222 盛曦 信12 */

#include <cstdio>   //NULL
//不再允许包含任何系统头文件

/* ----- 不允许定义任何形式的全局变量/全部数组/只读全局变量/宏定义!!!!! ----- */

/* 函数实现部分，{ }内的东西可以任意调整，目前的return只是一个示例，可改变 */
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strlen(const char *str)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str == NULL)
        return 0;
    const char *p = str;
    while (*p != '\0')
    {
        p++;
    }
    return (p-str);
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
char *tj_strcat(char *s1, const char *s2)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (s1 == NULL||s2==NULL)
        return s1;
    char *p1 = s1;
    const char *p2 = s2;
    for (; *p1 != '\0'; p1++)
        ;
    for (; *p2!='\0'; p2++,p1++)
    {
        *p1 = *p2;
    }
    *p1 = '\0';
    return s1;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
char *tj_strncat(char *s1, const char *s2, const int len)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (s1 == NULL || s2 == NULL)
        return s1;
    char* p1 = s1;
    const char* p2 = s2;
    int len1 = tj_strlen(s1);
    int len2 = tj_strlen(s2);
    if (len < len2 + 1)
    {
        for (p1+=len1; p1<s1+len1+len; p1++,p2++)//循环执行len次
            *p1=*p2;
        *p1='\0';
    }
    else
        tj_strcat(s1, s2);
    return s1;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
char *tj_strcpy(char *s1, const char *s2)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (s1 == NULL)
    {
        return s1;
    }
    if (s2 == NULL)
    {
        *s1 = '\0';
        return s1;
    }
    char* p1 = s1;
    const char* p2 = s2;
    for (; *p2!='\0'; p1++,p2++)
    {
        *p1=*p2;
    }
    *p1 = '\0';
    return s1;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
char *tj_strncpy(char *s1, const char *s2, const int len)
{
    if (s2 == NULL || s1==NULL)
        return s1;
    char* p1 = s1;
    const char* p2 = s2;
    int len2 = tj_strlen(s2);
    int end = (len > len2 ? len2 : len);
    for (; p1<s1+end; p1++, p2++)
    {
        *p1 = *p2;
    }
    return s1;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strcmp(const char *s1, const char *s2)
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
    for (; !(*p1=='\0'&&*p2=='\0'); p1++, p2++)//end+1
    {
        if (*p1!=*p2)
            return int(*p1 - *p2);
    }
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strcasecmp(const char *s1, const char *s2)
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

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strncmp(const char *s1, const char *s2, const int len)
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
    int len1 = tj_strlen(s1);
    int len2 = tj_strlen(s2);
    int lenmin = len1 > len2 ? len2 : len1;
    int end = len > (lenmin + 1) ? (lenmin + 1) : len;
    for (; p1<s1+end; p1++,p2++)
    {
        if (*p1 != *p2)
            return int(*p1 - *p2);
    }
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strcasencmp(const char *s1, const char *s2, const int len)
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
    int len1 = tj_strlen(s1);
    int len2 = tj_strlen(s2);
    int lenmin = len1 > len2 ? len2 : len1;
    int end = len > (lenmin + 1) ? (lenmin + 1) : len;
    char m1, m2;
    for (; p1<s1+end; p1++,p2++)
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

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
char *tj_strupr(char *str)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str == NULL)
        return str;
    char* p1 = str;
    for (; *p1!='\0'; p1++)
    {
        if (*p1 >= 'a' && *p1 <= 'z')
            *p1 += ('A' - 'a');
    }
    return str;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
char *tj_strlwr(char *str)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str == NULL)
        return str;
    char* p1 = str;
    for (; *p1 != '\0'; p1++)
    {
        if (*p1 >= 'A' && *p1 <= 'Z')
            *p1 -= ('A' - 'a');
    }
    return str;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strchr(const char *str, const char ch)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str == NULL)
        return 0;
    const char* p1 = str;
    for (; *p1!='\0'; p1++)//没有算\0
    {
        if (*p1 == ch)
        {
            return p1-str+1;
        }
    }
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strstr(const char *str, const char *substr)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str == NULL || substr == NULL)
        return 0;
    const char* p1 = str;
    const char* p2 = substr;
    int len=0;
    for (; *p1 != '\0'; p1++)
    {
        if (*p1 == *p2)
        {
            len=p1-str;
            for (; *p2!='\0'; p2++,p1++)
            {
                if (*p1 != *p2)
                {
                    p1 = str + len;//不用+1，因为最外层的for会让p1++
                    p2 = substr;
                    break;
                }
            }
            if (*p2=='\0')
            {
                return len+1;
            }
        }
    }
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strrchr(const char *str, const char ch)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str == NULL)
        return 0;
    const char* p1;
    for (p1=str+ tj_strlen(str) -1; p1>=str; p1--)//没有算\0
    {
        if (*p1 == ch)
        {
            return p1-str + 1;
        }
    }
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int tj_strrstr(const char *str, const char *substr)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str == NULL || substr == NULL)
        return 0;
    const char* p1;
    const char* p2;
    int len = tj_strlen(str);
    int sublen = tj_strlen(substr);
    int dist = 0;
    for (p1 = str+len - 1,p2=substr+sublen-1; p1 >= str; p1--)
    {
        if (*p1 == *p2)
        {
            dist = p1 - str;
            for (; p2 >= substr; p1--,p2--)
            {
                if (*p1 != *p2)
                {
                    p1 = str + dist;
                    p2 = substr + sublen - 1;
                    break;
                }
            }
            if (p2<substr)
            {
                return dist-sublen+2;
            }
        }
    }
    return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
char *tj_strrev(char *str)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    if (str==NULL||*str == '\0')//先检查str是否为NULL再检查*str是否为\0，因为若str为NULL则*str运算会导致程序崩溃
        return str;
    char m;
    int len = tj_strlen(str);
    char* psrt = str;
    char* pend = str + len-1;
    for (; psrt<pend; psrt++,pend--)
    {
        m = *psrt;
        *psrt = *pend;
        *pend = m;
    }
    return str;
}
