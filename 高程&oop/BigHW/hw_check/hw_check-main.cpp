/* 2350222 计科 盛曦 */
#include <iostream>
#include <iomanip>
#include <vector>
#include "../include/class_aat.h"
#include "../include/class_cft.h"
#include "../include/my_tools.h"
#include "hw_check.h"
using namespace std;

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
 ***************************************************************************/
static void usage(const char* const full_procname)
{
	const char* procname = strrchr(full_procname, '\\');
	if (procname == NULL)
		procname = full_procname;

	const int wkey = 7 + strlen(procname) + 1;
	const int wopt = 7 + strlen(procname) + 4;
	cout << endl;

	cout << "Usage: " << procname << " 必选项/可选项" << endl;
	cout << endl;
	cout << setw(wkey) << ' ' << "必选项：指定操作" << endl;
	cout << setw(wopt) << ' ' << "--action opname : 支持的opname如下" << endl;
	cout << setw(wopt) << ' ' << "\t base              : 基础检查(文件是否提交、是否被改动、编码是否正确)" << endl;
	cout << setw(wopt) << ' ' << "\t firstline         : 首行检查（仅源程序文件需要）" << endl;
	cout << setw(wopt) << ' ' << "\t secondline        : 次行检查（仅部分源程序文件需要）" << endl;

	cout << setw(wkey) << ' ' << "必选项：指定课号" << endl;
	cout << setw(wopt) << ' ' << "--cno course_no     : 课号" << endl;

	cout << setw(wkey) << ' ' << "必选项：指定学生" << endl;
	cout << setw(wopt) << ' ' << "--stu no/all        : 指定单个/全部(7位数字为学号,all为全部)" << endl;

	cout << setw(wkey) << ' ' << "必选项：指定文件" << endl;
	cout << setw(wopt) << ' ' << "--file filename/all : 具体文件名/全部(all为全部,其余为具体文件名)" << endl;

	cout << setw(wkey) << ' ' << "可选项：" << endl;
	cout << setw(wopt) << ' ' << "--chapter n         : 在--file的基础上再进行章节的筛选(无/-1则全部章节,可与--week共存)" << endl;
	cout << setw(wopt) << ' ' << "--week n            : 在--file的基础上再进行周次的筛选(无/-1则全部周次,可与--chapter共存)" << endl;
	cout << setw(wopt) << ' ' << "--display xxxxx     : 每位0/1分别表示正常信息/未提交信息/错误信息/汇总信息/严重错误信息" << endl;
	cout << setw(wopt) << ' ' << "--cfgfile filename  : 指定配置文件名(缺省:hw_check.conf)" << endl;
	cout << endl;

	const char* DEMO_CNO = "10108001";
	const char* DEMO_SRC_FNAME = "12-b1.cpp";
	const char* DEMO_STUNO = "2359999";

	cout << "e.g.   " << procname << " --action base --cno " << DEMO_CNO << " --stu all --file all             : 检查" << DEMO_CNO << "所有学生的所有作业的基本信息" << endl;
	cout << "       " << procname << " --action base --cno " << DEMO_CNO << " --stu all --file all --chapter 4 : 检查" << DEMO_CNO << "所有学生的第4章作业的基本信息" << endl;
	cout << "       " << procname << " --action base --cno " << DEMO_CNO << " --stu all --file all --week 6    : 检查" << DEMO_CNO << "所有学生的第6周作业的基本信息" << endl;
	cout << "       " << procname << " --action base --cno " << DEMO_CNO << " --stu " << DEMO_STUNO << " --file all         : 检查" << DEMO_CNO << "的" << DEMO_STUNO << "学生的所有作业的基本信息" << endl;
	cout << "       " << procname << " --action base --cno " << DEMO_CNO << " --stu " << DEMO_STUNO << " --file " << DEMO_SRC_FNAME << "   : 检查" << DEMO_CNO << "的" << DEMO_STUNO << "学生的" << DEMO_SRC_FNAME << "作业的基本信息" << endl;
	cout << endl;

	cout << "       " << procname << " --action firstline --cno " << DEMO_CNO << " --stu all --file all             : 检查" << DEMO_CNO << "所有学生的所有作业的首行信息" << endl;
	cout << "       " << procname << " --action firstline --cno " << DEMO_CNO << " --stu all --file all --chapter 4 : 检查" << DEMO_CNO << "所有学生的第4章作业的首行信息" << endl;
	cout << "       " << procname << " --action firstline --cno " << DEMO_CNO << " --stu all --file all --week 6    : 检查" << DEMO_CNO << "所有学生的第6周作业的首行信息" << endl;
	cout << "       " << procname << " --action firstline --cno " << DEMO_CNO << " --stu " << DEMO_STUNO << " --file all         : 检查" << DEMO_CNO << "的" << DEMO_STUNO << "学生的所有作业的首行信息" << endl;
	cout << "       " << procname << " --action firstline --cno " << DEMO_CNO << " --stu " << DEMO_STUNO << " --file " << DEMO_SRC_FNAME << "   : 检查" << DEMO_CNO << "的" << DEMO_STUNO << "学生的" << DEMO_SRC_FNAME << "作业的首行信息" << endl;
	cout << endl;

	cout << "       " << procname << " --action secondline --cno " << DEMO_CNO << " --stu all --file " << DEMO_SRC_FNAME << " : 检查" << DEMO_CNO << "的所有学生的" << DEMO_SRC_FNAME << "作业的次行信息" << endl;
	cout << endl;

	cout << "       " << procname << " --cfgfile E:\\test\\my.conf --action base --cno " << DEMO_CNO << " --stu all --file all : 检查" << DEMO_CNO << "所有学生的所有作业的基本信息(指定配置文件)" << endl;
	cout << endl;


	cout << endl;
}

