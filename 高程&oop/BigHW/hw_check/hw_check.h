/* 2350222 计科 盛曦 */
#pragma once

#include "../include_mariadb_x86/mysql/mysql.h"      // mysql特有

enum OPT_ARGS
{
	ARGS_HELP = 0,
	ARGS_DEBUG,
	ARGS_ACTION,
	ARGS_CNO,
	ARGS_STU,
	ARGS_FILE,
	ARGS_CHAPTER,
	ARGS_WEEK,
	ARGS_DISPLAY,
	ARGS_CFGILE,
	ARGS_MAX
};

typedef struct hw_check_cfg {
	string src_rootdir;
	string dbserver;
	string dbuser;
	string dbpasswd;
	string dbname;
}CFG;

enum STU_INDEX
{
	STU_CNO = 0,
	STU_GRADE,
	STU_NO,
	STU_NAME,
	STU_SEX,
	STU_MAJOR,
	STU_MAJOR_SIMPLE
};
enum HW_INDEX
{
	HW_TYPE = 0,
	HW_CNO,
	HW_NO,
	HW_CHAP,
	HW_WEEK,
	HW_NAME,
	HW_SCORE
};

int mysql(CFG* cfg,args_analyse_tools* args, vector <vector<string>>& ret_stu, vector <vector<string>>& ret_hw, int& stunum, int& hwnum, int& file_maxlen, int& stuname_maxlen);
int is_source_file(const string file);