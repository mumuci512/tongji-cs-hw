/* 2350222 盛曦 计科 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int usage(const char* const procname)
{
	printf("Usage : %s --check 文件名 | --convert { wtol|ltow } 源文件名 目标文件名\n", procname);
	printf("        %s --check a.txt\n", procname);
	printf("        %s --convert wtol a.win.txt a.linux.txt\n", procname);
	printf("        %s --convert ltow a.linux.txt a.win.txt\n", procname);
	return 0;
}

int check(const char* const name, int tf_out)
{
	FILE* fin;
	fin = fopen(name, "rb");//加binary，windows会将回车视为2个字符，linux视为一个字符
	if (!fin)
	{
		printf("文件打开失败\n");
		return 1;
	}

	signed char ch, ch1 = 0, ch2 = 0;
	int f_linux = 0, f_win = 0;//表示是linux还是windows，1是0不是
	ch = fgetc(fin);
	while (!feof(fin)) {
		ch1 = ch2;//ch1为上一个字符
		ch2 = ch;//ch2为当前字符
		if (ch2 == (char)(0x0A))
		{
			if (ch1 == (char)(0x0D))
				f_win = 1;
			else
				f_linux = 1;
		}
		ch = fgetc(fin);
	}
	if (f_win == 1 && f_linux == 0)
	{
		if (tf_out)
			printf("Windows格式\n");
		fclose(fin);
		return 2;
	}
	else if (f_win == 0 && f_linux == 1)
	{
		if (tf_out)
			printf("Linux格式\n");
		fclose(fin);
		return 3;
	}
	else
	{
		printf("文件格式无法识别\n");
		fclose(fin);
		return 4;
	}
}

int convert(const char* const from, const char* const to, const char* const way)
{
	FILE* fin;
	fin = fopen(from, "rb");
	if (!fin)
	{
		printf("文件打开失败\n");
		return 1;
	}

	FILE* fout;
	fout = fopen(to, "w");
	if (!fout)
	{
		printf("文件打开失败\n");
		fclose(fin);
		return 1;
	}

	signed char ch;
	int n = 0;
	if (strcmp(way, "wtol") == 0)
	{
		ch = fgetc(fin);
		while (!feof(fin)) {
			if (ch == (char)(0x0D))
			{
				ch = fgetc(fin);
				if (ch == (char)(0x0A))
				{
					n++;
					fputc((char)(0x0A),fout);
				}
				else if (ch == EOF)
				{
					fputc((char)(0x0D),fout);
					break;
				}
				else
				{
					fputc((char)(0x0D),fout);
					fputc(ch,fout);
				}
			}
			else
				fputc(ch,fout);
			ch = fgetc(fin);
		}
		printf("转换完成，去除%d个0x0D\n", n);
	}
	else if (strcmp(way, "ltow") == 0)
	{
		ch = fgetc(fin);
		while (!feof(fin)) {
			if (ch == (char)(0x0A))
			{
				n++;
				fputc((char)(0x0D),fout);
				fputc((char)(0x0A),fout);
			}
			else
				fputc(ch,fout);
			ch = fgetc(fin);
		}
		printf("转换完成，加入%d个0x0D\n", n);
	}
	else
	{
		printf("转换方式错\n");
		fclose(fin);
		fclose(fout);
		return 2;
	}
	fclose(fin);
	fclose(fout);
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
		check(argv[2],1);
	}
	else if (argc - 1 == 4)
	{
		if (strcmp(argv[1], "--convert") != 0)
			return usage(argv[0]);
		if (strcmp(argv[2], "wtol") == 0 || strcmp(argv[2], "ltow") == 0)
		{
			int t = check(argv[3], 0);
			if (t == 1 || t == 4)
				return 0;
			if (strcmp(argv[2], "wtol") == 0)
			{
				if (t != 2)
				{
					printf("文件格式无法识别\n");
					return 0;
				}
			}
			else
			{
				if (t != 3)
				{
					printf("文件格式无法识别\n");
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