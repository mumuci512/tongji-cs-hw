/* 2350222 计科 盛曦 */
#include <iostream>
#include <iomanip>
#include "../include/cmd_console_tools.h"
#include "../include/90-02-b1-with_magic_ball.h"
#include "90-02-b1.h"
using namespace std;

void input_prompt(int x,int y,struct location& pos)
{
	cout << "请以字母+数字形式[例：c2]输入矩阵坐标：";
	char input[2];
	struct location loc;
	//struct location loc1;
	while(1)
	{
		cct_getxy(loc.i, loc.j);
		// cin.get(input, 2, '\n');
		cin >> input[0] >> input[1];
		if (!(input[0] >= 'a' && input[0] < 'a' + x || input[0] >= 'A' && input[0] < 'A' + x) || !(input[1] >= '0' && input[1] < '0' + y ))
		{
			cct_setcursor(CURSOR_INVISIBLE);
			//cct_getxy(loc1.i, loc1.j);
			cct_gotoxy(loc.i, loc.j);
			//cout <<setw(loc1.i-loc.i)<<' '<< endl << "输入错误，请重新输入.";//loc1.i-loc.i是为了覆盖输入的所有东西
			cout << setw(50) << ' ' << endl << "输入错误，请重新输入.";
			cct_gotoxy(loc.i, loc.j);
			cct_setcursor(CURSOR_VISIBLE_NORMAL);
			continue;
		}
		else
		{
			if (input[0] >= 'a' && input[0] <= 'a' + x)
				input[0] += ('A' - 'a');
			cct_getxy(loc.i, loc.j);
			cout << setw(21) << ' ';//覆盖可能的“输入错误，请重新输入”一行字
			cct_gotoxy(loc.i, loc.j);
			cout << "输入为" << input[0] << "行" << input[1] << "列" << endl;
			pos.i = input[0] - 'A';
			pos.j = input[1] - '0';
			break;
		}
	}
}

void find_result(struct magic_ball& magic, struct location& pos)//将menuabc中的一大坨内容分出来
{
	int s = 0;
	while (1)
	{
		input_prompt(magic.x, magic.y, pos);
		int* p = &magic.ball[pos.i][pos.j];
		int* p1 = &magic.ball1[pos.i][pos.j];
		if (FIND_ELIMINATE(magic, p, p1, pos) == 0)//不可以将四个r的表达式直接放进if后括号里，否则会因为短路计算导致不执行
			cout << "输入的矩阵坐标位置处无连续相同值，请重新输入" << endl;
		else
		{
			//是否可以用output_init_array函数完成、
			cout << endl << "查找结果数组：" << endl;
			output_prompt(magic.x, magic.y, 1);
			int i, j;
			for (i = 0; i < magic.y; i++)
			{
				cout << char('A' + i) << " |";
				for (j = 0; j < magic.x; j++)
				{
					cout << "  ";
					if (magic.ball1[i][j] != 0)
						cout << "*";
					else
						cout << "0";
				}
				cout << endl;
			}
			cout << endl;

			cout << "当前数组(不同色标识)：" << endl;
			output_prompt(magic.x, magic.y, 1);
			output_array(magic, s);
			break;
		}
	}//end of while(1)
}

int eliminate_sth(struct magic_ball& magic, struct location& pos,char mode,int& score,int& s)
{
	int flag;//0表示break结束，1表示continue继续
	cout << "请确认是否把" << char(pos.i + 'A') << char(pos.j + '0') << "及周围的相同值消除(Y / N / Q)：";
	char sel = input_one_char("YyNnQq");
	if (sel == 'Q')
	{
		cout << endl;
		flag=0;
	}
	else if (sel == 'N')
	{
		if (mode == 'B')
		{
			cout << endl;
			flag = 0;
		}
		else
		{
			init_all(magic.ball1, 0);
			flag = 1;
		}
	}
	else if (sel == 'Y')
	{
		fall(magic, score, mode, 1);
		wait_for_enter("按回车键进行数组下落操作...");
		output_init_array(magic, "下落后的数组：", s, 1);
		if (mode == 'C')
		{
			wait_for_enter("本次消除结束，按回车键继续新一次的消除...");
			init_all(magic.ball1, 0);
			flag = 1;
		}
		else
			flag = 0;
	}
	return flag;
}

void menuabc(char mode)
{
	cct_cls();
	struct magic_ball magic;
	init_all(magic.ball, -1);
	init_all(magic.ball1, 0);
	input(&magic.x, &magic.y,1);
	generate_array(magic);
	int score = 0;
	int s = 0;

	while (1)
	{
		output_init_array(magic, "当前数组：", s, 1);
		if (TF_eliminate(magic, 1) == 0)
		{
			cout << "初始无可消除项，请重新开始" << endl;
			return;
		}

		struct location pos;
		find_result(magic, pos);
		if (mode == 'B' || mode == 'C')
		{
			if(eliminate_sth(magic,pos,mode,score,s))
				continue;
			else
				break;
		}
		else
			break;
	}//end of while(1)
	return;
}