int readconfig_hw_check_conf(const string fname, CFG* cfg)
{
	config_file_tools sCfg(fname); //分隔符是=

	if (sCfg.is_read_succeeded() == 0) {
		cout << "配置文件[" << fname << "]读取有错." << endl;
		return -1;
	}

	if (sCfg.item_get_string("[文件目录]", "src_rootdir", cfg->src_rootdir) == 0) {
		cout << "未取到配置文件中[文件目录]组的src_rootdir项." << endl;
		return -1;
	}

	if (sCfg.item_get_string("[数据库]", "db_host", cfg->dbserver) == 0) {
		cout << "未取到配置文件中[数据库]组的db_host项." << endl;
		return -1;
	}

	if (sCfg.item_get_string("[数据库]", "db_username", cfg->dbuser) == 0) {
		cout << "未取到配置文件中[数据库]组的db_username项." << endl;
		return -1;
	}

	if (sCfg.item_get_string("[数据库]", "db_passwd", cfg->dbpasswd) == 0) {
		cout << "未取到配置文件中[数据库]组的db_passwd项." << endl;
		return -1;
	}

	if (sCfg.item_get_string("[数据库]", "db_name", cfg->dbname) == 0) {
		cout << "未取到配置文件中[数据库]组的db_name项." << endl;
		return -1;
	}
	return 0;
}

int analyse(args_analyse_tools* args, const char* const argv)
{
	//检查必选参数是否存在
	if (!args[ARGS_ACTION].existed()) {
		usage(argv);
		cout << "需给出--action参数" << endl;
		return -1;
	}
	if (!args[ARGS_CNO].existed()) {
		usage(argv);
		cout << "需给出--cno参数" << endl;
		return -1;
	}
	if (!args[ARGS_STU].existed()) {
		usage(argv);
		cout << "需给出--stu参数" << endl;
		return -1;
	}
	if (!args[ARGS_FILE].existed()) {
		usage(argv);
		cout << "需给出--file参数" << endl;
		return -1;
	}

	//具体检查
	string file = args[ARGS_FILE].get_string();
	string cno = args[ARGS_CNO].get_string();
	string action = args[ARGS_ACTION].get_string();
	//action
	if (action == "firstline")
	{
		string file = args[ARGS_FILE].get_string();
		if (file != "all" && is_source_file(file) == 0)
		{
			cout << "首行检查的文件[" << file << "]必须是源程序文件." << endl;
			return -1;
		}
	}
	else if (action == "secondline")
	{
		string file = args[ARGS_FILE].get_string();
		string stu = args[ARGS_STU].get_string();
		if (file == "all")
		{
			cout << "HW_Check_SecondLine 只能针对单文件" << endl;
			return -1;
		}
		if (stu != "all")
		{
			cout << "HW_Check_SecondLine 只能针对全体学生" << endl;
			return -1;
		}
		if (is_source_file(file) == 0)
		{
			cout << "次行检查的文件[" << file << "]必须是源程序文件." << endl;
			return -1;
		}
	}
	//课程号
	if (cno != "all")
	{
		if (cno.length() != 8 && cno.length() != 13)
		{
			cout << "课程号" << cno << "不是8位或13位数字" << endl;
			return -1;
		}
		for (unsigned int i = 0; i < cno.length(); i++)
		{
			if (cno[i] < '0' || cno[i]>'9')
			{
				cout << "课程号" << cno << "有非法数字" << endl;
				return -1;
			}
		}
	}
	//stu
	//file
	if (file != "all")
	{
		if (args[ARGS_WEEK].existed() || args[ARGS_CHAPTER].existed())
		{
			usage(argv);
			cout << "参数[--chapter/--week]不能出现在[--file 单个文件名]时." << endl;
			return -1;
		}
	}
	return 0;
}

