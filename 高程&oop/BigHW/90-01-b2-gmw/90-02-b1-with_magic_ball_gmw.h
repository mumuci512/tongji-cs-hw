/* 2350222 计科 盛曦 */
#pragma once

#define YMIN 5
#define YMAX 9
#define XMIN 5
#define XMAX 9
#define YMIN_STAR 8
#define YMAX_STAR 10
#define XMIN_STAR 8
#define XMAX_STAR 10

#define YYMAX 10 //用于记录所有游戏中最大的ymax
#define XXMAX 10//用于记录所有游戏中最大的xmax

#define COLMAX 40
#define LINEMIN 6
#define COLMIN_STAR 6
#define LINEMIN_STAR 8

#define WIDTH_OF_FRAME 2//边框宽度

#define SCREEN 0
#define SCORE 1

#define FRAME_LEN 4//边框一个字符（串）的长度

struct magic_ball
{
	//ball和ball1的大小是所有游戏中最大的XMAX和YMAX
	int ball[XXMAX][YYMAX];
	int ball1[XXMAX][YYMAX];//ball1用于记录ball中哪些位置可以消去,1表示可消去
	int ball2[XXMAX][YYMAX];//ball2用于记录star中哪些被选中
	int x, y;//行数为y，列数为x
};

struct location {
	int i;//行
	int j;//列
};
//
//class frame_lines {
//public:
//	char line[4];//横线
//	char col[4];//竖线
//	char l_u_corner[FRAME_LEN];//左上角
//	char r_u_corner[FRAME_LEN];//右上角
//	char l_d_corner[FRAME_LEN];//左下角
//	char r_d_corner[FRAME_LEN];//右下角
//	char cross[FRAME_LEN];//十字
//	char d_cross[FRAME_LEN];//下十字
//	char u_cross[FRAME_LEN];//上十字
//	char r_cross[FRAME_LEN];//右十字
//	char l_cross[FRAME_LEN];//左十字
//	frame_lines(const char a[], const char b[], const char c[], const char d[], const char e[], const char f[], const char g[], const char h[], const char i[], const char j[], const char k[]);
//};
char menu(char menus[36]);
void output_init_array(struct magic_ball& magic,  const char prompt[],int& score,int game=0,int tf_yellow=0);//不知道为什么把score设成指针变量默认参数就不行，只好不设成默认参数了
void output_prompt(int x, int y,int game=0);
void output_array(struct magic_ball& magic, int& score,int game=0,int tf_yellow=0);
void input(int* x, int* y,int game=0);
void init_all(int array[XXMAX][YYMAX], int fill);
void generate_array(struct magic_ball& magic);
int TF_eliminateKey(int ball1[XXMAX][YYMAX]);
void input_end();
int TF_eliminate(struct magic_ball& magic, int game = 0);
void l_to_h(char& ch);
char input_one_char(const char str[]);
void fall(struct magic_ball& magic, int& score, int game, const CONSOLE_GRAPHICS_INFO* const cgi, BLOCK_DISPLAY_INFO* bdi);
void exchange(int* a, int* b);
void wait_for_enter(const char prompt[100], int y = 0, int line_parm = 0, int bottom = 0);
void print_bottom(int y, int line_parm, const char prompt[100], int x = 0);
void graph_prepare_end(int flag, int line_parm, int y, int x=0,int game=0);
//void print_frame_line(const char srt[3], const char end[3], int x, int y, char mode, frame_lines frame);
//void print_frame_col(int x, int y, char mode,frame_lines frame);
void print_one_ball(const char circle[],  struct location loc, struct magic_ball magic, int line_parm, int fgcolor=0, int game=0, int tf_fall=0);//0表示color_black
void print_window(int screen_or_score=SCREEN,int score=0, int full_score=0);
int mouse_pos_legal(int X, int Y, struct magic_ball& magic, int line_parm, struct location& origin,int game=0);
void set_lineparm(char mode, int& line_parm);
void print_head(struct magic_ball magic,int line_parm);
//void print_graph(struct magic_ball& magic, char mode, frame_lines frame);
void cancel_previous(struct magic_ball& magic, BLOCK_DISPLAY_INFO* bdi_can_elim, const CONSOLE_GRAPHICS_INFO* const cgi);
void select_end(int line_parm, int X, int Y, struct location& origin, int basex, int basey, struct magic_ball magic, char m_or_k);
void set_base(int game, int& basex, int& basey);
void one_fall(int line_parm, int j, int k, struct magic_ball magic, int game=0,int v_or_l='v');
