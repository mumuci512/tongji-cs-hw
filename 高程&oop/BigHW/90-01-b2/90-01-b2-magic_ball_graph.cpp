/* 2350222 计科 盛曦 */
#include <iostream>
#include <Windows.h>
#include <cmath>
#include "90-01-b2-magic_ball.h"
#include "../include/90-02-b1-with_magic_ball.h"
#include "../include/cmd_console_tools.h"
using namespace std;

void print_current_select(int y, int line_parm, struct location& loc)
{
	print_bottom(y, line_parm, "当前选择");
	print_bottom(y, line_parm, "", 11);
	cout << char('A' + loc.i) << "行" << 1 + loc.j << "列";
	Sleep(250);
}

void eliminate_graph(struct magic_ball& magic, char mode, int& score, int TF_add_score)
{
	int line_parm = (mode == '4' || mode == '6' ? 1 : 2);
	int k;
	struct location loc;
	cct_setcursor(CURSOR_INVISIBLE);
	for (loc.i = 0; loc.i < magic.y; loc.i++)
	{
		for (loc.j = 0; loc.j < magic.x; loc.j++)
		{
			if (magic.ball1[loc.i][loc.j] == 1)
			{
				if (TF_add_score)
					score++;
				for (k = 0; k < 7; k++)
				{
					print_one_ball("〇",  loc,magic, line_parm);
					Sleep(30);
					print_one_ball("¤",  loc,magic, line_parm);
					Sleep(30);
				}
				cct_showch(2 + loc.j * 2 * line_parm, 2 + loc.i * line_parm, ' ', COLOR_HWHITE, COLOR_HWHITE, 2);
				Sleep(50);
			}

		}
	}
	cct_setcursor(CURSOR_VISIBLE_NORMAL);
}

void EliminateKey_and_draw(struct magic_ball& magic, char mode)
{
	eliminate_middle(magic);
	eliminate_edge(magic);
	print_ball(magic, mode);
}

int pre_oneball(struct location& preloc, struct location& loc, struct magic_ball& magic, int line_parm, char mode)//上一次有选中一个ball
{
	if (loc.i == preloc.i && loc.j == preloc.j)//和上次选中的一样
	{
		print_one_ball("◎", loc, magic,line_parm);
		preloc.i = -1;
		preloc.j = -1;
	}
	else//和上次选中不一样
	{
		if (fabs(preloc.i - loc.i) == 1 && fabs(preloc.j - loc.j) == 0 || fabs(preloc.i - loc.i) == 0 && fabs(preloc.j - loc.j) == 1)//相邻
		{
			exchange(&magic.ball[loc.i][loc.j], &magic.ball[preloc.i][preloc.j]);
			if (TF_eliminate(magic) == 0)
			{
				exchange(&magic.ball[loc.i][loc.j], &magic.ball[preloc.i][preloc.j]);
				EliminateKey_and_draw(magic, mode);//每次交换ball中元素后，都应该调用一下，保证ball1中的元素不要错位
				print_one_ball("◎",  loc,magic, line_parm, COLOR_HWHITE);
				print_one_ball("◎", preloc, magic,line_parm);
				print_bottom(magic.y, line_parm, "不能交换");
				cout << char('A' + loc.i) << "行" << 1 + loc.j << "列" << "<=>" << char('A' + preloc.i) << "行" << 1 + preloc.j << "列";
				Sleep(250);
				preloc.i = loc.i;
				preloc.j = loc.j;
			}
			else//可以消除
			{
				print_current_select(magic.y, line_parm, loc);
				preloc.i = -1;
				preloc.j = -1;
				return 1;
			}
		}
		else//选中的和上次不相邻
		{
			print_one_ball("◎",  loc, magic,line_parm, COLOR_HWHITE);
			print_one_ball("◎",  preloc, magic,line_parm);
			print_current_select(magic.y, line_parm, loc);
			preloc.i = loc.i;
			preloc.j = loc.j;
		}
	}
	return 0;
}

