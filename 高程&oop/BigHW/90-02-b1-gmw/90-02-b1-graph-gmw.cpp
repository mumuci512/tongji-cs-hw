/* 2350222 计科 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <iomanip>
#include "../include/cmd_console_tools.h"
#include "../include/cmd_gmw_tools.h"
#include "90-02-b1-with_magic_ball_gmw2.h"
#include "90-02-b1-gmw.h"
using namespace std;

void to_be_continued(const char* prompt, const CONSOLE_GRAPHICS_INFO* const bgi)
{
	if (bgi->inited == CGI_INITED) { //初始化过
		cct_setcolor(bgi->area_bgcolor, bgi->area_fgcolor); //恢复初始颜色

		cct_gotoxy(0, bgi->SLI.lower_start_y + 2);//光标设到指定位置
		cout << setw(bgi->cols - 1) << ' '; //先用空格清空整行

		cct_gotoxy(0, bgi->SLI.lower_start_y + 2);//光标设到指定位置
	}
	else { //未初始化过
		cct_setcolor(); //缺省颜色
		cct_gotoxy(0, 0);//光标设到指定位置
	}

	if (prompt)
		cout << prompt << "，按回车键继续...   ";
	else
		cout << "按回车键继续...   ";

	while (_getch() != '\r')
		;

	return;
}

void cancel_select(struct player_parm& a,struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi, const BLOCK_DISPLAY_INFO* const bdi)
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
				gmw_draw_block(cgi, loc.i, loc.j, magic.ball[loc.i][loc.j], bdi);
				//print_one_ball("", loc, magic, a.line_parm, COLOR_BLACK, 1);//消除上一次选中后导致的白色色块
			}
		}
	}
	init_all(magic.ball2, 0);
}

void select0(struct player_parm& a,struct location& origin, struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi, const BLOCK_DISPLAY_INFO* const bdi_sel)
{
	gmw_status_line(cgi, LOWER_STATUS_LINE, "箭头键/鼠标移动取消当前选择，回车键/单击左键合成");
	//print_bottom(magic.y, a.line_parm, "箭头键/鼠标移动取消当前选择，回车键/单击左键合成");
	if(FIND_ELIMINATE(magic, &magic.ball[origin.i][origin.j], &magic.ball2[origin.i][origin.j], origin, 0,cgi,bdi_sel)==0)
		gmw_status_line(cgi, LOWER_STATUS_LINE, "当前选择周围无相同值，箭头键/鼠标移动取消当前选择");//print_bottom(magic.y, a.line_parm, "当前选择周围无相同值，箭头键/鼠标移动取消当前选择");
	else
	{
		a.sel = 1;
		a.enter_pos_leg = 0;
	}
}

void graph_eliminate(struct player_parm& a, struct magic_ball& magic,int& score, const CONSOLE_GRAPHICS_INFO* const cgi, const BLOCK_DISPLAY_INFO* const bdi)
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

				gmw_draw_block(cgi, i, j, magic.ball[i][j], bdi);
				/*cct_showstr(4 + j * 2 * a.line_parm, 3 + i * a.line_parm, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);
				cct_showstr(4 + j * 2 * a.line_parm, 3 + i * a.line_parm + 1, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);
				cct_showstr(4 + j * 2 * a.line_parm, 3 + i * a.line_parm + 2, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);*/
				
				Sleep(10);
			}
		}
	}
	score += (n * n * 5);
	char msg[256];

	sprintf(msg, "本次得分：%d 总得分：%d", n * n * 5, score);
	gmw_status_line(cgi, TOP_STATUS_LINE, msg);
	//print_window(SCORE, n * n * 5, score);

	a.sel = 0;
	a.tf_fall = 1;
}

