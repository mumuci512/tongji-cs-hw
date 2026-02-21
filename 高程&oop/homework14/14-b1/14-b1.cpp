/* 2350222 盛曦 计科 */
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
using namespace std;

#define N 10

void output(short on_off_switch,const char* prompt=NULL)
{
	if (prompt == NULL)
		cout << "当前状态";
	else
		cout << prompt;
	cout <<"：0x"<< hex <<setw(4)<<setfill('0')<< on_off_switch << endl;
	cout << setfill(' ');

	int i;
	for (i = 0; i < N; i++)
		cout << setiosflags(ios::left)<<setw(4) << char('A' + i);
	cout << resetiosflags(ios::left)<<endl;
	for (i = 0; i < N; i++)
		cout << (on_off_switch&int(pow(2,i))?"ON  ":"OFF ");
	cout << endl<<endl;
}

void input(int& bit,int& on_off)
{
	int loop = 1;
	char bit1;
	int i;
	char t=0;
	char on_off1[4] = { '\0' };
	while(loop)
	{
		cout << "请以(\"A On /" << char('A' + N - 1) << " Off\"形式输入，输入\"Q on/off\"退出)" << endl;
		//char t = getchar();
		//int flag = 0;
		////flag=0:判断a-j	1:代表判断输入on/of		2:判断第二个f	3:判断最后的空格/正确		4：错误
		////我想把a-j的输入和on/off输入放在一起判断
		//while (t =getchar())
		//{
		//	if (t == ' ')
		//		continue;
		//	if (t == '\n' || t == '\r')
		//	{
		//		if (flag == 3)
		//			loop = 0;
		//		else
		//			loop = 1;
		//		break;
		//	}

		//	if (flag == 0)
		//	{
		//		if ((t >= 'A' && t <= 'A' + N - 1) || t == 'Q')
		//		{
		//			flag = 1;
		//			bit = t - 'A';
		//			continue;
		//		}
		//		else
		//		{
		//			loop = 1;
		//			break;
		//		}
		//	}
		//	else if (flag == 1)
		//	{
		//		if (t == 'o' || t == 'O')
		//		{
		//			t = getchar();
		//			if (t == 'n' || t == 'N')
		//			{
		//				on_off = 1;
		//				flag = 3;
		//				continue;
		//			}
		//			else if (t == 'f' || t == 'F')
		//			{
		//				on_off = 0;
		//				flag = 2;
		//				continue;
		//			}
		//			else
		//			{
		//				loop = 1;
		//				break;
		//			}
		//		}
		//	}
		//	else if (flag == 2)
		//	{
		//		if (t == 'f' || t == 'F')
		//		{
		//			flag = 3;
		//			continue;
		//		}
		//		else
		//		{
		//			loop = 1;
		//			break;
		//		}
		//	}
		//	else//flag=3
		//	{
		//		if (t == ' ')
		//			continue;
		//		else
		//		{
		//			loop = 1;
		//			break;
		//		}
		//	}
		//}//end of while (t != '\n' && t != '\r')
		cin >> bit1;
		if(bit1>='a'&&bit1<='z')
			bit1 += 'A' - 'a';//转大写
		//getchar();//吃掉回车

		i = 0;
		while (1)
		{
			t = getchar();
			if (t == ' ')
				continue;
			else if (t == '\n' || t == '\r')
			{
				if (on_off1[0] == '\0')//还未输入
					continue;
				else
					break;
			}
			else if(i<3)
			{
				if (t >= 'a' && t <= 'z')
					t += 'A' - 'a';//转大写
				on_off1[i] = t;
				i++;
			}
		}

		if (((bit1 >= 'A' && bit1 <= 'A' + N - 1)&&(strcmp(on_off1, "ON") == 0 || strcmp(on_off1, "OFF") == 0))
			|| bit1 == 'Q')
		{
			bit = bit1 - 'A';
			if (strcmp(on_off1, "ON") == 0)
				on_off = 1;//on
			else
				on_off = 0;//off
			loop = 0;
		}
		else
			loop = 1;

		continue;
	}
}

int main()
{
	short on_off_switch = 0x0000;
	output(on_off_switch, "初始状态");

	int bit, on_off;
	while(1)
	{
		input(bit, on_off);
		if (bit == 'Q' - 'A')
			break;
		if(on_off==1)//on
			on_off_switch = on_off_switch | int(pow(2, bit));
		else//off
			on_off_switch = on_off_switch & (~int(pow(2, bit)));
		output(on_off_switch);
	}
	return 0;
}
