#include <iostream>
#include <cstring>
using namespace std;
#define N 999
//全局变量防止函数使用堆栈过多
char str[N][256] = { '\0' };
long long int num[N] = { 0 };

int main()
{
	cin.get(str[0], 255, '\r');
	//cin.ignore();
	while (getchar() != '\n');
	int i = 0;
	while (strcmp(str[i],"#")!=0)
	{
		i++;
		if (i >= N)
		{
			cout << "输入行数过多" << endl;
			break;
		}
		cin.get(str[i], 255, '\r');
		//cin.ignore();
		while (getchar() != '\n');
	}
	for (int j = 0; j < i; j++)
	{
		for (int k = 0; str[j][k] != '\0'&& k < 255; k++)
		{
			if (str[j][k] != ' ')
			{
				num[j] += (str[j][k] - 'A' + 1)*(k+1);
			}
		}
		cout << num[j] << endl;
	}
	return 0;
}