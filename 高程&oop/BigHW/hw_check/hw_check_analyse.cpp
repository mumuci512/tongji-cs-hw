/* 2350222 计科 盛曦 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "../include/class_aat.h"
#include "hw_check.h"
using namespace std;

int mysql(CFG* cfg, args_analyse_tools* args,vector <vector<string>>& ret_stu, vector <vector<string>>& ret_hw, int& stunum ,int& hwnum,int& file_maxlen,int& stuname_maxlen)
{
	MYSQL* mysql;
	MYSQL_RES* result;
	MYSQL_ROW  row;

	/* 初始化 mysql 变量，失败返回NULL */
	if ((mysql = mysql_init(NULL)) == NULL) {
		cout << "mysql_init failed" << endl;
		return -1;
	}

	/* 连接数据库，失败返回NULL
		1、mysqld没运行
		2、没有指定名称的数据库存在 */
	if (mysql_real_connect(mysql, cfg->dbserver.c_str(), cfg->dbuser.c_str(), cfg->dbpasswd.c_str(), cfg->dbname.c_str(), 0, NULL, 0) == NULL) {
		cout << "mysql_real_connect failed(" << mysql_error(mysql) << ")" << endl;
		return -1;
	}

	/* 设置字符集，否则读出的字符乱码 */
	mysql_set_character_set(mysql, "gbk");

	string stu = args[ARGS_STU].get_string();
	string cno = args[ARGS_CNO].get_string();
	string file = args[ARGS_FILE].get_string();

	string sql_stu,sql_hw;
	sql_stu = "select * from view_hwcheck_stulist where stu_cno = \"" + cno + "\"";
	sql_hw = "select * from view_hwcheck_hwlist where hw_cno = \"" + cno + "\"";
	if (stu != "all")
	{
		if (stu.length() != 7)
		{
			cout << "文件[" << stu << "]无法打开." << endl << endl;
			cout << "[--严重错误--] 读取 [--stu] 指定的文件出错."<<endl;
			return -1;
		}
		for (unsigned int i = 0; i < stu.length(); i++)
		{
			if (stu[i] < '0' || stu[i]>'9')
			{
				cout << "文件[" << stu << "]无法打开." << endl << endl;
				cout << "[--严重错误--] 读取 [--stu] 指定的文件出错."<<endl;
				return -1;
			}
		}
		sql_stu += " and stu_no = \"" + stu+"\"";
	}
	if (args[ARGS_CHAPTER].existed()&&args[ARGS_CHAPTER].get_int()!=-1 && args[ARGS_CHAPTER].get_int() != 99 && args[ARGS_CHAPTER].get_int() != 98)
		sql_hw += " and hw_chapter = \"" + to_string(args[ARGS_CHAPTER].get_int()) + "\"";
	if (args[ARGS_WEEK].existed() && args[ARGS_WEEK].get_int() != -1)
		sql_hw += " and hw_week = \"" + to_string(args[ARGS_WEEK].get_int()) + "\"";

	/* -----------------------------
		  进行第一次查询 - Begin
	   -----------------------------*/

	if (mysql_query(mysql, sql_stu.c_str())) {
		cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
		return -1;
	}

	/* 将查询结果存储起来，出现错误则返回NULL
		注意：查询结果为NULL，不会返回NULL */
	if ((result = mysql_store_result(mysql)) == NULL) {
		cout << "mysql_store_result failed" << endl;
		return -1;
	}

	while ((row = mysql_fetch_row(result)) != NULL) 
	{
		vector<string> row_data;//用于存放row
		for (unsigned int i = 0; i < mysql_num_fields(result); i++)
			row_data.push_back(row[i] ? row[i] : "");
		ret_stu.push_back(row_data);//存放了所有学生

		if (args[ARGS_STU].get_string() == "all")
		{
			if ((int)strlen(row[STU_NAME]) > stuname_maxlen)
				stuname_maxlen = strlen(row[STU_NAME]);
		}
		//cout << row[STU_NAME] << endl;
		stunum++;
	}

	/* 释放result，否则会丢失内存 */
	mysql_free_result(result);

	if (stunum == 0)
	{
		cout << "本次操作的学生数量为0/文件数量为0，未执行" << endl;
		return -1;
	}

	/* -----------------------------
		  进行第一次查询 - End
	   -----------------------------*/

	/* -----------------------------
			 进行第二次查询 - Begin
	   -----------------------------*/

	if (mysql_query(mysql, sql_hw.c_str())) {
		cout << "mysql_query failed(" << mysql_error(mysql) << ")" << endl;
		return -1;
	}

	/* 将查询结果存储起来，出现错误则返回NULL
		注意：查询结果为NULL，不会返回NULL */
	if ((result = mysql_store_result(mysql)) == NULL) {
		cout << "mysql_store_result failed" << endl;
		return -1;
	}

	int is_file_exist = 0;
	if (file == "all")
		is_file_exist = 1;
	file_maxlen = 0;
	while ((row = mysql_fetch_row(result)) != NULL)
	{
		vector<string> row_data;//用于存放row
		int flag=1;//1：需要全部文件 0：只需要源文件

		//假如chapter是特殊值
		if (args[ARGS_CHAPTER].existed() && (args[ARGS_CHAPTER].get_int() == 99 || args[ARGS_CHAPTER].get_int() == 98))
		{
			if (args[ARGS_ACTION].get_string() == "base")
			{
				if ((args[ARGS_CHAPTER].get_int() == 99 && (strcmp(row[HW_NAME],"id_rsa_3072.pub")==0 || strcmp(row[HW_NAME],"help.pdf")==0))//chapter=99，只需要这2个文件
					|| (args[ARGS_CHAPTER].get_int() == 98 && strlen(row[HW_NAME]) > strlen("Report") && strncmp(row[HW_NAME], "Report", strlen("Report")) == 0))//chapter=98,只需要Report文件
				{
					for (unsigned int i = 0; i < mysql_num_fields(result); i++)
						row_data.push_back(row[i] ? row[i] : "");
					ret_hw.push_back(row_data);//存放了所有作业
					if ((int)strlen(row[HW_NAME]) > file_maxlen)
						file_maxlen = strlen(row[HW_NAME]);
					hwnum++;
				}
			}
			else
				break;
		}
		else
		{
			if (args[ARGS_ACTION].get_string() != "base")
				flag = 0;
			if ((flag == 0 && is_source_file(row[HW_NAME]) == 1) || flag == 1)
			{
				for (unsigned int i = 0; i < mysql_num_fields(result); i++)
					row_data.push_back(row[i] ? row[i] : "");
				ret_hw.push_back(row_data);//存放了所有作业
				if (file == "all")
				{
					if ((int)strlen(row[HW_NAME]) > file_maxlen)
						file_maxlen = strlen(row[HW_NAME]);
				}
				if (file != "all")
				{
					if (file == row[HW_NAME])
					{
						is_file_exist = 1;
						hwnum = 1;
						break;
					}
				}
				hwnum++;
			}
		}
	}
	/* 释放result，否则会丢失内存 */
	mysql_free_result(result);

	if (is_file_exist == 0)//file参数的文件名不存在
	{
		//mysql execute [call proc_hwapp_get_onefile_subinfo("10108002", "123", "all"); ] 查询到符合要求的记录为[0].
		if(args[ARGS_ACTION].get_string() == "base")
			cout << "mysql execute [call proc_hwapp_get_onefile_subinfo(\"" << cno << "\", \"" << file << "\", \"all\"); ] 查询到符合要求的记录为[0]." << endl;
		else
			cout << "mysql execute [call proc_hwapp_get_onefile_subinfo(\"" << cno << "\", \"" << file << "\", \"c+h\"); ] 查询到符合要求的记录为[0]." << endl;
		return -1;
	}
	if (hwnum == 0)
	{
		cout << "本次操作的学生数量为0/文件数量为0，未执行" << endl;
		return -1;
	}

	/* -----------------------------
		  进行第二次查询 - End
	   -----------------------------*/
	return 0;
}
