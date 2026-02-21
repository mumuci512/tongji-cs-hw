/* 2350222 计科 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <iomanip>
#include "90-02-b2.h"
#include "../include/cmd_console_tools.h"
#include "../include/90-02-b1-with_magic_ball.h"
#include "../include/cmd_gmw_tools.h"
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

void edge(struct MINE* m, char drc)
{
	int x, y, i, j;
	int imin = -1, imax = 1, jmin = -1, jmax = 1;
	switch (drc)
	{
		case'l'://左边缘
			x = 0;
			jmin = 0;
			break;
		case 'r'://右边缘
			x = m->MAX_X - 1;
			jmax = 0;
			break;
		case 'u'://上边缘
			y = 0;
			imin = 0;
			break;
		case 'd'://下边缘
			y = m->MAX_Y - 1;
			imax = 0;
			break;
		default:
			break;
	}
	if (drc == 'l' || drc == 'r')
	{
		for (y = 1; y < m->MAX_Y - 1; y++)
		{
			if (m->mine[y* m->MAX_X+x] == '*')
			{
				continue;
			}
			else
			{
				m->mine[y * m->MAX_X + x] = '0';
				for (i = imin; i <= imax; i++)
				{
					for (j = jmin; j <= jmax; j++)
					{
						if (j == 0 && i == 0)
						{
							continue;
						}
						else
						{
							if (m->mine[(y + i)* m->MAX_X+(x + j)] == '*')
							{
								m->mine[y* m->MAX_X+x]++;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		for (x = 1; x < m->MAX_X - 1; x++)
		{
			if (m->mine[y * m->MAX_X + x] == '*')
			{
				continue;
			}
			else
			{
				m->mine[y * m->MAX_X + x] = '0';
				for (i = imin; i <= imax; i++)
				{
					for (j = jmin; j <= jmax; j++)
					{
						if (j == 0 && i == 0)
						{
							continue;
						}
						else
						{
							if (m->mine[(y + i) * m->MAX_X + (x + j)] == '*')
							{
								m->mine[y * m->MAX_X + x]++;
							}
						}
					}
				}
			}
		}
	}
}
void corner(struct MINE* m, char drc)
{
	int x = 0, y = 0;
	int i, j;
	int imin = -1, imax = 1, jmin = -1, jmax = 1;
	switch (drc)
	{
		case 1://左上角
			imin = 0;
			jmin = 0;
			break;
		case 2://右上角
			x = m->MAX_X - 1;
			imin = 0;
			jmax = 0;
			break;
		case 3://左下角
			y = m->MAX_Y - 1;
			imax = 0;
			jmin = 0;
			break;
		case 4://右下角
			x = m->MAX_X - 1;
			y = m->MAX_Y - 1;
			imax = 0;
			jmax = 0;
			break;
		default:
			break;
	}
	if (m->mine[y * m->MAX_X + x] != '*')
	{
		m->mine[y * m->MAX_X + x] = '0';
		for (i = imin; i <= imax; i++)
		{
			for (j = jmin; j <= jmax; j++)
			{
				if (i == 0 && j == 0)
					continue;
				if (m->mine[(y + i) * m->MAX_X + (x + j)] == '*')
					m->mine[y * m->MAX_X + x]++;
			}
		}
	}
}
void middle(struct MINE* m)
{
	int x, y, i, j;
	for (y = 1; y < m->MAX_Y - 1; y++)
	{
		for (x = 1; x < m->MAX_X - 1; x++)
		{
			if (m->mine[y * m->MAX_X + x] == '*')
			{
				continue;
			}
			else
			{
				m->mine[y * m->MAX_X + x] = '0';
				for (i = -1; i <= 1; i++)
				{
					for (j = -1; j <= 1; j++)
					{
						if (j == 0 && i == 0)
						{
							continue;
						}
						else
						{
							if (m->mine[(y + i) * m->MAX_X + (x + j)] == '*')
							{
								m->mine[y * m->MAX_X + x]++;
							}
						}
					}
				}
			}
		}
	}
}
void generate_mine(struct MINE* m)
{
	int i, x, y;
	srand((unsigned int)(time(0)));
	for (i = 0; i < m->mine_num; i++)
	{
		x = rand() % m->MAX_X;
		y = rand() % m->MAX_Y;
		if (m->mine[y * m->MAX_X + x] != '*')
		{
			m->mine[y * m->MAX_X + x] = '*';
		}
		else
		{
			i--;
		}
	}

	//中间
	middle(m);
	//边缘
	edge(m, 'l');
	edge(m, 'r');
	edge(m, 'u');
	edge(m, 'd');
	//四个角
	corner(m, 1);
	corner(m, 2);
	corner(m, 3);
	corner(m, 4);

}

void init_mine(struct MINE* m)//对struct mine初始化
{
	//有内存分配，最好不要放在函数里
}

void draw_mine(const CONSOLE_GRAPHICS_INFO* const cgi,struct MINE* m, BLOCK_DISPLAY_INFO* bdi_open, BLOCK_DISPLAY_INFO* bdi_close)
{
	int i, j;
	for (i = 0; i < m->MAX_Y; i++)
	{
		for (j = 0; j < m->MAX_X; j++)
		{
			if (m->state[i * m->MAX_X + j] == STATE_OPEN)
				gmw_draw_block(cgi, i, j, m->mine[i * m->MAX_X + j]-'0', bdi_open);
			else
				gmw_draw_block(cgi, i, j, m->state[i * m->MAX_X + j], bdi_close);
		}
	}
}

//step为1或-1，表示查找的方向，p表示查找点在ball中的位置，p1表示查找点在ball1中的位置，pos表示查找点的位置，
//v_or_l表示查找的是列还是行
int find_eliminate(const CONSOLE_GRAPHICS_INFO* const cgi,  BLOCK_DISPLAY_INFO* bdi_open,struct MINE* m, int step, struct location pos, char v_or_l)
{
	int mul = (v_or_l == 'v' ? m->MAX_X : 1);//假如是竖向查找（v_or_l为v），则指针需在行间移动，每一个step相应乘以列数
	if (v_or_l == 'v')
	{
		pos.i += step;
		if (pos.i >= m->MAX_Y || pos.i < 0)
			return 0;
	}
	else if (v_or_l == 'l')
	{
		pos.j += step;
		if (pos.j >= m->MAX_X || pos.j < 0)
			return 0;
	}
	if (m->mine[pos.i*m->MAX_X+pos.j]!='*'&& m->state[pos.i * m->MAX_X + pos.j] != STATE_OPEN)//若是打开的，则会在联通区域内死循环
	{
		if(m->state[pos.i * m->MAX_X + pos.j]!=STATE_MARK)
		{
			m->state[pos.i * m->MAX_X + pos.j] = STATE_OPEN;
			gmw_draw_block(cgi, pos.i, pos.j, m->mine[pos.i * m->MAX_X + pos.j]-'0', bdi_open);
			Sleep(2);
			if (m->mine[pos.i * m->MAX_X + pos.j] != '0')//不是空白也不必继续查找
				return 0;
		}
		return step * mul;
	}
	else
		return 0;
}

int FIND_ELIMINATE(const CONSOLE_GRAPHICS_INFO* const cgi, BLOCK_DISPLAY_INFO* bdi_open,struct MINE* m, struct location& pos, char dir)//dir用于表示上一次FIND_ELIMINATE已经查找过的方向，不用再向那个方向查找，否则会陷入死循环
{
	int r1 = 0;
	int r2 = 0;
	int r3 = 0;
	int r4 = 0;
	int flag = 0;
	if (dir != '1')
		r1 = find_eliminate(cgi,bdi_open,m, 1, pos, 'v');
	if (dir != '2')
		r2 = find_eliminate(cgi, bdi_open, m, -1, pos, 'v');
	if (dir != '3')
		r3 = find_eliminate(cgi, bdi_open, m, 1, pos, 'l');
	if (dir != '4')
		r4 = find_eliminate(cgi, bdi_open, m, -1, pos, 'l');
	if (r1)
	{
		flag = 1;
		pos.i++;
		FIND_ELIMINATE(cgi, bdi_open, m, pos, '2');
		pos.i--;
	}
	if (r2)
	{
		flag = 1;
		pos.i--;
		FIND_ELIMINATE(cgi, bdi_open, m, pos, '1');
		pos.i++;
	}
	if (r3)
	{
		flag = 1;
		pos.j++;
		FIND_ELIMINATE(cgi, bdi_open, m, pos, '4');
		pos.j--;
	}
	if (r4)
	{
		flag = 1;
		pos.j--;
		FIND_ELIMINATE(cgi, bdi_open, m ,pos, '3');
		pos.j++;
	}
	return flag;
}

int TF_game_end(struct MINE* m)
{
	int flag = 1;//flag=1表示游戏已经结束
	int i, j;
	for (i = 0; i < m->MAX_X; i++)
	{
		for (j = 0; j < m->MAX_Y; j++)
		{
			if (m->state[i * m->MAX_X + j] != STATE_OPEN && m->mine[i * m->MAX_X + j] != '*')
			{
				flag = 0;
				break;
			}
		}
	}
	return flag;
}

int player_cmd(const CONSOLE_GRAPHICS_INFO* const cgi, struct MINE* m, BLOCK_DISPLAY_INFO* bdi_open, BLOCK_DISPLAY_INFO* bdi_close)
{
	cct_enable_mouse();
	cct_setcursor(CURSOR_INVISIBLE);
	int time0 = (int)time(0);
	int time1;
	int maction, keycode1, keycode2, ret,mrow=-1,mcol=-1;
	char msg[256];
	int remain_m=m->mine_num;//剩余雷数
	while (1)
	{
		if (TF_game_end(m) == 1)//胜利
		{
			time1 = (int)time(0);
			sprintf(msg, "共用时%d秒,", time1 - time0);
			gmw_status_line(cgi, LOWER_STATUS_LINE, "恭喜胜利，游戏结束", msg);
			break;
		}
		ret = gmw_read_keyboard_and_mouse(cgi, maction, mrow, mcol, keycode1, keycode2, true);
		if(ret==CCT_MOUSE_EVENT)
		{
			if (maction == MOUSE_LEFT_BUTTON_CLICK)
			{
				/*if (mcol != mcol1 || mrow != mrow1)
				{
					mrow = mrow1;
					mcol = mcol1;
				}
				else
					continue;*/
				if (m->mine[mrow * m->MAX_X + mcol] == '*')
				{
					gmw_draw_block(cgi, mrow, mcol, '*', bdi_open);
					time1 = (int)time(0);
					sprintf(msg, "共用时%d秒,", time1 - time0);
					gmw_status_line(cgi, LOWER_STATUS_LINE, "你输了，游戏结束", msg);
					break;
				}
				else if (m->state[mrow * m->MAX_X + mcol] == STATE_CLOSE)
				{
					m->state[mrow * m->MAX_X + mcol] = STATE_OPEN;
					gmw_draw_block(cgi, mrow, mcol, m->mine[mrow * m->MAX_X + mcol] - '0', bdi_open);
					struct location loc = { mrow,mcol };
					FIND_ELIMINATE(cgi, bdi_open, m, loc, '0');
					Sleep(2);
				}
				sprintf(msg, "剩余雷数：%d,", remain_m);
				gmw_status_line(cgi, TOP_STATUS_LINE, "ESC退出，空格显示时间", msg);
			}
			else if (maction == MOUSE_RIGHT_BUTTON_CLICK)
			{
				if (m->state[mrow * m->MAX_X + mcol] != STATE_MARK)
				{
					m->state[mrow * m->MAX_X + mcol] = STATE_MARK;
					remain_m--;
				}
				else
				{
					m->state[mrow * m->MAX_X + mcol] = STATE_CLOSE;
					remain_m++;
				}
				gmw_draw_block(cgi, mrow, mcol, m->state[mrow * m->MAX_X + mcol], bdi_close);
				sprintf(msg, "剩余雷数：%d,", remain_m);
				gmw_status_line(cgi, TOP_STATUS_LINE, "ESC退出，空格显示时间", msg);
			}
		}
		else if (ret == CCT_KEYBOARD_EVENT)
		{
			if (keycode1 == 27)//ESC
				return 0;
			else if (keycode1 == 32)//空格
			{
				time1 = (int)time(0);
				sprintf(msg, "共用时%d秒,", time1 - time0);
				gmw_status_line(cgi, TOP_STATUS_LINE, "ESC退出，空格显示时间", msg);
			}
		}
	}//end of while
	cct_disable_mouse();
	cct_setcursor(CURSOR_VISIBLE_NORMAL);
	return 0;
}