int is_source_file(const string file)//1:是源文件；0：不是源文件
{
	if (file.length() > 2 && (file.substr(file.length() - 2, 2) == ".c" || file.substr(file.length() - 2, 2) == ".h"))
		return 1;
	if (file.length() > 4 && (file.substr(file.length() - 4, 4) == ".cpp" || file.substr(file.length() - 4, 4) == ".hpp"))
		return 1;
	return 0;
}

int check_gb(istream& in, int& ret)
{
	unsigned char c;
	int byte = 0;
	bool is_utf8 = true;
	bool is_multi_byte = false;//记录是否有多字节字符
	while (in)
	{
		c = in.get();
		if (in.eof())
			break;
		if (byte != 0)
		{
			if ((c >> 6) != 0b10)
			{
				is_utf8 = false;
				break;
			}
			byte--;
		}
		else
		{
			if ((c >> 7) == 0b0)
				byte = 0;
			else if ((c >> 5) == 0b110)//后续还需检查1个字节
			{
				is_multi_byte = true;
				byte = 1;
			}
			else if ((c >> 4) == 0b1110)//后续还需检查2个字节
			{
				is_multi_byte = true;
				byte = 2;
			}
			else if ((c >> 3) == 0b11110)//后续还需检查3个字节
			{
				is_multi_byte = true;
				byte = 3;
			}
			else
			{
				is_utf8 = false;
				break;
			}
		}
	}//end of while

	if (byte != 0 || is_multi_byte == false)//没有多字节则是ascii文件
		is_utf8 = false;

	if (is_utf8)
		ret = -3;
	else
		ret = 0;

	return 0;
}

