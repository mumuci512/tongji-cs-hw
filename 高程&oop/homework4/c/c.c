#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main()
{
    char mine[3][4] = { '\0' };
    scanf("%c ", &mine[1][2]);
    scanf("%c ", &mine[1][3]);
    printf("%c", mine[1][2]);

    printf("%c", mine[1][3]);
    return 0;
}
