#include <iostream>
using namespace std;

int main()
{
	//输入
	double month[12];
	double sum = 0;
	for (int i = 0; i < 12; i++)
	{
		cin >> month[i];
		sum += month[i];
	}
	double average = sum / 12;
	//输出
	cout << "¥" << average << endl;
	return 0;
}