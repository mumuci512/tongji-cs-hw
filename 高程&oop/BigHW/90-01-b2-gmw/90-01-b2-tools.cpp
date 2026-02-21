/* 2350222 计科 盛曦 */
#include <iostream>
#include <conio.h>
#include <Windows.h>
#include "../include/cmd_console_tools.h"
#include "../include/cmd_gmw_tools.h"
#include "90-02-b1-with_magic_ball_gmw.h"
#include "90-01-b2.h"
using namespace std;

/*原本的fall用了冒泡，不适合graph*/
//for (j = 0; j < x; j++)
//{
//	for (i = 0; i < y; i++)
//	{
//		if (ball1[i][j] == 1)
//		{
//			ball[i][j] = 0;
//		}
//	}
//	for (i = 0; i < y - 1; i++)
//	{
//		for (k = 1; k < y - i; k++)
//		{
//			if (ball[k][j] == 0 && ball[k - 1][j] != 0)
//			{
//				exchange(&ball[k][j], &ball[k - 1][j]);
//				exchange(&ball1[k][j], &ball1[k - 1][j]);
//			}
//		}
//	}
//}

void FillBlank(struct magic_ball& magic, const CONSOLE_GRAPHICS_INFO* const cgi,const BLOCK_DISPLAY_INFO* const bdi)
{
	int i, j;
	srand((unsigned int)(time(0)));
	for (i = 0; i < magic.y; i++)
	{
		for (j = 0; j < magic.x; j++)
		{
			if (magic.ball[i][j] == 0)
			{
				magic.ball[i][j] = rand() % 8 + 1;
				gmw_draw_block(cgi, i, j, magic.ball[i][j], bdi);
				Sleep(50);
			}
		}
	}
}

void eliminate_middle(struct magic_ball& magic)
{
	int i, j;
	for (i = 0; i < magic.y; i++)
	{
		for (j = 0; j < magic.x; j++)
		{
			if (i - 1 >= 0 && j - 1 >= 0 && i + 1 < magic.y && magic.ball[i - 1][j] == magic.ball[i + 1][j] && magic.ball[i - 1][j] == magic.ball[i][j - 1])
			{
				magic.ball1[i][j] = 2;
				magic.ball1[i][j - 1] = 2;
			}
			if (i - 1 >= 0 && j + 1 < magic.x && i + 1 < magic.y && magic.ball[i - 1][j] == magic.ball[i + 1][j] && magic.ball[i - 1][j] == magic.ball[i][j + 1])
			{
				magic.ball1[i][j] = 2;
				magic.ball1[i][j + 1] = 2;
			}
			if (i - 1 >= 0 && j + 1 < magic.x && j - 1 >= 0 && magic.ball[i - 1][j] == magic.ball[i][j + 1] && magic.ball[i - 1][j] == magic.ball[i][j - 1])
			{
				magic.ball1[i][j] = 2;
				magic.ball1[i - 1][j] = 2;
			}
			if (i + 1 < magic.y && j + 1 < magic.x && j - 1 >= 0 && magic.ball[i + 1][j] == magic.ball[i][j - 1] && magic.ball[i + 1][j] == magic.ball[i][j + 1])
			{
				magic.ball1[i][j] = 2;
				magic.ball1[i + 1][j] = 2;
			}
		}
	}

}
void eliminate_edge(struct magic_ball& magic)
{
	int i, j;
	for (i = 0; i < magic.y; i++){
		for (j = 0; j < magic.x; j++){
			if (i + 2 < magic.y && magic.ball[i + 1][j] == magic.ball[i + 2][j]){
				if (i - 1 >= 0 && magic.ball[i - 1][j] == magic.ball[i + 1][j]){
					magic.ball1[i][j] = 2;
					magic.ball1[i - 1][j] = 2;
				}
				if (j - 1 >= 0 && magic.ball[i][j - 1] == magic.ball[i + 1][j]){
					magic.ball1[i][j] = 2;
					magic.ball1[i][j - 1] = 2;
				}
				if (j + 1 < magic.x && magic.ball[i][j + 1] == magic.ball[i + 1][j]){
					magic.ball1[i][j] = 2;
					magic.ball1[i][j + 1] = 2;
				}
			}
			if (i - 2 >= 0 && magic.ball[i - 1][j] == magic.ball[i - 2][j]){
				if (i + 1 < magic.y && magic.ball[i + 1][j] == magic.ball[i - 1][j]){
					magic.ball1[i][j] = 2;
					magic.ball1[i + 1][j] = 2;
				}
				if (j - 1 >= 0 && magic.ball[i][j - 1] == magic.ball[i - 1][j]){
					magic.ball1[i][j] = 2;
					magic.ball1[i][j - 1] = 2;
				}
				if (j + 1 < magic.x && magic.ball[i][j + 1] == magic.ball[i - 1][j]){
					magic.ball1[i][j] = 2;
					magic.ball1[i][j + 1] = 2;
				}
			}
			if (j + 2 < magic.x && magic.ball[i][j + 1] == magic.ball[i][j + 2]){
				if (i - 1 >= 0 && magic.ball[i - 1][j] == magic.ball[i][j + 1]){
					magic.ball1[i][j] = 2;
					magic.ball1[i - 1][j] = 2;
				}
				if (j - 1 >= 0 && magic.ball[i][j - 1] == magic.ball[i][j + 1]){
					magic.ball1[i][j] = 2;
					magic.ball1[i][j - 1] = 2;
				}
				if (i + 1 < magic.y && magic.ball[i + 1][j] == magic.ball[i][j + 1]){
					magic.ball1[i][j] = 2;
					magic.ball1[i + 1][j] = 2;
				}
			}
			if (j - 2 >= 0 && magic.ball[i][j - 1] == magic.ball[i][j - 2]){
				if (i - 1 >= 0 && magic.ball[i - 1][j] == magic.ball[i][j - 1]){
					magic.ball1[i][j] = 2;
					magic.ball1[i - 1][j] = 2;
				}
				if (j + 1 <magic.x && magic.ball[i][j + 1] == magic.ball[i][j - 1]){
					magic.ball1[i][j] = 2;
					magic.ball1[i][j + 1] = 2;
				}
				if (i + 1 < magic.y && magic.ball[i + 1][j] == magic.ball[i][j - 1]){
					magic.ball1[i][j] = 2;
					magic.ball1[i + 1][j] = 2;
				}
			}
		}
	}
}

int TF_eliminateKey(int ball1[XXMAX][YYMAX])//是否有可消除提示项
{
	int i, j;
	int flag=0;
	for (i = 0; i < XMAX ; i++)
	{
		for (j = 0; j < YMAX; j++)
		{
			if (ball1[i][j] == 2)
				flag = 1;
		}
	}
	return flag;
}