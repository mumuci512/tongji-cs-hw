/* 2350222 计科 盛曦 */
#pragma once

struct player_parm
{
	int sel;//sel为0表示未选中，1表示已选中
	int tf_fall;//fall为0表示不可下落，1表示可以下落
	int end;//end表示本次是否需要结束,1表示结束，0表示不结束
	int flag_pos;
	int line_parm;
	char mode;
	int enter_pos_leg;
};

char menu(char menus[36]);
void menuabc(char mode);
int find_eliminate(struct magic_ball magic, int* p, int* p1, int step, struct location pos, char v_or_l,char mode);
int FIND_ELIMINATE(struct magic_ball magic, int* p, int* p1, struct location& pos, char dir = 0,char mode=0);
void menudefg(char mode);