/*
base:0:正确；-1：未提交；-2：pdf格式不正确； -3：源文件格式不正确
first/secondline:0:正确；-1：未提交； -2：首行多行注释格式不正确； -3：非gb -4:首行不是三项 -5:首行检查出错 -6：文件小于4字节；-7:首行不是注释行
					-8:写了自己的学号 -9：非7位学号 -10:学号有非法符号 -11:信息不全
*/
int check(string file, string action, vector <string>& ret_stu, bool& name, bool& stuno, bool& major, char* buf,int& error_pos, vector <string>& divided_content)
{
	ifstream in;
	in.open(file, ios::in);
	int ret;
	if (!in)//没有提交
	{
		//cout << file << "打开失败" << endl;
		ret = -1;
	}
	else
	{
		//检查
		if (action == "base")
		{
			if (file.length() > 4 && file.substr(file.length() - 4, 4) == ".pdf")//检查是否为PDF文件
			{
				char buf[9];//存放文件的前8个字节
				in.read(buf, 8);
				if (in.gcount() != 8)
					ret = -2;
				else
				{
					buf[8] = '\0';
					if (strncmp(buf, "%PDF-1.x", strlen("%PDF-1.")) == 0)
					{
						if (buf[7] >= '0' && buf[7] <= '7')//x是0-7的数字
							ret = 0;
						else
							ret = -2;
					}
					else
						ret = -2;
				}
			}
			else if (is_source_file(file))//检查是否为UTF-8编码
				check_gb(in, ret);
			else
				ret = 0;
		}
		else//firstline or secondline
		{
			check_gb(in, ret);
			if (ret == -3)
			{
				in.close();
				return -3;
			}

			if (in.good() == 0)
				in.clear();
			in.seekg(0,ios::beg);			

			in.getline(buf, 1024 + 1, '\n');
			int byte = 0;
			//跳过空行
			while (strlen(buf) == 0 && !in.eof())
			{
				if (in.good() == 0)
					in.clear();
				in.getline(buf, 1024 + 1, '\n');
				byte++;
			}

			if (action == "secondline")//再读一行
			{
				if (in.good() == 0)
					in.clear();
				in.getline(buf, 1024 + 1, '\n');
				while (strlen(buf) == 0 && !in.eof())
				{
					if (in.good() == 0)
						in.clear();
					in.getline(buf, 1024 + 1, '\n');
				}
			}

			//文件小于 4 字节
			if (strlen(buf) + byte < 4)
			{
				in.close();
				return -6;
			}

			int end_of_buf = 0;
			while (buf[end_of_buf] != '\0'&& end_of_buf<1024)
				end_of_buf++;
			while (buf[end_of_buf] == ' ' || buf[end_of_buf] == '\t'||buf[end_of_buf]=='\0')
				end_of_buf--;
			buf[end_of_buf + 1] = '\0';

			int start, end;

			if ((ret = get_effective_comment(buf, start, end)) != 0)
			{
				in.close();
				return ret;
			}

			name = false,stuno = false,	major = false;

			//提取注释内容
			string content(buf + start, buf + end + 1);
			int space_count = 0;

			if (divided_content.size() != 0)
				divided_content.clear();
			space_count=divide_comment(content,divided_content);

			if(action == "firstline")
			{
				if (space_count != 2)//如果不是三项
				{
					in.close();
					return -4;//首行不是三项
				}

				for (int i = 0; i < 3; i++)
				{
					if (divided_content[i] == ret_stu[STU_NO])
						stuno = true;
					else if (divided_content[i] == ret_stu[STU_NAME])
						name = true;
					else
					{
						int pos;
						if ((pos = tj_strchr(ret_stu[STU_MAJOR_SIMPLE].c_str(), '|')) != 0)
						{
							if (divided_content[i] == ret_stu[STU_MAJOR_SIMPLE].substr(0, pos - 1) || divided_content[i] == ret_stu[STU_MAJOR_SIMPLE].substr(pos))
								major = true;
						}
						else
						{
							if (divided_content[i] == ret_stu[STU_MAJOR_SIMPLE])
								major = true;
						}
					}
				}

				if (!name || !stuno || !major)
				{
					in.close();
					return -5;
				}
			}
			else//secondline
			{
				for (int i = 0; i <= space_count; i++)
				{
					if (divided_content[i] == ret_stu[STU_NO])
					{
						error_pos = i;
						in.close();
						return -8;//写了自己
					}
					if (divided_content[i].length() != 7)
					{
						error_pos = i;
						in.close();
						return -9;//学号不是7位
					}
					for(int j=0;j<7;j++)
					{
						if (divided_content[i][j] < '0' || divided_content[i][j]>'9')
						{
							error_pos = i;
							in.close();
							return -10;//学号有非法符号
						}
					}
					//本项学号没问题
					if (divided_content.size() < (unsigned int)(i + 1 + 1))//后面没东西了
					{
						error_pos = i;
						in.close();
						return -11;//信息不全
					}
					i++;
					error_pos = i;
				}
			}

			ret = 0;
		}//end of firstline & secondline
	}//end of else（提交了）
	in.close();
	return ret;
}

int cross_check(vector <int>& error_pos, vector <vector <string>>& divided_content, vector <vector<string>>& ret_stu,string cno)
{
	cout << "交叉检查结果：" << endl;
	//i:本同学索引 k：另一同学索引
	unsigned int i, j, k, m;
	bool flag = true;//是否还需继续检查这个“对方”
	for ( i = 0; i < error_pos.size(); i++)//遍历每个文件
	{
		cout << setiosflags(ios::left) << setw(3) << i + 1 << resetiosflags(ios::left) << ": " << cno << '-' << ret_stu[i][STU_NO] <<'-' << ret_stu[i][STU_NAME] << endl;
		for ( j = 0; (int)j < error_pos[i]; j+=2)//遍历该文件中的所有同学
		{
			flag = true;
			cout << '\t' << divided_content[i][j] << ' ' << setiosflags(ios::left) <<  divided_content[i][j + 1] << resetiosflags(ios::left)<<'\t';
			for (k = 0; k < ret_stu.size(); k++)
			{
				if (divided_content[i][j] == ret_stu[k][STU_NO])
				{
					if (ret_stu[k][STU_NAME] != divided_content[i][j + 1])
					{
						flag = false;
						cout << "对方姓名不正确";
					}
					break;
				}
			}
			if (k == ret_stu.size())//没找到该同学
				cout << "对方学号不存在";
			else if (flag)
			{
				for (m = 0; (int)m < error_pos[k]; m++)
				{
					if (ret_stu[i][STU_NO] == divided_content[k][m])//在另一同学的文件中找到了该同学的学号
					{
						if (!(m + 1 < divided_content[k].size() && ret_stu[i][STU_NAME] == divided_content[k][m + 1]))//在学号后没找到该同学的名字
							cout << "没写对你名字";
						break;
					}
				}
				if (m == error_pos[k])//在另一同学的文件中没有找到该同学的学号
					cout << "抛弃了你";
			}
			cout << endl;
		}
	}
	return 0;
}