void MineSweeper()
{
	cct_cls();
	struct MINE m;
	int mode;
	cct_gotoxy(0, 0);
	cout << "请输入扫雷游戏的等级（数字），初级\\中级\\高级（1\\2\\3）";
	mode = input_one_char("123");
	switch (mode)
	{
		case LEVEL_LOW-1+'1':
			m.MAX_X = LOW_X;
			m.MAX_Y = LOW_Y;
			m.mine_num = LOW_MINE;
			break;
		case LEVEL_MEDIUM - 1 + '1':
			m.MAX_X = MEDIUM_X;
			m.MAX_Y = MEDIUM_Y;
			m.mine_num = MEDIUM_MINE;
			break;
		case LEVEL_HIGH - 1 + '1':
			m.MAX_X = HIGH_X;
			m.MAX_Y = HIGH_Y;
			m.mine_num = HIGH_MINE;
			break;
	}

	m.mine = new(nothrow) char[m.MAX_Y * m.MAX_X];
	if (m.mine == NULL)
	{
		cout << "内存分配失败！";
		return;
	}
	m.state = new(nothrow) char[m.MAX_Y * m.MAX_X];
	if (m.state == NULL)
	{
		cout << "内存分配失败！";
		return;
	}
	for (int i = 0; i < m.MAX_Y * m.MAX_X; i++)//初始全部未翻开
		m.state[i] = STATE_CLOSE;

	generate_mine(&m);

	CONSOLE_GRAPHICS_INFO cgi;
	gmw_init(&cgi);
	//1
	gmw_set_rowcol(&cgi, m.MAX_Y , m.MAX_X);
	//1
	gmw_set_delay(&cgi, DELAY_OF_DRAW_FRAME,0);
	gmw_set_delay(&cgi, DELAY_OF_DRAW_BLOCK, 0);
	//2
	gmw_set_frame_color(&cgi, COLOR_HWHITE, COLOR_BLACK);
	gmw_set_frame_style(&cgi, 4, 2);
	//9
	gmw_set_rowno_switch(&cgi,true);
	gmw_set_colno_switch(&cgi,true);


	gmw_draw_frame(&cgi);
	gmw_status_line(&cgi, TOP_STATUS_LINE, "ESC退出，空格显示时间");

	BLOCK_DISPLAY_INFO bdi_open[]{
		{BDI_VALUE_BLANK, COLOR_WHITE, COLOR_WHITE, "  "},  //0不显示，用空格填充即可
		{1,  COLOR_WHITE, 1, NULL},
		{2,  COLOR_WHITE, 2, NULL},
		{3,  COLOR_WHITE, 3, NULL},
		{4,  COLOR_WHITE, 4, NULL},
		{5,  COLOR_WHITE, 5, NULL},
		{6,  COLOR_WHITE, 6, NULL},
		{7,  COLOR_WHITE, 7, NULL},
		{8,  COLOR_WHITE, 8, NULL},
		{int('*'),  COLOR_WHITE, 9, "* "},
	};
	BLOCK_DISPLAY_INFO bdi_close[]{
		{STATE_CLOSE,  COLOR_YELLOW, -1, "  "},
		{STATE_MARK,  COLOR_HRED, -1, "# "}
	};
	draw_mine(&cgi, &m, bdi_open, bdi_close);

	player_cmd(&cgi, &m, bdi_open, bdi_close);

	delete[] m.mine;
	delete[] m.state;

	to_be_continued("本小题结束",&cgi);
}