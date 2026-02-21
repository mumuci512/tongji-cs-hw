/* 2350222 计科 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
/* 添加自己需要的头文件，注意限制 */
#include <fstream>
#include <sstream>
#include "../include/class_cft.h"
#include "../include/class_aat.h"
#include "../include/my_tools.h"
using namespace std;

/* 给出各种自定义函数的实现（已给出的内容不全） */
int get_effective_content(char* temp,int& start,int& end)
{
	int space_flag = 1;//表示一直是空格
	int i, j;
	for (i = 0; temp[i] != '\0'; i++)
	{
		if (temp[i] != ' ' && temp[i] != '\t' && space_flag == 1)
		{
			space_flag = 0;
			start = i;
		}
		end = i;
		if (temp[i] == '#' || temp[i] == ';' || (temp[i] == '/' && i + 1 < MAX_LINE && temp[i + 1] == '/'))
		{
			for (j = i; temp[j] != '\0'; j++)
				temp[j] = '\0';
			if(end>start)
				end--;
			break;
		}
	}
	for (i = end; i >= 0; i--)
	{
		if (temp[i] != ' ' && temp[i] != '\t')
		{
			end = i;
			break;
		}
	}
	return 0;
}

int get_item_name_content(char* temp,int start,int& end1,int& start1,char separator)
{
	int i, j;
	for (i = start; temp[i] != '\0'; i++)
	{
		if (temp[i] == separator)
		{
			if (i == start)
			{
				//把item_name当空串，在按item_name查找时，怎么区分只被get_item_all读取的没有分隔符的项，和这种项？？？？？？？？？？？？？？？？？？？？？？？？？？？
				cout << "该行第一个有效字符就是分隔符" << endl;
				end1 = start;
			}
			end1 = i-1;//end1是separator前
			break;
		}
	}
	if (temp[i] == '\0')//本行没有分隔符
		return -1;
	if(end1!=start)
	{
		for (j = end1; j>=start; j--)
		{
			if (temp[j] != ' ' && temp[j] != '\t')
			{
				end1 = j;
				break;
			}
		}
	}
	for (i++; temp[i] != '\0'; i++)
	{
		if (temp[i] != ' ' && temp[i] != '\t')
		{
			start1 = i;
			break;
		}
	}
	if (temp[i] == '\0')//分隔符后全是空格
		start1 = i;
	return 0;
}

int find_group(config_file_tools* const cft, const char* const group_name, const bool is_case_sensitive,vector<string>::iterator& p)
{
	for (p = cft->group_name_list.begin(); p != cft->group_name_list.end() && strcmp_twoways(group_name, p->c_str(), is_case_sensitive) != 0; p++)
		;
	if (p == cft->group_name_list.end())
	{
		//cout << "未找到组" << group_name << endl;
		return -1;
	}
	return p- cft->group_name_list.begin();
}

int find_item(config_file_tools* const cft, const char* const item_name, const bool is_case_sensitive,const int group_index,vector<string>::iterator& q)
{
	for (q = cft->item_name_list[group_index].begin(); q != cft->item_name_list[group_index].end() && strcmp_twoways(item_name, q->c_str(), is_case_sensitive) != 0; q++)
		;
	if (q == cft->item_name_list[group_index].end())
	{
		//cout << "未找到项" << item_name << endl;
		return -1;
	}
	return q- cft->item_name_list[group_index].begin();
}

