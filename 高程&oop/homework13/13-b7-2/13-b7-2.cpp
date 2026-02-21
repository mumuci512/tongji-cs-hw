#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstring>
using namespace std;

int main()
{
    int p[10] = { 0,1,2,3,4,5,6,7,8,9 };
    p[10] = 33;    //此句越界1
    p[14] = 65;    //此句越界
    p[15] = 66;    //此句越界
    p[10] = -858993460;//此句越界2
    cout << "addr:" << hex << (void*)(p) << endl;
    for (int i = -4; i < 16; i++) //注意，只有0-9是合理范围，其余都是越界读
        cout  << (void*)(p + i) << ":" << int(p[i]) << endl;
    delete[]p;//3

    return 0;
}
