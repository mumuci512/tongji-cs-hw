/* 2350222 计科 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <conio.h>
#include <Windows.h>
#include "../include/cmd_console_tools.h"
#include "../include/90-02-b1-with_magic_ball.h"
using namespace std;

frame_lines::frame_lines(const char a[], const char b[], const char c[], const char d[], const char e[], const char f[], const char g[], const char h[], const char i[], const char j[], const char k[])
{
	for (int i = 0; i < FRAME_LEN; i++)
	{
		line[i] = '\0';
		col[i] = '\0';
		l_u_corner[i] = '\0';
		r_u_corner[i] = '\0';
		l_d_corner[i] = '\0';
		r_d_corner[i] = '\0';
		d_cross[i] = '\0';
		u_cross[i] = '\0';
		r_cross[i] = '\0';
		l_cross[i] = '\0';
	}
	strcpy(line, a);
	strcpy(col, b);
	strcpy(l_u_corner, c);
	strcpy(r_u_corner, d);
	strcpy(l_d_corner, e);
	strcpy(r_d_corner, f);
	strcpy(cross, g);
	strcpy(d_cross, h);
	strcpy(u_cross, i);
	strcpy(r_cross, j);
	strcpy(l_cross, k);
}

char input_one_char(const char str[])
{
	char mode;
	char medium;
	do
	{
		mode = _getch();
		if (mode >= 'a' && mode <= 'z')
			medium = mode + ('A' - 'a');
		else
			medium = mode;
	} while (strchr(str, medium) == NULL);
	cout << mode;//因为menu中用了无回显的getch(为了不显示除了0-9意外输入的错误字符），这里需要人工回显
	l_to_h(mode);
	Sleep(100);
	return mode;
}

void l_to_h(char& ch)
{
	if (ch >= 'a' && ch <= 'z')
		ch += 'A' - 'a';
}

void output_prompt(int x, int y,int game)
{
	int i;
	//第一行
	cout << setw(3) << "|";
	for (i = 0; i < x; i++)
	{
		if(game==0)
			cout << setw(3) << i + 1;
		else
			cout << setw(3) << i + 0;
	}
	cout << endl;
	//第二行
	cout << "--+";
	for (i = 0; i < x; i++)
		cout << setw(3) << setfill('-') << "-";
	cout << setfill(' ') << "-";
	cout << endl;
}
void output_array(struct magic_ball& magic,int& score,int game,int tf_yellow)
{
	int i, j;
	int n = 0;
	for (i = 0; i < magic.y; i++)
	{
		cout << char('A' + i) << " |";
		for (j = 0; j < magic.x; j++)
		{
			cout << "  ";
			if (magic.ball1[i][j] != 0||(magic.ball[i][j] == 0)&&(game==1)&&(tf_yellow==0))
			{
				cct_setcolor(COLOR_HYELLOW, COLOR_BLUE);
			}
			if (game == 1&&magic.ball[i][j]==0)
				n++;
			cout << magic.ball[i][j];
			cct_setcolor(COLOR_BLACK, COLOR_WHITE);
		}
		cout << endl;
	}
	cout << endl;
	score = n * n * 5;
}
void output_init_array(struct magic_ball& magic,const char prompt[], int& score, int game,int tf_yellow)
{
		cout << endl << prompt << endl;
	output_prompt(magic.x, magic.y,game);
	output_array(magic,score,game,tf_yellow);
}

void input(int* x, int* y,int game)//game=0:彩球；game=1：星星
{
	int xlow, xhigh,ylow,yhigh;
	switch(game)
	{
		case 0:
			xlow = XMIN;
			xhigh = XMAX;
			ylow = YMIN;
			yhigh = YMAX;
			break;
		case 1:
			xlow = XMIN_STAR;
			xhigh = XMAX_STAR;
			ylow = YMIN_STAR;
			yhigh = YMAX_STAR;
			break;
		default:
			break;
	}
	while (1)
	{
		cout << "请输入行数("<<ylow<<"-"<<yhigh<<")："<<endl;
		cin >> *y;
		if (*y >= ylow && *y <= yhigh && cin.good() == 1)
			break;
		if (cin.good() == 0)
		{
			cin.clear();
			cin.ignore(1410065407, '\n');
		}
	}
	while (1)
	{
		cout << "请输入列数(" << xlow << "-" << xhigh << ")：" << endl;
		cin >> *x;
		if (*x >= xlow && *x <= xhigh && cin.good() == 1)
			break;
		if (cin.good() == 0)
		{
			cin.clear();
			cin.ignore(1410065407, '\n');
		}
	}
	getchar();//吸收input的回车
}

void init_all(int array[XXMAX][YYMAX], int fill)
{
	int i, j;
	for (i = 0; i < XXMAX; i++)
	{
		for (j = 0; j < YYMAX; j++)
		{
			array[i][j] = fill;
		}
	}
}

void generate_array(struct magic_ball& magic)
{
	srand((unsigned int)(time(0)));
	int i, j;
	for (i = 0; i < magic.y; i++)
	{
		for (j = 0; j < magic.x; j++)
		{
			magic.ball[i][j] = rand() % 8 + 1;//生成1-9
		}
	}
}

//char input_end(const char prompt[],const char key[])
//{
//	cout << prompt;
//	int X, Y;
//	unsigned int i;
//	cct_getxy(X, Y);
//	char end[4];
//	while (1)
//	{
//		cct_setcolor(COLOR_BLACK, COLOR_WHITE);
//		cct_setcursor(CURSOR_INVISIBLE);
//		cct_showch(X, Y, ' ', COLOR_BLACK, COLOR_WHITE, 20);//覆盖上一次输入的
//		cct_setcursor(CURSOR_VISIBLE_NORMAL);
//		cct_gotoxy(X, Y);
//		for (i = 0; i < strlen(key)+1; i++)
//		{
//			end[i] = '\0';
//		}
//		for (i = 0; i < strlen(key)+1; i++)
//		{
//			end[i] = _getche();
//			l_to_h(end[i]);
//			if (end[i] == '\n' || end[i] == '\r')
//			{
//				break;
//			}
//		}
//		if (i == strlen(key)+1)//说明没有读到回车,输入的超过了3个
//		{
//			continue;
//		}
//		/*cin.getline(end, 3, '\n');
//		if (end[3] != '\n'&&end[3]!='\r')
//			continue;*/
//			/*fgets(end, 3, stdin);*/
//		if (strcmp(end, key) == 0)
//		{
//			cout << endl << "输入错误，请重新输入";
//			continue;
//		}
//		else
//			break;
//	}
//	return end[0];
//}
void input_end()
{
	cout << "本小题结束，请输入End继续...";
	int X, Y, i;
	cct_getxy(X, Y);
	char end[4];
	while (1)
	{
		cct_showch(X, Y, ' ', COLOR_BLACK, COLOR_WHITE, 20);//覆盖上一次输入的
		cct_gotoxy(X, Y);
		for (i = 0; i < 4; i++)
		{
			end[i] = '\0';
		}
		for (i = 0; i < 4; i++)
		{
			end[i] = _getche();
			if (end[i] == '\n' || end[i] == '\r')
			{
				break;
			}
		}
		if (i == 4)//说明没有读到回车,输入的超过了3个
		{
			continue;
		}
		/*cin.getline(end, 3, '\n');
		if (end[3] != '\n'&&end[3]!='\r')
			continue;*/
			/*fgets(end, 3, stdin);*/
		if (!((end[0] == 'E' || end[0] == 'e') && (end[1] == 'N' || end[1] == 'n') && (end[2] == 'D' || end[2] == 'd')))
		{
			cout << endl << "输入错误，请重新输入";
			continue;
		}
		else
			break;
	}
}

