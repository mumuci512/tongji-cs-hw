/* 2350222 盛曦 计科 */
#pragma once

//不允许加入任何头文件，特别是<Windows.h>，查到就是0分甚至是倒扣-20 ！！！

//自行查阅相关资料，了解下面这几个预编译命令的作用，看能否给你的作业带来方便！！！
//#pragma pack(show) //以警告信息的形式显示当前字节对齐的值
//#pragma pack(push) //将当前字节对齐值压入编译器栈的栈顶
//#pragma pack(push, 4) //将当前字节对齐值压入编译器栈的栈顶，然后再将4设置当前值
//#pragma pack(pop)  //将编译器栈栈顶的字节对齐值弹出并设置为当前值
//#pragma pack() //不带参数是恢复默认值

#pragma pack(push,1)

//允许定义其它需要的结构体（类）、常量、常变量等
struct BMPFH {
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
};

struct BMPIH {
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
};

struct RGBQ {
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
};

class bitmap {
private:
    //自行添加你需要的数据成员及成员函数，不限制数量
	struct BMPFH file_header;
	struct BMPIH info_header;
	struct RGBQ* rgb_quad;
	unsigned char* data;
	int RowSize;
	
public:
    //公有部分不允许添加任何内容
    bitmap(const char* const filename);
    ~bitmap();
    int show(const int top_left_x, const int top_left_y, const int angle, const bool is_mirror, 
                void (*draw_point)(const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue)) const;
};

#pragma pack(pop)