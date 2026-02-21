/* 2350222 计科 盛曦 */
#include <iostream>
#include <iomanip>
#include <Windows.h>
#include "../include/cmd_console_tools.h"
#include "../include/90-02-b1-with_magic_ball.h"
#include "90-02-b1.h"
using namespace std;

//step为1或-1，表示查找的方向，p表示查找点在ball中的位置，p1表示查找点在ball1中的位置，pos表示查找点的位置，
//v_or_l表示查找的是列还是行
int find_eliminate(struct magic_ball magic, int* p, int* p1, int step, struct location pos, char v_or_l,char mode)
{
	int mul = (v_or_l == 'v' ? YYMAX : 1);//假如是竖向查找（v_or_l为v），则指针需在行间移动，每一个step相应乘以列数
	struct location loc = pos;
	if (v_or_l == 'v')
	{
		loc.i += step;
		if (pos.i + step > magic.y || pos.i + step < 0)
			return 0;
	}
	else if (v_or_l == 'l')
	{
		loc.j += step;
		if (pos.j + step > magic.x || pos.j + step < 0)
			return 0;
	}
	if (*p == *(p + step * mul))//万一有联通区域？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	{
		*p1 = 2;
		*(p1 + step * mul) = 2;
		if(mode!=0)
		{
			int line_parm;
			set_lineparm(mode, line_parm);
			print_one_ball(" ", loc, magic, line_parm, COLOR_HWHITE, 1);
			Sleep(10);
		}
		//每个return前必须将pos复位，不然下一次非递归调用时pos就不是原点
		return step * mul;
	}
	else
		return 0;
}

int FIND_ELIMINATE(struct magic_ball magic, int* p, int* p1, struct location& pos, char dir,char mode)//dir用于表示上一次FIND_ELIMINATE已经查找过的方向，不用再向那个方向查找，否则会陷入死循环
{
	int r1 = 0;
	int r2 = 0;
	int r3 = 0;
	int r4 = 0;
	int flag = 0;
	if (dir != '1')
		r1 = find_eliminate(magic, p, p1, 1, pos, 'v',mode);
	if (dir != '2')
		r2 = find_eliminate(magic, p, p1, -1, pos, 'v', mode);
	if (dir != '3')
		r3 = find_eliminate(magic, p, p1, 1, pos, 'l', mode);
	if (dir != '4')
		r4 = find_eliminate(magic, p, p1, -1, pos, 'l', mode);
	if (r1)
	{
		flag = 1;
		pos.i++;
		FIND_ELIMINATE(magic, p + r1, p1 + r1, pos, '2',mode);
		pos.i--;
	}
	if (r2)
	{
		flag = 1;
		pos.i--;
		FIND_ELIMINATE(magic, p + r2, p1 + r2, pos, '1',mode);
		pos.i++;
	}
	if (r3)
	{
		flag = 1;
		pos.j++;
		FIND_ELIMINATE(magic, p + r3, p1 + r3, pos, '4',mode);
		pos.j--;
	}
	if (r4)
	{
		flag = 1;
		pos.j--;
		FIND_ELIMINATE(magic, p + r4, p1 + r4, pos, '3',mode);
		pos.j++;
	}
	return flag;
}
