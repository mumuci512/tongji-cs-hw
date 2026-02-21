/* 2350222 盛曦 计科 */
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <time.h>
/* 如果有需要，此处可以添加头文件 */
#include <cmath>

using namespace std;

/* 允许定义常变量/宏定义，但不允许定义全局变量 */
#define M 3//3个玩家
#define N 17//3人斗地主，每人17张牌

/* 可以添加自己需要的函数 */

/***************************************************************************
  函数名称：
  功    能：打印某个玩家的牌面信息，如果是地主，后面加标记
  输入参数：
  返 回 值：
  说    明：
 ***************************************************************************/
int print(const char* prompt, const bool landlord, const unsigned long long player)
{
	/* 只允许定义不超过三个基本类型的简单变量，不能定义数组变量、结构体、string等 */
	cout << prompt;

	for (int i = 0; i < 54; i++)
	{
		if (player & (unsigned long long)pow(2, i))
		{
			if (i < 52)
			{
				if (i % 4 == 0)
					cout << "";//梅花
				else if (i % 4 == 1)
					cout << "";//方片
				else if (i % 4 == 2)
					cout << "";//红桃
				else
					cout << "";//黑桃
				int t=(i / 4 + 3);
				if (t < 10)
					cout << t;
				else
				{
					switch (t)
					{
						case 10:
							cout << 'T';
							break;
						case 11:
							cout << 'J';
							break;
						case 12:
							cout << 'Q';
							break;
						case 13:
							cout << 'K';
							break;
						case 14:
							cout << 'A';
							break;
						case 15:
							cout << '2';
							break;
						default:
							break;
					}
				}
			}
			else if (i == 52)
				cout << "BJ";
			else
				cout << "RJ";
			cout << " ";
		}//END OF if (player & (unsigned long long)pow(2, i))
	}

	if (landlord)
		cout << "(地主)";

	cout << endl;

	return 0;
}

/***************************************************************************
  函数名称：
  功    能：发牌（含键盘输入地主）
  输入参数：
  返 回 值：
  说    明：
 ***************************************************************************/
int deal(unsigned long long* player)
{
	/* 只允许定义不超过十个基本类型的简单变量，不能定义数组变量、结构体、string等 */
	srand((unsigned int)(time(0)));
	int i,j;
	unsigned long long cards = (unsigned long long)pow(2,54)-1; //存放所有牌的信息，共用54位，每张牌用1位表示，1表示有牌，0表示无牌
	int c;//用于存放当前发放的牌
	int lord;//地主
	for (i = 0; i < N; i++)
	{
		for(j=0;j<M;j++)
		{
			do
			{
				c = rand() % 54;
			} while (!(cards & (unsigned long long)pow(2, c)));
			cards -= (unsigned long long)pow(2, c);
			player[j] += (unsigned long long)pow(2, c);
		}
		cout << "第" << i + 1 << "轮结束：" << endl;
		print("甲的牌：", false, player[0]);
		print("乙的牌：", false, player[1]);
		print("丙的牌：", false, player[2]);
	}
	cout << endl;
	while (1)
	{
		cout << "请选择一个地主[0-2]：" << endl;
		cin >> lord;
		if (lord >= 0 && lord <= 2 && cin.good() == 1)
			break;
		if (cin.good() == 0)
		{
			cin.clear();
			cin.ignore(1410065407, '\n');
		}
	}
	player[lord] += cards;
	return lord; //此处修改为选定的地主(0-2)
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：main函数，不准修改
 ***************************************************************************/
int main()
{
	unsigned long long player[3] = { 0 }; //存放三个玩家的发牌信息
	int landlord; //返回0-2表示哪个玩家是地主

	cout << "按回车键开始发牌";
	while (getchar() != '\n')
		;

	landlord = deal(player);
	print("甲的牌：", (landlord == 0), player[0]);
	print("乙的牌：", (landlord == 1), player[1]);
	print("丙的牌：", (landlord == 2), player[2]);

	return 0;
}