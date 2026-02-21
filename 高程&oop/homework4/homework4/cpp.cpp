#include <iostream>
using namespace std; 
int tj_strlen(const char str[])
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    int i=0;
    while (str[i] != '\0')
    {
        i++;
    }
    return i;
}
int tj_strcat(char s1[], const char s2[])
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    int i, j;
    for (i = 0; i <= tj_strlen(s1); i++)
    {
        if (s1[i] == '\0')
            break;
    }
    for (j = 0; j <= i + tj_strlen(s2); j++)
    {
        s1[i + j] = s2[j];
    }
    return 0; //return值可根据需要修改
}
int tj_strncat(char s1[], const char s2[], const int len)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    int i;
    int len1 = tj_strlen(s1);
    int len2 = tj_strlen(s2);
    if (len < len2 + 1)
    {
        for (i = 0; i < len; i++)
            s1[len1 + i] = s2[i];
        s1[len1 + len] = '\0';
    }
    else
        tj_strcat(s1, s2);
    return 0; //return值可根据需要修改
}
int tj_strcpy(char s1[], const char s2[])
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    int i;
    int len2 = tj_strlen(s2);
    for (i = 0; i < len2 + 1; i++)
    {
        s1[i] = s2[i];
    }
    return 0; //return值可根据需要修改
}int tj_strncpy(char s1[], const char s2[], const int len)
{
    /* 注意：函数内不允许定义任何形式的数组（包括静态数组） */
    int i;
    int len2 = tj_strlen(s2);
    int end = (len > len2 ? len2 : len);
    for (i = 0; i < end; i++)
    {
        s1[i] = s2[i];
    }
    return 0; //return值可根据需要修改
}
int main()
{
    char s1[80] = "abcdefghij";
    char s2[] = "abcde";
    char s3[] = "hello\0UVWXYZ";

    cout << "tj_strncat()测试部分：" << endl;
    cout << "1.s1的输出应该是abcdefghij，                实际是：" << s1 << endl;
    cout << "  s1的长度应该是10，                        实际是：" << tj_strlen(s1) << endl;

    tj_strncat(s1, s2, 3);
    cout << "2.s1的输出应该是abcdefghijabc，             实际是：" << s1 << endl;
    cout << "  s1的长度应该是13，                        实际是：" << tj_strlen(s1) << endl;

    tj_strncat(s1, s3, 100);
    cout << "3.s1的输出应该是abcdefghijabchello，        实际是：" << s1 << endl;
    cout << "  s1的长度应该是18，                        实际是：" << tj_strlen(s1) << endl;

    tj_strncat(s1, &s3[6], 6);
    cout << "4.s1的输出应该是abcdefghijabchelloUVWXYZ，实际是：" << s1 << endl;
    cout << "  s1的长度应该是24，                        实际是：" << tj_strlen(s1) << endl;
    return 0;
}