int TF_eliminate(struct magic_ball& magic, int game)//判断并记录可消除项
{
	int i, j, m;
	int flag = 0;//记录是否有可消除项
	init_all(magic.ball1, 0);
	for (i = 0; i < magic.y; i++)
	{
		for (j = 0; j < magic.x; j++)
		{
			if(game==0)
			{
				for (m = -2; m <= 0; m++)
				{
					if (j + m >= 0 && j + m < magic.x - 2 && magic.ball[i][j + m] == magic.ball[i][j + m + 1] && magic.ball[i][j + m] == magic.ball[i][j + m + 2])
					{
						magic.ball1[i][j] = 1;
						flag = 1;
					}
					if (i + m >= 0 && i + m < magic.y - 2 && magic.ball[i + m][j] == magic.ball[i + m + 1][j] && magic.ball[i + m][j] == magic.ball[i + m + 2][j])
					{
						magic.ball1[i][j] = 1;
						flag = 1;
					}
				}
			}
			else if (game == 1)
			{
				if(i+1<magic.y&&magic.ball[i][j]== magic.ball[i+1][j]&& magic.ball[i][j]!=0)
					flag = 1;
				else if (i - 1 >= 0 && magic.ball[i][j] == magic.ball[i - 1][j] && magic.ball[i][j] != 0)
					flag = 1;
				else if (j + 1 < magic.x && magic.ball[i][j] == magic.ball[i][j+1] && magic.ball[i][j] != 0)
					flag = 1;
				else if (j - 1 >= 0 && magic.ball[i][j] == magic.ball[i][j-1] && magic.ball[i][j] != 0)
					flag = 1;
			}
		}
	}
	return flag;
}