void graph_fall(struct player_parm& a, struct magic_ball& magic,int& score, const CONSOLE_GRAPHICS_INFO* const cgi, const BLOCK_DISPLAY_INFO* const bdi)
{
	fall(magic, score,cgi,bdi);
	a.sel = 0;
	a.tf_fall = 0;
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

int player_cmd(struct magic_ball& magic, int& score,const CONSOLE_GRAPHICS_INFO* const cgi,const BLOCK_DISPLAY_INFO* const bdi_sel, const BLOCK_DISPLAY_INFO* const bdi)
{
	cct_enable_mouse();
	cct_setcursor(CURSOR_INVISIBLE);
	int X = -1, Y = -1;
	int maction, keycode1, keycode2,ret;
	struct location origin;//用于记录当前选中的坐标
	origin.i = 0;
	origin.j = 0;//初始选中为（0,0）
	struct player_parm a;
	a.sel = 0;//sel为0表示未选中，1表示已选中
	a.tf_fall = 0;//fall为0表示不可下落，1表示可以下落
	a.end = 0;//end表示本次是否需要结束,1表示结束，0表示不结束
	a.enter_pos_leg=1;//是否要执行mouse_pos_legal函数的指标
	//a.line_parm = line_parm;
	//a.mode = mode;
	a.flag_pos = 1;
	char msg[256];
	int last_cmd = CCT_KEYBOARD_EVENT;
	while (1)
	{
		if(TF_eliminate(magic,1)==0)
		{
			to_be_continued("无可消除项，本关结束，", cgi);
			//wait_for_enter("无可消除项，本关结束，回车进入下一关",magic.y,a.line_parm,1);
			return 1;
		}
		ret = gmw_read_keyboard_and_mouse(cgi,maction, X, Y, keycode1, keycode2);
		if (maction == MOUSE_RIGHT_BUTTON_CLICK)//个人认为不论鼠标位置是否合法，只要右击就应该可以退出
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
				cancel_select(a,magic,cgi,bdi);
			if (a.enter_pos_leg)
			{// mouse_pos_legal(X, Y, magic, line_parm, origin, 1);//记录位置是否合法,1表示合法，会改变origin cancel_previous(magic, line_parm);
				origin.i = X;
				origin.j = Y;
				if (X != -1 && Y != -1 && magic.ball[origin.i][origin.j] != 0)//合法
				{
					a.flag_pos = 1;
					cancel_previous(magic, cgi,bdi);
					magic.ball2[origin.i][origin.j] = 2;//变白色
					struct location loc2;
					loc2.i = origin.i;
					loc2.j = origin.j;
					gmw_draw_block(cgi, loc2.i, loc2.j,magic.ball[loc2.i][loc2.j], bdi_sel);
					//print_one_ball("◎", loc2, magic, line_parm, COLOR_HWHITE, 1);
				}
				else
				{
					a.flag_pos = 0;
					cancel_previous(magic, cgi,bdi);
				}
			}
			if(maction == MOUSE_LEFT_BUTTON_CLICK)
			{
				if (a.flag_pos == 1&& a.sel == 0 && a.tf_fall == 0)//若选中
				{
					select0(a, origin,magic,cgi,bdi_sel);
					ret = cct_read_keyboard_and_mouse(X, Y, maction, keycode1, keycode2);//消除返回的多余的MOUSE_ONLY_MOVED
					continue;
				}
				if (a.sel == 1 && a.tf_fall == 0)//若选中后消除
				{
					graph_eliminate(a, magic,score,cgi,bdi);
					graph_fall(a, magic, score,cgi,bdi);
					ret = cct_read_keyboard_and_mouse(X, Y, maction, keycode1, keycode2);//消除返回的多余的MOUSE_ONLY_MOVED

					gmw_status_line(cgi, LOWER_STATUS_LINE, "");
					//print_bottom(magic.y, a.line_parm, "");

					a.enter_pos_leg = 1;
					continue;
				}
			}
		}
		else if (ret == CCT_KEYBOARD_EVENT)
		{
			if (keycode1 == 'Q' || keycode1 == 'q')
				return 0;
			if (keycode1 == 224 && a.sel == 1)
				cancel_select(a, magic,cgi,bdi);
			if (keycode1 == 13)//ENTER
			{
				if(a.flag_pos == 1 && a.sel == 0 && a.tf_fall == 0&&(last_cmd == CCT_KEYBOARD_EVENT) )//|| ((last_cmd == CCT_MOUSE_EVENT) && X!=-1&&Y!=-1)且位置合法,只要上一次是keyboard，短路运算就不会调用mouse_pos_legal函数，因此origin不会被改变
				{
					select0(a, origin, magic, cgi,bdi_sel);
					continue;
				}
				else if (a.flag_pos == 1 && a.sel == 0 && a.tf_fall == 0 && last_cmd == CCT_MOUSE_EVENT)
				{
					origin.i = X;
					origin.j = Y;
					cancel_previous(magic, cgi, bdi);
					if(X != -1 && Y != -1)
					{
						magic.ball2[origin.i][origin.j] = 2;//变白色
						struct location loc2;
						loc2.i = origin.i;
						loc2.j = origin.j;
						gmw_draw_block(cgi, loc2.i, loc2.j, magic.ball[loc2.i][loc2.j], bdi_sel);
						select0(a, origin, magic, cgi, bdi_sel);
						continue;
					}
				}
				if (a.sel == 1 && a.tf_fall == 0)//可消除
				{
					graph_eliminate(a, magic,score,cgi,bdi);
					graph_fall(a, magic, score,cgi,bdi);

					gmw_status_line(cgi, LOWER_STATUS_LINE, "");
					//print_bottom(magic.y, a.line_parm, "");

					a.enter_pos_leg = 1;
					continue;
				}
				if (a.tf_fall == 1)//下落
				{
					graph_fall(a,magic,score,cgi,bdi);
					continue;
				}
			}
			
			if (keycode1 == 224)
			{
				last_cmd = CCT_KEYBOARD_EVENT;
				if (a.sel == 1)
					cancel_select(a, magic,cgi,bdi);
				cancel_previous(magic, cgi,bdi);
				arrow_move(keycode2, origin, magic);

				sprintf(msg, "当前键盘] %c行%d列", char('A' + origin.i), origin.j);
				gmw_status_line(cgi, LOWER_STATUS_LINE, msg);
				/*print_bottom(magic.y, line_parm, "[当前键盘] ");
				print_bottom(magic.y, line_parm, "", 11);
				cout << char('A' + origin.i) << "行" << origin.j << "列";*/

				magic.ball2[origin.i][origin.j] = 2;//变白色

				gmw_draw_block(cgi, origin.i, origin.j, magic.ball[origin.i][origin.j], bdi_sel);
				//print_one_ball("◎",  origin, magic, line_parm, COLOR_HWHITE, 1);
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
	//frame_lines frame("━","┃","┏","┓","┗", "┛","╋","┳","┻","┣","┫");
	struct magic_ball magic;
	init_all(magic.ball, -1);
	input(&magic.x, &magic.y, 1);
	/*int lineparm;
	if (mode == 'D' || mode == 'F')
		lineparm = 3;
	else
		lineparm = 4;*/
	//graph_prepare_end(1,lineparm,magic.y,magic.x,1);
	CONSOLE_GRAPHICS_INFO cgi;
	gmw_init(&cgi);

	gmw_set_rowcol(&cgi,magic.y, magic.x);
	//4
	gmw_set_frame_default_linetype(&cgi, 2);
	gmw_set_frame_color(&cgi, COLOR_HWHITE,COLOR_BLACK);
	gmw_set_frame_style(&cgi, 6, 3);
	//1
	gmw_set_block_border_switch(&cgi, true);
	//7
	gmw_set_rowno_switch(&cgi);
	gmw_set_colno_switch(&cgi);

	BLOCK_DISPLAY_INFO bdi[]{
		{BDI_VALUE_BLANK, -1, -1, "  "},  //0不显示，用空格填充即可
		{1, 1, COLOR_BLACK, "★"},
		{2, 2, COLOR_BLACK,  "★"},
		{3, 3, COLOR_BLACK,  "★"},
		{4, 4, COLOR_BLACK,  "★"},
		{5,  5, COLOR_BLACK, "★"},
		{6,6,  COLOR_BLACK,  "★"},
		{7,7,  COLOR_BLACK,  "★"},
		{8, 8, COLOR_BLACK,  "★"},
		{9, 9, COLOR_BLACK,  "★"},
	};
	BLOCK_DISPLAY_INFO bdi_sel[]{
		{BDI_VALUE_BLANK, -1, -1, "  "},  //0不显示，用空格填充即可
		{1, 1, COLOR_HWHITE, "★"},
		{2, 2, COLOR_HWHITE,  "★"},
		{3, 3, COLOR_HWHITE,  "★"},
		{4, 4, COLOR_HWHITE,  "★"},
		{5,  5, COLOR_HWHITE, "★"},
		{6,6,  COLOR_HWHITE,  "★"},
		{7,7,  COLOR_HWHITE,  "★"},
		{8, 8, COLOR_HWHITE,  "★"},
		{9, 9, COLOR_HWHITE,  "★"},
	};
	int score = 0;
	do
	{
		init_all(magic.ball1, 0);
		init_all(magic.ball2, 0);
		generate_array(magic);
		magic.ball2[0][0] = 2;

		gmw_draw_frame(&cgi);
		print_ball(magic, bdi, bdi_sel, &cgi);
		//print_graph(magic, mode,frame);

		gmw_status_line(&cgi, LOWER_STATUS_LINE,"箭头键/鼠标移动，回车键/单击左键选择, Q/单击右键结束");
		//print_bottom(magic.y, lineparm, "箭头键/鼠标移动，回车键/单击左键选择, Q/单击右键结束");
	}while(player_cmd(magic,score,&cgi,bdi_sel,bdi));
	//graph_prepare_end(2, lineparm, magic.y, magic.x, 1);
}