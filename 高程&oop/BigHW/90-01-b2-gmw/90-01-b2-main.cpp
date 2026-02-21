/* 2350222 计科 盛曦 */
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include "../include/cmd_console_tools.h"
#include "../include/cmd_gmw_tools.h"
#include "90-02-b1-with_magic_ball_gmw.h"
#include "90-01-b2.h"
using namespace std;

int main()
{
	while (1)
	{
		cct_setconsoleborder(120, 30, 1000, 1000);
		cct_cls();
		cct_setfontsize("新宋体", 16);
		//cct_setcolor(COLOR_BLACK, COLOR_WHITE);
		char menus[36] = "90";
		char mode = menu(menus);

		if (mode == '0')
			return 0;
		else if (mode == '9')
			menu456789(mode);
		input_end();
	}
	return 0;
}