void exchange(int* a, int* b)
{
	int t = *a;
	*a = *b;
	*b = t;
}

void set_lineparm(char mode, int& line_parm)
{
	switch (mode)
	{
		case '4':
		case '6':
			line_parm = 1;
			break;
		case '7':
		case '8':
		case '5':
		case '9':
			line_parm = 2;
			break;
		case 'D':
		case 'F':
			line_parm = 3;
			break;
		case 'E':
		case 'G':
			line_parm = 4;
			break;
		default:
			break;
	}
}
void set_base(int game, int& basex, int& basey)
{
	switch (game)
	{
		case 1:
			basex = 2;
			basey = 3;
			break;
		case 0:
			basex = 0;
			basey = 2;
			break;
		default:
			break;
	}
}

void print_one_ball(const char circle[],  struct location loc, struct magic_ball magic, int line_parm, int fgcolor, int game,int tf_fall)
{
	if (game == 0)
		cct_showstr(2 + loc.j * 2 * line_parm, 2 + loc.i * line_parm, circle, magic.ball[loc.i][loc.j], fgcolor, 1);
	else if (game == 1)
	{
		cct_showstr(4 + loc.j * 2 * line_parm, 3 + loc.i * line_parm, "╔═╗", magic.ball[loc.i][loc.j], fgcolor,1);
		cct_showstr(4 + loc.j * 2 * line_parm, 3 + loc.i * line_parm + 1, "║★║", magic.ball[loc.i][loc.j], fgcolor, 1);
		cct_showstr(4 + loc.j * 2 * line_parm, 3 + loc.i * line_parm + 2, "╚═╝", magic.ball[loc.i][loc.j], fgcolor, 1);
	}
}

