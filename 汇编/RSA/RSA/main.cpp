#include <iostream>
#include <cmath>
#include <time.h>
#include <windows.h>
#include "head.h"
using namespace std;

int main()
{	
	srand((unsigned int)(time(0)));
	short p, q;//为了演示，把p和q限制在16比特范围内
	//生成pq
	prime(p);
	prime(q);

	int e, n,c,m,phi,d,k;

	//生成n、phi
	n = p * q;
	phi = (p - 1)* (q - 1);

	//生成e
	int cc;
	do
	{
		e = rand();
		if (e % 2 != 1)
			continue;
		gcd(e, phi, &cc);
	} while ( cc!= 1);

	//生成dk
	exgcd(e, phi, &d, &k);

	//输入错误处理
	cin >> m;
	while (!(m < n))
	{
		cout << "输入错误，请重新输入" << endl;
		cin >> m;
	}

	/*加密，将明文m加密生成密文c：（m < n）
		c ≡ me mod n*/

	//c可以不止一个吗？
	do
	{
		c = rand();
	} while (c % n != pow(m, e));
	

	return 0;
}