/* 2350222 盛曦 计科 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <climits>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include "../include/cmd_console_tools.h"
#include "../include/cmd_gmw_tools.h"
using namespace std;

/* --------------------------------------------------
		此处可以给出需要的静态全局变量（尽可能少，最好没有）、静态全局只读变量/宏定义（个数不限）等
   -------------------------------------------------- */
#define FRAME_H_LEN (CFI_LEN-1)		//单个边框线宽度，即该字符串长度，即CFI_LEN减去\0
#define FRAME_V_LEN 1				//单个边框线高度

/* --------------------------------------------------
		此处可以给出需要的内部辅助工具函数
		1、函数名不限，建议为 gmw_inner_* 
		2、个数不限
		3、必须是static函数，确保只在本源文件中使用
   -------------------------------------------------- */
//static int gmw_inner_set_status_line(CONSOLE_GRAPHICS_INFO* const pCGI, const bool top=true, const bool lower=true)
//{
//	pCGI->top_status_line = top;
//	pCGI->lower_status_line = lower;
//	return 0;
//}

static int gmw_inner_bool_num(bool b, int x = 1,const bool flag=1)//flag为1：b为真取x，为0：b为假取x
{
	if(flag)
		return (b ? x : 0);
	else
		return (b ? 0 : x);
}

   //设置图形界面的起始xy和上下状态栏的起始xy
static int gmw_inner_set_start_point(CONSOLE_GRAPHICS_INFO* const pCGI)
{
	//pCGI->start_x=(pCGI->SLI.is_top_status_line?1:0)+ (pCGI->draw_frame_with_col_no ? 1 : 0);
	//pCGI->start_y = (pCGI->draw_frame_with_row_no ? 2 : 0);
	pCGI->start_y = pCGI->extern_up_lines;
	pCGI->start_x = pCGI->extern_left_cols;

	pCGI->SLI.top_start_x = pCGI->start_x;
	pCGI->SLI.top_start_y = pCGI->start_y;
	pCGI->SLI.lower_start_x = pCGI->SLI.top_start_x;
	pCGI->SLI.lower_start_y = pCGI->SLI.top_start_y + (pCGI->top_status_line ? 1 : 0) + (pCGI->draw_frame_with_col_no ? 1 : 0) + FRAME_V_LEN + pCGI->CFI.bhigh * pCGI->row_num+gmw_inner_bool_num(pCGI->CFI.separator,1,0);
	return 0;
}

static int gmw_inner_set_SLI_width(CONSOLE_GRAPHICS_INFO* const pCGI)
{
	pCGI->SLI.width = (pCGI->draw_frame_with_row_no?2:0) + pCGI->col_num * pCGI->CFI.bwidth+ FRAME_H_LEN+gmw_inner_bool_num(pCGI->CFI.separator,2,0);//bwidth是一个色块对应一个边框，最后会多出来一个边框
	return 0;
}

/* cmd窗口的大小 */
static int gmw_inner_set_cmd_size(CONSOLE_GRAPHICS_INFO* const pCGI)
{
	pCGI->lines = (pCGI->extern_up_lines + pCGI->extern_down_lines + (pCGI->top_status_line ? 1 : 0) + (pCGI->draw_frame_with_col_no ? 1 : 0) + pCGI->row_num * pCGI->CFI.bhigh + gmw_inner_bool_num(pCGI->CFI.separator, 1, 0)+ + (pCGI->lower_status_line ? 1 : 0)) + (1 + 3)+1;
	pCGI->cols = pCGI->extern_left_cols + pCGI->extern_right_cols + pCGI->SLI.width+1;
	//+1是为了美观吗？
	return 0;
}

static int gmw_inner_set(CONSOLE_GRAPHICS_INFO* const pCGI)
{
	gmw_inner_set_start_point(pCGI);
	gmw_inner_set_SLI_width(pCGI);
	gmw_inner_set_cmd_size(pCGI);
	return 0;
}

static int gmw_inner_draw_block(const CONSOLE_GRAPHICS_INFO* const pCGI, const int x, const int y, const int bdi_value,const BLOCK_DISPLAY_INFO* const bdi)
{
	int bgcolor, fgcolor;

	int a = 0;//a用于记录是bdi数组中的哪一个
	while (bdi_value != (bdi+a)->value)
		a++;

	char top_left[CBI_LEN];			// "╔"
	char lower_left[CBI_LEN];		// "╚"
	char top_right[CBI_LEN];			// "╗"
	char lower_right[CBI_LEN];		// "╝"
	char h_normal[CBI_LEN];			// "═"	//Horizontal
	char v_normal[CBI_LEN];			// "║"	//Vertical

	if (bdi_value == BDI_VALUE_BLANK)
	{
		strcpy(top_left, "  ");
		strcpy(lower_left, "  ");
		strcpy(top_right, "  ");
		strcpy(lower_right, "  ");
		strcpy(h_normal, "  ");
		strcpy(v_normal, "  ");

		bgcolor = pCGI->CFI.bgcolor;
		fgcolor = pCGI->CFI.fgcolor;
	}
	else
	{
		//gmw_set_block_linetype(&temp, pCGI->CBI.top_left, pCGI->CBI.lower_left, pCGI->CBI.top_right, pCGI->CBI.lower_right, pCGI->CBI.h_normal, pCGI->CBI.v_normal);
		strcpy(top_left, pCGI->CBI.top_left);
		strcpy(lower_left, pCGI->CBI.lower_left);
		strcpy(top_right, pCGI->CBI.top_right);
		strcpy(lower_right, pCGI->CBI.lower_right);
		strcpy(h_normal, pCGI->CBI.h_normal);
		strcpy(v_normal, pCGI->CBI.v_normal);
		if ((bdi + a)->bgcolor == -1)
			bgcolor = pCGI->area_bgcolor;
		else
			bgcolor = (bdi + a)->bgcolor;

		if ((bdi + a)->fgcolor == -1)
			fgcolor = pCGI->area_fgcolor;
		else
			fgcolor = (bdi + a)->fgcolor;
	}

	
	cct_setcursor(CURSOR_INVISIBLE);

	int i;
	int X, Y;

	if (bdi_value == BDI_VALUE_BLANK)
	{
		for(i=0;i<pCGI->CFI.block_high;i++)
		cct_showch(x, y + i, ' ', bgcolor, fgcolor, pCGI->CFI.block_width);
		Sleep(pCGI->delay_of_draw_block);
		return 0;
	}

	if (pCGI->CBI.block_border)//第一行边框
	{
		cct_showstr(x, y, top_left, bgcolor, fgcolor);
		Sleep(pCGI->delay_of_draw_block);
		cct_showstr(x+FRAME_H_LEN, y, h_normal, bgcolor, fgcolor, (pCGI->CFI.block_width - gmw_inner_bool_num(pCGI->CBI.block_border, 2 * FRAME_H_LEN))/2);
		Sleep(pCGI->delay_of_draw_block);
		cct_getxy(X, Y);
		cct_showstr(X,Y, top_right, bgcolor, fgcolor);
		Sleep(pCGI->delay_of_draw_block);
	}

	//中间行
	for(i=0;i<pCGI->CFI.block_high-gmw_inner_bool_num(pCGI->CBI.block_border,2*FRAME_V_LEN);i++)
	{
		//中间行的左边边框
		if (pCGI->CBI.block_border)
		{
			cct_showstr(x, y + (i+1)*FRAME_V_LEN, v_normal, bgcolor, fgcolor);
			Sleep(pCGI->delay_of_draw_block);
		}

		//输出内容
		if (i == (pCGI->CFI.block_high - gmw_inner_bool_num(pCGI->CBI.block_border, 2 * FRAME_V_LEN)) / 2)//最中间那行
		{
			int len_of_blank;
			if ((bdi + a)->content == NULL&& (bdi + a)->value!= BDI_VALUE_BLANK)//输出数字(数字不为0的情况）为什么需要判断？？？？？上面已经判断过了？？？？？？？？？？？？？？？？？
			{
				len_of_blank = pCGI->CFI.block_width - gmw_inner_bool_num(pCGI->CBI.block_border, 2 * FRAME_H_LEN) - (int(log10((bdi + a)->value))+1);//log10((bdi + a)->value)+1为数字的长度（位数）
				if (len_of_blank < 0)//idth不够放数字
					len_of_blank = 0;
				cct_showch(x + (pCGI->CBI.block_border ? FRAME_H_LEN : 0), y + (pCGI->CBI.block_border ? FRAME_V_LEN : 0) + i * FRAME_V_LEN, ' ', bgcolor, fgcolor, len_of_blank / 2);//打印前半个空格
				Sleep(pCGI->delay_of_draw_block);
				cct_getxy(X, Y);
				cct_showint(X, Y, (bdi + a)->value, bgcolor, fgcolor);//打印内容.数字超过长度怎么办？
				Sleep(pCGI->delay_of_draw_block);
				cct_getxy(X, Y);
				cct_showch(X, Y, ' ', bgcolor, fgcolor, len_of_blank - len_of_blank / 2);//打印后半个空格
				Sleep(pCGI->delay_of_draw_block);
			}
			else//输出图形
			{
				len_of_blank = pCGI->CFI.block_width - gmw_inner_bool_num(pCGI->CBI.block_border, 2 * FRAME_H_LEN) - strlen((bdi + a)->content);
				cct_showch(x + (pCGI->CBI.block_border ? FRAME_H_LEN : 0), y + (pCGI->CBI.block_border ? FRAME_V_LEN : 0) + i * FRAME_V_LEN, ' ', bgcolor, fgcolor, len_of_blank / 2);//打印前半个空格
				Sleep(pCGI->delay_of_draw_block);
				cct_getxy(X, Y);
				cct_showstr(X, Y, (bdi + a)->content, bgcolor, fgcolor);//打印内容
				Sleep(pCGI->delay_of_draw_block);
				cct_getxy(X, Y);
				cct_showch(X, Y, ' ', bgcolor, fgcolor, len_of_blank - len_of_blank / 2);//打印后半个空格
				Sleep(pCGI->delay_of_draw_block);
			}
		}
		else//并非最中间那行
		{
			cct_showch(x + (pCGI->CBI.block_border ? FRAME_H_LEN : 0), y + (pCGI->CBI.block_border ? FRAME_V_LEN : 0) + i * FRAME_V_LEN, ' ', bgcolor, fgcolor, pCGI->CFI.block_width - gmw_inner_bool_num(pCGI->CBI.block_border, 2 * FRAME_H_LEN));
			Sleep(pCGI->delay_of_draw_block);
		}

		//中间行的右边边框
		if (pCGI->CBI.block_border)
		{
			cct_getxy(X, Y);
			cct_showstr(X,Y, v_normal, bgcolor, fgcolor);
			Sleep(pCGI->delay_of_draw_block);
		}
	}//end of for

	if (pCGI->CBI.block_border)//最后一行的边框
	{
		cct_showstr(x, y+(i+1)*FRAME_V_LEN, lower_left, bgcolor, fgcolor);
		Sleep(pCGI->delay_of_draw_block);
		cct_showstr(x+FRAME_H_LEN, y + (i + 1) * FRAME_V_LEN, h_normal, bgcolor, fgcolor,(pCGI->CFI.block_width - gmw_inner_bool_num(pCGI->CBI.block_border, 2 * FRAME_H_LEN)) / 2);
		Sleep(pCGI->delay_of_draw_block);
		cct_getxy(X, Y);
		cct_showstr(X,Y, lower_right, bgcolor, fgcolor);
	}

	Sleep(pCGI->delay_of_draw_block);

	cct_setcursor(CURSOR_VISIBLE_NORMAL);

	return 0;
}
/* ----------------------------------------------- 
		实现下面给出的函数（函数声明不准动）
   ----------------------------------------------- */
