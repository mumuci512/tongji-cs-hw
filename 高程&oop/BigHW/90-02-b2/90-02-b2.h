/* 2350222 计科 盛曦 */
#pragma once

//游戏难度
#define LEVEL_LOW 1
#define LEVEL_MEDIUM 2
#define LEVEL_HIGH 3

//雷区大小,x是宽度（列数），y是高度（行数）
#define LOW_X 9
#define LOW_Y 9
#define MEDIUM_X 16
#define MEDIUM_Y 16
#define HIGH_X 30
#define HIGH_Y 16

//显示状态
#define STATE_CLOSE 1
#define STATE_MARK 2
#define STATE_OPEN 0

//雷数
#define LOW_MINE 10
#define MEDIUM_MINE 40
#define HIGH_MINE 99

struct MINE
{
	char* mine;//用于记录雷区
	char* state;//用于记录显示状态，0表示翻开STATE_OPEN，1表示未翻开STATE_CLOSE，2表示标记STATE_MARK
	int MAX_X;//雷区大小,x是宽度（列数），y是高度（行数）
	int MAX_Y;
	int mine_num;//雷数
};

void MineSweeper();