void one_fall(int line_parm, int j, int k,struct magic_ball magic,int game,int v_or_l)//画掉落单个球
{
	struct location size;//一个球的长宽
	if (game == 0)//用game参数更好
	{
		size.i = 1;//长（对应x）
		size.j = 1;//宽（对应y）
	}
	else if (game == 1)
	{
		size.i = 3;
		size.j = 3;
	}
	int basex, basey;
	set_base(game, basex, basey);
	if (game == 0)
		cct_showch(basex + 2 + j * 2 * line_parm, 2 + k * line_parm, ' ', COLOR_HWHITE, COLOR_HWHITE, size.i * 2);//覆盖上一格
	else
	{
		cct_showstr(4 + j * 2 * line_parm, 3 + k * line_parm, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);
		cct_showstr(4 + j * 2 * line_parm, 3 + k * line_parm + 1, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);
		cct_showstr(4 + j * 2 * line_parm, 3 + k * line_parm + 2, "      ", COLOR_HWHITE, COLOR_HWHITE, 1);
	}
	Sleep(20);
	struct location loc;
	if(v_or_l=='v')
	{
		loc.j = j;
		loc.i = k + 1;
	}
	else
	{
		loc.j = j-1;
		loc.i = k;
	}
	print_one_ball("〇", loc, magic, line_parm, COLOR_BLACK, game, 1);//绘画下一个
	Sleep(20);
		//cct_showstr(2 + j * 2 * line_parm, 2 + (k + 1) * line_parm - 1, "〇", magic.ball[k + 1][j], COLOR_BLACK, 1);
	//cct_showstr(2 + j * 2 * line_parm, 2 + k * line_parm + 1, "═", COLOR_HWHITE, COLOR_BLACK, 1);
	//cct_showstr(2 + j * 2 * line_parm, 2 + (k + 1) * line_parm, "〇", magic.ball[k + 1][j], COLOR_BLACK, 1);
	//Sleep(20);//一个球掉下一个格子，停顿

}

void fall(struct magic_ball& magic, int& score, char mode,int game)//下落除0
{
	int i, j, k;
	int graph = ((mode == '0'||mode=='B'||mode=='C') ? 0 : 1);//graph缺省为0，不绘制伪图形
	for (j = 0; j < magic.x; j++)
	{
		for (i = 0; i < magic.y; i++)
		{
			if (magic.ball1[i][j] == 1||game==1&&magic.ball1[i][j]!=0)
				magic.ball[i][j] = 0;
			/*if (magic.ball[i][j] == 0)
				magic.ball2[i][j] = 0;*/
		}
	}
	if (game == 1&&mode!='F'&&mode!='G')
	{
		int score0 = score;
		output_init_array(magic, "相同值归并后的数组(不同色标识)：", score, game,1);
		cout << endl << "本次得分：" << score-score0 << " 总得分：" << score << endl<<endl;
	}

	int flag = 0;//flag为1，表示当前位置的下方有0，即当前位置需要下落
	if (mode >= 'F' && mode <= 'G')
		init_all(magic.ball2, 0);
	for (j = 0; j < magic.x; j++)
	{
		for (i = magic.y - 1; i >= 0; i--)
		{
			if (flag == 1 && magic.ball[i][j] != 0 && magic.ball[i + 1][j] == 0)//当前不为0，当前位置的下方一个为0，这时候才需要下落；因为刚进循环的时候flag为0，不可能进这个if，所以横坐标i+1不会越界
			{
				for (k = i; k + 1 < magic.y && magic.ball[k + 1][j] == 0; k++)
				{
					exchange(&magic.ball[k][j], &magic.ball[k + 1][j]);
					exchange(&magic.ball1[k][j], &magic.ball1[k + 1][j]);
					magic.ball2[k + 1][j] = 0;
					if (graph != 0)
					{
						int line_parm;
						if(game==0)
							line_parm = 2;
						else if (game == 1)
							set_lineparm(mode, line_parm);
						one_fall(line_parm, j, k,magic,game);
					}
				}
				if (graph != 0)
					Sleep(40);//一个球的掉落过程完毕，停顿
			}
			if (magic.ball[i][j] == 0)
				flag = 1;
		}
	}
	if (game == 1)//一整列移动
	{
		for (j = magic.x-1; j >=0; j--)
		{
			flag = 0;//flag为0表示这一列是全0
			for (i = magic.y-1; i >=0; i--)
			{
				if (magic.ball[i][j] != 0)
				{
					flag = 1;
					break;
				}
			}
			if (flag == 0)
			{
				for (k = j; k <magic.x-1; k++)
				{
					for (i = magic.y - 1; i >= 0; i--)//因为是整列移动，无法用行指针，若数组长宽相反则可以
					{
						if (magic.ball[i][k+1] == 0)
							break;
						exchange(&magic.ball[i][k], &magic.ball[i][k + 1]);
						if (graph != 0)
						{
							int line_parm;
							set_lineparm(mode, line_parm);
							one_fall(line_parm, k+1, i, magic, 1, 'l');
						}
					}
				}
			}
		}
	}
}