/***************************************************************************
  函数名称：
  功    能：设置游戏主框架的行列数
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int row						：行数(错误则为0，不设上限，人为保证正确性)
			const int col						：列数(错误则为0，不设上限，人为保证正确性)
  返 回 值：
  说    明：1、指消除类游戏的矩形区域的行列值
            2、行列的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_rowcol(CONSOLE_GRAPHICS_INFO *const pCGI, const int row, const int col)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	
	//设置列
	if(col<0)
		pCGI->col_num = 0;
	else
		pCGI->col_num = col;

	//设置行
	if (row < 0)
		pCGI->row_num = 0;
	else
		pCGI->row_num = row;
	
	gmw_inner_set(pCGI);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置整个窗口（含游戏区、附加区在内的整个cmd窗口）的颜色
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int bg_color					：前景色（缺省COLOR_BLACK）
		   const int fg_color					：背景色（缺省COLOR_WHITE）
		   const bool cascade					：是否级联（缺省为true-级联）
  返 回 值：
  说    明：1、cascade = true时
				同步修改游戏主区域的颜色
				同步修改上下状态栏的正常文本的背景色和前景色，醒目文本的背景色（前景色不变）
			2、不检查颜色值错误及冲突，需要人为保证
				例：颜色非0-15
				    前景色背景色的值一致导致无法看到内容
					前景色正好是状态栏醒目前景色，导致无法看到醒目提示
					...
***************************************************************************/
int gmw_set_color(CONSOLE_GRAPHICS_INFO *const pCGI, const int bgcolor, const int fgcolor, const bool cascade)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	//设置整个图形界面颜色
	pCGI->area_bgcolor = bgcolor;
	pCGI->area_fgcolor = fgcolor;

	if(cascade)
	{
		//设置游戏主区域颜色
		gmw_set_frame_color(pCGI, bgcolor, fgcolor);

		//设置上下状态栏正常文本颜色
		pCGI->SLI.top_normal_bgcolor = bgcolor;
		pCGI->SLI.lower_normal_bgcolor = bgcolor;
		pCGI->SLI.top_normal_fgcolor = fgcolor;
		pCGI->SLI.lower_normal_fgcolor = fgcolor;

		//设置上下状态栏醒目文本背景色
		pCGI->SLI.top_catchy_bgcolor = bgcolor;
		pCGI->SLI.lower_catchy_bgcolor = bgcolor;
	}

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置窗口的字体
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const char *fontname					：字体名称（只能是"Terminal"和"新宋体"两种，错误则返回-1，不改变字体）
		   const int fs_high					：字体高度（缺省及错误为16，不设其它限制，人为保证）
		   const int fs_width					：字体高度（缺省及错误为8，不设其它限制，人为保证）
  返 回 值：
  说    明：1、与cmd_console_tools中的setfontsize相似，目前只支持“点阵字体”和“新宋体”
            2、若设置其它字体则直接返回，保持原字体设置不变
***************************************************************************/
int gmw_set_font(CONSOLE_GRAPHICS_INFO *const pCGI, const char *fontname, const int fs_high, const int fs_width)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	//设置字体高度
	if (fs_high < 0)
		pCGI->CFT.font_size_high = 16;
	else
		pCGI->CFT.font_size_high = fs_high;

	//设置字体宽度
	if (fs_width < 0)
		pCGI->CFT.font_size_width = 8;
	else
		pCGI->CFT.font_size_width = fs_width;

	//设置字体名称
	if (strcmp(fontname, "Terminal") == 0)
		cct_setfontsize("Terminal", pCGI->CFT.font_size_high, pCGI->CFT.font_size_width);
	else if (strcmp(fontname, "新宋体") == 0)
		cct_setfontsize("新宋体", pCGI->CFT.font_size_high,0);
	else
		return -1;

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置延时
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int type						：延时的类型（目前为3种）
		   const int delay_ms					：以ms为单位的延时
			   画边框的延时：0 ~ 不设上限，人为保证正确（<0则置0）
			   画色块的延时：0 ~ 不设上限，人为保证正确（<0则置0）
			   色块移动的延时：BLOCK_MOVED_DELAY_MS ~ 不设上限，人为保证正确（ <BLOCK_MOVED_DELAY_MS 则置 BLOCK_MOVED_DELAY_MS）
  返 回 值：
  说    明：
