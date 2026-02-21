/* 2350222 计科 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <cmath>
#include "../include/cmd_console_tools.h"
#include "../include/cmd_gmw_tools.h"
#include "90-02-b1-with_magic_ball_gmw.h"
#include "90-01-b2.h"
using namespace std;

void print_all_balls(struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi,BLOCK_DISPLAY_INFO* bdi, BLOCK_DISPLAY_INFO* bdi_will_elim, BLOCK_DISPLAY_INFO* bdi_can_elim)
{
	for (int i = 0; i < magic.y; i++)
	{
		for (int j = 0; j < magic.x; j++)
		{
			if (magic.ball1[i][j] == 1)//可消除
				gmw_draw_block(cgi, i, j, magic.ball[i][j], bdi_will_elim);
			else if (magic.ball1[i][j] == 0)//普通
				gmw_draw_block(cgi, i, j, magic.ball[i][j], bdi);
			else if (magic.ball1[i][j] == 2)//消除提示
				gmw_draw_block(cgi, i, j, magic.ball[i][j], bdi_can_elim);
		}
	}

}
//void print_current_select(int y, int line_parm, struct location& loc)
//{
//	print_bottom(y, line_parm, "当前选择");
//	print_bottom(y, line_parm, "", 11);
//	cout << char('A' + loc.i) << "行" << 1 + loc.j << "列";
//	Sleep(250);
//}

void eliminate_graph(struct magic_ball& magic, int& score, int TF_add_score, const CONSOLE_GRAPHICS_INFO* const cgi,BLOCK_DISPLAY_INFO* bdi, BLOCK_DISPLAY_INFO* bdi_eliminating)
{
	int i,j,k;
	cct_setcursor(CURSOR_INVISIBLE);
	for (i = 0; i < magic.y; i++)
	{
		for (j = 0; j < magic.x; j++)
		{
			if (magic.ball1[i][j] == 1)
			{
				if (TF_add_score)
					score++;
				for (k = 0; k < 7; k++)
				{
					gmw_draw_block(cgi, i, j, magic.ball[i][j], bdi);
					Sleep(30);
					gmw_draw_block(cgi, i, j, magic.ball[i][j], bdi_eliminating);
					Sleep(30);
				}					
				gmw_draw_block(cgi, i, j, BDI_VALUE_BLANK, bdi);
				Sleep(50);
			}
		}
	}
	cct_setcursor(CURSOR_VISIBLE_NORMAL);
}

void EliminateKey_and_draw(struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi, BLOCK_DISPLAY_INFO* bdi, BLOCK_DISPLAY_INFO* bdi_will_elim, BLOCK_DISPLAY_INFO* bdi_can_elim)
{
	eliminate_middle(magic);
	eliminate_edge(magic);
	print_all_balls(magic, cgi, bdi, bdi_will_elim, bdi_can_elim);
	//print_ball(magic, mode);
}

int pre_oneball(struct location& preloc, struct location& loc, struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi,
	BLOCK_DISPLAY_INFO* bdi, BLOCK_DISPLAY_INFO* bdi_will_elim, BLOCK_DISPLAY_INFO* bdi_sel,BLOCK_DISPLAY_INFO* bdi_can_elim)//上一次有选中一个ball
{
	char msg[256];
	if (loc.i == preloc.i && loc.j == preloc.j)//和上次选中的一样
	{
		gmw_draw_block(cgi, loc.i, loc.j, magic.ball[loc.i][loc.j], bdi_can_elim);
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
				EliminateKey_and_draw(magic, cgi,bdi,bdi_will_elim,bdi_can_elim);//每次交换ball中元素后，都应该调用一下，保证ball1中的元素不要错位
				gmw_draw_block(cgi, loc.i, loc.j, magic.ball[loc.i][loc.j], bdi_sel);
				//print_one_ball("◎",  loc,magic, line_parm, COLOR_HWHITE);

				gmw_draw_block(cgi, preloc.i, preloc.j, magic.ball[preloc.i][preloc.j], bdi_can_elim);
				//print_one_ball("◎", preloc, magic,line_parm);

				sprintf(msg, "不能交换%c行%d列<=>%c行%d列", char('A' + loc.i), loc.j, char('A' + preloc.i), preloc.j);
				gmw_status_line(cgi, LOWER_STATUS_LINE, msg);
				Sleep(250);
				preloc.i = loc.i;
				preloc.j = loc.j;
			}
			else//可以消除
			{
				sprintf(msg, "当前选择%c行%d列", char('A' + loc.i), loc.j);
				gmw_status_line(cgi, LOWER_STATUS_LINE, msg);
				//print_current_select(magic.y, line_parm, loc);
				preloc.i = -1;
				preloc.j = -1;
				return 1;
			}
		}
		else//选中的和上次不相邻
		{
			gmw_draw_block(cgi, loc.i, loc.j, magic.ball[loc.i][loc.j], bdi_sel);
			//print_one_ball("◎",  loc, magic,line_parm, COLOR_HWHITE);

			gmw_draw_block(cgi, preloc.i, preloc.j, magic.ball[preloc.i][preloc.j], bdi_can_elim);
			//print_one_ball("◎",  preloc, magic,line_parm);

			sprintf(msg, "当前选择%c行%d列", char('A' + loc.i), loc.j);
			gmw_status_line(cgi, LOWER_STATUS_LINE, msg);
			//print_current_select(magic.y, line_parm, loc);
			preloc.i = loc.i;
			preloc.j = loc.j;
		}
	}
	return 0;
}

void cursor_cmd(struct magic_ball& magic, struct location& preloc, int& end, const CONSOLE_GRAPHICS_INFO* const cgi, 
	BLOCK_DISPLAY_INFO* bdi,BLOCK_DISPLAY_INFO* bdi_can_elim, BLOCK_DISPLAY_INFO* bdi_will_elim, BLOCK_DISPLAY_INFO* bdi_sel,int game=0)
{
	cct_enable_mouse();
	int X = 0, Y = 0;
	int maction, keycode1, keycode2;
	int flag_pos;
	char msg[256];
	while (1) {
		if (gmw_read_keyboard_and_mouse(cgi,maction,Y, X,  keycode1, keycode2) == CCT_MOUSE_EVENT) {
			struct location loc;
			if (X == -1 || (game == 1 && magic.ball[X][Y] == 0))
				flag_pos = 0;// mouse_pos_legal(X, Y, magic, line_parm, origin);//记录位置是否合法,1表示合法
			else
				flag_pos = 1;
			loc.i = Y ;
			loc.j = X;
			if (flag_pos == 0)
			{
				if (game == 1)
				{
					cancel_previous(magic, bdi_can_elim, cgi);
					magic.ball2[Y][X] = 2;//变白色
					struct location loc2;
					loc2.i = Y;
					loc2.j = X;
					gmw_draw_block(cgi, loc2.i, loc2.j, magic.ball[loc2.i][loc2.j], bdi_sel);
				}
			}
			else{//合法位置
				if (game == 1)
					cancel_previous(magic, bdi_can_elim,cgi);
				if (maction == MOUSE_LEFT_BUTTON_CLICK) {//左键
					if (magic.ball1[loc.i][loc.j] != 2)
					{
						sprintf(msg, "不能选择%c行%d列", char('A' + loc.i), loc.j);
						gmw_status_line(cgi, LOWER_STATUS_LINE, msg);
						Sleep(250);
						continue;
					}
					else {
						if (preloc.i != -1)
						{//上一次有选择一个ball
							if (pre_oneball(preloc, loc, magic, cgi, bdi, bdi_will_elim, bdi_sel, bdi_can_elim))
								break;
						}
						else 
						{//上次没有选中一个ball
							gmw_draw_block(cgi, loc.i, loc.j, magic.ball[loc.i][loc.j], bdi_sel);
							//print_one_ball("◎", loc,magic, line_parm, COLOR_HWHITE);

							sprintf(msg, "当前选择%c行%d列", char('A' + loc.i), loc.j);
							gmw_status_line(cgi, LOWER_STATUS_LINE, msg);
							//print_current_select(magic.y, line_parm, loc);

							preloc.i = loc.i;
							preloc.j = loc.j;
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

void eliminate_and_fill(struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi, int& score, int TF_add_score, 
	BLOCK_DISPLAY_INFO* bdi, BLOCK_DISPLAY_INFO* bdi_can_elim, BLOCK_DISPLAY_INFO* bdi_sel, BLOCK_DISPLAY_INFO* bdi_will_elim, BLOCK_DISPLAY_INFO* bdi_eliminating)
{
	while (TF_eliminate(magic) != 0)//下落除0，新值填充
	{
		print_all_balls(magic, cgi, bdi, bdi_will_elim, bdi_can_elim);
		//print_ball(magic, mode);
		eliminate_graph(magic, score, TF_add_score,cgi,bdi,bdi_eliminating);
		fall(magic,score,0,cgi,bdi);
		FillBlank(magic, cgi,bdi);
	}
}

void game_mode(struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi)
{
	int score = 0;
	int TF_add_score = 0;//用于记录当前是否为初始消除，若为初始消除则不加score
	char msg[256];
	BLOCK_DISPLAY_INFO bdi[]{//普通球
		{BDI_VALUE_BLANK, -1, -1, "  "},  //0不显示，用空格填充即可
		{1, 1, COLOR_BLACK, "〇"},
		{2, 2, COLOR_BLACK,  "〇"},
		{3, 3, COLOR_BLACK,  "〇"},
		{4, 4, COLOR_BLACK,  "〇"},
		{5,  5, COLOR_BLACK, "〇"},
		{6,6,  COLOR_BLACK,  "〇"},
		{7,7,  COLOR_BLACK,  "〇"},
		{8, 8, COLOR_BLACK,  "〇"},
		{9, 9, COLOR_BLACK,  "〇"},
	};
	BLOCK_DISPLAY_INFO bdi_can_elim[]{//可消除球
		{BDI_VALUE_BLANK, -1, -1, "  "},  //0不显示，用空格填充即可
		{1, 1, COLOR_BLACK, "◎"},
		{2, 2, COLOR_BLACK,  "◎"},
		{3, 3, COLOR_BLACK,  "◎"},
		{4, 4, COLOR_BLACK,  "◎"},
		{5,  5, COLOR_BLACK, "◎"},
		{6,6,  COLOR_BLACK,  "◎"},
		{7,7,  COLOR_BLACK,  "◎"},
		{8, 8, COLOR_BLACK,  "◎"},
		{9, 9, COLOR_BLACK,  "◎"},
	};
	BLOCK_DISPLAY_INFO bdi_sel[]{//选中球
		{BDI_VALUE_BLANK, -1, -1, "  "},  //0不显示，用空格填充即可
		{1, 1, COLOR_HWHITE, "◎"},
		{2, 2, COLOR_HWHITE,  "◎"},
		{3, 3, COLOR_HWHITE,  "◎"},
		{4, 4, COLOR_HWHITE,  "◎"},
		{5,  5, COLOR_HWHITE, "◎"},
		{6,6,  COLOR_HWHITE,  "◎"},
		{7,7,  COLOR_HWHITE,  "◎"},
		{8, 8, COLOR_HWHITE,  "◎"},
		{9, 9, COLOR_HWHITE,  "◎"},
	};
	BLOCK_DISPLAY_INFO bdi_will_elim[]{//将要消除的球
		{BDI_VALUE_BLANK, -1, -1, "  "},  //0不显示，用空格填充即可
		{1, 1, COLOR_BLACK, "●"},
		{2, 2, COLOR_BLACK,  "●"},
		{3, 3, COLOR_BLACK,  "●"},
		{4, 4, COLOR_BLACK,  "●"},
		{5,  5, COLOR_BLACK, "●"},
		{6,6,  COLOR_BLACK,  "●"},
		{7,7,  COLOR_BLACK,  "●"},
		{8, 8, COLOR_BLACK,  "●"},
		{9, 9, COLOR_BLACK,  "●"},
	};
	BLOCK_DISPLAY_INFO bdi_eliminating[]{//正在消除的球
		{BDI_VALUE_BLANK, -1, -1, "  "},  //0不显示，用空格填充即可
		{1, 1, COLOR_BLACK, "¤"},
		{2, 2, COLOR_BLACK,  "¤"},
		{3, 3, COLOR_BLACK,  "¤"},
		{4, 4, COLOR_BLACK,  "¤"},
		{5,  5, COLOR_BLACK, "¤"},
		{6,6,  COLOR_BLACK,  "¤"},
		{7,7,  COLOR_BLACK,  "¤"},
		{8, 8, COLOR_BLACK,  "¤"},
		{9, 9, COLOR_BLACK,  "¤"},
	};

	print_all_balls(magic, cgi, bdi, bdi_will_elim, bdi_can_elim);

	while (1)
	{
		int end = 0;//用于记录右键退出
		eliminate_and_fill(magic, cgi,score,TF_add_score,bdi,bdi_can_elim,bdi_sel,bdi_will_elim,bdi_eliminating);
		sprintf(msg, "当前分数:%d 右键退出", score);
		gmw_status_line(cgi, TOP_STATUS_LINE, msg);
		TF_add_score = 1;
		struct location preloc;
		preloc.i = -1;
		preloc.j = -1;//用于记录上一次选中的ball，初始为-1表示没有选中，因此放在下落除0后初始化
		EliminateKey_and_draw(magic, cgi,bdi,bdi_will_elim,bdi_can_elim);//消除提示，画新球
		if (TF_eliminateKey(magic.ball1) == 0)
		{
			cct_showstr(15, 0, "(无可消除项，游戏结束！)", COLOR_BLACK, COLOR_WHITE, 1);
			break;
		}

		cct_setcursor(CURSOR_INVISIBLE);
		cursor_cmd(magic,preloc,end,cgi,bdi,bdi_can_elim,bdi_will_elim,bdi_sel);

		if (TF_eliminate(magic) == 1)
			continue;
		cct_setcursor(CURSOR_VISIBLE_NORMAL);

		if (end == 1)
			break;
	}//end of while(1)
}

void menu456789(char mode)
{
	cct_cls();
	struct magic_ball magic;
	init_all(magic.ball, -1);
	init_all(magic.ball1, 0);
	input(&magic.x, &magic.y);
	generate_array(magic);

	CONSOLE_GRAPHICS_INFO CGI;
	gmw_init(&CGI);
	gmw_set_rowcol(&CGI, magic.y, magic.x);//行数为y,列数为x
	//4
	gmw_set_frame_color(&CGI, COLOR_HWHITE, COLOR_BLACK);
	//10
	gmw_set_rowno_switch(&CGI,true);
	gmw_set_colno_switch(&CGI,true);

	gmw_draw_frame(&CGI);

	int s = 0;
	//graph_prepare_end(1, line_parm, magic.y);//1代表prepare
	//print_graph(magic, mode,frame);
	game_mode(magic,&CGI);
	//graph_prepare_end(2, line_parm, magic.y);//2代表end
}
