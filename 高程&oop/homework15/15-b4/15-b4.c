/* 2350222 盛曦 计科 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

//输入文件中的换行都被视为0D 0A，说明in处加binary
//输出文件中的换行都被视为0D 0A，说明out处不加binary

#define LINE 16 //一行的数据个数
#define BYTE 8 //一个数据的位数

int usage(const char* const procname)
{
	printf("Usage : %s --infile 原始文件 [ --outfile hex格式文件 ]\n", procname);
	printf("        %s --infile a.docx\n", procname);
	printf("        %s --infile a.docx --outfile a.hex\n", procname);
	return 0;
}

int output(const char* const in_name, int way_of_out, const char* const out_name)//way_of_out=0表示输出到屏幕，way_of_out=1表示输出到文件
{
	FILE* fin;
	fin=fopen(in_name, "rb");
	if (!fin)
	{
		printf("输入文件%s打开失败！\n", in_name);
		return 1;
	}
	signed char ch;
	int line = 0;//记录行数
	int c = 0;//用于记录输出了几个字符，便于换行

	FILE* fout=NULL;
	if (way_of_out == 1)
	{
		fout = fopen(out_name, "w");
		if (!fout)
		{
			printf("输出文件%s打开失败！\n", out_name);
			fclose(fin);
			return 1;
		}
	}

	while (1) {
		ch = fgetc(fin);
		if (feof(fin))
			break;
		if (c % BYTE == 0 && c % LINE != 0)
			if (way_of_out == 0)
				printf("- ");
			else
				fprintf(fout,"- ");
		else if (c % LINE == 0)
		{
			if (way_of_out == 0)
				printf("%08X  ",line * LINE);
			else
				fprintf(fout,"%08X  ", line * LINE);
			line++;
		}

		if (way_of_out == 0)
			printf("%02x ",(int)((unsigned char)ch));
		else
			fprintf(fout,"%02x ", (int)((unsigned char)ch));
		c++;

		if (c % LINE == 0)
		{
			if (way_of_out == 0)
				printf("    ");
			else
				fprintf(fout,"    ");
			int i;
			for (fseek(fin,-LINE, SEEK_CUR), i = 0; i < LINE; i++)
			{
				ch = fgetc(fin);
				/*if (ch == EOF)
					break;*/
				if (way_of_out == 0)
					printf("%c",(char)(ch >= 33 && ch <= 126 ? ch : '.'));
				else
					fprintf(fout,"%c", (char)(ch >= 33 && ch <= 126 ? ch : '.'));
			}

			if (way_of_out == 0)
				printf("\r\n");
			else
				fprintf(fout,"\r\n");
		}
	}
	if ((c % LINE != 0))
	{
		int wide = (LINE - (c % LINE)) * 3;
		if (c % LINE <= 8)
			wide += 2;
		if (way_of_out == 0)
			for(int i=0;i< wide + 5 - 1;i++)
				printf(" ");
		else
			for (int i = 0; i < wide + 5 - 1; i++)
				fprintf(fout," ");;
		int i;
		/*if (in.good() == 0)
			in.clear();*/
		for (fseek(fin,c - c % LINE, SEEK_SET), i = 0; i < c % LINE; i++)
		{
			ch = fgetc(fin);
			/*if (ch == EOF)
				break;*/
			if (way_of_out == 0)
				printf("%c", (char)(ch >= 33 && ch <= 126 ? ch : '.'));
			else
				fprintf(fout,"%c", (char)(ch >= 33 && ch <= 126 ? ch : '.'));
		}
		if (way_of_out == 0)
			printf("\r\n");
		else
			fprintf(fout,"\r\n");
	}

	if (way_of_out == 1&&fout!=NULL)
		fclose(fout);

	fclose(fin);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc - 1 != 2 && argc - 1 != 4)
		return usage(argv[0]);
	if (argc - 1 == 2)
	{
		if (strcmp(argv[1], "--infile") != 0)
			return usage(argv[0]);
		output(argv[2], 0,NULL);
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