***************************************************************************/
int gmw_set_delay(CONSOLE_GRAPHICS_INFO *const pCGI, const int type, const int delay_ms)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	switch (type)
	{
		case DELAY_OF_DRAW_FRAME:
			if (delay_ms < 0)
				pCGI->delay_of_draw_frame = 0;
			else
				pCGI->delay_of_draw_frame = delay_ms;
			break;
		case DELAY_OF_DRAW_BLOCK:
			if (delay_ms < 0)
				pCGI->delay_of_draw_block = 0;
			else
				pCGI->delay_of_draw_block = delay_ms;
			break;
		case DELAY_OF_BLOCK_MOVED:
			if (delay_ms < BLOCK_MOVED_DELAY_MS)
				pCGI->delay_of_block_moved = BLOCK_MOVED_DELAY_MS;
			else
				pCGI->delay_of_block_moved = delay_ms;
			break;
		default:
			break;
	}

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  输入参数：设置游戏主框架结构之外需要保留的额外区域
  功    能：CONSOLE_GRAPHICS_INFO *const pCGI	：
		   const int up_lines					：上部额外的行（缺省及错误为0，不设上限，人为保证）
		   const int down_lines				：下部额外的行（缺省及错误为0，不设上限，人为保证）
		   const int left_cols					：左边额外的列（缺省及错误为0，不设上限，人为保证）
		   const int right_cols				：右边额外的列（缺省及错误为0，不设上限，人为保证）
  返 回 值：
  说    明：额外行列的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_ext_rowcol(CONSOLE_GRAPHICS_INFO *const pCGI, const int up_lines, const int down_lines, const int left_cols, const int right_cols)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	if (up_lines < 0)
		pCGI->extern_up_lines = 0;
	else
		pCGI->extern_up_lines = up_lines;

	if (down_lines < 0)
		pCGI->extern_down_lines = 0;
	else
		pCGI->extern_down_lines = down_lines;

	if (left_cols < 0)
		pCGI->extern_left_cols = 0;
	else
		pCGI->extern_left_cols = left_cols;

	if (right_cols < 0)
		pCGI->extern_right_cols = 0;
	else
		pCGI->extern_right_cols = right_cols;

	gmw_inner_set(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_FRAME_INFO 结构中的11种线型（缺省4种）
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int type						：1 - 全线 2 - 全单线 3 - 横双竖单 4 - 横单竖双
  返 回 值：
  说    明：
***************************************************************************/
int gmw_set_frame_default_linetype(CONSOLE_GRAPHICS_INFO *const pCGI, const int type)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	/*	双线框架："╔", "╚", "╗", "╝", "═", "║", "╦", "╩", "╠", "╣", "╬"
		单线框架："┏", "┗", "┓", "┛", "━", "┃", "┳", "┻", "┣", "┫", "╋"
		横双竖单: "╒", "╘", "╕", "╛", "═", "│", "╤", "╧", "╞", "╡", "╪"
		横单竖双："╓", "╙", "╖", "╜", "─", "║", "╥", "╨", "╟", "╢", "╫"
		*/
	switch (type)
	{
		case 1://全线
			gmw_set_frame_linetype(pCGI, "╔", "╚", "╗", "╝", "═", "║", "╦", "╩", "╠", "╣", "╬");
			break;
		case 2://全单线
			gmw_set_frame_linetype(pCGI, "┏", "┗", "┓", "┛", "━", "┃", "┳", "┻", "┣", "┫", "╋");
			break;
		case 3://横双竖单
			gmw_set_frame_linetype(pCGI, "╒", "╘", "╕", "╛", "═", "│", "╤", "╧", "╞", "╡", "╪");
			break;
		case 4://横单竖双
			gmw_set_frame_linetype(pCGI, "╓", "╙", "╖", "╜", "─", "║", "╥", "╨", "╟", "╢", "╫");
			break;
		default:
			break;
	}

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_FRAME_TYPE 结构中的11种线型
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const char *...						：共11种，具体见.h，此处略
  返 回 值：
  说    明：约定为一个中文制表符，可以使用其它内容，人为保证2字节
			1、超过2字节则只取前2字节
			2、如果给NULL，用两个空格替代
			3、如果给1字节，则补一个空格，如果因此而导致显示乱，不算错
***************************************************************************/
int gmw_set_frame_linetype(CONSOLE_GRAPHICS_INFO *const pCGI, const char *top_left, const char *lower_left, const char *top_right,
	const char *lower_right, const char *h_normal, const char *v_normal, const char *h_top_separator,
	const char *h_lower_separator, const char *v_left_separator, const char *v_right_separator, const char *mid_separator)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	strcpy(pCGI->CFI.top_left, top_left);
	strcpy(pCGI->CFI.lower_left, lower_left);
	strcpy(pCGI->CFI.top_right, top_right);
	strcpy(pCGI->CFI.lower_right, lower_right);
	strcpy(pCGI->CFI.h_normal, h_normal);
	strcpy(pCGI->CFI.v_normal, v_normal);
	strcpy(pCGI->CFI.h_top_separator, h_top_separator);
	strcpy(pCGI->CFI.h_lower_separator, h_lower_separator);
	strcpy(pCGI->CFI.v_left_separator, v_left_separator);
	strcpy(pCGI->CFI.v_right_separator, v_right_separator);
	strcpy(pCGI->CFI.mid_separator, mid_separator);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_FRAME_TYPE 结构中的色块数量大小、是否需要分隔线等
  输入参数：输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int block_width						：宽度（错误及缺省2，因为约定表格线为中文制表符，如果给出奇数，要+1）
			const int block_high						：高度（错误及缺省1）
			const bool separator						：是否需要分隔线（缺省为true，需要分隔线）
  返 回 值：
  说    明：框架大小/是否需要分隔线等的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_frame_style(CONSOLE_GRAPHICS_INFO *const pCGI, const int block_width, const int block_high, const bool separator)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	if (block_width <= 0)
		pCGI->CFI.block_width = 2;
	else if(block_width%2==1)
		pCGI->CFI.block_width = block_width+1;
	else
		pCGI->CFI.block_width = block_width;

	if(block_high<=0)
		pCGI->CFI.block_high = 1;
	else
		pCGI->CFI.block_high = block_high;

	pCGI->CFI.separator = separator;

	if (pCGI->CFI.separator)
	{
		pCGI->CFI.block_width_ext = CFI_LEN-1;
		pCGI->CFI.block_high_ext = 1;
	}
	else
	{
		pCGI->CFI.block_width_ext = 0;
		pCGI->CFI.block_high_ext = 0;
	}

	pCGI->CFI.bwidth = pCGI->CFI.block_width + pCGI->CFI.block_width_ext;
	pCGI->CFI.bhigh = pCGI->CFI.block_high + pCGI->CFI.block_high_ext;

	gmw_inner_set(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_BORDER_TYPE 结构中的颜色
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int bg_color					：背景色（缺省 -1表示用窗口背景色）
			const int fg_color					：前景色（缺省 -1表示用窗口前景色）
  返 回 值：
  说    明：不检查颜色值错误及冲突，需要人为保证
				例：颜色非0-15，前景色背景色的值一致导致无法看到内容等
***************************************************************************/
int gmw_set_frame_color(CONSOLE_GRAPHICS_INFO *const pCGI, const int bgcolor, const int fgcolor)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	if(bgcolor!=-1)
		pCGI->CFI.bgcolor = bgcolor;
	else
		pCGI->CFI.bgcolor = pCGI->area_bgcolor;

	if(fgcolor!=-1)
		pCGI->CFI.fgcolor = fgcolor;
	else
		pCGI->CFI.fgcolor = pCGI->area_fgcolor;

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_BLOCK_INFO 结构中的6种线型（缺省4种）
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int type						：1 - 全双线 2 - 全单线 3 - 横双竖单 4 - 横单竖双
  返 回 值：
  说    明：
***************************************************************************/
int gmw_set_block_default_linetype(CONSOLE_GRAPHICS_INFO *const pCGI, const int type)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	/*	双线框架："╔", "╚", "╗", "╝", "═", "║", "╦", "╩", "╠", "╣", "╬"
		单线框架："┏", "┗", "┓", "┛", "━", "┃", "┳", "┻", "┣", "┫", "╋"
		横双竖单: "╒", "╘", "╕", "╛", "═", "│", "╤", "╧", "╞", "╡", "╪"
		横单竖双："╓", "╙", "╖", "╜", "─", "║", "╥", "╨", "╟", "╢", "╫"
		*/

	switch (type)
	{
		case 1://全双线
			gmw_set_block_linetype(pCGI, "╔", "╚", "╗", "╝", "═", "║");
			break;
		case 2://全单线
			gmw_set_block_linetype(pCGI, "┏", "┗", "┓", "┛", "━", "┃");
			break;
		case 3://横双竖单
			gmw_set_block_linetype(pCGI, "╒", "╘", "╕", "╛", "═", "│");
			break;
		case 4://横单竖双
			gmw_set_block_linetype(pCGI, "╓", "╙", "╖", "╜", "─", "║");
			break;
		default:
			break;
	}

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：填充 CONSOLE_BLOCK_INFO 结构中的6种线型
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const char *...					：共6种，具体见.h，此处略
  返 回 值：
  说    明：约定为一个中文制表符，可以使用其它内容，人为保证2字节
			1、超过2字节则只取前2字节
			2、如果给NULL，用两个空格替代
			3、如果给1字节，则补一个空格，如果因此而导致显示乱，不算错
***************************************************************************/
int gmw_set_block_linetype(CONSOLE_GRAPHICS_INFO *const pCGI, const char *top_left, const char *lower_left, const char *top_right, const char *lower_right, const char *h_normal, const char *v_normal)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	if(top_left==NULL)
		strcpy(pCGI->CBI.top_left, "  ");
	else if (strlen(top_left) > 2)
	{
		strncpy(pCGI->CBI.top_left, top_left, 2);
		pCGI->CBI.top_left[2] = '\0';
	}
	else if (strlen(top_left) == 1)
	{
		strncpy(pCGI->CBI.top_left, top_left, 1);
		pCGI->CBI.top_left[1] = ' ';
		pCGI->CBI.top_left[2] = '\0';
	}
	else
		strcpy(pCGI->CBI.top_left, top_left);

	if (lower_left == NULL)
		strcpy(pCGI->CBI.lower_left, "  ");
	else if (strlen(lower_left) > 2)
	{
		strncpy(pCGI->CBI.lower_left, lower_left, 2);
		pCGI->CBI.lower_left[2] = '\0';
	}
	else if (strlen(lower_left) == 1)
	{
		strncpy(pCGI->CBI.lower_left, lower_left, 1);
		pCGI->CBI.lower_left[1] = ' ';
		pCGI->CBI.lower_left[2] = '\0';
	}
	else
		strcpy(pCGI->CBI.lower_left, lower_left);

	if (top_right == NULL)
		strcpy(pCGI->CBI.top_right, "  ");
	else if (strlen(top_right) > 2)
	{
		strncpy(pCGI->CBI.top_right, top_right, 2);
		pCGI->CBI.top_right[2] = '\0';
	}
	else if (strlen(top_right) == 1)
	{
		strncpy(pCGI->CBI.top_right, top_right, 1);
		pCGI->CBI.top_right[1] = ' ';
		pCGI->CBI.top_right[2] = '\0';
	}
	else
		strcpy(pCGI->CBI.top_right, top_right);

	if (lower_right == NULL)
		strcpy(pCGI->CBI.lower_right, "  ");
	else if (strlen(lower_right) > 2)
	{
		strncpy(pCGI->CBI.lower_right, lower_right, 2);
		pCGI->CBI.lower_right[2] = '\0';
	}
	else if (strlen(lower_right) == 1)
	{
		strncpy(pCGI->CBI.lower_right, lower_right, 1);
		pCGI->CBI.lower_right[1] = ' ';
		pCGI->CBI.lower_right[2] = '\0';
	}
	else
		strcpy(pCGI->CBI.lower_right, lower_right);

	if (h_normal == NULL)
		strcpy(pCGI->CBI.h_normal, "  ");
	else if (strlen(h_normal) > 2)
	{
		strncpy(pCGI->CBI.h_normal, h_normal, 2);
		pCGI->CBI.h_normal[2] = '\0';
	}
	else if (strlen(h_normal) == 1)
	{
		strncpy(pCGI->CBI.h_normal, h_normal, 1);
		pCGI->CBI.h_normal[1] = ' ';
		pCGI->CBI.h_normal[2] = '\0';
	}
	else
		strcpy(pCGI->CBI.h_normal, h_normal);

	if (v_normal == NULL)
		strcpy(pCGI->CBI.v_normal, "  ");
	else if (strlen(v_normal) > 2)
	{
		strncpy(pCGI->CBI.v_normal, v_normal, 2);
		pCGI->CBI.v_normal[2] = '\0';
	}
	else if (strlen(v_normal) == 1)
	{
		strncpy(pCGI->CBI.v_normal, v_normal, 1);
		pCGI->CBI.v_normal[1] = ' ';
		pCGI->CBI.v_normal[2] = '\0';
	}
	else
		strcpy(pCGI->CBI.v_normal, v_normal);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置每个游戏色块(彩球)是否需要小边框
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const bool on_off					：true - 需要 flase - 不需要（缺省false）
  返 回 值：
  说    明：边框约定为中文制表符，双线
***************************************************************************/
int gmw_set_block_border_switch(CONSOLE_GRAPHICS_INFO *const pCGI, const bool on_off)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	pCGI->CBI.block_border = on_off;

	gmw_inner_set(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置是否显示上下状态栏
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int type						：状态栏类型（上/下）
			const bool on_off					：true - 需要 flase - 不需要（缺省true）
  返 回 值：
  说    明：1、状态栏的相关约定如下：
			   1.1、上状态栏只能一行，在主区域最上方框线/列标的上面，为主区域的最开始一行（主区域的左上角坐标就是上状态栏的坐标）
			   1.2、下状态栏只能一行，在主区域最下方框线的下面
			   1.3、状态栏的宽度为主区域宽度，如果信息过长则截断
		   2、行列的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_status_line_switch(CONSOLE_GRAPHICS_INFO *const pCGI, const int type, const bool on_off)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	switch (type)
	{
		case TOP_STATUS_LINE:
			pCGI->SLI.is_top_status_line = on_off;
			pCGI->top_status_line = on_off;
			break;
		case LOWER_STATUS_LINE:
			pCGI->SLI.is_lower_status_line = on_off;
			pCGI->lower_status_line = on_off;
			break;
		default:
			break;
	}
	
	gmw_inner_set(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置上下状态栏的颜色
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const int type						：状态栏类型（上/下）
			const int normal_bgcolor			：正常文本背景色（缺省 -1表示使用窗口背景色）
			const int normal_fgcolor			：正常文本前景色（缺省 -1表示使用窗口前景色）
			const int catchy_bgcolor			：醒目文本背景色（缺省 -1表示使用窗口背景色）
			const int catchy_fgcolor			：醒目文本前景色（缺省 -1表示使用亮黄色）
  输入参数：
  返 回 值：
  说    明：不检查颜色值错误及冲突，需要人为保证
				例：颜色非0-15，前景色背景色的值一致导致无法看到内容等
***************************************************************************/
int gmw_set_status_line_color(CONSOLE_GRAPHICS_INFO *const pCGI, const int type, const int normal_bgcolor, const int normal_fgcolor, const int catchy_bgcolor, const int catchy_fgcolor)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	switch (type)
	{
		case TOP_STATUS_LINE:
			if (normal_bgcolor != -1)
				pCGI->SLI.top_normal_bgcolor = normal_bgcolor;
			else
				pCGI->SLI.top_normal_bgcolor = pCGI->area_bgcolor;

			if (normal_fgcolor != -1)
				pCGI->SLI.top_normal_fgcolor = normal_fgcolor;
			else
				pCGI->SLI.top_normal_fgcolor = pCGI->area_fgcolor;

			if (catchy_bgcolor != -1)
				pCGI->SLI.top_catchy_bgcolor = catchy_bgcolor;
			else
				pCGI->SLI.top_catchy_bgcolor = pCGI->area_bgcolor;

			if (catchy_fgcolor != -1)
				pCGI->SLI.top_catchy_fgcolor = catchy_fgcolor;
			else
				pCGI->SLI.top_catchy_fgcolor = COLOR_HYELLOW;
			break;
		case LOWER_STATUS_LINE:
			if (normal_bgcolor != -1)
				pCGI->SLI.lower_normal_bgcolor = normal_bgcolor;
			else
				pCGI->SLI.lower_normal_bgcolor = pCGI->area_bgcolor;

			if (normal_fgcolor != -1)
				pCGI->SLI.lower_normal_fgcolor = normal_fgcolor;
			else
				pCGI->SLI.lower_normal_fgcolor = pCGI->area_fgcolor;

			if (catchy_bgcolor != -1)
				pCGI->SLI.lower_catchy_bgcolor = catchy_bgcolor;
			else
				pCGI->SLI.lower_catchy_bgcolor = pCGI->area_bgcolor;

			if (catchy_fgcolor != -1)
				pCGI->SLI.lower_catchy_fgcolor = catchy_fgcolor;
			else
				pCGI->SLI.lower_catchy_fgcolor = COLOR_HYELLOW;
			break;
		default:
			break;
	}

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置是否显示行号
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const bool on_off					：true - 显示 flase - 不显示（缺省false）
  返 回 值：
  说    明：1、行号约定为字母A开始连续排列（如果超过26，则从a开始，超过52的统一为*，实际应用不可能）
            2、是否显示行号的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_rowno_switch(CONSOLE_GRAPHICS_INFO *const pCGI, const bool on_off)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	pCGI->draw_frame_with_row_no = on_off;

	gmw_inner_set(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：设置是否显示列标
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
			const bool on_off					：true - 显示 flase - 不显示（缺省false）
  返 回 值：
  说    明：1、列标约定为数字0开始连续排列（数字0-99，超过99统一为**，实际应用不可能）
            2、是否显示列标的变化会导致CONSOLE_GRAPHICS_INFO结构体中其它成员值的变化，要处理
***************************************************************************/
int gmw_set_colno_switch(CONSOLE_GRAPHICS_INFO *const pCGI, const bool on_off)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	pCGI->draw_frame_with_col_no = on_off;

	gmw_inner_set(pCGI);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：打印 CONSOLE_GRAPHICS_INFO 结构体中的各成员值
  输入参数：
  返 回 值：
  说    明：1、仅供调试用，打印格式自定义
            2、本函数测试用例中未调用过，可以不实现
***************************************************************************/
int gmw_print(const CONSOLE_GRAPHICS_INFO *const pCGI)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;
	int cols; int lines; int buffer_cols; int buffer_lines;
	cct_getconsoleborder(cols, lines, buffer_cols, buffer_lines);
	cct_setconsoleborder(100, 100, 1000, 1000);

	// 打印 CONSOLE_FRAME_INFO
	cout << "CONSOLE_FRAME_INFO:" << endl;
	cout << "  top_left: " << pCGI->CFI.top_left << endl;
	cout << "  lower_left: " << pCGI->CFI.lower_left << endl;
	cout << "  top_right: " << pCGI->CFI.top_right << endl;
	cout << "  lower_right: " << pCGI->CFI.lower_right << endl;
	cout << "  h_normal: " << pCGI->CFI.h_normal << endl;
	cout << "  v_normal: " << pCGI->CFI.v_normal << endl;
	cout << "  h_top_separator: " << pCGI->CFI.h_top_separator << endl;
	cout << "  h_lower_separator: " << pCGI->CFI.h_lower_separator << endl;
	cout << "  v_left_separator: " << pCGI->CFI.v_left_separator << endl;
	cout << "  v_right_separator: " << pCGI->CFI.v_right_separator << endl;
	cout << "  mid_separator: " << pCGI->CFI.mid_separator << endl;
	cout << "  bgcolor: " << pCGI->CFI.bgcolor << endl;
	cout << "  fgcolor: " << pCGI->CFI.fgcolor << endl;
	cout << "  block_width: " << pCGI->CFI.block_width << endl;
	cout << "  block_high: " << pCGI->CFI.block_high << endl;
	cout << "  separator: " << (pCGI->CFI.separator ? "true" : "false") << endl;
	cout << "  block_width_ext: " << pCGI->CFI.block_width_ext << endl;
	cout << "  block_high_ext: " << pCGI->CFI.block_high_ext << endl;
	cout << "  bwidth: " << pCGI->CFI.bwidth << endl;
	cout << "  bhigh: " << pCGI->CFI.bhigh << endl;
	cout << endl;

	// 打印 CONSOLE_BLOCK_INFO
	cout << "CONSOLE_BLOCK_INFO:" << endl;
	cout << "top_left: " << pCGI->CBI.top_left << endl;
	cout << "lower_left: " << pCGI->CBI.lower_left << endl;
	cout << "top_right: " << pCGI->CBI.top_right << endl;
	cout << "lower_right: " << pCGI->CBI.lower_right << endl;
	cout << "h_normal: " << pCGI->CBI.h_normal << endl;
	cout << "v_normal: " << pCGI->CBI.v_normal << endl;
	cout << "block_border: " << (pCGI->CBI.block_border ? "true" : "false") << endl;
	cout << endl;

	// 打印 STATUS_LINE_INFO
	cout << "STATUS_LINE_INFO:" << endl;
	cout << "  is_top_status_line: " << (pCGI->SLI.is_top_status_line ? "true" : "false") << endl;
	cout << "  is_lower_status_line: " << (pCGI->SLI.is_lower_status_line ? "true" : "false") << endl;
	cout << "  top_start_x: " << pCGI->SLI.top_start_x << endl;
	cout << "  top_start_y: " << pCGI->SLI.top_start_y << endl;
	cout << "  top_normal_bgcolor: " << pCGI->SLI.top_normal_bgcolor << endl;
	cout << "  top_normal_fgcolor: " << pCGI->SLI.top_normal_fgcolor << endl;
	cout << "  top_catchy_bgcolor: " << pCGI->SLI.top_catchy_bgcolor << endl;
	cout << "  top_catchy_fgcolor: " << pCGI->SLI.top_catchy_fgcolor << endl;
	cout << "  lower_start_x: " << pCGI->SLI.lower_start_x << endl;
	cout << "  lower_start_y: " << pCGI->SLI.lower_start_y << endl;
	cout << "  lower_normal_bgcolor: " << pCGI->SLI.lower_normal_bgcolor << endl;
	cout << "  lower_normal_fgcolor: " << pCGI->SLI.lower_normal_fgcolor << endl;
	cout << "  lower_catchy_bgcolor: " << pCGI->SLI.lower_catchy_bgcolor << endl;
	cout << "  lower_catchy_fgcolor: " << pCGI->SLI.lower_catchy_fgcolor << endl;
	cout << "  width: " << pCGI->SLI.width << endl;
	cout << endl;

	// 打印 CONSOLE_FONT_TYPE
	cout << "CONSOLE_FONT_TYPE:" << endl;
	cout << "font_type: " << pCGI->CFT.font_type << endl;
	cout << "font_size_width: " << pCGI->CFT.font_size_width << endl;
	cout << "font_size_high: " << pCGI->CFT.font_size_high << endl;
	cout << endl;

	// 打印其他成员
	cout << "其他成员:" << endl;
	if (pCGI->inited == CGI_INITED)
		cout << "  inited: " << "已初始化" << endl;
	else
		cout << "  inited: " << "未初始化" << endl;
	cout << "  extern_up_lines: " << pCGI->extern_up_lines << endl;
	cout << "  extern_down_lines: " << pCGI->extern_down_lines << endl;
	cout << "  extern_left_cols: " << pCGI->extern_left_cols << endl;
	cout << "  extern_right_cols: " << pCGI->extern_right_cols << endl;
	cout << "  row_num: " << pCGI->row_num << endl;
	cout << "  col_num: " << pCGI->col_num << endl;
	cout << "  area_bgcolor: " << pCGI->area_bgcolor << endl;
	cout << "  area_fgcolor: " << pCGI->area_fgcolor << endl;
	cout << "  top_status_line: " << (pCGI->top_status_line ? "true" : "false") << endl;
	cout << "  lower_status_line: " << (pCGI->lower_status_line ? "true" : "false") << endl;
	cout << "  draw_frame_with_row_no: " << (pCGI->draw_frame_with_row_no ? "true" : "false") << endl;
	cout << "  draw_frame_with_col_no: " << (pCGI->draw_frame_with_col_no ? "true" : "false") << endl;
	cout << "  delay_of_draw_frame: " << pCGI->delay_of_draw_frame << endl;
	cout << "  delay_of_draw_block: " << pCGI->delay_of_draw_block << endl;
	cout << "  delay_of_block_moved: " << pCGI->delay_of_block_moved << endl;
	cout << "  start_x: " << pCGI->start_x << endl;
	cout << "  start_y: " << pCGI->start_y << endl;
	cout << "  lines: " << pCGI->lines << endl;
	cout << "  cols: " << pCGI->cols << endl;

	cct_setconsoleborder(cols, lines, buffer_cols, buffer_lines);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：将 CONSOLE_GRAPHICS_INFO 结构体用缺省值进行初始化
  输入参数：CONSOLE_GRAPHICS_INFO *const pCGI：整体结构指针
		   const int row					：游戏区域色块行数（缺省10）
		   const int col					：游戏区域色块列数（缺省10）
		   const int bgcolor				：整个窗口背景色（缺省 COLOR_BLACK）
		   const int fgcolor				：整个窗口背景色（缺省 COLOR_WHITE）
  返 回 值：
  说    明：窗口背景黑/前景白，点阵16*8，上下左右无额外行列，上下状态栏均有，无行号/列标，框架线型为双线，色块宽度2/高度1/无小边框，颜色略
***************************************************************************/
int gmw_init(CONSOLE_GRAPHICS_INFO *const pCGI, const int row, const int col, const int bgcolor, const int fgcolor)
{
	/* 首先置标记 */
	pCGI->inited = CGI_INITED;

	gmw_set_rowcol(pCGI, row, col);

	gmw_set_color(pCGI);

	//gmw_inner_set_status_line(pCGI);

	gmw_set_delay(pCGI, DELAY_OF_DRAW_FRAME,0);
	gmw_set_delay(pCGI, DELAY_OF_DRAW_BLOCK, 0);
	gmw_set_delay(pCGI, DELAY_OF_BLOCK_MOVED, 15);

	//初始化CFI
	gmw_set_frame_default_linetype(pCGI, 1);

	gmw_set_frame_color(pCGI);

	gmw_set_frame_style(pCGI);

	//初始化CBI
	gmw_set_block_default_linetype(pCGI,1);
	gmw_set_block_border_switch(pCGI);

	//初始化SLI
	gmw_set_status_line_switch(pCGI, TOP_STATUS_LINE);
	gmw_set_status_line_switch(pCGI, LOWER_STATUS_LINE);

	gmw_set_status_line_color(pCGI, TOP_STATUS_LINE);
	gmw_set_status_line_color(pCGI, LOWER_STATUS_LINE);

	//初始化CFT
	strcpy(pCGI->CFT.font_type, "新宋体");
	gmw_set_font(pCGI,"新宋体", 16, 0);

	gmw_set_ext_rowcol(pCGI);
	gmw_set_rowno_switch(pCGI);
	gmw_set_colno_switch(pCGI);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：画主游戏框架
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
  返 回 值：
  说    明：具体可参考demo的效果
***************************************************************************/
int gmw_draw_frame(const CONSOLE_GRAPHICS_INFO *const pCGI)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	//设置窗口大小
	cct_setconsoleborder(pCGI->cols, pCGI->lines);

	int x, y;//坐标

	//设置窗口颜色,人工打印整个区域的颜色
	for(x=0;x<pCGI->lines;x++)
		cct_showch(0,x, ' ', pCGI->area_bgcolor, pCGI->area_fgcolor,pCGI->cols);

	cct_setcursor(CURSOR_INVISIBLE);

	int ex_x=0, ex_y=0;//额外的行列，由行标列标造成
	//若显示上状态栏
	if (pCGI->top_status_line)
		ex_y++;
	//若需要列标
	if (pCGI->draw_frame_with_col_no)
	{
		ex_y ++;
		cct_setcolor(pCGI->area_bgcolor, pCGI->area_fgcolor);
		cct_gotoxy(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0)+FRAME_H_LEN, pCGI->start_y + (pCGI->top_status_line ? 1 : 0));
		cout << setw(pCGI->CFI.block_width / 2) << 0;
		int flag = 0;//记录是否超过block_width
		int n=10;//用于记录i是否为10,100,1000...
		for (int i = 1; i < pCGI->col_num; i++)
		{
			cout << setw(pCGI->CFI.bwidth) << i;
			if (i==9)//(i+1) == n
			{
				n *= 10;
				cout << ' ';
			}
			if ((i+1)/int(pow(10,pCGI->CFI.block_width)) > 0)//当列标超过block_width，就显示为‘*’
			{
				flag = 1;
				break;
			}
		}
		if (flag)
		{
			for (int i = 1; i < pCGI->col_num-99; i++)
				cout << setw(pCGI->CFI.bwidth) << setfill('*') << '*'<< setfill(' ');
		}
	}
	//若需要行号
	if (pCGI->draw_frame_with_row_no)
	{
		ex_x +=2;
		int i;
		int flag = 0;//用于记录是否需要接着打印行号（行号超过26后，从a开始）
		for (i = 1; i <= pCGI->row_num; i++)
		{
			cct_showch(pCGI->start_x, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) - (pCGI->CFI.bhigh / 2) + pCGI->CFI.bhigh * i, char('A' + i - 1), pCGI->area_bgcolor, pCGI->area_fgcolor);
			if (i == 26)
			{
				flag = 1;
				i++;
				break;
			}
		}
		if (flag)
		{
			flag = 0;
			for (; i <= pCGI->row_num; i++)
			{
				cct_showch(pCGI->start_x, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) - (pCGI->CFI.bhigh / 2) + pCGI->CFI.bhigh * i, char('a' + i-1-26), pCGI->area_bgcolor, pCGI->area_fgcolor);
				if (i == 26+26)
				{
					flag = 1;
					i++;
					break;
				}
			}
		}
		if (flag)
		{
			for (; i <= pCGI->row_num; i++)
				cct_showch(pCGI->start_x, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) - (pCGI->CFI.bhigh / 2) + pCGI->CFI.bhigh * i, '*', pCGI->area_bgcolor, pCGI->area_fgcolor);
		}
	}

	//左上角
	cct_showstr(pCGI->start_x+ ex_x, pCGI->start_y+ ex_y,pCGI->CFI.top_left, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
	Sleep(pCGI->delay_of_draw_frame);
	//上方横线
	if (pCGI->CFI.separator)//block需要分割线
	{
		for (int i = 1; i < (pCGI->col_num * pCGI->CFI.bwidth)/FRAME_H_LEN; i++)
		{
			if (i% (pCGI->CFI.bwidth/2) == 0)//上半十字
				cct_showstr(pCGI->start_x + FRAME_H_LEN*i + ex_x, pCGI->start_y + ex_y, pCGI->CFI.h_top_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			else//普通横线
				cct_showstr(pCGI->start_x + FRAME_H_LEN*i + ex_x, pCGI->start_y + ex_y, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			Sleep(pCGI->delay_of_draw_frame);
		}
	}
	else//block不需要分割线
		cct_showstr(pCGI->start_x +FRAME_H_LEN + ex_x,pCGI->start_y + ex_y, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor,((pCGI->CFI.bwidth)*pCGI->col_num-FRAME_H_LEN + gmw_inner_bool_num(pCGI->CFI.separator, 2, 0))/2);
	Sleep(pCGI->delay_of_draw_frame);
	//右上角
	cct_getxy(y, x);
	cct_showstr(y, x, pCGI->CFI.top_right, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
	Sleep(pCGI->delay_of_draw_frame);

	//左右竖线
	for (int i = 1; i < pCGI->row_num*pCGI->CFI.bhigh + gmw_inner_bool_num(pCGI->CFI.separator, 1, 0); i++)
	{
		if (pCGI->CFI.separator)//block之间有分割线
		{
			if(i%(pCGI->CFI.block_high + FRAME_V_LEN) == 0)//上下两行block之间的分割线
			{
				cct_showstr(pCGI->start_x + ex_x, pCGI->start_y + i * FRAME_V_LEN + ex_y, pCGI->CFI.v_left_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);//左半十字

				for (int j = 1; j < pCGI->col_num; j++)//普通横线+全十字。因为不是const char*，所以无法用strcat
				{
					cct_showstr(pCGI->start_x + FRAME_H_LEN + (pCGI->CFI.bwidth) * (j - 1) + ex_x, pCGI->start_y + i * FRAME_V_LEN + ex_y, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor, (pCGI->CFI.bwidth - FRAME_H_LEN) / 2);
					cct_getxy(y, x);
					cct_showstr(y, x, pCGI->CFI.mid_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				}
				cct_getxy(y, x);
				cct_showstr(y, x, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor, (pCGI->CFI.bwidth - FRAME_H_LEN) / 2);//补一个普通横线

				cct_showstr(pCGI->start_x + (pCGI->CFI.bwidth) * pCGI->col_num + ex_x, pCGI->start_y + i * FRAME_V_LEN + ex_y, pCGI->CFI.v_right_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);//右半十字
			}
			else//一行block，此处画左右block之间的分割线
			{
				cct_showstr(pCGI->start_x + ex_x, pCGI->start_y + i * FRAME_V_LEN + ex_y, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);//最左边的起始竖线
				for (int j = 1; j < pCGI->col_num; j++)//中间分割block的若干竖线
					cct_showstr(pCGI->start_x + FRAME_H_LEN + (pCGI->CFI.bwidth) * (j - 1) + (pCGI->CFI.bwidth - FRAME_H_LEN) + ex_x, pCGI->start_y + i * FRAME_V_LEN + ex_y, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
				cct_showstr(pCGI->start_x + (pCGI->CFI.bwidth) * pCGI->col_num + ex_x, pCGI->start_y + i * FRAME_V_LEN + ex_y, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);//最右边的终止竖线
			}
		}
		else//block之间无分割线，普通地画最左边的起始竖线和最右边的终止竖线即可
		{
			cct_showstr(pCGI->start_x + ex_x, pCGI->start_y + i * FRAME_V_LEN + ex_y, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			cct_showstr(y, pCGI->start_y + i * FRAME_V_LEN + ex_y, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);//此处的y，x是上面画右上角时设置的
			//pCGI->start_x + (pCGI->CFI.bwidth) * pCGI->col_num + gmw_inner_bool_num(pCGI->CFI.separator, 1, 0) + ex_x,pCGI->start_y + i * FRAME_V_LEN + ex_y
		}
		Sleep(pCGI->delay_of_draw_frame);
	}
	//左下角
	cct_getxy(y, x);
	cct_showstr(pCGI->start_x + ex_x, x+1,pCGI->CFI.lower_left, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
	Sleep(pCGI->delay_of_draw_frame);
	//下方横线
	if (pCGI->CFI.separator)//block需要分割线
	{
		for (int i = 1; i < (pCGI->col_num * pCGI->CFI.bwidth)/FRAME_H_LEN; i++)
		{
			if (i % (pCGI->CFI.bwidth / 2) == 0)
				cct_showstr(pCGI->start_x + FRAME_H_LEN * i + ex_x, x+1, pCGI->CFI.h_lower_separator, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			else
				cct_showstr(pCGI->start_x + FRAME_H_LEN * i + ex_x, x+1, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
			Sleep(pCGI->delay_of_draw_frame);
		}
	}
	else//block不需要分割线
		cct_showstr(pCGI->start_x + FRAME_H_LEN + ex_x, x+1,pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor, ((pCGI->CFI.bwidth) * pCGI->col_num - FRAME_H_LEN + gmw_inner_bool_num(pCGI->CFI.separator, 2, 0)) / 2);
	Sleep(pCGI->delay_of_draw_frame);
	//右下角
	cct_getxy(y, x);
	cct_showstr(y, x, pCGI->CFI.lower_right, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);
	Sleep(pCGI->delay_of_draw_frame);

	cct_setcursor(CURSOR_VISIBLE_NORMAL);
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：在状态栏上显示信息
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int type							：指定是上/下状态栏
		   const char *msg						：正常信息
		   const char *catchy_msg					：需要特别标注的信息（在正常信息前显示）
  返 回 值：
  说    明：1、输出宽度限定为主框架的宽度（含行号列标位置），超出则截去
            2、如果最后一个字符是某汉字的前半个，会导致后面乱码，要处理
***************************************************************************/
int gmw_status_line(const CONSOLE_GRAPHICS_INFO *const pCGI, const int type, const char *msg, const char *catchy_msg)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	int start_x,start_y,catchy_bgcolor,catchy_fgcolor,normal_fgcolor,normal_bgcolor=0;
	switch (type)
	{
		case TOP_STATUS_LINE:
			if (!pCGI->top_status_line)
				return 0;
			start_x = pCGI->SLI.top_start_x;
			start_y = pCGI->SLI.top_start_y;
			catchy_fgcolor = pCGI->SLI.top_catchy_fgcolor;
			catchy_bgcolor = pCGI->SLI.top_catchy_bgcolor;
			normal_fgcolor = pCGI->SLI.top_normal_fgcolor;
			normal_bgcolor = pCGI->SLI.top_normal_bgcolor;
			break;
		case LOWER_STATUS_LINE:
			if (!pCGI->lower_status_line)
				return 0;
			start_x = pCGI->SLI.lower_start_x;
			start_y = pCGI->SLI.lower_start_y;
			catchy_fgcolor = pCGI->SLI.lower_catchy_fgcolor;
			catchy_bgcolor = pCGI->SLI.lower_catchy_bgcolor;
			normal_fgcolor = pCGI->SLI.lower_normal_fgcolor;
			normal_bgcolor = pCGI->SLI.lower_normal_bgcolor;
			break;
	}

	if(catchy_msg)
	{
		if (strlen(catchy_msg) > unsigned int(pCGI->SLI.width))
		{
			char* catchy_msg1;
			while (1)//这样不好，但是先这样吧，因为不想改变msg的值
			{
				catchy_msg1 = new char[pCGI->SLI.width + 1];
				if (catchy_msg1 == NULL)
				{
					delete[] catchy_msg1;
					cout << "内存分配失败" << endl;
					continue;
				}
				break;
			}
			strncpy(catchy_msg1, msg, pCGI->SLI.width);
			if (catchy_msg1[pCGI->SLI.width - 1] >= 0xA1 && catchy_msg1[pCGI->SLI.width - 1] <= 0xFE)//最后一个字符是汉字的前半个
				catchy_msg1[pCGI->SLI.width - 1] = ' ';
			catchy_msg1[pCGI->SLI.width] = '\0';
			cct_showstr(start_x,start_y, catchy_msg1, catchy_bgcolor, catchy_fgcolor);
			delete[] catchy_msg1;
			return 0;
		}
		else
			cct_showstr(start_x, start_y, catchy_msg, catchy_bgcolor,catchy_fgcolor);
	}

	int len_catchy;
	if (catchy_msg == NULL)
		len_catchy=0;
	else
		len_catchy = strlen(catchy_msg);
	if(msg)
	{
		if (strlen(msg) > unsigned int(pCGI->SLI.width) - len_catchy)
		{
			char* msg1;
			while (1)//这样不好，但是先这样吧，因为不想改变msg的值
			{
				msg1 = new char[pCGI->SLI.width + 1];
				if (msg1 == NULL)
				{
					delete[] msg1;
					cout << "内存分配失败" << endl;
					continue;
				}
				break;
			}
			strncpy(msg1, msg, pCGI->SLI.width);
			if (msg1[pCGI->SLI.width - 1] >= 0xA1 && msg1[pCGI->SLI.width - 1] <= 0xFE)//最后一个字符是汉字的前半个
				msg1[pCGI->SLI.width - 1] = ' ';
			msg1[pCGI->SLI.width] = '\0';
			cct_showstr(start_x, start_y, msg1, normal_bgcolor, normal_fgcolor);
			delete[] msg1;
		}
		else
			cct_showstr(start_x + len_catchy, start_y, msg, normal_bgcolor, normal_fgcolor);
	}

	if (len_catchy + strlen(msg) < unsigned int(pCGI->SLI.width))
		cct_showch(start_x + len_catchy + strlen(msg), start_y, ' ', normal_bgcolor, normal_fgcolor, pCGI->SLI.width - len_catchy - strlen(msg));
	
	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：显示某一个色块(内容为字符串，坐标为row/col)
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int row_no						：行号（从0开始，人为保证正确性，程序不检查）			为什么程序不检查？？？？？？？？？？？？？？？
		   const int col_no						：列号（从0开始，人为保证正确性，程序不检查）
		   const int bdi_value						：需要显示的值
		   const BLOCK_DISPLAY_INFO *const bdi		：存放该值对应的显示信息的结构体数组
  返 回 值：
  说    明：1、BLOCK_DISPLAY_INFO 的含义见头文件，用法参考测试样例
            2、bdi_value为 BDI_VALUE_BLANK 表示空白块，要特殊处理				为什么不是（bdi+bdi_value)->value????????????????????????????
***************************************************************************/
int gmw_draw_block(const CONSOLE_GRAPHICS_INFO *const pCGI, const int row_no, const int col_no, const int bdi_value, const BLOCK_DISPLAY_INFO *const bdi)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	if (row_no < 0 || col_no < 0)//输入错误or移动色块时移动到了边界外，回绕？？？？？？？？？？？？？？？
		return -2;
	//CONSOLE_GRAPHICS_INFO temp;//用于记录此次的六种线型，用类是因为方便一次性赋值
	//gmw_init(&temp);

	int x = pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + FRAME_H_LEN + col_no * pCGI->CFI.bwidth;
	int y = pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh;

	gmw_inner_draw_block(pCGI,x , y, bdi_value, bdi);

	//int bgcolor, fgcolor;

	//char top_left[CBI_LEN];			// "╔"
	//char lower_left[CBI_LEN];		// "╚"
	//char top_right[CBI_LEN];			// "╗"
	//char lower_right[CBI_LEN];		// "╝"
	//char h_normal[CBI_LEN];			// "═"	//Horizontal
	//char v_normal[CBI_LEN];			// "║"	//Vertical

	//if (bdi_value == BDI_VALUE_BLANK)
	//{
	//	strcpy(top_left, "  ");
	//	strcpy(lower_left, "  ");
	//	strcpy(top_right, "  ");
	//	strcpy(lower_right, "  ");
	//	strcpy(h_normal, "  ");
	//	strcpy(v_normal, "  ");

	//	bgcolor = pCGI->CFI.bgcolor;
	//	fgcolor = pCGI->CFI.fgcolor;
	//}
	//else
	//{
	//	//gmw_set_block_linetype(&temp, pCGI->CBI.top_left, pCGI->CBI.lower_left, pCGI->CBI.top_right, pCGI->CBI.lower_right, pCGI->CBI.h_normal, pCGI->CBI.v_normal);
	//	strcpy(top_left, pCGI->CBI.top_left);
	//	strcpy(lower_left, pCGI->CBI.lower_left);
	//	strcpy(top_right, pCGI->CBI.top_right);
	//	strcpy(lower_right, pCGI->CBI.lower_right);
	//	strcpy(h_normal, pCGI->CBI.h_normal);
	//	strcpy(v_normal, pCGI->CBI.v_normal);
	//	if ((bdi + bdi_value)->bgcolor == -1)
	//		bgcolor = pCGI->area_bgcolor;
	//	else
	//		bgcolor = (bdi + bdi_value)->bgcolor;

	//	if ((bdi + bdi_value)->fgcolor == -1)
	//		fgcolor = pCGI->area_fgcolor;
	//	else
	//		fgcolor = (bdi + bdi_value)->fgcolor;
	//}

	//cct_setcursor(CURSOR_INVISIBLE);

	//if (pCGI->CBI.block_border)//第一行边框+中间一行的左边边框
	//{
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 1*FRAME_H_LEN + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0)+(pCGI->top_status_line?1:0) + FRAME_V_LEN+row_no * pCGI->CFI.bhigh,top_left, bgcolor, fgcolor);
	//	Sleep(pCGI->delay_of_draw_block);
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 2 * FRAME_H_LEN + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh,  h_normal, bgcolor, fgcolor);
	//	Sleep(pCGI->delay_of_draw_block);
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 3*FRAME_H_LEN + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh,  top_right, bgcolor, fgcolor);
	//	Sleep(pCGI->delay_of_draw_block);
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 1*FRAME_H_LEN + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh+FRAME_V_LEN,  v_normal, bgcolor, fgcolor);
	//	Sleep(pCGI->delay_of_draw_block);
	//}
	////输出内容
	//if(bdi_value == BDI_VALUE_BLANK)
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 1 * FRAME_H_LEN + (pCGI->CBI.block_border ? FRAME_H_LEN : 0) + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh + (pCGI->CBI.block_border ? FRAME_V_LEN : 0), "  ", bgcolor, fgcolor);
	//else if((bdi+bdi_value)->content==NULL)
	//{
	//	cct_showint(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 1*FRAME_H_LEN + (pCGI->CBI.block_border ? FRAME_H_LEN : 0) + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh+(pCGI->CBI.block_border?FRAME_V_LEN:0), (bdi+bdi_value)->value, bgcolor, fgcolor);
	//	Sleep(pCGI->delay_of_draw_block);
	//	cct_showch(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 1 * FRAME_H_LEN + (pCGI->CBI.block_border ? FRAME_H_LEN : 0) + col_no * pCGI->CFI.bwidth+1, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh + (pCGI->CBI.block_border ? FRAME_V_LEN : 0), ' ', bgcolor, fgcolor);
	//}
	//else
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 1*FRAME_H_LEN + (pCGI->CBI.block_border ? FRAME_H_LEN : 0)+ col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh + (pCGI->CBI.block_border ? FRAME_V_LEN : 0), (bdi+bdi_value)->content, bgcolor, fgcolor);
	//Sleep(pCGI->delay_of_draw_block);

	//if (pCGI->CBI.block_border)//中间一行的右边边框+最后一行的边框
	//{
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 3*FRAME_H_LEN + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh + FRAME_V_LEN,  v_normal, bgcolor, fgcolor);
	//	Sleep(pCGI->delay_of_draw_block);

	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 1*FRAME_H_LEN + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh+2,  lower_left, bgcolor, fgcolor);
	//	Sleep(pCGI->delay_of_draw_block);
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 2 * FRAME_H_LEN + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh+2,  h_normal, bgcolor, fgcolor);
	//	Sleep(pCGI->delay_of_draw_block);
	//	cct_showstr(pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + 3 * FRAME_H_LEN + col_no * pCGI->CFI.bwidth, pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh + 2,  lower_right, bgcolor, fgcolor);
	//}

	//Sleep(pCGI->delay_of_draw_block);

	//cct_setcursor(CURSOR_VISIBLE_NORMAL);

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：移动某一个色块
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   const int row_no						：行号（从0开始，人为保证正确性，程序不检查）
		   const int col_no						：列号（从0开始，人为保证正确性，程序不检查）
		   const int bdi_value						：需要显示的值
		   const int blank_bdi_value				：移动过程中用于动画效果显示时用于表示空白的值（一般为0，此处做为参数代入，是考虑到可能出现的特殊情况）
		   const BLOCK_DISPLAY_INFO *const bdi		：存放显示值/空白值对应的显示信息的结构体数组
		   const int direction						：移动方向，一共四种，具体见cmd_gmw_tools.h
		   const int distance						：移动距离（从1开始，人为保证正确性，程序不检查）
  返 回 值：
  说    明：
***************************************************************************/
int gmw_move_block(const CONSOLE_GRAPHICS_INFO *const pCGI, const int row_no, const int col_no, const int bdi_value, const int blank_bdi_value, const BLOCK_DISPLAY_INFO *const bdi, const int direction, const int distance)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
	{
		cct_setcursor(CURSOR_VISIBLE_NORMAL);
		return -1;
	}

	if (distance <= 0)
		return 0;

	int x = pCGI->start_x + (pCGI->draw_frame_with_row_no ? 2 : 0) + FRAME_H_LEN + col_no * pCGI->CFI.bwidth;
	int y = pCGI->start_y + (pCGI->draw_frame_with_col_no ? 1 : 0) + (pCGI->top_status_line ? 1 : 0) + FRAME_V_LEN + row_no * pCGI->CFI.bhigh;
	int X, Y;//坐标

	cct_setcursor(CURSOR_INVISIBLE);

	switch (direction)
	{
		case DOWN_TO_UP:
			//移动一格
			if (pCGI->CFI.separator)//block之间有分割线时，要画跨越分割线的移动
			{
				for (int i = 0; i < (pCGI->CFI.bhigh - FRAME_V_LEN);i++)//是为了防止block_high不为1,边界线的高度不为1等情况，若这样则需改变for的花括号内的内容以及inner draw block的内容？？？？？？？？？？？？？？？？？？？？？？？？？？
				{
					gmw_inner_draw_block(pCGI, x , y-i, BDI_VALUE_BLANK, bdi);//填补当前空缺
					cct_getxy(X, Y);
					gmw_inner_draw_block(pCGI, x, y-i -FRAME_V_LEN, bdi_value, bdi);//y-i - pCGI->CFI.block_high
					Sleep(pCGI->delay_of_block_moved);
				}
				cct_showstr(X - pCGI->CFI.block_width, Y -FRAME_V_LEN, pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor, pCGI->CFI.block_width / 2);//填补框架线
				gmw_draw_block(pCGI, row_no - 1, col_no, bdi_value, bdi);//画下一个色块
				Sleep(pCGI->delay_of_block_moved);
			}
			else
			{
				gmw_draw_block(pCGI, row_no, col_no, blank_bdi_value, bdi);//填补当前空缺
				gmw_draw_block(pCGI, row_no - 1, col_no, bdi_value, bdi);//画下一个色块
				Sleep(pCGI->delay_of_block_moved);
			}
			gmw_move_block(pCGI, row_no - 1, col_no, bdi_value, blank_bdi_value, bdi, DOWN_TO_UP, distance - 1);//递归
			break;
		case UP_TO_DOWN:
			//移动一格
			if (pCGI->CFI.separator)//block之间有分割线时，要画跨越分割线的移动
			{
				for (int i = 0; i < (pCGI->CFI.bhigh - FRAME_V_LEN); i++)//是为了防止block_high不为1,边界线的高度不为1等情况，若这样则需改变for的花括号内的内容以及inner draw block的内容
				{
					gmw_inner_draw_block(pCGI, x, y+i, BDI_VALUE_BLANK, bdi);//填补当前空缺
					cct_getxy(X, Y);
					gmw_inner_draw_block(pCGI, x, y+i +FRAME_V_LEN, bdi_value, bdi);//y+i + pCGI->CFI.block_high
					Sleep(pCGI->delay_of_block_moved);
				}
				cct_showstr(X - pCGI->CFI.block_width, Y -(pCGI->CFI.block_high-2), pCGI->CFI.h_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor, pCGI->CFI.block_width / 2);//填补框架线
				gmw_draw_block(pCGI, row_no + 1, col_no, bdi_value, bdi);//画下一个色块
				Sleep(pCGI->delay_of_block_moved);
			}
			else
			{
				gmw_draw_block(pCGI, row_no, col_no, blank_bdi_value, bdi);//填补当前空缺
				gmw_draw_block(pCGI, row_no + 1, col_no, bdi_value, bdi);//画下一个色块
				Sleep(pCGI->delay_of_block_moved);
			}
			gmw_move_block(pCGI, row_no + 1, col_no, bdi_value, blank_bdi_value, bdi, UP_TO_DOWN, distance - 1);//递归
			break;
		case RIGHT_TO_LEFT:
			//移动一格
			if (pCGI->CFI.separator)//block之间有分割线时，要画跨越分割线的移动
			{
				for (int i = 0; i < (pCGI->CFI.bwidth - FRAME_H_LEN); i+=2)//是为了防止block_width不为1,边界线的宽度不为1等情况，若这样则需改变for的花括号内的内容以及inner draw block的内容
				{
					gmw_inner_draw_block(pCGI, x-i, y, BDI_VALUE_BLANK, bdi);//填补当前空缺
					cct_getxy(X, Y);
					gmw_inner_draw_block(pCGI, x-i - FRAME_H_LEN, y, bdi_value, bdi);// x-i - pCGI->CFI.block_width
					Sleep(pCGI->delay_of_block_moved);
				}
				for(int i=0;i< pCGI->CFI.block_high;i++)
					cct_showstr(X -2*FRAME_H_LEN, Y-(pCGI->CFI.block_high-1)+i, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);//填补框架线
				gmw_draw_block(pCGI, row_no, col_no - 1, bdi_value, bdi);//画下一个色块
				Sleep(pCGI->delay_of_block_moved);
			}
			else
			{
				gmw_draw_block(pCGI, row_no, col_no, blank_bdi_value, bdi);//填补当前空缺
				gmw_draw_block(pCGI, row_no, col_no - 1, bdi_value, bdi);//画下一个色块
				Sleep(pCGI->delay_of_block_moved);
			}
			gmw_move_block(pCGI, row_no, col_no - 1, bdi_value, blank_bdi_value, bdi, RIGHT_TO_LEFT, distance - 1);//递归
			break;
		case LEFT_TO_RIGHT:
			//移动一格
			if (pCGI->CFI.separator)//block之间有分割线时，要画跨越分割线的移动
			{
				for (int i = 0; i < (pCGI->CFI.bwidth - FRAME_H_LEN); i+=2)//是为了防止block_width不为1,边界线的宽度不为1等情况，若这样则需改变for的花括号内的内容以及inner draw block的内容
				{
					gmw_inner_draw_block(pCGI, x + i, y, BDI_VALUE_BLANK, bdi);//填补当前空缺
					cct_getxy(X, Y);
					gmw_inner_draw_block(pCGI, x + i + FRAME_H_LEN, y, bdi_value, bdi);//x + i + pCGI->CFI.block_width
					Sleep(pCGI->delay_of_block_moved);
				}
				for (int i = 0; i < pCGI->CFI.block_high; i++)
					cct_showstr(X-(pCGI->CFI.block_width-FRAME_H_LEN), Y - (pCGI->CFI.block_high - 1) + i, pCGI->CFI.v_normal, pCGI->CFI.bgcolor, pCGI->CFI.fgcolor);//填补框架线
				gmw_draw_block(pCGI, row_no, col_no + 1, bdi_value, bdi);//画下一个色块
				Sleep(pCGI->delay_of_block_moved);
			}
			else
			{
				gmw_draw_block(pCGI, row_no, col_no, blank_bdi_value, bdi);//填补当前空缺
				gmw_draw_block(pCGI, row_no, col_no + 1, bdi_value, bdi);//画下一个色块
				Sleep(pCGI->delay_of_block_moved);
			}
			gmw_move_block(pCGI, row_no, col_no + 1, bdi_value, blank_bdi_value, bdi, LEFT_TO_RIGHT, distance - 1);//递归
			break;
		default:
			break;
	}

	return 0; //此句可根据需要修改
}