void print_bottom(int y, int line_parm, const char prompt[100], int x)
{
	int basey;
	if (line_parm == 1 || line_parm == 2)
		basey = 2;
	else if (line_parm == 3 || line_parm == 4)
		basey = 3;
	cct_showch(x, basey + line_parm * y+1, ' ', COLOR_BLACK, COLOR_WHITE, 50);//覆盖上一次的prompt
	cct_setcolor(COLOR_BLACK, COLOR_WHITE);
	cct_gotoxy(x, basey + line_parm * y+1);
	cout << prompt;
}

void wait_for_enter(const char prompt[100], int y, int line_parm, int bottom)
{
	if (bottom != 0)
		print_bottom(y, line_parm, prompt);
	else
		cout << prompt;
	char enter = _getch();
	while (enter != '\n' && enter != '\r')
	{
		enter = _getch();
	}
}

void graph_prepare_end(int flag, int line_parm, int y, int x, int game)//flag为1，表示prepare，flag为2，表示end
{
	int linemin, colmax, colmin, ymin, mul,basey;
	switch (game)
	{
		case 0:
			colmax = COLMAX;
			linemin = LINEMIN;
			ymin = YMIN;
			mul = 52;
			basey = 2;
			break;
		case 1:
			colmin = COLMIN_STAR;
			linemin = LINEMIN_STAR;
			ymin = YMIN_STAR;
			mul = 32;
			basey = 3;
			break;
		default:
			break;
	}
	if (flag == 1)
	{
		cct_setcursor(CURSOR_INVISIBLE);
		if (game == 0)
			cct_setconsoleborder(colmax, linemin + y * line_parm - (line_parm - 1)+1, 1000, 1000);
		else if (game == 1)
			cct_setconsoleborder(colmin+1 + line_parm*2 * x, linemin + y * line_parm - (line_parm - 1)+2, 1000, 1000);
		cct_cls();
		cct_setfontsize("新宋体", mul - (y - ymin) * 4 - (line_parm - 1) * 4);
	}
	else
	{
		cct_showch(0, basey+line_parm*y+2, ' ', COLOR_BLACK, COLOR_WHITE, 100);//覆盖上一次的prompt
		cct_setcolor(COLOR_BLACK, COLOR_WHITE);
		cct_gotoxy(0, basey + line_parm * y + 2);
		cct_setcursor(CURSOR_VISIBLE_NORMAL);
	}
}

