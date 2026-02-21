#include <iostream>
using namespace std;
#define N 100

int main()
{
	int n[N] = { 0 };
	int i = 0;
	cin >> n[i];
	while (n[i] != -1)
	{
		i++;
		cin >> n[i];
	}
	long long int male[N], female[N];
	long long int mm, mf;
	for (i = 0; n[i] != -1; i++)
	{
		male[i] = 0;
		female[i] = 1;
		for (int j = 0; j < n[i]; j++)
		{
			mm =male[i]+female[i];
			mf = male[i] + 1;
			male[i] = mm;
			female[i] = mf;
		}
		cout << male[i] << " " << male[i] + female[i] << endl;
	}
	return 0;
}