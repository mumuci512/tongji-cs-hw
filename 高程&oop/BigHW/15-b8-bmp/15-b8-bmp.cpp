/* 2350222 盛曦 计科 */
#include <iostream>
#include <fstream>
//不再允许加入任何头文件，特别是<Windows.h>，查到就是0分甚至是倒扣-20!!!!!
using namespace std;

#include "15-b8-bmp.h"

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：按需补充
***************************************************************************/
bitmap::bitmap(const char *const filename)
{
	ifstream in(filename, ios::binary);
	if (!in)
	{
		cout << "无法打开文件" << filename << endl;
		exit(1);
	}

	in.read((char*)&file_header, sizeof(file_header));
	in.read((char*)&info_header, sizeof(info_header));
	if (info_header.biBitCount == 1 || info_header.biBitCount == 4 || info_header.biBitCount == 8)
	{
		unsigned int rgb_quad_size = info_header.biClrUsed > 0 ? info_header.biClrUsed : (1 << info_header.biBitCount);
		rgb_quad = new (nothrow) RGBQ[rgb_quad_size];
		if (rgb_quad == NULL)
		{
			cout << "内存申请失败." << endl;
			return;
		}
		in.read((char*)rgb_quad, rgb_quad_size * sizeof(RGBQ));
	}
	else
		rgb_quad = NULL;

	data = new (nothrow) unsigned char[info_header.biSizeImage];
	if (data == NULL)
	{
		cout << "内存申请失败." << endl;
		return;
	}
	in.read((char*)data, info_header.biSizeImage);

	in.close();

	RowSize = ((info_header.biBitCount * info_header.biWidth+31) / 32) * 4;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：按需补充
***************************************************************************/
bitmap::~bitmap()
{
	if (data != NULL)
		delete[] data;
	if (rgb_quad != NULL)
		delete[] rgb_quad;
}

//按需增加内容
int bitmap::show(const int top_left_x, const int top_left_y, const int angle, const bool is_mirror,
	void (*draw_point)(const int x, const int y, const unsigned char red, const unsigned char green, const unsigned char blue)) const
{
	int start_x, start_y;//绘画起始点
	int dir_x, dir_y;//绘画方向
	char row_col;//一行行画还是一列列画

	switch (angle)
	{
		case 0:
			start_x = top_left_x;
			start_y = top_left_y + info_header.biHeight;
			dir_x = 1;//向右;
			dir_y = -1;//向上
			row_col = 'r';
			break;
		case 90:
			start_x = top_left_x+info_header.biHeight;
			start_y = top_left_y + info_header.biWidth;
			dir_x = -1;//向左
			dir_y = -1;//向上
			row_col = 'c';
			break;
		case 180:
			start_x = top_left_x + info_header.biWidth;
			start_y = top_left_y;
			dir_x = -1;//向左
			dir_y = +1;//向下
			row_col = 'r';
			break;
		case 270:
			start_x = top_left_x;
			start_y = top_left_y;
			dir_x = 1;//向右
			dir_y = 1;//向下
			row_col = 'c';
			break;
	}
	if (is_mirror)
	{
		if (row_col == 'r')
		{
			dir_x = -dir_x;//画行方向相反
			if(start_x==top_left_x)
				start_x = top_left_x + info_header.biWidth;
			else
				start_x = top_left_x;
		}
		else
		{
			dir_y = -dir_y;
			if(start_y==top_left_y)
				start_y = top_left_y + info_header.biWidth;
			else
				start_y = top_left_y;
		}
	}

	int X=start_x, Y=start_y;//表示当前坐标
	unsigned char* start = NULL;
	unsigned char pixel_index;
	for (int i = 0; i < info_header.biHeight; i++)
	{
		for (int j = 0; j < info_header.biWidth; j++)
		{
			start = data + i * RowSize + j * info_header.biBitCount / 8;
			if(info_header.biBitCount == 24|| info_header.biBitCount == 32)
				draw_point(X, Y, *(start + 2), *(start + 1), *(start + 0));
			else
			{
				//pixel_index = (*start >> (info_header.biBitCount * ((7 - j % 8) / info_header.biBitCount))) & (1 << (info_header.biBitCount-1));
				if (info_header.biBitCount == 1)
					pixel_index = (*start >> (7 - j % 8)) & 0x01;
				else if (info_header.biBitCount == 4)
				{
					if (j % 2 == 0)
						pixel_index = (*start >> 4) & 0x0f;
					else
						pixel_index = *start & 0x0f;
				}
				else if (info_header.biBitCount == 8)
					pixel_index = *start;

				draw_point(X, Y, rgb_quad[pixel_index].rgbRed, rgb_quad[pixel_index].rgbGreen, rgb_quad[pixel_index].rgbBlue);
			}

			if (row_col == 'r')
				X += dir_x;
			else
				Y += dir_y;
		}
		if (row_col == 'r')
		{
			Y += dir_y;
			X = start_x;
		}
		else
		{
			X += dir_x;
			Y = start_y;
		}
	}
	return 0;
}