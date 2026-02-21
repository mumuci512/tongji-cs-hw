/* 计科 2350222 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int p[10] = { 0,1,2,3,4,5,6,7,8,9};
    p[10] = 33;    //此句越界1
    p[14] = 65;    //此句越界
    p[15] = 66;    //此句越界
    p[10] = -858993460; //此句越界2
    printf("addr:%p\n", p);
    for (int i = -4; i < 16; i++) //注意，只有0-9是合理范围，其余都是越界读
        printf("%p:%02d\n", (p + i), p[i]);
    free(p);
    return 0;
}