void cursor_cmd(struct magic_ball& magic, int line_parm, char mode, struct location& preloc, int& end)
{
	cct_enable_mouse();
	int X = 0, Y = 0;
	int maction, keycode1, keycode2;
	while (1) {
		if (cct_read_keyboard_and_mouse(X, Y, maction, keycode1, keycode2) == CCT_MOUSE_EVENT) {
			struct location loc,origin;
			int flag_pos = mouse_pos_legal(X, Y, magic, line_parm,origin);//记录位置是否合法,1表示合法
			loc.i = Y / 2 - 1;
			loc.j = X / 4;
			if (flag_pos == 1) {//合法位置
				if (maction == MOUSE_LEFT_BUTTON_CLICK) {//左键
					if (magic.ball1[loc.i][loc.j] != 2)
					{
						print_bottom(magic.y, line_parm, "不能选择");
						print_bottom(magic.y, line_parm, "", 11);
						cout << char('A' + loc.i) << "行" << 1 + loc.j << "列";
						Sleep(250);
						continue;
					}
					else {
						if (mode == '9') {
							if (preloc.i != -1){//上一次有选择一个ball
								if (pre_oneball(preloc, loc, magic, line_parm, mode))
									break;
							}
							else {//上次没有选中一个ball
								print_one_ball("◎", loc,magic, line_parm, COLOR_HWHITE);
								print_current_select(magic.y, line_parm, loc);
								preloc.i = loc.i;
								preloc.j = loc.j;
							}
						}
						if (mode == '8') {
							print_current_select(magic.y, line_parm, loc);
							Sleep(250);
							break;
						}
					}
				}
				else if (maction == MOUSE_RIGHT_BUTTON_CLICK) {//右键
					Sleep(100);//不加这个右键就会有弹窗
					end = 1;
					break;
				}//其余键（双击等）不管
			}
		}//键盘操作不管
	}//end of while(1)
	cct_enable_mouse();//不加这个右键就会有弹窗
	cct_disable_mouse();
}

void eliminate_and_fill(struct magic_ball& magic, char mode, int& score, int TF_add_score, int line_parm)
{
	while (TF_eliminate(magic) != 0)//下落除0，新值填充
	{
		print_ball(magic, mode);
		if (mode == '7')
			wait_for_enter("按回车键进行消除及下落除0操作...", magic.y, line_parm, 1);
		eliminate_graph(magic, mode, score, TF_add_score);
		fall(magic,score, mode);
		if (mode == '7')
			wait_for_enter("按回车键进行新值填充...", magic.y, line_parm, 1);
		FillBlank(magic, mode);
	}
}

void game_mode(struct magic_ball& magic, char mode, int line_parm)
{
	int score = 0;
	int TF_add_score = 0;//用于记录当前是否为初始消除，若为初始消除则不加score
	while (1)
	{
		int end = 0;//用于记录右键退出
		eliminate_and_fill(magic, mode, score, TF_add_score, line_parm);
		if (mode >= '8')
		{
			cct_showstr(15, 0, "(当前分数:", COLOR_BLACK, COLOR_WHITE, 1);
			cout << score << " 右键退出)";
			TF_add_score = 1;
		}
		struct location preloc;
		preloc.i = -1;
		preloc.j = -1;//用于记录上一次选中的ball，初始为-1表示没有选中，因此放在下落除0后初始化
		if (mode == '7')
			wait_for_enter("按回车键显示消除提示...", magic.y, line_parm, 1);
		EliminateKey_and_draw(magic, mode);//消除提示，画新球
		if (mode == '9' && TF_eliminateKey(magic.ball1) == 0)
		{
			cct_showstr(15, 0, "(无可消除项，游戏结束！)", COLOR_BLACK, COLOR_WHITE, 1);
			break;
		}
		if (mode >= '8')
		{
			cct_setcursor(CURSOR_INVISIBLE);
			cursor_cmd(magic, line_parm, mode, preloc, end);
			if (mode == '9' && TF_eliminate(magic) == 1)
				continue;
			cct_setcursor(CURSOR_VISIBLE_NORMAL);
		}//end of if(mode>='8')
		if (mode != '9' || mode == '9' && end == 1)
			break;
	}//end of while(1)
}

void menu456789(char mode)
{
	int line_parm = (mode == '4' || mode == '6' ? 1 : 2);
	cct_cls();
	frame_lines frame("═", "║", "╔", "╗", "╚", "╝", "╬","╦","╩", "╠", "╣");
	struct magic_ball magic;
	init_all(magic.ball, -1);
	init_all(magic.ball1, 0);
	input(&magic.x, &magic.y);
	generate_array(magic);
	int s = 0;
	if (mode == '4' || mode == '5')
	{
		output_init_array(magic, "初始数组：",s);
		wait_for_enter("按回车键显示图形...");
	}
	graph_prepare_end(1, line_parm, magic.y);//1代表prepare
	if (mode > '5' && mode < '8')
	{
		if (TF_eliminate(magic) == 0)
			cct_showstr(15, 0, "(未找到初始可消除项)", COLOR_BLACK, COLOR_WHITE, 1);
	}
	print_graph(magic, mode,frame);
	if (mode > '6')
		game_mode(magic, mode, line_parm);
	graph_prepare_end(2, line_parm, magic.y);//2代表end
}
