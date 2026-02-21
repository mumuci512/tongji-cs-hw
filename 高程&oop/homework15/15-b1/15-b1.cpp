/* 2350222 盛曦 计科 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
using namespace std;

//输入文件中的换行都被视为0D 0A，说明in处加binary
//输出文件中的换行都被视为0D 0A，说明out处不加binary

#define LINE 16 //一行的数据个数
#define BYTE 8 //一个数据的位数

int usage(const char* const procname)
{
	cout << "Usage : " << procname << " --infile 原始文件 [ --outfile hex格式文件 ]" << endl;
	cout << "        " << procname << " --infile a.docx" << endl;
	cout << "        " << procname << " --infile a.docx --outfile a.hex" << endl;
    return 0;
}

int output(const char* const in_name, int way_of_out,const char* const out_name=NULL)//way_of_out=0表示输出到屏幕，way_of_out=1表示输出到文件
{
	ifstream in(in_name, ios::in | ios::binary);
	if (!in)
	{
		cout << "输入文件"<<in_name<<"打开失败！" << endl;
		return 1;
	}
	signed char ch;
	int line = 0;//记录行数
	int c=0;//用于记录输出了几个字符，便于换行

	ofstream out;
	if(way_of_out==1)
	{
		out.open(out_name, ios::out);
		if (!out)
		{
			cout << "输出文件" << out_name << "打开失败！" << endl;
			return 1;
		}
	}

	while (1) {
		ch = in.get();
		if (in.eof())
			break;
		if (c % BYTE == 0 && c % LINE != 0)
			if (way_of_out == 0)
				cout << "- ";
			else
				out << "- ";
		else if (c % LINE == 0)
		{
			if (way_of_out == 0)
				cout << hex << setfill('0') << setw(8) << line * LINE << setfill(' ') << "  ";
			else
				out << hex << setfill('0') << setw(8) << line * LINE << setfill(' ') << "  ";
			line++;
		}

		if (way_of_out == 0)
			cout << hex << setfill('0') << setw(2) << (int)((unsigned char)ch) << setfill(' ') << ' ';
		else
			out << hex << setfill('0') << setw(2) << (int)((unsigned char)ch) << setfill(' ') << ' ';
		c++;

		if (c % LINE == 0)
		{
			if (way_of_out == 0)
				cout << setw(5-1) << ' ';
			else
				out << setw(5-1) << ' ';
			int i;
			for (in.seekg(-LINE, ios::cur), i = 0; i < LINE; i++)
			{
				ch = in.get();
				/*if (ch == EOF)
					break;*/
				if(way_of_out==0)
					cout << char(ch >= 33 && ch <= 126 ? ch : '.');
				else
					out << char(ch >= 33 && ch <= 126 ? ch : '.');
			}

			if (way_of_out == 0)
				cout << '\r'<<'\n';
			else
				out << endl;
		}
	}
	if ((c % LINE != 0))
	{
		int wide = (LINE - (c % LINE)) * 3;
		if (c % LINE <= 8)
			wide += 2;
		if (way_of_out == 0)
			cout << setw(wide+5-1) << ' ';
		else
			out << setw(wide + 5-1) << ' ';
		int i;
		if (in.good() == 0)
			in.clear();
		for (in.seekg(c - c % LINE, ios::beg), i = 0; i < c % LINE; i++)
		{
			ch = in.get();
			/*if (ch == EOF)
				break;*/
			if (way_of_out == 0)
				cout << char(ch >= 33 && ch <= 126 ? ch : '.');
			else
				out << char(ch >= 33 && ch <= 126 ? ch : '.');
		}
		if (way_of_out == 0)
			cout << '\r' << '\n';
		else
			out << endl;
	}

	if(way_of_out == 1)
		out.close();

	in.close();
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc-1!=2&&argc-1!=4)
		return usage(argv[0]);
	if (argc - 1 == 2)
	{
		if (strcmp(argv[1], "--infile")!=0)
			return usage(argv[0]);
		output(argv[2], 0);
		return 0;
	}
	else if (argc - 1 == 4)
	{
		if (strcmp(argv[1], "--infile") == 0)
		{
			if (strcmp(argv[3], "--outfile") == 0)
				output(argv[2], 1, argv[4]);
			else
				return usage(argv[0]);
		}
		else if (strcmp(argv[1], "--outfile") == 0)
		{
			if (strcmp(argv[3], "--infile") == 0)
				output(argv[4], 1, argv[2]);
			else
				return usage(argv[0]);
		}
		else
			return usage(argv[0]);
	}
    return 0;
}