int deal_with_ret(int ret,int error_pos,char* buf,string action,string divided_content,bool stuno,bool name,bool major,int& pass,int& unsubmit,int& error_pdf,int& first_2,int& error_src,int& first_4, int& first_5, int& first_6, int& first_7)
{
	if (ret == 0)
	{
		cout << "正确" << endl;
		pass++;
	}
	else if (ret == -1)
	{
		cout << "未提交" << endl;
		unsubmit++;
	}
	else if (ret == -2)
	{
		if (action == "base")
		{
			cout << "PDF文件格式不正确" << endl;
			error_pdf++;
		}
		else if (action == "firstline")
		{
			cout << "首行不是符合要求的/* */格式 [" << buf << ']' << endl;
			first_2++;
		}
	}
	else if (ret == -3)
	{
		cout << "源文件格式不正确(非GB编码)" << endl;
		error_src++;
	}
	else if (ret == -4)
	{
		cout << "首行不是三项 [" << buf << ']' << endl;
		first_4++;
	}
	else if (ret == -5)
	{
		cout << "首行";
		if (!stuno)
			cout << "学号不匹配";
		if (!name)
			cout << "姓名不匹配";
		if (!major)
			cout << "班级不匹配";
		cout << " [" << buf << ']' << endl;
		first_5++;
	}
	else if (ret == -6)
	{
		cout << "文件小于4字节" << endl;
		first_6++;
	}
	else if (ret == -7)
	{
		cout << "首行不是注释行 [" << buf << ']' << endl;
		first_7++;
	}
	else if (ret == -8)
	{
		cout << "第[" << error_pos / 2 + 1 << "]项写了自己，后续内容忽略" << endl;
		//second_8++;
		pass++;
	}
	else if (ret == -9)
	{
		cout << "第" << error_pos / 2 + 1 << "位同学的学号[" << divided_content << "]不是7位，后续内容忽略" << endl;
		//second_9++;
		pass++;
	}
	else if (ret == -10)
	{
		cout << "第" << error_pos / 2 + 1 << "位同学的学号[" << divided_content << "]中有非数字存在，后续内容忽略" << endl;
		//second_10++;
		pass++;
	}
	else if (ret == -11)
	{
		cout << "第[" << error_pos / 2 << "]个学生后面的信息不全(只读到一项)，后续内容忽略" << endl;
		//second_11++;
		pass++;
	}
	return 0;
}
int set_maxlen(unsigned int& maxlen,int pass,int unsubmit,int error_pdf,int error_src,int first_2,int first_4,int first_7,int first_5,int first_6)
{
	if (pass != 0)
		maxlen = strlen("正确");
	if (unsubmit != 0)
		maxlen = strlen("未提交") > maxlen ? strlen("未提交") : maxlen;
	if (error_pdf != 0)
		maxlen = strlen("PDF文件格式不正确") > maxlen ? strlen("PDF文件格式不正确") : maxlen;
	if (error_src != 0)
		maxlen = strlen("源文件格式不正确(非GB编码)") > maxlen ? strlen("源文件格式不正确(非GB编码)") : maxlen;
	if (first_2 != 0)
		maxlen = strlen("首行多行注释格式不正确") > maxlen ? strlen("首行多行注释格式不正确") : maxlen;
	if (first_4 != 0)
		maxlen = strlen("首行不是三项") > maxlen ? strlen("首行不是三项") : maxlen;
	if (first_7 != 0)
		maxlen = strlen("首行不是注释行") > maxlen ? strlen("首行不是注释行") : maxlen;
	if (first_5 != 0)
		maxlen = strlen("首行检查出错") > maxlen ? strlen("首行检查出错") : maxlen;
	if (first_6 != 0)
		maxlen = strlen("文件小于4字节") > maxlen ? strlen("文件小于4字节") : maxlen;
	maxlen += (2 + 10);//前面空2格，后面空10格
	return 0;
}
int out_prompt(char separator,unsigned int maxlen , int pass, int unsubmit, int error_pdf, int error_src, int first_2, int first_4, int first_7, int first_5, int first_6)
{
	cout << setfill(separator)<< setw(maxlen) << separator << setfill(' ') << endl;
	if (pass != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "正确 :" << ' ' << pass << resetiosflags(ios::right) << endl;
	if (unsubmit != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "未提交 :" << ' ' << unsubmit << resetiosflags(ios::right) << endl;
	if (error_pdf != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "PDF文件格式不正确 :" << ' ' << error_pdf << resetiosflags(ios::right) << endl;
	if (error_src != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "源文件格式不正确(非GB编码) :" << ' ' << error_src << resetiosflags(ios::right) << endl;
	if (first_2 != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "首行多行注释格式不正确 :" << ' ' << first_2 << resetiosflags(ios::right) << endl;
	if (first_7 != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "首行不是注释行 :" << ' ' << first_7 << resetiosflags(ios::right) << endl;
	if (first_4 != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "首行不是三项 :" << ' ' << first_4 << resetiosflags(ios::right) << endl;
	if (first_5 != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "首行检查出错 :" << ' ' << first_5 << resetiosflags(ios::right) << endl;
	if (first_6 != 0)
		cout << setiosflags(ios::right) << setw(maxlen - 8) << "文件小于4字节 :" << ' ' << first_6 << resetiosflags(ios::right) << endl;
	cout << setfill(separator) << setw(maxlen) << separator << setfill(' ') << endl << endl;
	return 0;
}
int func(string action, args_analyse_tools* args, vector <vector<string>>& ret_stu, vector <vector<string>>& ret_hw, int stunum, int hwnum, string src_rootdir, int file_maxlen, int stuname_maxlen)
{
	string fbase = src_rootdir + "\\" + args[ARGS_CNO].get_string() + "-";
	string stu_no, hw_name, file;
	int stu_pass = 0, stu_unsubmit = 0, stu_error_src = 0;
	int pass_all = 0, unsubmit_all = 0, error_src_all = 0;
	//base
	int stu_error_pdf = 0;//单个文件，全体学生时，最后的输出
	int error_pdf_all = 0;//用于最后总输出
	//firstline
	int first_stu_2 = 0, first_stu_4 = 0, first_stu_5 = 0, first_stu_6 = 0,first_stu_7=0;
	int first_2_all = 0, first_4_all = 0, first_5_all = 0, first_6_all = 0,first_7_all=0;
	bool name = false, stuno = false, major = false;
	char buf[1024 + 1] = { 0 };
	//secondline
	vector <int> error_pos;
	vector <vector<string>> divided_content;

	if (args[ARGS_FILE].get_string() == "all")//所有文件
	{
		int pass = 0, unsubmit = 0, error_src = 0;
		//base
		int error_pdf = 0;//记录每一个学生的情况
		//firstline
		int first_2 = 0, first_4 = 0, first_5 = 0, first_6 = 0,first_7=0;
		//secondline
		int second_8 = 0, second_9 = 0, second_10 = 0, second_11 = 0;

		for (int i = 0; i < stunum; i++)
		{
			pass = 0, unsubmit = 0, error_src = 0;
			error_pdf = 0;
			first_2 = 0, first_4 = 0, first_5 = 0, first_6 = 0,first_7=0;
			stu_no = ret_stu[i][STU_NO];
			cout << setiosflags(ios::left) << setw(3) << i + 1 << resetiosflags(ios::left) << ": 学号-" << stu_no << " 姓名-" << ret_stu[i][STU_NAME] << " 课号-" << args[ARGS_CNO].get_string() << " 文件数量-" << hwnum << endl;
			for (int j = 0; j < hwnum; j++)
			{
				hw_name = ret_hw[j][HW_NAME];
				file = fbase + stu_no + "\\" + hw_name;
				//检查
				for (int k = 0; k < 1024 + 1; k++)
					buf[k] = '\0';
				error_pos.push_back(0);//压入一个初始值
				divided_content.push_back(vector<string>());//压入一个初始值
				int ret = check(file, action, ret_stu[i], name, stuno, major, buf,error_pos[i], divided_content[i]);
				cout << "  " << setiosflags(ios::left) << setw(file_maxlen) << hw_name << resetiosflags(ios::left) << " : ";
				deal_with_ret(ret, error_pos[i], buf, action, "", stuno, name, major, pass, unsubmit, error_pdf, first_2, error_src, first_4, first_5, first_6, first_7);
				//  12-b1-1.cpp                 : 正确
			}//end of for hw
			pass_all += pass;
			unsubmit_all += unsubmit;
			error_pdf_all += error_pdf;
			error_src_all += error_src;
			first_2_all += first_2;
			first_4_all += first_4;
			first_5_all += first_5;
			first_6_all += first_6;
			first_7_all += first_7;

			unsigned int maxlen = 0;
			set_maxlen(maxlen, pass, unsubmit, error_pdf, error_src, first_2, first_4, first_7, first_5, first_6);

			if (pass == hwnum)
				cout << "全部";
			else
				cout << "检查";
			cout << "通过" << pass << "/" << hwnum << "个文件，本次通过" << pass << "个" << endl;
			cout << setfill('-') << setw(maxlen) << '-' << setfill(' ') << endl;
			cout << "学生详细信息" << endl;
			out_prompt('-', maxlen, pass, unsubmit, error_pdf, error_src, first_2, first_4, first_7, first_5, first_6);
			
			/*检查通过65 / 67个文件，本次通过65个
				------------------
				学生详细信息
				------------------
				正确 : 65
				未提交 : 2
				------------------*/
		}//end of for stu
	}
	else//单个文件
	{
		cout << "课号 : " << args[ARGS_CNO].get_string() << " 学生数量 : " << stunum << " 源文件名 : " << args[ARGS_FILE].get_string() << endl;
		hw_name = args[ARGS_FILE].get_string();

		for (int i = 0; i < stunum; i++)
		{
			stu_no = ret_stu[i][STU_NO];
			file = fbase + stu_no + "\\" + hw_name;
			//检查
			error_pos.push_back(0);//压入一个初始值
			divided_content.push_back(vector<string>());//压入一个初始值
			int ret = check(file, action, ret_stu[i], name, stuno, major, buf, error_pos[i], divided_content[i]);
			//stuname_maxlen不用，默认最大8
			cout << setiosflags(ios::left) << setw(2) << i + 1 << " : " << stu_no << "/" << setw(8) << ret_stu[i][STU_NAME] << resetiosflags(ios::left) << " : ";
			//cout << "  " << setiosflags(ios::left) << setw(file_maxlen) << hw_name << resetiosflags(ios::left) << " : ";
			if (divided_content[i].size()!=0)
				deal_with_ret(ret, error_pos[i], buf, action, divided_content[i][error_pos[i]], stuno, name, major, stu_pass, stu_unsubmit, stu_error_pdf, first_stu_2, stu_error_src, first_stu_4, first_stu_5, first_stu_6, first_stu_7);
			else
				deal_with_ret(ret, error_pos[i], buf, action, "", stuno, name, major, stu_pass, stu_unsubmit, stu_error_pdf, first_stu_2, stu_error_src, first_stu_4, first_stu_5, first_stu_6, first_stu_7);
			//1  : 1951792/苏天齐   : 正确
		}//end of for stu
	}//end of else（单个文件）

	//最后输出
	if (args[ARGS_FILE].get_string() != "all")//单个文件
	{
		pass_all = stu_pass;
		unsubmit_all = stu_unsubmit;
		error_pdf_all = stu_error_pdf;
		error_src_all = stu_error_src;
		first_2_all = first_stu_2;
		first_4_all = first_stu_4;
		first_5_all = first_stu_5;
		first_6_all = first_stu_6;
		first_7_all = first_stu_7;
	}

	unsigned int maxlen = 0;
	set_maxlen(maxlen, pass_all, unsubmit_all, error_pdf_all, error_src_all, first_2_all, first_4_all, first_7_all, first_5_all, first_6_all);

	if (args[ARGS_FILE].get_string() != "all")//单个文件
	{
		if (stu_pass != stunum)
			cout << endl << "检查通过";
		else
			cout << endl << "全部通过";
		cout << stu_pass << "/" << stunum << "个学生，本次通过" << stu_pass << "个" << endl;
		cout << setfill('=') << setw(maxlen) << '=' << setfill(' ') << endl;
		cout << "详细信息" << endl;
	}
	else//多个文件
	{
		cout << endl << "共完成" << stunum << "个学生的检查，文件总数:" << hwnum * stunum << "，通过总数:" << pass_all << "，本次通过" << pass_all << "个" << endl;
		cout << setfill('=') << setw(maxlen) << '=' << setfill(' ') << endl;
		cout << "整体详细信息" << endl;
	}
	out_prompt('=', maxlen, pass_all, unsubmit_all, error_pdf_all, error_src_all, first_2_all, first_4_all, first_7_all, first_5_all, first_6_all);
	
	if (action == "secondline")
	{
		cross_check(error_pos, divided_content, ret_stu, args[ARGS_CNO].get_string());
		cout << endl;
	}
	return 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
 ***************************************************************************/
int main(int argc, char** argv)
{
	/*const char* argv[] = {
		"hw_check",      
		"--action", "secondline",
		"--cno", "10108002",
		"--stu", "all",
		"--file", "15-b5.c",
	};
	int argc = 9;*/

	const string Action_Name[] = { "base","firstline","secondline","" }; //也可以根据需要，放入头文件中以便共享 

	args_analyse_tools args[] = {
		args_analyse_tools("--help",     ST_EXTARGS_TYPE::boolean,            0, false),
		args_analyse_tools("--debug",    ST_EXTARGS_TYPE::boolean,            0, false),
		args_analyse_tools("--action",   ST_EXTARGS_TYPE::str_with_set_error, 1, -1, Action_Name),	//参数-1无意义，表示无默认，必须指定
		args_analyse_tools("--cno",      ST_EXTARGS_TYPE::str,                1, string("")),
		args_analyse_tools("--stu",      ST_EXTARGS_TYPE::str,                1, string("")),
		args_analyse_tools("--file",     ST_EXTARGS_TYPE::str,                1, string("")),
		args_analyse_tools("--chapter",  ST_EXTARGS_TYPE::int_with_error,     1, -1, -1, 99), //参数-1表示不进行章节选择
		args_analyse_tools("--week",     ST_EXTARGS_TYPE::int_with_error,     1, -1, -1, 20), //参数-1表示不进行周次选择
		args_analyse_tools("--display",  ST_EXTARGS_TYPE::str,                1, string("11111")),
		args_analyse_tools("--cfgfile",  ST_EXTARGS_TYPE::str,                1, string("hw_check.conf")),
		args_analyse_tools()  //最后一个，用于结束
	};

	/* 不带任何参数做特殊处理，打印提示信息后结束 */
	if (argc == 1) {
		usage(argv[0]);
		return -1;
	}

	int need_fixed_args = 0;	//需要固定参数
	int cur_argc;
	int num_of_fixed_args = 0;

	if ((cur_argc = args_analyse_process(argc, argv, args, need_fixed_args)) < 0)
	{
		//错误信息在函数中已打印
		return -1;
	}

	/* 带--help参数做特殊处理，打印提示信息后结束 */
	if (args[ARGS_HELP].existed()) {
		usage(argv[0]);
		return -1;
	}

	/* 可变参数分析完成后，看是否还有固定参数，以及固定参数的数量是否符合要求 */
	if (need_fixed_args) {
		if (cur_argc == argc) {  //说明后面没有跟需要的固定参数
			cout << "需要固定参数，但未给出" << endl;
			return -1;
		}
		if (argc - cur_argc != num_of_fixed_args) {
			cout << "除可变参数外，必须给出" << num_of_fixed_args << "个固定参数，但现在只给出了" << argc - cur_argc << "个固定参数" << endl;
			return -1;
		}
	}

	string action = args[ARGS_ACTION].get_string();

	//参数分析
	if (analyse(args, argv[0]) == -1)
		return -1;

	//读配置文件
	CFG cfg;
	if (readconfig_hw_check_conf(args[ARGS_CFGILE].get_string(), &cfg) != 0)
		return -1;

	//检查参数是否存放在cfg的数据库中
	vector <vector<string>> ret_stu, ret_hw;
	int stunum = 0, hwnum = 0;
	int file_maxlen = 0, stuname_maxlen = 0;
	if (mysql(&cfg, args, ret_stu, ret_hw, stunum, hwnum, file_maxlen, stuname_maxlen) == -1)
		return -1;

	//然后根据数据库中的文件拼成一个文件路径，打开指定的学生作业文件
	func(action, args, ret_stu, ret_hw, stunum, hwnum, cfg.src_rootdir, file_maxlen, stuname_maxlen);

	return 0;
}