void print_frame_line(const char srt[3], const char end[3], int x, int y, char mode,frame_lines frame)//x表示这一行的宽度，y表示这一行所在的行数
{
	int line_parm;
	int base,basey;
	set_lineparm(mode, line_parm);
	if (mode >= 'A' && mode <= 'G')
	{
		base = 2;
		basey = 3;
	}
	else if (mode >= '0' && mode <= '9')
	{
		base = 0;
		basey = 2;
	}
	cct_showstr(base, y, srt, COLOR_HWHITE, COLOR_BLACK, 1);
	if (line_parm == 1||line_parm==3)
		cct_showstr(2 + base, y, frame.line, COLOR_HWHITE, COLOR_BLACK, x * line_parm);
	else if(line_parm==2||line_parm==4)
	{
		if (y == basey-1)
		{
			for(int i=0;i<x-1;i++)
			{
				cct_showstr(2+base+i*line_parm*2, y, frame.line, COLOR_HWHITE, COLOR_BLACK, line_parm - 1);
				cct_showstr(2+base+i*line_parm*2+2*(line_parm-1), y, frame.d_cross, COLOR_HWHITE, COLOR_BLACK, 1);
			}
		}
		else 
		{
			for (int i = 0; i < x - 1; i++)
			{
				cct_showstr(2 + base + i * line_parm*2, y, frame.line, COLOR_HWHITE, COLOR_BLACK, line_parm - 1);
				cct_showstr(2 + base + i * line_parm *2+ 2 * (line_parm - 1), y, frame.u_cross, COLOR_HWHITE, COLOR_BLACK, 1);
			}
		}
		cct_showstr(2+base+2 * line_parm *( x-1) , y, frame.line, COLOR_HWHITE, COLOR_BLACK,line_parm- 1);
	}
	cct_showstr(2+base + 2 * line_parm * x - (1 - line_parm % 2) * 2, y, end, COLOR_HWHITE, COLOR_BLACK, 1);//-(1-line_parm%2)*2
	if (mode == '4' || mode == '5'||mode>='D'&&mode<='G')
		Sleep(20);//打完一行停顿
}
void print_frame_col(int x, int y, char mode,frame_lines frame)//x表示这一行所在的列数，y表示这一行的高度
{
	int line_parm;
	int base=0;
	int basey;
	set_lineparm(mode, line_parm);
	if (mode >= 'A' && mode <= 'G')
	{
		base = 2;
		basey = 3;
	}
	else if (mode >= '0' && mode <= '9')
	{
		base = 0;
		basey = 2;
	}
	int i, j;
	for (i = 0; i < y * line_parm; i++)
	{
		if (line_parm == 2||line_parm==4)
		{
			if ((i+1) % line_parm != 0)
			{
				cct_showstr(base, basey + i, frame.col, COLOR_HWHITE, COLOR_BLACK, 1);//该行第一个║
				for (j = base+2; j < base+2 + line_parm*2 * x; j += line_parm*2)
				{
					cct_showstr(j, basey + i, "  ", COLOR_HWHITE, COLOR_HWHITE, (line_parm - 1));
					cct_showstr(j+ (line_parm - 1) * 2, basey + i, frame.col, COLOR_HWHITE, COLOR_BLACK, 1);
				}
				//cct_showstr(base, basey + i, "║", COLOR_HWHITE, COLOR_BLACK, 1);//该行第一个
				//for (j = 2; j <= 2 + (x - 1) * 4; j += 4)
				//{
				//	cct_showch(j+base, 2 + i, ' ', COLOR_HWHITE, COLOR_HWHITE, 2);
				//	if (j != 2 + (x - 1) * 4)
				//		cct_showstr(j + 2+base, 2 + i, "║", COLOR_HWHITE, COLOR_BLACK, 1);
				//	else
				//		cct_showstr(j + 2+base, 2 + i, "║", COLOR_HWHITE, COLOR_BLACK, 1);//该行最后一个
				//}
			}
			else
			{
				cct_showstr(base, basey + i, frame.r_cross, COLOR_HWHITE, COLOR_BLACK, 1);//该行第一个
				for (j = base+2; j < base+2 + line_parm * 2*x; j += line_parm*2)
				{
					cct_showstr(j, basey + i, frame.line, COLOR_HWHITE, COLOR_BLACK, (line_parm - 1));
					if(j==base +2+ line_parm*2*(x-1))//该行最后一个
						cct_showstr(j + (line_parm - 1) * 2, basey + i, frame.l_cross, COLOR_HWHITE, COLOR_BLACK, 1);
					else
						cct_showstr(j + (line_parm - 1) * 2, basey + i, frame.cross, COLOR_HWHITE, COLOR_BLACK, 1);
					//cct_showstr(j + base, 2 + i, "═", COLOR_HWHITE, COLOR_BLACK, 1);
					//if (j != 2 + (x - 1) * 4)
					//	cct_showstr(j + 2 + base, 2 + i, "╬", COLOR_HWHITE, COLOR_BLACK, 1);
					//else
					//	cct_showstr(j + 2 + base, 2 + i, "╣", COLOR_HWHITE, COLOR_BLACK, 1);//该行最后一个
				}
			}
		}
		else if(line_parm==1|| line_parm == 3)
		{
			cct_showstr(base, basey + i, frame.col, COLOR_HWHITE, COLOR_BLACK, 1);
			cct_showstr(2 * line_parm * x+base+2, basey+ i, frame.col, COLOR_HWHITE, COLOR_BLACK, 1);
		}
		if (mode == '4' || mode == '5'|| mode >= 'D' && mode <= 'G')
			Sleep(20);//打完一行停顿
	}
}

