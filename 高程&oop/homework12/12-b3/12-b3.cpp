/* 2350222 盛曦 计科 */
#include <iostream>
using namespace std;
int main()
{
	cout << "请输入月份(1-12)" << endl;
	int m;
	const char *month[12] = { "January","February","March","April","May","June","July","August","September","October","November","December" };
	cin >> m;
	if (cin.good() == 1 && m >= 1 && m <= 12)
		cout << month[m - 1] << endl;
	else
		cout << "Invalid" << endl;
	return 0;
}