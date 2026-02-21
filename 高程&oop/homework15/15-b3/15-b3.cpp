/* 2350222 盛曦 计科 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
using namespace std;

int usage(const char* const procname)
{
	cout << "Usage : " << procname << " --check 文件名 | --convert { wtol|ltow } 源文件名 目标文件名" << endl;
	cout << "        " << procname << " --check a.txt" << endl;
	cout << "        " << procname << " --convert wtol a.win.txt a.linux.txt" << endl;
	cout << "        " << procname << " --convert ltow a.linux.txt a.win.txt" << endl;
	return 0;
}

int check(const char* const name, int tf_out = 1)
{
	ifstream in(name, ios::in | ios::binary);//加binary，windows会将回车视为2个字符，linux视为一个字符
	if (!in)
	{
		cout << "文件打开失败" << endl;
		return 1;
	}

	signed char ch,ch1=0,ch2=0;
	int f_linux=0, f_win=0;//表示是linux还是windows，1是0不是
	ch = in.get();
	while (!in.eof()) {
		ch1 = ch2;//ch1为上一个字符
		ch2 = ch;//ch2为当前字符
		if (ch2 == char(0x0A))
		{
			if(ch1==char(0x0D))
				f_win=1;
			else
				f_linux = 1;
		}
		ch = in.get();
	}
	if (f_win == 1 && f_linux == 0)
	{
		if(tf_out)
			cout << "Windows格式" << endl;
		return 2;
	}
	else if (f_win == 0 && f_linux == 1)
	{
		if(tf_out)
			cout << "Linux格式" << endl;
		return 3;
	}
	else
	{
		cout << "文件格式无法识别" << endl;
		return 4;
	}
}

int convert(const char* const from, const char* const to, const char* const way)
{
	ifstream in(from, ios::in | ios::binary);
	if (!in)
	{
		cout << "文件打开失败" << endl;
		return 1;
	}

	ofstream out(to, ios::out);
	if (!out)
	{
		cout << "文件打开失败" << endl;
		return 1;
	}

	signed char ch;
	int n = 0;
	if (strcmp(way, "wtol") == 0)
	{
		ch = in.get();
		while (!in.eof()) {
			if (ch == char(0x0D))
			{
				ch = in.get();
				if (ch == char(0x0A))
				{
					n++;
					out.put(char(0x0A));
				}
				else if (ch == EOF)
				{
					out.put(char(0x0D));
					break;
				}
				else
				{
					out.put(char(0x0D));
					out.put(ch);
				}
			}
			else
				out.put(ch);
			ch = in.get();
		}
		cout << "转换完成，去除" << n << "个0x0D" << endl;
	}
	else if (strcmp(way, "ltow") == 0)
	{
		ch = in.get();
		while (!in.eof()) {
			if (ch == char(0x0A))
			{
				n++;
				out.put(char(0x0D));
				out.put(char(0x0A));
			}
			else
				out.put(ch);
			ch = in.get();
		}
		cout << "转换完成，加入" << n << "个0x0D" << endl;
	}
	else
	{
		cout << "转换方式错" << endl;
		return 2;
	}
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc - 1 != 2 && argc - 1 != 4)
		return usage(argv[0]);

	if (argc - 1 == 2)
	{
		if (strcmp(argv[1], "--check") != 0)
			return usage(argv[0]);
		check(argv[2]);
	}
	else if (argc - 1 == 4)
	{
		if (strcmp(argv[1], "--convert") != 0)
			return usage(argv[0]);
		if (strcmp(argv[2], "wtol") == 0||strcmp(argv[2],"ltow")==0)
		{
			int t = check(argv[3],0);
			if (t == 1 || t == 4)
				return 0;
			if (strcmp(argv[2], "wtol") == 0)
			{
				if (t != 2)
				{
					cout << "文件格式无法识别" << endl;
					return 0;
				}
			}
			else
			{
				if (t != 3)
				{
					cout << "文件格式无法识别" << endl;
					return 0;
				}
			}
			convert(argv[3], argv[4], argv[2]);
		}
		else
			return usage(argv[0]);
	}
	return 0;
}