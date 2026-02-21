/* 2350222 计科 盛曦 */
#include <iostream>
#include <Windows.h>
#include "../include/cmd_console_tools.h"
#include "../include/90-02-b1-with_magic_ball.h"
#include "90-02-b1.h"
using namespace std;

void cancel_select(struct player_parm& a,struct magic_ball& magic)
{
	a.sel = 0;
	a.enter_pos_leg = 1;
	for (int i = 0; i < magic.y; i++)
	{
		for (int j = 0; j < magic.x; j++)
		{
			if (magic.ball2[i][j] != 0)
			{
				struct location loc;
				loc.i = i;
				loc.j = j;
				print_one_ball("", loc, magic, a.line_parm, COLOR_BLACK, 1);//消除上一次选中后导致的白色色块
			}
		}
	}
	init_all(magic.ball2, 0);
}

void select0(struct player_parm& a,struct location& origin, struct magic_ball& magic)
{
	print_bottom(magic.y, a.line_parm, "箭头键/鼠标移动取消当前选择，回车键/单击左键合成");
	if(FIND_ELIMINATE(magic, &magic.ball[origin.i][origin.j], &magic.ball2[origin.i][origin.j], origin, 0, a.mode)==0)
		print_bottom(magic.y, a.line_parm, "当前选择周围无相同值，箭头键/鼠标移动取消当前选择");
	else
	{
		a.sel = 1;
		a.enter_pos_leg = 0;
	}
}

void graph_eliminate(struct player_parm& a, struct magic_ball& magic,int& score)
{
	a.enter_pos_leg = 0;
	int n = 0;//消除个数
	for (int i = 0; i < magic.y; i++)
	{
		for (int j = 0; j < magic.x; j++)
		{
			if (magic.ball2[i][j] != 0)//消除
			{
				magic.ball[i][j] = 0;
				n++;
				cct_showstr(4 + j * 2 * a.line_parm, 3 + i * a.line_parm, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);
				cct_showstr(4 + j * 2 * a.line_parm, 3 + i * a.line_parm + 1, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);
				cct_showstr(4 + j * 2 * a.line_parm, 3 + i * a.line_parm + 2, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);
				Sleep(10);
			}
		}
	}
	score += (n * n * 5);
	print_window(SCORE, n * n * 5, score);
	if(a.mode=='F')
		print_bottom(magic.y, a.line_parm, "合成完成，回车键 / 单击左键下落0");
	a.sel = 0;
	a.tf_fall = 1;
}

void graph_fall(struct player_parm& a, struct magic_ball& magic,int& score)
{
	fall(magic, score, a.mode, 1);
	a.sel = 0;
	a.tf_fall = 0;
	if (a.mode == 'F')
	{
		print_bottom(magic.y, a.line_parm, "本次合成结束，按C/单击左键进行新一次合成");
		a.end = 1;
	}
}

void arrow_move(int keycode2, struct location& origin,struct magic_ball& magic)
{
	switch (keycode2) {
		case KB_ARROW_UP:
			if (!(origin.i == 0||magic.ball[origin.i-1][origin.j]==0))//没有移动到上顶且上方不是空的，因短路计算不会导致数组越界
				origin.i--;
			else//否则移动到最底下
				origin.i = (magic.y - 1);
			break;
		case KB_ARROW_DOWN:
			if (origin.i != magic.y - 1)
				origin.i++;
			else
				origin.i = 0;
			if (magic.ball[origin.i][origin.j] == 0)
				arrow_move(KB_ARROW_DOWN, origin, magic);
			break;
		case KB_ARROW_LEFT:
			if (origin.j == 0)
				origin.j = (magic.x - 1);
			else
				origin.j--;
			if (magic.ball[origin.i][origin.j] == 0)
				arrow_move(KB_ARROW_LEFT, origin, magic);
			break;
		case KB_ARROW_RIGHT:
			if (origin.j == magic.x - 1)
				origin.j=0;
			else
				origin.j ++;
			if (magic.ball[origin.i][origin.j] == 0)
				arrow_move(KB_ARROW_RIGHT, origin, magic);
			break;
		default:
			break;
	}
}

