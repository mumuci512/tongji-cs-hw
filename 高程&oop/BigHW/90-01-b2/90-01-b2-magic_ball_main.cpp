/* 2350222 计科 盛曦 */
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include "90-01-b2-magic_ball.h"
#include "../include/90-02-b1-with_magic_ball.h"
#include "../include/cmd_console_tools.h"
using namespace std;

int main()
{
	while (1)
	{
		cct_setconsoleborder(120, 30, 1000, 1000);
		cct_cls();
		cct_setfontsize("新宋体", 16);
		//cct_setcolor(COLOR_BLACK, COLOR_WHITE);
		char menus[36] = "1234567890";
		char mode = menu(menus);

		if (mode == '0')
			return 0;
		else if (mode >= '1' && mode <= '3')
			menu123(mode);
		else if (mode >= '4' && mode <= '9')
			menu456789(mode);
		input_end();
	}
	return 0;
}