/* 2350222 计科 盛曦 */
#include <iostream>
#include <Windows.h>
#include "../include/cmd_console_tools.h"
#include "../include/90-02-b1-with_magic_ball.h"
#include "90-02-b1.h"
using namespace std;

int main()
{
	/*cct_cls();
	cct_showstr(10, 10, "Hello, world!", COLOR_HYELLOW, COLOR_HBLUE);
	cct_setcolor();
	cct_gotoxy(0, 24);*/
	while (1)
	{
		cct_setconsoleborder(120, 30, 1000, 1000);
		cct_cls();
		cct_setfontsize("新宋体", 16);
		char menus[36] = "ABCDEFGQ";
		char mode = menu(menus);

		if (mode == 'Q')
			return 0;
		else if (mode == 'A' || mode == 'B' || mode == 'C')
			menuabc(mode);
		else if (mode >= 'D' && mode <= 'G')
			menudefg(mode);

		input_end();
	}
	return 0;
}
