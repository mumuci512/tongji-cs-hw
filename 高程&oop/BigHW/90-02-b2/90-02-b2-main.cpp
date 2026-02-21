/* 2350222 计科 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "../include/cmd_console_tools.h"
#include "../include/cmd_gmw_tools.h"
#include "../include/90-02-b1-with_magic_ball.h"
#include "90-02-b2.h"
using namespace std;

int main()
{
	while (1)
	{
		cct_setconsoleborder(120, 30, 1000, 1000);
		cct_cls();
		cct_setfontsize("新宋体", 16);
		//cct_setcolor(COLOR_BLACK, COLOR_WHITE);
		char menus[36] = "QH";
		char mode = menu(menus);

		if (mode == 'Q')
			return 0;
		else if (mode == 'H')
			MineSweeper();
	}
	return 0;
}