/***************************************************************************
  函数名称：
  功    能：读键盘或鼠标
  输入参数：const CONSOLE_GRAPHICS_INFO *const pCGI	：整体结构指针
		   int &MAction							：如果返回 CCT_MOUSE_EVENT，则此值有效，为 MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK/MOUSE_RIGHT_BUTTON_CLICK 三者之一
		                                               如果返回 CCT_KEYBOARD_EVENT，则此值无效
		   int &MRow								：如果返回 CCT_MOUSE_EVENT 且 MAction = MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK，则此值有效，表示左键选择的游戏区域的行号（从0开始）
												  其余情况此值无效（如果访问无效值导致错误，不是本函数的错!!!）
		   int &MCol								：如果返回 CCT_MOUSE_EVENT 且 MAction = MOUSE_ONLY_MOVED/MOUSE_LEFT_BUTTON_CLICK，则此值有效，表示左键选择的游戏区域的列号（从0开始）
												  其余情况此值无效（如果访问无效值导致错误，不是本函数的错!!!）
		   int &KeyCode1							：如果返回 CCT_KEYBOARD_EVENT，则此值有效，为读到的键码（如果双键码，则为第一个）
												  其余情况此值无效（如果访问无效值导致错误，不是本函数的错!!!）
		   int &KeyCode2							：如果返回 CCT_KEYBOARD_EVENT，则此值有效，为读到的键码（如果双键码，则为第二个，如果是单键码，则为0）
												  其余情况此值无效（如果访问无效值导致错误，不是本函数的错!!!）
		   const bool update_lower_status_line		：鼠标移动时，是否要在本函数中显示"[当前光标] *行*列/位置非法"的信息（true=显示，false=不显示，缺省为true）
  返 回 值：函数返回约定
		   1、如果是鼠标移动，得到的MRow/MCol与传入的相同(鼠标指针微小的移动)，则不返回，继续读
							  得到行列非法位置，则不返回，根据 update_lower_status_line 的设置在下状态栏显示"[当前光标] 位置非法"
							  得到的MRow/MCol与传入的不同(行列至少一个变化)，根据 update_lower_status_line 的设置在下状态栏显示"[当前光标] *行*列"，再返回MOUSE_ONLY_MOVED（有些游戏返回后要处理色块的不同颜色显示）
		   2、如果是按下鼠标左键，且当前鼠标指针停留在主游戏区域的*行*列上，则返回 CCT_MOUSE_EVENT ，MAction 为 MOUSE_LEFT_BUTTON_CLICK, MRow 为行号，MCol 为列标
		                          且当前鼠标指针停留在非法区域（非游戏区域，游戏区域中的分隔线），则不返回，根据 update_lower_status_line 的设置在下状态栏显示"[当前光标] 位置非法"
		   3、如果是按下鼠标右键，则不判断鼠标指针停留区域是否合法，直接返回 CCT_MOUSE_EVENT ，MAction 为 MOUSE_RIGHT_BUTTON_CLICK, MRow、MCol取当前值（因为消灭星星的右键标记需要坐标）
		   4、如果按下键盘上的某键（含双键码按键），则直接返回 CCT_KEYBOARD_EVENT，KeyCode1/KeyCode2中为对应的键码值
 说    明：通过调用 cmd_console_tools.cpp 中的 read_keyboard_and_mouse 函数实现
***************************************************************************/
int gmw_read_keyboard_and_mouse(const CONSOLE_GRAPHICS_INFO *const pCGI, int &MAction, int &MRow, int &MCol, int &KeyCode1, int &KeyCode2, const bool update_lower_status_line)
{
	/* 防止在未调用 gmw_init 前调用其它函数 */
	if (pCGI->inited != CGI_INITED)
		return -1;

	int X = 0, Y = 0;
	int ret;
	int loop = 1;
	int flag=0;//记录光标是否合法

	char* msg = new(nothrow) char[20];
	if (msg == NULL)
	{
		cout << "内存分配失败" << endl;
		return -2;
	}

	cct_enable_mouse();

	cct_setcursor(CURSOR_INVISIBLE);

	while (loop)
	{
		ret = cct_read_keyboard_and_mouse(X, Y, MAction, KeyCode1, KeyCode2);

		if (ret == CCT_MOUSE_EVENT) {
			if (MAction == MOUSE_ONLY_MOVED || MAction == MOUSE_LEFT_BUTTON_CLICK)
			{
				//if (MAction == MOUSE_LEFT_BUTTON_CLICK)
				//{
				//	int a, b, c, d, e;
				//	cct_read_keyboard_and_mouse(a, b, c, d, e);//按左键实际返回左键+鼠标移动，因此再读一次消除多余的鼠标移动
				//}
				flag = 1;//先假设合法
				if (X < pCGI->start_x + gmw_inner_bool_num(pCGI->draw_frame_with_row_no, 2) + FRAME_H_LEN ||
					X >= pCGI->start_x + gmw_inner_bool_num(pCGI->draw_frame_with_row_no, 2) + pCGI->CFI.bwidth * pCGI->col_num ||
					Y < pCGI->start_y + gmw_inner_bool_num(pCGI->top_status_line) + gmw_inner_bool_num(pCGI->draw_frame_with_col_no) + FRAME_V_LEN ||
					Y >= pCGI->start_y + gmw_inner_bool_num(pCGI->top_status_line) + gmw_inner_bool_num(pCGI->draw_frame_with_col_no) + pCGI->CFI.bhigh * pCGI->row_num)
					flag = 0;
				if (flag == 1 && pCGI->CFI.separator)
				{
					if ((X - pCGI->start_x - gmw_inner_bool_num(pCGI->draw_frame_with_row_no, 2) - FRAME_H_LEN) % pCGI->CFI.bwidth >= pCGI->CFI.block_width ||
						(Y - pCGI->start_y - gmw_inner_bool_num(pCGI->top_status_line) - gmw_inner_bool_num(pCGI->draw_frame_with_col_no) - FRAME_V_LEN) % pCGI->CFI.bhigh >= pCGI->CFI.block_high)
						flag = 0;
				}
				if (flag)
				{
					int MRow0 = (Y - pCGI->start_y - gmw_inner_bool_num(pCGI->draw_frame_with_col_no) - gmw_inner_bool_num(pCGI->top_status_line) - FRAME_V_LEN) / pCGI->CFI.bhigh;
					int MCol0 = (X - pCGI->start_x - gmw_inner_bool_num(pCGI->draw_frame_with_row_no, 2) - FRAME_H_LEN) / pCGI->CFI.bwidth;
					if (MRow0 == MRow && MCol0 == MCol&&MAction==MOUSE_ONLY_MOVED)
						continue;
					else
					{
						MRow = MRow0;
						MCol = MCol0;
						if (MAction == MOUSE_ONLY_MOVED&& update_lower_status_line)
						{
							sprintf(msg, "[当前光标] %c行%d列", char('A' + MRow), MCol);
							gmw_status_line(pCGI, LOWER_STATUS_LINE, msg);
						}
						/*else if(MAction == MOUSE_LEFT_BUTTON_CLICK)
							sprintf(msg, "[读到左键] %c行%d列", char('A' + MRow), MCol);*/
					}
				}
				else
				{
					MRow = -1;
					MCol = -1;
					if (MAction == MOUSE_ONLY_MOVED && update_lower_status_line)
					{
						sprintf(msg, "[当前光标] 位置非法");
						gmw_status_line(pCGI, LOWER_STATUS_LINE, msg);
					}
					continue;
				}
			}
		} //end of if (CCT_MOUSE_EVENT)
		else if (ret == CCT_KEYBOARD_EVENT) {
			if (KeyCode1 != 224) {//27:ESC键;32:SPACE键
				KeyCode2 = 0;
				/*if (KeyCode1 == 13)
					sprintf(msg,"[读到回车键]");
				else
					sprintf(msg, "[读到键码] %d/%d", KeyCode1, KeyCode2);*/
			}
			else
			{
				switch (KeyCode2) {
					case KB_ARROW_UP:	//上箭头
						//sprintf(msg, "[读到上箭头]");
						break;
					case KB_ARROW_DOWN:	//下箭头
						//sprintf(msg, "[读到下箭头]");
						break;
					case KB_ARROW_LEFT:	//左箭头
						//sprintf(msg, "[读到左箭头]");
						break;
					case KB_ARROW_RIGHT:	//右箭头
						//sprintf(msg, "[读到右箭头]");
						break;
				}
			}
		}//end of else if(ret == CCT_KEYBOARD_EVENT）
		loop = 0;
	}//end of while(loop)

	delete[] msg;
	cct_setcursor(CURSOR_VISIBLE_NORMAL);
	return ret; //此句可根据需要修改
}
