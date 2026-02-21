/* 2350222 盛曦 计科 */
#include <iostream>
#include <cstring>
using namespace std;

int usage(const char* const procname)
{
	cout << "Usage: " << procname << " 要检查的学号/all 匹配学号/all 源程序名/all 相似度阀值(60-100) 输出(filename/screen)" << endl << endl;
	cout << "e.g. : " << procname << " 2159999 2159998 all       80 screen" << endl;
	cout << "       " << procname << " 2159999 all     14-b1.cpp 75 result.txt" << endl;
	cout << "       " << procname << " all     all     14-b2.cpp 80 check.dat" << endl;
	cout << "       " << procname << " all     all     all       85 screen" << endl;

	return 0;
}

void output(char** argv, int sim)
{
	cout << "参数检查通过" << endl;
	cout << "检查学号：" << argv[1] << endl;
	cout << "匹配学号：" << argv[2] << endl;
	cout << "源文件名：" << argv[3] << endl;
	cout << "匹配阀值：" << sim << endl;
	cout << "输出目标：" << argv[5] << endl;
}

int main(int argc, char** argv)
{
	if (argc != 5+1)
	{
		usage(argv[0]);
		return 0;
	}
	int i;

	//要检查的学号
	int flag = 1;//默认合法
	if(strcmp(argv[1],"all")!=0)
	{
		if (strlen(argv[1]) != 7)
		{
			cout << "要检查的学号不是7位" << endl;
			return 0;
		}
		else
		{
			for (i = 0; i < 7; i++)
			{
				if (argv[1][i] < '0' || argv[1][i]>'9')
				{
					flag = 0;
					break;
				}
			}
			if (flag == 0)
			{
				cout << "要检查的学号不是7位数字" << endl;
				return 0;
			}
		}
	}

	//匹配学号
	flag = 1;
	if (strcmp(argv[2], "all") != 0)
	{
		if (strlen(argv[2]) != 7)
		{
			cout << "要匹配的学号不是7位" << endl;
			return 0;
		}
		else
		{
			for (i = 0; i < 7; i++)
			{
				if (argv[2][i] < '0' || argv[2][i]>'9')
				{
					flag = 0;
					break;
				}
			}
			if (flag == 0)
			{
				cout << "要匹配的学号不是7位数字" << endl;
				return 0;
			}
		}
	}
	if (strcmp(argv[1], "all") == 0 && strcmp(argv[2], "all") != 0)
	{
		cout << "检查学号是all，匹配学号必须是all" << endl;
		return 0;
	}

	//匹配文件名
	if (strlen(argv[3]) > 8*32)
	{
		cout << "源程序文件名超过了32字节" << endl;
		return 0;
	}

	//相似度
	int sim=-1;
	for (i = 0; argv[4][i] == '0'; i++);
	if (argv[4][i+2] == '\0')
		sim = (argv[4][i] - '0')*10+ (argv[4][i+1]-'0');
	if (sim < 60 || sim>100)
		sim = 80;

	//输出
	if (strcmp(argv[5], "screen") != 0)
	{
		if (strlen(argv[5]) > 8*32)
		{
			cout << "输出结果文件名超过了32字节" << endl;
			return 0;
		}
	}

	output(argv, sim);

	return 0;
}