void print_head(struct magic_ball magic,int line_parm)
{
	cct_setcolor(COLOR_BLACK, COLOR_HWHITE);
	cct_gotoxy(4,1);
	for (int i = 0; i < magic.x; i++)
		cout << setw(line_parm) << char(i + '0') << setw(line_parm) << ' ';
	for (int j = 0; j < magic.y; j++)
		cct_showch(0, 4 + line_parm * j, char('A' + j), COLOR_BLACK, COLOR_HWHITE,1);
}

void print_ball(struct magic_ball& magic, char mode)
{
	cct_setcursor(CURSOR_INVISIBLE);
	int line_parm;
	set_lineparm(mode, line_parm);
	struct location loc;
	for (loc.i = 0; loc.i < magic.y; loc.i++)
	{
		for (loc.j = 0; loc.j < magic.x; loc.j++)
		{
			if(mode>='0'&&mode<='9')
			{
				if (magic.ball1[loc.i][loc.j] == 1)//可消除
					print_one_ball("●", loc, magic,line_parm);
				else if (magic.ball1[loc.i][loc.j] == 0)//普通
					print_one_ball("〇",  loc, magic, line_parm);
				else if (magic.ball1[loc.i][loc.j] == 2)//消除提示
					print_one_ball("◎",  loc, magic, line_parm);
			}
			else if (mode >= 'A' && mode <= 'Z')
			{
				if (magic.ball2[loc.i][loc.j] == 2)
					print_one_ball("◎",  loc, magic, line_parm, COLOR_HWHITE,1);
				else
					print_one_ball("◎",  loc, magic, line_parm,COLOR_BLACK,1);
			}
			if (mode == '4' || mode == '5'||mode=='D')
				Sleep(20);//打完一行停顿
		}
	}
	cct_setcursor(CURSOR_VISIBLE_NORMAL);
}

void print_window(int screen_or_score,int score,int full_score)
{
	cct_showch(0, 0, ' ', COLOR_BLACK, COLOR_BLACK, 50);//覆盖上一次所写
	cct_setcolor(COLOR_BLACK, COLOR_WHITE);
	cct_gotoxy(0, 0);
	if (screen_or_score == SCREEN)
	{
		int cols, lines, buffer_cols, buffer_lines;
		cct_getconsoleborder(cols, lines, buffer_cols, buffer_lines);
		cout << "屏幕：" << lines << "行" << cols << "列";
	}
	else if (screen_or_score == SCORE)
		cout << "本次得分：" << score << "总得分：" << full_score;
}