int player_cmd(char mode, struct magic_ball& magic, int line_parm,int& score)
{
	cct_enable_mouse();
	cct_setcursor(CURSOR_INVISIBLE);
	int basex = 2, basey = 3;
	int X = 0, Y = 0;
	int maction, keycode1, keycode2,ret;
	struct location origin;//用于记录当前选中的坐标
	origin.i = 0;
	origin.j = 0;//初始选中为（0,0）
	struct player_parm a;
	a.sel = 0;//sel为0表示未选中，1表示已选中
	a.tf_fall = 0;//fall为0表示不可下落，1表示可以下落
	a.end = 0;//end表示本次是否需要结束,1表示结束，0表示不结束
	a.enter_pos_leg=1;//是否要执行mouse_pos_legal函数的指标
	a.line_parm = line_parm;
	a.mode = mode;
	a.flag_pos = 1;
	int last_cmd = CCT_KEYBOARD_EVENT;
	while (1)
	{
		if(TF_eliminate(magic,1)==0)
		{
			wait_for_enter("无可消除项，本关结束，回车进入下一关",magic.y,a.line_parm,1);
			return 1;
		}
		ret = cct_read_keyboard_and_mouse(X, Y, maction, keycode1, keycode2);
		if (maction == MOUSE_RIGHT_BUTTON_CLICK && (mode == 'F' || mode == 'G'))//个人认为不论鼠标位置是否合法，只要右击就应该可以退出
			return 0;
		if (a.end)
		{
			if (maction == MOUSE_LEFT_BUTTON_CLICK || input_one_char("Cc") == 'C')
				break;
		}
		if (ret == CCT_MOUSE_EVENT)
		{
			last_cmd = CCT_MOUSE_EVENT;
			if (maction == MOUSE_ONLY_MOVED && a.sel == 1)
				cancel_select(a,magic);
			if(a.enter_pos_leg)
				a.flag_pos = mouse_pos_legal(X, Y, magic, line_parm,origin,1);//记录位置是否合法,1表示合法，会改变origin
			if(maction == MOUSE_LEFT_BUTTON_CLICK)
			{
				if (a.flag_pos == 1&& a.sel == 0 && a.tf_fall == 0)//若选中
				{
					if (mode == 'D' || mode == 'E')
					{
						select_end(line_parm, X, Y, origin, basex, basey, magic, 'm');
						break;
					}
					else
					{
						select0(a, origin,magic);
						ret = cct_read_keyboard_and_mouse(X, Y, maction, keycode1, keycode2);//消除返回的多余的MOUSE_ONLY_MOVED
						continue;
					}
				}
				if (a.sel == 1 && a.tf_fall == 0)//若选中后消除
				{
					graph_eliminate(a, magic,score);
					if (mode == 'G')
					{
						graph_fall(a, magic,score);
						ret = cct_read_keyboard_and_mouse(X, Y, maction, keycode1, keycode2);//消除返回的多余的MOUSE_ONLY_MOVED
						print_bottom(magic.y, a.line_parm, "");
						a.enter_pos_leg = 1;
					}
					continue;
				}
				if (a.tf_fall == 1&&mode=='F')//下落
				{
					graph_fall(a, magic,score);
					ret = cct_read_keyboard_and_mouse(X, Y, maction, keycode1, keycode2);//消除返回的多余的MOUSE_ONLY_MOVED
					continue;
				}
			}
		}
		else if (ret == CCT_KEYBOARD_EVENT)
		{
			if ((keycode1 == 'Q' || keycode1 == 'q') && (mode == 'F' || mode == 'G'))
				return 0;
			if (keycode1 == 224 && a.sel == 1)
				cancel_select(a, magic);
			if (keycode1 == 13)//ENTER
			{
				if(a.flag_pos == 1 && a.sel == 0 && a.tf_fall == 0&&(last_cmd == CCT_KEYBOARD_EVENT || ((last_cmd == CCT_MOUSE_EVENT) && (mouse_pos_legal(X, Y, magic, line_parm, origin, 1) == 1))))//且位置合法,只要上一次是keyboard，短路运算就不会调用mouse_pos_legal函数，因此origin不会被改变
				{
					if (mode == 'D' || mode == 'E')
					{
						select_end(line_parm, X, Y, origin, basex, basey, magic, 'k');
						break;
					}
					else
					{
						select0(a, origin, magic);
						continue;
					}
				}
				if (a.sel == 1 && a.tf_fall == 0)//可消除
				{
					graph_eliminate(a, magic,score);
					if (mode == 'G')
					{
						graph_fall(a, magic,score);
						print_bottom(magic.y, a.line_parm, "");
						a.enter_pos_leg = 1;
					}
					continue;
				}
				if (a.tf_fall == 1)//下落
				{
					graph_fall(a,magic,score);
					continue;
				}
			}
			
			if (keycode1 == 224)
			{
				last_cmd = CCT_KEYBOARD_EVENT;
				if (a.sel == 1)
					cancel_select(a, magic);
				cancel_previous(magic, line_parm);
				arrow_move(keycode2, origin, magic);
				print_bottom(magic.y, line_parm, "[当前键盘] ");
				print_bottom(magic.y, line_parm, "", 11);
				cout << char('A' + origin.i) << "行" << origin.j << "列";
				magic.ball2[origin.i][origin.j] = 2;//变白色
				print_one_ball("◎",  origin, magic, line_parm, COLOR_HWHITE, 1);
			}
		}
	}//end of while(1)
	cct_disable_mouse();
	cct_setcursor(CURSOR_VISIBLE_NORMAL);
	return 0;
}

void menudefg(char mode)
{
	cct_cls();
	frame_lines frame("━","┃","┏","┓","┗", "┛","╋","┳","┻","┣","┫");
	struct magic_ball magic;
	init_all(magic.ball, -1);
	input(&magic.x, &magic.y, 1);
	int lineparm;
	if (mode == 'D' || mode == 'F')
		lineparm = 3;
	else
		lineparm = 4;
	graph_prepare_end(1,lineparm,magic.y,magic.x,1);
	int score = 0;
	do
	{
		init_all(magic.ball1, 0);
		init_all(magic.ball2, 0);
		generate_array(magic);
		magic.ball2[0][0] = 2;
		print_graph(magic, mode,frame);
		if (mode == 'D' || mode == 'E')
			print_bottom(magic.y, lineparm, "箭头键/鼠标移动，回车键/单击左键选择并结束");
		else
			print_bottom(magic.y, lineparm, "箭头键/鼠标移动，回车键/单击左键选择, Q/单击右键结束");
	}while(player_cmd(mode,magic,lineparm,score));
	graph_prepare_end(2, lineparm, magic.y, magic.x, 1);
}