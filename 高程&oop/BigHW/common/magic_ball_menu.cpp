/* 2350222 计科 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <iomanip>
#include <Windows.h>
#include "../include/90-02-b1-with_magic_ball.h"
using namespace std;

char menu(char menus[36])
{
	char prompt[36][255] = { '\0' };
	strcpy(prompt[0],"0.退出");
	strcpy(prompt[1], "1.内部数组，生成初始状态，寻找是否有初始可消除项");
	strcpy(prompt[2], "2.内部数组，消除初始可消除项后非0项下落并用0填充");
	strcpy(prompt[3], "3.内部数组，消除初始可消除项后查找消除提示");
	strcpy(prompt[4], "4.n * n的框架(无分隔线)，显示初始状态");
	strcpy(prompt[5], "5.n * n的框架(有分隔线)，显示初始状态");
	strcpy(prompt[6], "6.n * n的框架(无分隔线)，显示初始状态及初始可消除项");
	strcpy(prompt[7], "7.n * n的框架(有分隔线)，消除初始可消除项后显示消除提示");
	strcpy(prompt[8], "8.cmd图形界面完整版(有分隔线，鼠标移动时显示坐标，右键退出)");
	strcpy(prompt[9], "9.cmd图形界面完整版");
	strcpy(prompt[10+'A'-'A'], "A.命令行找出可消除项并标识");
	strcpy(prompt[10 + 'B' - 'A'], "B.命令行完成一次消除（分步骤显示）");
	strcpy(prompt[10 + 'C' - 'A'], "C.命令行完成一关（分步骤显示）");
	strcpy(prompt[10 + 'D' - 'A'], "D.伪图形界面下用鼠标选择一个色块（无分隔线）");
	strcpy(prompt[10 + 'E' - 'A'], "E.伪图形界面下用鼠标选择一个色块（有分隔线）");
	strcpy(prompt[10 + 'F' - 'A'], "F.伪图形界面完成一次消除（分步骤）");
	strcpy(prompt[10 + 'G' - 'A'], "G.伪图形界面完整版");
	strcpy(prompt[10 + 'Q' - 'A'], "Q.退出");
	strcpy(prompt[10 + 'H' - 'A'], "H.完整扫雷游戏");
	
	//计算上下横线长度
	int i;
	unsigned int len = 0;
	for (i = 0; menus[i] != '\0'; i++)
	{
		if (menus[i] >= 'a' && menus[i] <= 'z')
			menus[i] = menus[i] - 'a' + 'A';
		if (menus[i] >= '0' && menus[i] <= '9')
		{
			if (strlen(prompt[menus[i] - '0']) > len)
				len = strlen(prompt[menus[i] - '0']);
		}
		else
		{
			if (strlen(prompt[10 + menus[i] - 'A']) > len)
				len = strlen(prompt[10 + menus[i] - 'A']);
		}
	}

	//输出
	cout << setfill('-')<<setw(len) << '-' << endl;
	for (i = 0; menus[i] != '\0'; i++)
	{
		if (menus[i] >= '0' && menus[i] <= '9')
			cout << prompt[menus[i] - '0'] << endl;
		else
			cout << prompt[10 + menus[i] - 'A'] << endl;
	}
	cout << setw(len) << '-' << endl;
	cout << setfill(' ');
	cout << "请选择：" << endl;

	//输入
	char mode = input_one_char(menus);
	return mode;
}