void print_graph(struct magic_ball& magic, char mode,frame_lines frame)
{
	int line_parm;
	set_lineparm(mode, line_parm);
	int basey;
	if (mode >= 'A' && mode <= 'G')
		basey = 3;
	else if (mode >= '0' && mode <= '9')
		basey = 2;
	cct_setcursor(CURSOR_INVISIBLE);
	if (mode >= 'A' && mode <= 'G')
		print_head(magic, line_parm);
	print_frame_line(frame.l_u_corner, frame.r_u_corner, magic.x, basey - 1, mode,frame);
	print_frame_col(magic.x, magic.y, mode,frame);
	print_frame_line(frame.l_d_corner, frame.r_d_corner, magic.x, magic.y * line_parm + basey - (1 - line_parm % 2), mode,frame);
	print_ball(magic, mode);
	print_window();
	cct_setcursor(CURSOR_VISIBLE_NORMAL);
}

void cancel_previous(struct magic_ball& magic,int line_parm)
{
	struct location loc;
	for (loc.i = 0; loc.i < magic.y; loc.i++)
	{
		for (loc.j = 0; loc.j < magic.x; loc.j++)
		{
			if (magic.ball2[loc.i][loc.j] !=0&&magic.ball[loc.i][loc.j]!=0)//将上一次选中的消除
			{
				magic.ball2[loc.i][loc.j] = 0;
				print_one_ball("◎", loc, magic, line_parm, COLOR_BLACK, 1);
			}
		}
	}
}

int mouse_pos_legal(int X, int Y, struct magic_ball& magic, int line_parm, struct location& origin,int game)//判断左键单击位置是否合法
{
	int basex, basey;
	set_base(game,basex,basey);
	//X--;
	//Y--;
	print_bottom(magic.y, line_parm, "[当前光标] ");
	origin.i = (Y-basey)/line_parm;//basey包含了边框上界
	origin.j = (X-basex-2)/(line_parm*2);//basex不包含边框左界
	int leg;//leg=1表示合法，0表示非法
	if (line_parm % 2 == 1)
	{
		if (Y >= basey && Y < (basey + line_parm * magic.y) && X >= (basex + 2) && X < (basex + 2 + line_parm * 2 * magic.x))
			leg = 1;
		else
			leg = 0;
	}
	else
	{
		if (((Y-basey+1)%line_parm!=0) && Y >= basey && Y < (basey + line_parm * magic.y) && (X - basex-2+1) % (line_parm * 2) != 0 && X >= (basex + 2) && X < (basex + 2 + line_parm * 2 * magic.x))//Y % 2 == 0 && Y != 0 && i < magic.y && (X % 4 == 2 || X % 4 == 3) && j < magic.x
			leg = 1;
		else
			leg = 0;
	}
	if (game == 1 && leg == 1 && magic.ball[origin.i][origin.j] == 0)
		leg = 0;//在星星游戏中，即使位置正确，若鼠标在的地方没有球，也是非法位置
	if (leg)
	{
		print_bottom(magic.y, line_parm, "", 11);
		cout << char('A' + origin.i) << "行" ;
		if (game == 0)
			cout << 1 + origin.j << "列";
		else if (game == 1)
		{
			cout << origin.j << "列";
			cancel_previous(magic, line_parm);
			magic.ball2[origin.i][origin.j] = 2;//变白色
			struct location loc2;
			loc2.i = origin.i;
			loc2.j = origin.j;
			print_one_ball("◎",  loc2, magic, line_parm, COLOR_HWHITE, 1);
		}
		return 1;
	}
	else
	{
		if(game==1)
			cancel_previous(magic, line_parm);
		print_bottom(magic.y, line_parm, "位置非法", 11);
		return 0;
	}
}

void select_end(int line_parm, int X, int Y, struct location& origin, int basex, int basey, struct magic_ball magic,char m_or_k)
{
	if(m_or_k=='m')
	{
		origin.i = (Y - basey) / line_parm;//basey包含了边框上界
		origin.j = (X - basex - 2) / (line_parm * 2);//basex不包含边框左界
	}
	print_bottom(magic.y, line_parm, "[当前选择] ");
	print_bottom(magic.y, line_parm, "", 11);
	cout << char('A' + origin.i) << "行";
	cout << origin.j << "列";
}
