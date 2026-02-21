#include <time.h>
#include <windows.h>
#include <cmath>

void prime(short &p)
{
	srand((unsigned int)(time(0)));
	short a;
	long long int pr;
	int flag;
	do
	{
		flag = 0;
		do
		{
			p = rand();
		} while (p == 0);
		for (int i = 0; i < 100; i++)
		{
			a = rand();
			pr = pow(a, p - 1);
			pr %= p;
			if (pr != 1)
				flag=1;
		}
	} while (flag == 1);
}