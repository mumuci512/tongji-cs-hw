/* 2350222 盛曦 计科 */
/* 2353244 廖妤兮 2350218 陈佳怡 2351274 王亦玮 2353806 赵翌佟 2350501 王杏 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

//输入文件中的换行都被视为0D 0A，说明in处加binary
//输出文件中的换行都被视为0D 0A，说明out处不加binary

#define LINE 16 //一行的数据个数
#define BYTE 8 //一个数据的位数

int usage(const char* const procname)
{
	printf("Usage : %s --infile hex格式文件 --outfile bin格式文件\n", procname);
	printf("        %s --infile a.hex --outfile a.bin\n", procname);
	return 0;
}

int output(const char* const in_name, const char* const out_name)
{
	FILE* fin;
	fin=fopen(in_name, "r");//不加binary，将回车视为一个字符，便于处理。但文件中的0D 0A会被视为一个回车
	if (!fin)
	{
		printf("输入文件%s打开失败！\n", in_name);
		return 1;
	}

	signed char ch1, ch2, t;
	int c = 0;//用于记录输出了几个字符，便于换行
	//int flag = 0;//表示上一个是否是0D,即下一个是否需要判断是不是0A

	FILE* fout;
	fout = fopen(out_name, "wb");
	if (!fout)
	{
		printf("输出文件%s打开失败！\n", out_name);
		fclose(fin);
		return 1;
	}

	while (1) {
		if (c % LINE == 0)
		{
			if (c != 0)
			{
				//in.seekg(4 + LINE+1, ios::cur);//跳过本行末尾的4个空格+本行的16个字符+回车
				for (int i = 0; i < 4 + LINE + 1; i++, fgetc(fin))
					;
			}
			if (feof(fin))//正好在文件末尾
				break;
			fseek(fin,8 + 2, SEEK_CUR);//跳过下一行开头的表示地址的数字（8位）+2个空格
		}
		else if (c % BYTE == 0 && c % LINE != 0)
			fseek(fin,2, SEEK_CUR);//跳过每八位就会有一个的‘-’+一个空格

		ch1 = fgetc(fin);//该字符的第一位
		if (feof(fin))
			break;
		if (ch1 == ' ')//该字符处是空白，说明后面没有字符了，跳出
			break;
		ch2 = fgetc(fin);
		if (feof(fin))
			break;
		fgetc(fin);//消除每个字符后的空格
		if (feof(fin))
			break;

		if (ch1 >= '0' && ch1 <= '9')
			t = (ch1 - '0') * 16;
		else if (ch1 >= 'A' && ch1 <= 'F')
			t = (ch1 - 'A' + 10) * 16;
		else if (ch1 >= 'a' && ch1 <= 'f')
			t = (ch1 - 'a' + 10) * 16;

		if (ch2 >= '0' && ch2 <= '9')
			t += (ch2 - '0');
		else if (ch2 >= 'A' && ch2 <= 'F')
			t += (ch2 - 'A' + 10);
		else if (ch2 >= 'a' && ch2 <= 'f')
			t += (ch2 - 'a' + 10);

		//		if (t == 0x0D)
		//		{
		//			flag = 1;
		//			c++;
		//			continue;
		//		}
		//		else if (flag==1)
		//		{
		//			if(t == 0x0A)
		//			{
		////#if (_MSC_VER)
		////				out << endl;
		////#elif (__linux__)
		////				out << char(0x0D) << char(0x0A);
		////#endif
		//				out << char(0x0D)<< char(0x0A);
		//			}
		//			else
		//			{
		//				out << char(0x0D);
		//				out << char(t);
		//			}
		//			flag = 0;
		//		}
		//		else
		//		{
		//			flag = 0;
		//			out << char(t);
		//		}

		fprintf(fout,"%c",(char)(t));
		c++;
	}//end of while

	fclose(fout);

	fclose(fin);
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc - 1 != 4)
		return usage(argv[0]);

	if (strcmp(argv[1], "--infile") == 0)
	{
		if (strcmp(argv[3], "--outfile") == 0)
			output(argv[2], argv[4]);
		else
			return usage(argv[0]);
	}
	else if (strcmp(argv[1], "--outfile") == 0)
	{
		if (strcmp(argv[3], "--infile") == 0)
			output(argv[4], argv[2]);
		else
			return usage(argv[0]);
	}
	else
		return usage(argv[0]);

	return 0;
}
