#include <iostream>
using namespace std;
#define N 4500

int Josephus(int n,int m,int medium,int nn)
{
	if (n == nn)
		return medium;
	else
		return (Josephus(n - 1, m,medium,nn) + m) % n;
}

//int JosephusPlus(int n, int m,int medium)
//{
//	if (n == N)
//		return medium;
//	else
//		return (JosephusPlus(n - 1, m,medium) + m) % n;
//}
//int JosephusPlusPlus(int n, int m, int medium)
//{
//	if (n == 2*N)
//		return medium;
//	else
//		return (JosephusPlusPlus(n - 1, m, medium) + m) % n;
//}

int main()
{
	int n, m;
	cin >> n >> m;
	int medium=0;
	int num = n / N;
	for (int i = 0; i < num; i++)
	{
		medium = Josephus(N * (i + 1), m, medium, N * i);
	}
	cout << Josephus(n, m, medium,num* N);
	/*if (n > N)
	{
		medium = Josephus(N, m,0,0);
		if (n > 2 * N)
		{
			medium=Josephus(2*N, m, medium,N);
			cout << Josephus(n, m, medium,2*N);
		}
		else
			cout << Josephus(n, m, medium,N);
	}
	else
		cout<< Josephus(n, m,0,0);*/

	return 0;
}