int find_group_item(config_file_tools* const cft, const char* const group_name, const char* const item_name, const bool group_is_case_sensitive, const bool item_is_case_sensitive,int& group_index,int& item_index)
{
	vector<string>::iterator p, q;
	group_index = find_group(cft, group_name, group_is_case_sensitive, p);
	if (group_index == -1)
	{
		item_index = -1;
		return 0;
	}
	item_index = find_item(cft, item_name, item_is_case_sensitive, group_index, q);
	return 0;
}
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
config_file_tools::config_file_tools(const char* const _cfgname, const enum BREAK_CTYPE _ctype)
{
	/* 按需完成 */
	cfgname = _cfgname;
	item_separate_character_type = _ctype;
	is_line_legal = true;
	is_open_file_success = true;

	ifstream in;
	in.open(cfgname, ios::in || ios::binary);
	if (!in)
	{
		//cout << "打开配置文件[" << cfgname << "]失败." << endl;
		is_open_file_success = false;
		return;
	}

	char separator='\0';
	switch (item_separate_character_type)
	{
		case BREAK_CTYPE::Space:
			separator = ' ';
			break;
		case BREAK_CTYPE::Equal:
			separator = '=';
			break;
		default:
			cout << "分隔符错误，已改为=" << endl;
			separator = '=';
			break;
	}

	int start,end1,start1, end;//每行去除注释和前后空格的首末位置
	char temp[MAX_LINE+1] = { '\0' };
	int line = 0;//目前读到的行数
	int group_index = -1;
	while(!in.eof())
	{
		in.getline(temp, MAX_LINE+1, '\n');//实际只会读MAX_LINE个字符
		line++;
		if (temp[0] == '\0')//空行
			continue;
		if (temp[MAX_LINE - 1] != '\0')
		{
			cout << "非法格式的配置文件，第["<<line<<"]行超过最大长度"<<MAX_LINE<<"." << endl;
			is_line_legal = false;
			break;
		}

		get_effective_content(temp, start, end);
		if (start == end)//全是空格或注释
			continue;

		if(temp[start]=='['&&temp[end]==']')//是组名
		{
			temp[end+1] = '\0';
			group_name_list.push_back(string(temp+start));
			item_name_list.push_back(vector<string>());
			item_content_list.push_back(vector<string>());
			item_list.push_back(vector<string>());
			group_index++;
		}
		else//不是组名
		{
			if (group_name_list.size()==0)
			{
				group_name_list.push_back("");
				item_name_list.push_back(vector<string>());
				item_content_list.push_back(vector<string>());
				item_list.push_back(vector<string>());
				group_index++;
			}
			item_list[group_index].push_back(string(temp + start,end-start+1));

			if (get_item_name_content(temp, start, end1, start1, separator) == -1)//本行没有分隔符
			{
				item_name_list[group_index].push_back("");//装入空串，否则后续item索引不对？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
				item_content_list[group_index].push_back("");//装入空串
				continue;//直接读下一行，本行只存储在item_list中，只能用get_all_item读取
			}
			if (temp[start1] == '\0')//分隔符后没有内容
				start1 = end;
			item_name_list[group_index].push_back(string(temp+start, end1 - start + 1));
			//cout << item_name_list[group_index][0];
			item_content_list[group_index].push_back(string(temp+start1,end-start1+1));
		}
	}
	
	in.close();
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
config_file_tools::config_file_tools(const string& _cfgname, const enum BREAK_CTYPE _ctype)
{
	/* 按需完成 */
	cfgname = _cfgname;
	item_separate_character_type = _ctype;
	is_line_legal = true;
	is_open_file_success = true;

	ifstream in;
	in.open(cfgname, ios::in || ios::binary);
	if (!in)
	{
		//cout << "打开配置文件[" << cfgname << "]失败." << endl;
		is_open_file_success = false;
		return;
	}

	char separator = '\0';
	switch (item_separate_character_type)
	{
		case BREAK_CTYPE::Space:
			separator = ' ';
			break;
		case BREAK_CTYPE::Equal:
			separator = '=';
			break;
		default:
			cout << "分隔符错误，已改为=" << endl;
			separator = '=';
			break;
	}

	int start, end1, start1, end;//每行去除注释和前后空格的首末位置
	char temp[MAX_LINE + 1] = { '\0' };
	int line = 0;//目前读到的行数
	int group_index = -1;
	while (!in.eof())
	{
		in.getline(temp, MAX_LINE + 1, '\n');//实际只会读MAX_LINE个字符
		line++;
		if (temp[0] == '\0')//空行
			continue;
		if (temp[MAX_LINE - 1] != '\0')
		{
			cout << "非法格式的配置文件，第[" << line << "]行超过最大长度" << MAX_LINE << "." << endl;
			is_line_legal = false;
			break;
		}

		get_effective_content(temp, start, end);
		if (start == end)//全是空格或注释
			continue;

		if (temp[start] == '[' && temp[end] == ']')//是组名
		{
			temp[end + 1] = '\0';
			group_name_list.push_back(string(temp + start));
			item_name_list.push_back(vector<string>());
			item_content_list.push_back(vector<string>());
			item_list.push_back(vector<string>());
			group_index++;
		}
		else//不是组名
		{
			if (group_name_list.size() == 0)
			{
				group_name_list.push_back("");
				item_name_list.push_back(vector<string>());
				item_content_list.push_back(vector<string>());
				item_list.push_back(vector<string>());
				group_index++;
			}
			item_list[group_index].push_back(string(temp + start, end - start + 1));

			if (get_item_name_content(temp, start, end1, start1, separator) == -1)//本行没有分隔符
			{
				item_name_list[group_index].push_back("");//装入空串，否则后续item索引不对？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
				item_content_list[group_index].push_back("");//装入空串
				continue;//直接读下一行，本行只存储在item_list中，只能用get_all_item读取
			}
			//if (temp[start1] == '\0')//分隔符后没有内容
			//	start1 = end;
			item_name_list[group_index].push_back(string(temp + start, end1 - start + 1));
			//cout << item_name_list[group_index][0];
			item_content_list[group_index].push_back(string(temp + start1, end - start1 + 1));
		}
	}

	in.close();
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
config_file_tools::~config_file_tools()
{
	/* 按需完成 */
	group_name_list.clear();
	item_list.clear();
	item_name_list.clear();
	item_content_list.clear();
}


/***************************************************************************
  函数名称：
  功    能：判断读配置文件是否成功
  输入参数：
  返 回 值：true - 成功，已读入所有的组/项
		   false - 失败，文件某行超长/文件全部是注释语句
  说    明：
***************************************************************************/
bool config_file_tools::is_read_succeeded() const
{
	/* 按需完成，根据需要改变return的值 */
	if (is_open_file_success == false)
		return false;
	if (group_name_list.empty()&& item_list.empty())//什么都没读到
		return false;
	return is_line_legal;
}

/***************************************************************************
  函数名称：
  功    能：返回配置文件中的所有组
  输入参数：vector <string>& ret : vector 中每项为一个组名
  返 回 值：读到的组的数量（简单配置文件的组数量为1，组名为"）
  说    明：
***************************************************************************/
int config_file_tools::get_all_group(vector <string>& ret)
{
	/* 按需完成，根据需要改变return的值 */
	int n = 0;
	for (vector<string>::iterator p = group_name_list.begin(); p != group_name_list.end(); p++)
	{
		ret.push_back(*p);
		n++;
	}
	return n;
}

/***************************************************************************
  函数名称：
  功    能：查找指定组的所有项并返回项的原始内容
  输入参数：const char* const group_name：组名
		   vector <string>& ret：vector 中每项为一个项的原始内容
		   const bool is_case_sensitive = false : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：项的数量，0表示空
  说    明：
***************************************************************************/
int config_file_tools::get_all_item(const char* const group_name, vector <string>& ret, const bool is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	int n = 0;
	vector<string>::iterator p,q;
	int group_index;
	if ((group_index=find_group(this, group_name, is_case_sensitive, p) )== -1)
	{
		//cout << "未找到组" << group_name << endl;
		return 0;
	}

	for (q = item_list[group_index].begin(); q != item_list[group_index].end(); q++)
	{
		ret.push_back(*q);
		n++;
	}
	return n;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int config_file_tools::get_all_item(const string& group_name, vector <string>& ret, const bool is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	return this->get_all_item(group_name.c_str(), ret, is_case_sensitive);
}

/***************************************************************************
  函数名称：
  功    能：取某项的原始内容（=后的所有字符，string方式）
  输入参数：const char* const group_name
		   const char* const item_name
		   string &ret
		   const bool group_is_case_sensitive = false : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
		   const bool item_is_case_sensitive = false  : 项名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：
  说    明：
***************************************************************************/
int config_file_tools::item_get_raw(const char* const group_name, const char* const item_name, string& ret, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	//若item_name为空串，怎么办？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	int group_index, item_index;
	find_group_item(this, group_name, item_name, group_is_case_sensitive, item_is_case_sensitive, group_index, item_index);
	if (group_index == -1 || item_index == -1)
		return 0;
	ret = item_content_list[group_index][item_index].c_str();
	return 1;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：
***************************************************************************/
int config_file_tools::item_get_raw(const string& group_name, const string& item_name, string& ret, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 本函数已实现 */
	return this->item_get_raw(group_name.c_str(), item_name.c_str(), ret, group_is_case_sensitive, item_is_case_sensitive);
}

/***************************************************************************
  函数名称：
  功    能：取某项的内容，返回类型为char型
  输入参数：const char* const group_name               ：组名
		   const char* const item_name                ：项名
		   const bool group_is_case_sensitive = false : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
		   const bool item_is_case_sensitive = false  : 项名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：1 - 该项的项名存在
		   0 - 该项的项名不存在
  说    明：
***************************************************************************/
int config_file_tools::item_get_null(const char* const group_name, const char* const item_name, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	//若item_name为空串，怎么办？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	int group_index,item_index;
	find_group_item(this, group_name, item_name, group_is_case_sensitive, item_is_case_sensitive, group_index, item_index);
	if (group_index == -1 || item_index == -1)
		return 0;

	return 1;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：因为工具函数一般在程序初始化阶段被调用，不会在程序执行中被高频次调用，
		   因此这里直接套壳，会略微影响效率，但不影响整体性能（对高频次调用，此方法不适合）
***************************************************************************/
int config_file_tools::item_get_null(const string& group_name, const string& item_name, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 本函数已实现 */
	return this->item_get_null(group_name.c_str(), item_name.c_str(), group_is_case_sensitive, item_is_case_sensitive);
}

/***************************************************************************
  函数名称：
  功    能：取某项的内容，返回类型为char型
  输入参数：const char* const group_name               ：组名
		   const char* const item_name                ：项名
		   char& value                                ：读到的char的值（返回1时可信，返回0则不可信）
		   const char* const choice_set = nullptr     ：合法的char的集合（例如："YyNn"表示合法输入为Y/N且不分大小写，该参数有默认值nullptr，表示全部字符，即不检查）
		   const char def_value = DEFAULT_CHAR_VALUE  ：读不到/读到非法的情况下的默认值，该参数有默认值DEFAULT_CHAR_VALUE，分两种情况
															当值是   DEFAULT_CHAR_VALUE 时，返回0（值不可信）
															当值不是 DEFAULT_CHAR_VALUE 时，令value=def_value并返回1
		   const bool group_is_case_sensitive = false : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
		   const bool item_is_case_sensitive = false  : 项名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：1 - 取到正确值
			   未取到值/未取到正确值，设置了缺省值（包括设为缺省值）
		   0 - 未取到（只有为未指定默认值的情况下才会返回0）
  说    明：
***************************************************************************/
int config_file_tools::item_get_char(const char* const group_name, const char* const item_name, char& value,
	const char* const choice_set, const char def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	//若item_name为空串，怎么办？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	int group_index, item_index;
	find_group_item(this, group_name, item_name, group_is_case_sensitive, item_is_case_sensitive, group_index, item_index);
	if (group_index == -1 || item_index == -1|| item_content_list[group_index][item_index][0] == '\0')
	{
		if (def_value == DEFAULT_CHAR_VALUE)
			return 0;
		value = def_value;
		return 1;
	}

	value = item_content_list[group_index][item_index][0];
	if (choice_set != nullptr)
	{
		for (int i = 0; choice_set[i] != '\0'; i++)
		{
			if (value == choice_set[i])
				return 1;
		}
		if (def_value == DEFAULT_CHAR_VALUE)
			return 0;
		value = def_value;
		return 1;
	}

	/*istringstream in(item_content_list[group_index][item_index]);
	in >> value;
	if(in.fail())
	{
		if (def_value == DEFAULT_CHAR_VALUE)
			return 0;
		value = def_value;
		return 1;
	}*/
	return 1;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：因为工具函数一般在程序初始化阶段被调用，不会在程序执行中被高频次调用，
		   因此这里直接套壳，会略微影响效率，但不影响整体性能（对高频次调用，此方法不适合）
***************************************************************************/
int config_file_tools::item_get_char(const string& group_name, const string& item_name, char& value,
						const char* const choice_set, const char def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 本函数已实现 */
	return this->item_get_char(group_name.c_str(), item_name.c_str(), value, choice_set, def_value, group_is_case_sensitive, item_is_case_sensitive);
}

/***************************************************************************
  函数名称：
  功    能：取某项的内容，返回类型为int型
  输入参数：const char* const group_name               ：组名
		   const char* const item_name                ：项名
		   int& value                                 ：读到的int的值（返回1时可信，返回0则不可信）
		   const int min_value = INT_MIN              : 期望数据范围的下限，默认为INT_MIN
		   const int max_value = INT_MAX              : 期望数据范围的上限，默认为INT_MAX
		   const int def_value = DEFAULT_INT_VALUE    ：读不到/读到非法的情况下的默认值，该参数有默认值 DEFAULT_INT_VALUE，分两种情况
															当值是   DEFAULT_INT_VALUE 时，返回0（值不可信）
															当值不是 DEFAULT_INT_VALUE 时，令value=def_value并返回1
		   const bool group_is_case_sensitive = false : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
		   const bool item_is_case_sensitive = false  : 项名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：
  说    明：
***************************************************************************/
int config_file_tools::item_get_int(const char* const group_name, const char* const item_name, int& value,
						const int min_value, const int max_value, const int def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	//若item_name为空串，怎么办？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	int group_index, item_index;
	find_group_item(this, group_name, item_name, group_is_case_sensitive, item_is_case_sensitive, group_index, item_index);
	if (group_index == -1 || item_index == -1 || item_content_list[group_index][item_index][0] == '\0')
	{
		if (def_value == DEFAULT_INT_VALUE)
			return 0;
		value = def_value;
		return 1;
	}

	//for (int i = 0; item_content_list[group_index][item_index][i] != '\0'; i++)
	//{
	//	if (item_content_list[group_index][item_index][i] < '0' || item_content_list[group_index][item_index][i]>'9')//有非数字字符
	//	{
	//		if (item_content_list[group_index][item_index][i] == '-' && i == 0)
	//			continue;
	//		if (def_value == DEFAULT_INT_VALUE)
	//			return 0;
	//		value = def_value;
	//		return 1;
	//	}
	//}

	//long long int t = atoll(item_content_list[group_index][item_index].c_str());//用long long int防止溢出
	//if (t > max_value || t < min_value)
	//{
	//	if (def_value == DEFAULT_INT_VALUE)
	//		return 0;
	//	value = def_value;
	//	return 1;
	//}
	//value = (int)t;

	istringstream in(item_content_list[group_index][item_index]);
	in >> value;
	if (in.fail()|| value > max_value || value < min_value)
	{
		if (def_value == DEFAULT_INT_VALUE)
			return 0;
		value = def_value;
		return 1;
	}
	return 1;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：因为工具函数一般在程序初始化阶段被调用，不会在程序执行中被高频次调用，
		   因此这里直接套壳，会略微影响效率，但不影响整体性能（对高频次调用，此方法不适合）
***************************************************************************/
int config_file_tools::item_get_int(const string& group_name, const string& item_name, int& value,
						const int min_value, const int max_value, const int def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 本函数已实现 */
	return this->item_get_int(group_name.c_str(), item_name.c_str(), value, min_value, max_value, def_value, group_is_case_sensitive, item_is_case_sensitive);
}

/***************************************************************************
  函数名称：
  功    能：取某项的内容，返回类型为double型
  输入参数：const char* const group_name                  ：组名
		   const char* const item_name                   ：项名
		   double& value                                 ：读到的int的值（返回1时可信，返回0则不可信）
		   const double min_value = __DBL_MIN__          : 期望数据范围的下限，默认为INT_MIN
		   const double max_value = __DBL_MAX__          : 期望数据范围的上限，默认为INT_MAX
		   const double def_value = DEFAULT_DOUBLE_VALUE ：读不到/读到非法的情况下的默认值，该参数有默认值DEFAULT_DOUBLE_VALUE，分两种情况
																当值是   DEFAULT_DOUBLE_VALUE 时，返回0（值不可信）
																当值不是 DEFAULT_DOUBLE_VALUE 时，令value=def_value并返回1
		   const bool group_is_case_sensitive = false     : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
		   const bool item_is_case_sensitive = false      : 项名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：
  说    明：
***************************************************************************/
int config_file_tools::item_get_double(const char* const group_name, const char* const item_name, double& value,
						const double min_value, const double max_value, const double def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	//若item_name为空串，怎么办？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	int group_index, item_index;
	find_group_item(this, group_name, item_name, group_is_case_sensitive, item_is_case_sensitive, group_index, item_index);
	if (group_index == -1 || item_index == -1 || item_content_list[group_index][item_index][0] == '\0')
	{
		if (abs(def_value - DEFAULT_DOUBLE_VALUE) <= DOUBLE_DELTA)
			return 0;
		value = def_value;
		return 1;
	}

	//int num_of_point = 0;
	//for (int i = 0; item_content_list[group_index][item_index][i] != '\0'; i++)
	//{
	//	if (item_content_list[group_index][item_index][i] < '0' || item_content_list[group_index][item_index][i]>'9')//有非数字字符
	//	{
	//		if(item_content_list[group_index][item_index][i]=='-'&&i==0)
	//			continue;
	//		if (item_content_list[group_index][item_index][i] == '.')
	//		{
	//			if (i != 0 && num_of_point == 0)
	//			{
	//				num_of_point++;
	//				continue;
	//			}
	//		}
	//		if (def_value == DEFAULT_INT_VALUE)
	//			return 0;
	//		value = def_value;
	//		return 1;
	//	}
	//}

	//long double t = strtold(item_content_list[group_index][item_index].c_str(),NULL);//用long double防止溢出
	//if (t > max_value || t < min_value)
	//{
	//	if (def_value == DEFAULT_INT_VALUE)
	//		return 0;
	//	value = def_value;
	//	return 1;
	//}
	//value = (double)t;

	istringstream in(item_content_list[group_index][item_index]);
	in >> value;
	if (in.fail()|| value > max_value || value < min_value)
	{
		if (abs(def_value - DEFAULT_DOUBLE_VALUE) <= DOUBLE_DELTA)
			return 0;
		value = def_value;
		return 1;
	}
	return 1;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：因为工具函数一般在程序初始化阶段被调用，不会在程序执行中被高频次调用，
		   因此这里直接套壳，会略微影响效率，但不影响整体性能（对高频次调用，此方法不适合）
***************************************************************************/
int config_file_tools::item_get_double(const string& group_name, const string& item_name, double& value,
						const double min_value, const double max_value, const double def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 本函数已实现 */
	return this->item_get_double(group_name.c_str(), item_name.c_str(), value, min_value, max_value, def_value, group_is_case_sensitive, item_is_case_sensitive);
}

/***************************************************************************
  函数名称：
  功    能：取某项的内容，返回类型为char * / char []型
  输入参数：const char* const group_name                  ：组名
		   const char* const item_name                   ：项名
		   char *const value                             ：读到的C方式的字符串（返回1时可信，返回0则不可信）
		   const int str_maxlen                          ：指定要读的最大长度（含尾零）
																如果<1则返回空串(不是DEFAULT_CSTRING_VALUE，虽然现在两者相同，但要考虑default值可能会改)
																如果>MAX_STRLEN 则上限为MAX_STRLEN
		   const char* const def_str                     ：读不到情况下的默认值，该参数有默认值DEFAULT_CSTRING_VALUE，分两种情况
																当值是   DEFAULT_CSTRING_VALUE 时，返回0（值不可信）
																当值不是 DEFAULT_CSTRING_VALUE 时，令value=def_value并返回1（注意，不是直接=）
		   const bool group_is_case_sensitive = false : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
		   const bool item_is_case_sensitive = false  : 项名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：
  说    明：1、为简化，未对\"等做转义处理，均按普通字符
		   2、含尾零的最大长度为str_maxlen，调用时要保证有足够空间
		   3、如果 str_maxlen 超过了系统预设的上限 MAX_STRLEN，则按 MAX_STRLEN 取
***************************************************************************/
int config_file_tools::item_get_cstring(const char* const group_name, const char* const item_name, char* const value,
						const int str_maxlen, const char* const def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	//若item_name为空串，怎么办？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	int group_index, item_index;

	if (str_maxlen < 1)//还需不需要后续判断？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
		value[0] = '\0';

	find_group_item(this, group_name, item_name, group_is_case_sensitive, item_is_case_sensitive, group_index, item_index);
	if (group_index == -1 || item_index == -1 || item_content_list[group_index][item_index][0] == '\0')
	{
		if (def_value == DEFAULT_CSTRING_VALUE)
			return 0;
		//strncpy(value, def_value, str_maxlen-1);
		//value[str_maxlen - 1] = '\0';
		strcpy(value, def_value);
		return 1;
	}

	if (str_maxlen >= 1)
	{
		int maxlen = str_maxlen;
		if (str_maxlen > MAX_STRLEN)
			maxlen = MAX_STRLEN;
		istringstream in(item_content_list[group_index][item_index]);
		in.getline(value, maxlen,' ');
		if (in.eof())//in.getline超过最大长度也会使in.fail为1，故不用in.fail判断
		{
			if (def_value == DEFAULT_CSTRING_VALUE)
				return 0;
			//strncpy(value, def_value, str_maxlen-1);
			//value[str_maxlen - 1] = '\0';
			strcpy(value, def_value);
			return 1;
		}
	}
	return 1;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：因为工具函数一般在程序初始化阶段被调用，不会在程序执行中被高频次调用，
		   因此这里直接套壳，会略微影响效率，但不影响整体性能（对高频次调用，此方法不适合）
***************************************************************************/
int config_file_tools::item_get_cstring(const string& group_name, const string& item_name, char* const value,
						const int str_maxlen, const char* const def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)

{
	/* 本函数已实现 */
	return item_get_cstring(group_name.c_str(), item_name.c_str(), value, str_maxlen, def_value, group_is_case_sensitive, item_is_case_sensitive);
}

/***************************************************************************
  函数名称：
  功    能：取某项的内容，返回类型为 string 型
  输入参数：const char* const group_name               ：组名
		   const char* const item_name                ：项名
		   string &value                              ：读到的string方式的字符串（返回1时可信，返回0则不可信）
		   const string &def_value                    ：读不到情况下的默认值，该参数有默认值DEFAULT_STRING_VALUE，分两种情况
															当值是   DEFAULT_STRING_VALUE 时，返回0（值不可信）
															当值不是 DEFAULT_STRING_VALUE 时，令value=def_value并返回1
		   const bool group_is_case_sensitive = false : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
		   const bool item_is_case_sensitive = false  : 项名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：
  说    明：为简化，未对\"等做转义处理，均按普通字符
***************************************************************************/
int config_file_tools::item_get_string(const char* const group_name, const char* const item_name, string& value,
						const string& def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	//若item_name为空串，怎么办？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	int group_index, item_index;
	find_group_item(this, group_name, item_name, group_is_case_sensitive, item_is_case_sensitive, group_index, item_index);
	if (group_index == -1 || item_index == -1 || item_content_list[group_index][item_index][0] == '\0')
	{
		if (def_value == DEFAULT_STRING_VALUE)
			return 0;
		value = def_value;
		return 1;
	}

	istringstream in(item_content_list[group_index][item_index]);
	in >> value;
	//value = item_content_list[group_index][item_index];
	return 1;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：因为工具函数一般在程序初始化阶段被调用，不会在程序执行中被高频次调用，
		   因此这里直接套壳，会略微影响效率，但不影响整体性能（对高频次调用，此方法不适合）
***************************************************************************/
int config_file_tools::item_get_string(const string& group_name, const string& item_name, string& value,
						const string& def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 本函数已实现 */
	return this->item_get_string(group_name.c_str(), item_name.c_str(), value, def_value, group_is_case_sensitive, item_is_case_sensitive);
}

/***************************************************************************
  函数名称：
  功    能：取某项的内容，返回类型为 IPv4 地址的32bit整型（主机序）
  输入参数：const char* const group_name               ：组名
		   const char* const item_name                ：项名
		   unsigned int &value                        ：读到的IP地址，32位整型方式（返回1时可信，返回0则不可信）
		   const unsigned int &def_value              ：读不到情况下的默认值，该参数有默认值DEFAULT_IPADDR_VALUE，分两种情况
															当值是   DEFAULT_IPADDR_VALUE 时，返回0（值不可信）
															当值不是 DEFAULT_IPADDR_VALUE 时，令value=def_value并返回1
		   const bool group_is_case_sensitive = false : 组名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
		   const bool item_is_case_sensitive = false  : 项名是否大小写敏感，true-大小写敏感 / 默认false-大小写不敏感
  返 回 值：
  说    明：
***************************************************************************/
int config_file_tools::item_get_ipaddr(const char* const group_name, const char* const item_name, unsigned int& value,
						const unsigned int& def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 按需完成，根据需要改变return的值 */
	//若item_name为空串，怎么办？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
	int group_index, item_index;
	find_group_item(this, group_name, item_name, group_is_case_sensitive, item_is_case_sensitive, group_index, item_index);
	if (group_index == -1 || item_index == -1 || item_content_list[group_index][item_index][0] == '\0')
	{
		if (def_value == DEFAULT_IPADDR_VALUE)
			return 0;
		value = def_value;
		return 1;
	}

	if (iptonum(item_content_list[group_index][item_index].c_str(), value) !=3)
	{
		if (def_value == DEFAULT_IPADDR_VALUE)
			return 0;
		value = def_value;
		return 1;
	}
	return 1;
}

/***************************************************************************
  函数名称：
  功    能：组名/项目为string方式，其余同上
  输入参数：
  返 回 值：
  说    明：因为工具函数一般在程序初始化阶段被调用，不会在程序执行中被高频次调用，
		   因此这里直接套壳，会略微影响效率，但不影响整体性能（对高频次调用，此方法不适合）
***************************************************************************/
int config_file_tools::item_get_ipaddr(const string& group_name, const string& item_name, unsigned int& value,
						const unsigned int& def_value, const bool group_is_case_sensitive, const bool item_is_case_sensitive)
{
	/* 本函数已实现 */
	return this->item_get_ipaddr(group_name.c_str(), item_name.c_str(), value, def_value, group_is_case_sensitive, item_is_case_sensitive);
}
