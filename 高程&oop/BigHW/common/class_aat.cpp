/* 2350222 盛曦 计科 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include "../include/class_aat.h"
//如有必要，可以加入其它头文件
using namespace std;

#if !ENABLE_LIB_COMMON_TOOLS //不使用lib才有效

/* ---------------------------------------------------------------
	 允许加入其它需要static函数（内部工具用）
   ---------------------------------------------------------------- */
static int iptonum(const string ip,u_int& c)
{
	int a[4] = { 0 };
	int flag = 0;
	c = 0;

	for (int j = 0; ip[j] != '\0'; j++)
	{
		if (ip[j] == '.')
		{
			flag++;
			continue;
		}
		if (ip[j] < '0' || ip[j]>'9')//有不是数字的部分
			return 0;
		if (flag > 3)//超过了4段（超过3个点）
			return 1;

		a[flag] = a[flag] * 10 + ip[j] - '0';
	}
	for (int j = 0; j < 4; j++)
	{
		if (a[j] > 255)//数字超上限
			return 2;
		c = c * 256 + a[j];
	}
	return 3;//成功转换
}

static string get_ipaddr_str(const u_int ip)
{
	int a1, a2, a3, a4;
	a1 = (ip >> 24) & 0xff;
	a2 = (ip>> 16) & 0xff;
	a3 = (ip >> 8) & 0xff;
	a4 = ip & 0xff;
	string ipstr = "" + to_string(a1) + "." + to_string(a2) + "." + to_string(a3) + "." + to_string(a4);
	return ipstr; //此句根据需要修改
}

static int num_of_digits(int num)
{
	int count = 0;
	if (num == 0)
	{
		count = 1;
		return count;
	}
	while (num)
	{
		num /= 10;
		count++;
	}
	if (num < 0)
		count++;//加上负号位
	return count;
}

static int num_of_double(double num)
{
	return num_of_digits((int)(num)) + 6 + 1;//加上一个小数点和6位小数
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：null
 ***************************************************************************/
args_analyse_tools::args_analyse_tools()
{
	extargs_type = ST_EXTARGS_TYPE::null;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：boolean
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const bool def)
{
	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	extargs_bool_default = def;
	args_existed = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：int_with_default、int_with_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const int def, const int _min, const int _max)
{
	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	extargs_int_default = def;
	extargs_int_value = def;
	extargs_int_min = _min;
	extargs_int_max = _max;
	args_existed = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：int_with_set_default、int_with_set_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const enum ST_EXTARGS_TYPE type, const int ext_num, const int def_of_set_pos, const int* const set)
{
	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	if (type == ST_EXTARGS_TYPE::int_with_set_default)
	{
		extargs_int_default = *(set + def_of_set_pos);
		extargs_int_value = *(set + def_of_set_pos);
	}
	else
	{
		extargs_int_default = INVALID_INT_VALUE_OF_SET;
		extargs_int_value = INVALID_INT_VALUE_OF_SET;
	}
	extargs_int_set = (int*)set;
	args_existed = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：str、ipaddr_with_default、ipaddr_with_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const string def)
{
	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	extargs_string_default = def;
	extargs_string_value = def;
	iptonum(def, extargs_ipaddr_default);
	extargs_ipaddr_value = extargs_ipaddr_default;
	args_existed = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：str_with_set_default、str_with_set_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const int def_of_set_pos, const string* const set)
{
	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	if(type== ST_EXTARGS_TYPE::str_with_set_default)
	{
		extargs_string_default = *(set + def_of_set_pos);
		extargs_string_value = *(set + def_of_set_pos);
	}
	else
	{
		extargs_string_default = "";
		extargs_string_value = "";
	}
	extargs_string_set = (string*)set;
	args_existed = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：double_with_default、double_with_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const ST_EXTARGS_TYPE type, const int ext_num, const double	def, const double _min, const double _max)
{
	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	extargs_double_default = def;
	extargs_double_value = def;
	extargs_double_min = _min;
	extargs_double_max = _max;
	args_existed = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：double_with_set_default、double_with_set_error
 ***************************************************************************/
args_analyse_tools::args_analyse_tools(const char* name, const enum ST_EXTARGS_TYPE type, const int ext_num, const int def_of_set_pos, const double* const set)
{
	args_name = name;
	extargs_type = type;
	extargs_num = ext_num;
	if (type == ST_EXTARGS_TYPE::double_with_set_default)
	{
		extargs_double_default = *(set + def_of_set_pos);
		extargs_double_value = *(set + def_of_set_pos);
	}
	else
	{
		extargs_double_default = INVALID_DOUBLE_VALUE_OF_SET;
		extargs_double_value = INVALID_DOUBLE_VALUE_OF_SET;
	}
	extargs_double_set = (double*)set;
	args_existed = 0;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：
 ***************************************************************************/
args_analyse_tools::~args_analyse_tools()
{
}

/* ---------------------------------------------------------------
	 允许AAT中自定义成员函数的实现（private）
   ---------------------------------------------------------------- */

const int args_analyse_tools::print_type() const
{
	cout << "类型:";
	switch (extargs_type)
	{
		case ST_EXTARGS_TYPE::boolean:
			cout << "bool";
			break;
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
			cout << "int";
			break;
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
			cout << "double";
			break;
		case ST_EXTARGS_TYPE::str:
		case ST_EXTARGS_TYPE::str_with_set_default:
		case ST_EXTARGS_TYPE::str_with_set_error:
			cout << "string";
			break;
		case ST_EXTARGS_TYPE::ipaddr_with_default:
		case ST_EXTARGS_TYPE::ipaddr_with_error:
			cout << "IP地址";
			break;
		default:
			break;
	}
	return 0;
}

const int args_analyse_tools::print_range_set() const
{
	switch (extargs_type)
	{
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
			cout << "范围[" << extargs_int_min << ".." << extargs_int_max << "]";
			break;
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
			cout << "可取值[";
			for (int* x = extargs_int_set; *x != INVALID_INT_VALUE_OF_SET; x++)
			{
				if (x != extargs_int_set)
					cout << '/';
				cout << *x;
			}
			cout << "]";
			break;
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
			cout << "范围[" << extargs_double_min << ".." << extargs_double_max << "]";
			break;
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
			cout << "可取值[";
			for (double* y = extargs_double_set; *y != INVALID_DOUBLE_VALUE_OF_SET; y++)
			{
				if (y != extargs_double_set)
					cout << '/';
				cout << *y;
			}
			cout << "]";
			break;
		case ST_EXTARGS_TYPE::str_with_set_default:
		case ST_EXTARGS_TYPE::str_with_set_error:
			cout << "可取值[";
			for (string* z = extargs_string_set; z->empty() == false; z++)
			{
				if (z != extargs_string_set)
					cout << '/';
				cout << *z;
			}
			cout << "]";
			break;
		default:
			break;
	}
	return 0;
}

const int args_analyse_tools::print_default() const
{
	switch (extargs_type)
	{
		case ST_EXTARGS_TYPE::boolean:
			cout << "缺省:";
			if (extargs_bool_default)
				cout << "true";
			else
				cout << "false";
			break;
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_set_default:
			cout << "缺省:" << extargs_int_default;
			break;
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_set_default:
			cout << "缺省:" << extargs_double_default;
			break;
		case ST_EXTARGS_TYPE::str:
		case ST_EXTARGS_TYPE::str_with_set_default:
			cout << "缺省:" << extargs_string_default;
			break;
		case ST_EXTARGS_TYPE::ipaddr_with_default:
			cout << "缺省:" << get_str_ipaddr();
			break;
	}
	return 0;
}

const int args_analyse_tools::print_prompt(int type, int range_set, int Default) const
{
	cout << "(";
	int flag = 0;
	if (type)
	{
		print_type();
		flag = 1;
	}
	if(extargs_type!= ST_EXTARGS_TYPE::ipaddr_with_default && extargs_type != ST_EXTARGS_TYPE::ipaddr_with_error && extargs_type != ST_EXTARGS_TYPE::str)
	{
		if (range_set)
		{
			if (flag)
				cout << ", ";
			print_range_set();
			flag = 2;
		}
	}
	if (extargs_type != ST_EXTARGS_TYPE::int_with_error && extargs_type != ST_EXTARGS_TYPE::double_with_error
		&& extargs_type != ST_EXTARGS_TYPE::int_with_set_error && extargs_type != ST_EXTARGS_TYPE::double_with_set_error
		&& extargs_type != ST_EXTARGS_TYPE::str_with_set_error && extargs_type != ST_EXTARGS_TYPE::ipaddr_with_error)
	{
		if(!((extargs_type == ST_EXTARGS_TYPE::str || extargs_type == ST_EXTARGS_TYPE::str_with_set_default)
			&& extargs_string_default.empty() == true))
		{
			if (Default)
			{
				if (flag)
					cout << " ";
				print_default();
			}
		}
	}
	cout << ")" << endl;;
	return 0;
}

int args_analyse_tools::extra_para(const char* const para)
{
	int a[4] = { 0 };
	int flag = 0;
	switch (extargs_type)
	{
		/*case ST_EXTARGS_TYPE::boolean:
			if (strcmp(argv[i + 1], "true") == 0 || strcmp(argv[i + 1], "TRUE") == 0 || strcmp(argv[i + 1], "True") == 0)
				extargs_int_value = 1;
			else if (strcmp(argv[i + 1], "false") == 0 || strcmp(argv[i + 1], "FALSE") == 0 || strcmp(argv[i + 1], "False") == 0)
				extargs_int_value = 0;
			else
			{
				cout << "参数[" << args_name << "]附加参数不是bool型." << endl;
				return -1;
			}
			break;*/
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
			for (int i = 0; para[i] != '\0'; i++)
			{
				if ((para[i] < '0' || para[i]>'9') && para[i] != '-')
				{
					/*if (extargs_type == ST_EXTARGS_TYPE::int_with_default)
						extargs_int_value = extargs_int_default;
					else
					{
						cout << "参数[" << args_name << "]的附加参数不是整数. ";
						print_prompt(1, 1, 1);
						return -1;
					}*/
					cout << "参数[" << args_name << "]的附加参数不是整数. ";
					print_prompt(1, 1, 1);
					return -1;
				}
			}
			extargs_int_value = atoi(para);
			if (get_int() < extargs_int_min || get_int() > extargs_int_max)
			{
				if (extargs_type == ST_EXTARGS_TYPE::int_with_default)
					extargs_int_value = extargs_int_default;
				else
				{
					cout << "参数[" << args_name << "]的附加参数值(" << atoi(para) << ")非法. ";
					print_prompt(1, 1, 0);
					return -1;
				}
			}
			break;
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
			for (int i = 0; para[i] != '\0'; i++)
			{
				if ((para[i] < '0' || para[i]>'9') && para[i] != '-')
				{
					/*if (extargs_type == ST_EXTARGS_TYPE::int_with_set_default)
						extargs_int_value = extargs_int_default;
					else
					{
						cout << "参数[" << args_name << "]的附加参数不是整数. ";
						print_prompt(1, 1, 1);
						return -1;
					}*/
					cout << "参数[" << args_name << "]的附加参数不是整数. ";
					print_prompt(1, 1, 1);
					return -1;
				}
			}
			extargs_int_value = atoi(para);
			int* x;
			for (x = extargs_int_set; *x != INVALID_INT_VALUE_OF_SET; x++)
			{
				if (get_int() == *x)
					break;
			}
			if (*x == INVALID_INT_VALUE_OF_SET)
			{
				if (extargs_type == ST_EXTARGS_TYPE::int_with_set_default)
					extargs_int_value = extargs_int_default;
				else
				{
					cout << "参数[" << args_name << "]的附加参数值(" << para << ")非法. ";
					print_prompt(1, 1, 0);
					return -1;
				}
			}
			break;
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
			for (int i = 0; para[i] != '\0'; i++)
			{
				if ((para[i] < '0' || para[i]>'9')&&para[i]!='.'&&para[i]!='-')
				{
					/*if (extargs_type == ST_EXTARGS_TYPE::double_with_default)
						extargs_double_value = extargs_double_default;
					else
					{
						cout << "参数[" << args_name << "]的附加参数不是浮点数. ";
						print_prompt(1, 1, 1);
						return -1;
					}*/
					cout << "参数[" << args_name << "]的附加参数不是浮点数. ";
					print_prompt(1, 1, 1);
					return -1;
				}
			}
			extargs_double_value = atof(para);
			if (get_double() < extargs_double_min - DOUBLE_DELTA || get_double() > extargs_double_max + DOUBLE_DELTA)
			{
				if (extargs_type == ST_EXTARGS_TYPE::double_with_default)
					extargs_double_value = extargs_double_default;
				else
				{
					cout << "参数[" << args_name << "]的附加参数值(" << para << ")非法. ";
					print_prompt(1, 1, 0);
					return -1;
				}
			}
			break;
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
			for (int i = 0; para[i] != '\0'; i++)
			{
				if ((para[i] < '0' || para[i]>'9') && para[i] != '.' && para[i] != '-')
				{
					/*if (extargs_type == ST_EXTARGS_TYPE::double_with_set_default)
						extargs_double_value = extargs_double_default;
					else
					{
						cout << "参数[" << args_name << "]的附加参数不是浮点数. ";
						print_prompt(1, 1, 1);
						return -1;
					}*/
					cout << "参数[" << args_name << "]的附加参数不是浮点数. ";
					print_prompt(1, 1, 1);
					return -1;
				}
			}
			extargs_double_value = atof(para);
			double* y;
			for (y = extargs_double_set; *y != INVALID_DOUBLE_VALUE_OF_SET; y++)
			{
				if (abs(get_double() - *y) < DOUBLE_DELTA)
					break;
			}
			if (*y == INVALID_DOUBLE_VALUE_OF_SET)
			{
				if (extargs_type == ST_EXTARGS_TYPE::double_with_set_default)
					extargs_double_value = extargs_double_default;
				else
				{
					cout << "参数[" << args_name << "]的附加参数值(" << para << ")非法. ";
					print_prompt(1, 1, 0);
					return -1;
				}
			}
			break;
		case ST_EXTARGS_TYPE::str:
			extargs_string_value = para;
			break;
		case ST_EXTARGS_TYPE::str_with_set_default:
		case ST_EXTARGS_TYPE::str_with_set_error:
			extargs_string_value = para;
			string* z;
			for (z = extargs_string_set; z->empty() == false; z++)
			{
				if (get_string() == *z)
					break;
			}
			if (z->empty())
			{
				if (extargs_type == ST_EXTARGS_TYPE::str_with_set_default)
					extargs_string_value = extargs_string_default;
				else
				{
					cout << "参数[" << args_name << "]的附加参数值(" << para << ")非法. ";
					print_prompt(1, 1, 0);
					return -1;
				}
			}
			break;
		case ST_EXTARGS_TYPE::ipaddr_with_default:
		case ST_EXTARGS_TYPE::ipaddr_with_error:
			if (iptonum(para, extargs_ipaddr_value) != 3)
			{
				if (extargs_type == ST_EXTARGS_TYPE::ipaddr_with_error)
				{
					cout << "参数[" << args_name << "]的附加参数值(" << para << ")非法. ";
					print_prompt(1, 0, 0);
					return -1;
				}
				else
					extargs_ipaddr_value = extargs_ipaddr_default;
			}
			break;
		default:
			break;
	}
	return 0;
}

const int args_analyse_tools::cal_width(u_int& lname, u_int& ltype, u_int& ldefault, u_int& lexists, u_int& lvalue, u_int& lrs, const string* typen) const
{
	u_int tdefault, trs, tvalue = 1;//tvalue默认值是1，因为existed为0时默认是一个斜杠

	if (lname < args_name.length())
		lname = args_name.length();

	if (ltype < typen[(int)extargs_type].length())
		ltype = typen[(int)extargs_type].length();

	switch (extargs_type)
	{
		case ST_EXTARGS_TYPE::boolean:
			if (extargs_bool_default)
				tdefault = 4;//true
			else
				tdefault = 5;//false

			trs = 1;//1个斜杠

			if (existed())
				tvalue = 4;//true
			break;
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
			if (extargs_type == ST_EXTARGS_TYPE::int_with_default)
				tdefault = num_of_digits(extargs_int_default);
			else
				tdefault = 1;//1个斜杠

			trs = num_of_digits(extargs_int_min) + num_of_digits(extargs_int_max) + 4;//上下限的位数加上两个方括号和两个点

			if (existed())
				tvalue = num_of_digits(extargs_int_value);
			break;
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
			if (extargs_type == ST_EXTARGS_TYPE::int_with_set_default)
				tdefault = num_of_digits(extargs_int_default);
			else
				tdefault = 1;//1个斜杠

			trs = 0;
			for (int* x = extargs_int_set; *x != INVALID_INT_VALUE_OF_SET; x++)
				trs += num_of_digits(*x) + 1;//加上一个数字的位数和一个斜杠
			trs--;//减去最后一个斜杠

			if (existed())
				tvalue = num_of_digits(extargs_int_value);
			break;
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
			if (extargs_type == ST_EXTARGS_TYPE::double_with_default)
				tdefault = num_of_double(extargs_double_default);//加上一个小数点和6位小数
			else
				tdefault = 1;//1个斜杠

			//上下限的位数(精确到小数点后6位？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
			trs = num_of_double(extargs_double_min) + num_of_double(extargs_double_max) + 4;

			if (existed())
				tvalue = num_of_double(extargs_double_value);
			break;
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
			if (extargs_type == ST_EXTARGS_TYPE::double_with_set_default)
				tdefault = num_of_double(extargs_double_default);//加上一个小数点
			else
				tdefault = 1;

			//上下限的位数(精确到小数点后6位？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
			trs = 0;
			for (double* y = extargs_double_set; *y != INVALID_DOUBLE_VALUE_OF_SET; y++)
				trs += num_of_double(*y) + 1;//加上一个数字的位数和一个斜杠和一个小数点
			trs--;//减去最后一个斜杠

			if (existed())
				tvalue = num_of_double(extargs_double_value);
			break;
		case ST_EXTARGS_TYPE::str:
			tdefault = extargs_string_default.length();//没有默认串的时候这个值是0吗？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
			if (tdefault == 0)
				tdefault = 1;//1个斜杠

			trs = 1;//1个斜杠

			if (existed())
				tvalue = extargs_string_value.length();
			break;
		case ST_EXTARGS_TYPE::str_with_set_default:
		case ST_EXTARGS_TYPE::str_with_set_error:
			if (extargs_type == ST_EXTARGS_TYPE::str_with_set_default)
			{
				tdefault = extargs_string_default.length();//假如default是空串怎么办，输出斜杠还是什么都不输出？？？？？？？？？？？？？？？？？？？、
				if (tdefault == 0)
					tdefault = 1;
			}
			else
				tdefault = 1;//一个斜杠

			trs = 0;
			for (string* z = extargs_string_set; z->empty() == false; z++)
				trs += (unsigned int)z->length() + 1;//加上一个字符串的长度和一个斜杠
			trs--;//减去最后一个斜杠

			if (existed())
				tvalue = extargs_string_value.length();
			break;
		case ST_EXTARGS_TYPE::ipaddr_with_default:
		case ST_EXTARGS_TYPE::ipaddr_with_error:
			if (extargs_type == ST_EXTARGS_TYPE::ipaddr_with_default)
				tdefault = get_ipaddr_str(extargs_ipaddr_default).length();
			else
				tdefault = 1;//一个斜杠

			trs = 1;

			if (existed())
				tvalue = get_str_ipaddr().length();
			break;
	}
	if (ldefault < tdefault)
		ldefault = tdefault;
	if (lrs < trs)
		lrs = trs;
	if (lvalue < tvalue)
		lvalue = tvalue;
	
	return 0;
}

const int args_analyse_tools::print_form_default(u_int lname, u_int ltype, u_int ldefault, u_int lexists, u_int lvalue, u_int lrs, const string* typen) const
{
	switch (extargs_type)
	{
		case ST_EXTARGS_TYPE::boolean:
			if (extargs_bool_default)
				cout << setw(ldefault + 1) << "true";
			else
				cout << setw(ldefault + 1) << "false";
			break;
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_set_default:
			cout << setw(ldefault + 1) << extargs_int_default;
			break;
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_set_default:
			cout << setw(ldefault + 1) << setiosflags(ios::fixed) << extargs_double_default;
			break;
		case ST_EXTARGS_TYPE::str:
		case ST_EXTARGS_TYPE::str_with_set_default:
			/*if (extargs_string_default.empty())
				cout << setw(ldefault + 1) << "/";
			else*/
			cout << setw(ldefault + 1) << extargs_string_default;
			break;
		case ST_EXTARGS_TYPE::ipaddr_with_default:
			cout << setw(ldefault + 1) << get_ipaddr_str(extargs_ipaddr_default);
			break;
		default:
			cout << setw(ldefault + 1) << "/";
			break;
	}
	return 0;
}

const int args_analyse_tools::print_form_value(u_int lname, u_int ltype, u_int ldefault, u_int lexists, u_int lvalue, u_int lrs, const string* typen) const
{
	if (existed())
	{
		switch (extargs_type)
		{
			case ST_EXTARGS_TYPE::boolean:
				cout << setw(lvalue + 1) << "true";
				break;
			case ST_EXTARGS_TYPE::int_with_default:
			case ST_EXTARGS_TYPE::int_with_error:
			case ST_EXTARGS_TYPE::int_with_set_default:
			case ST_EXTARGS_TYPE::int_with_set_error:
				cout << setw(lvalue + 1) << extargs_int_value;
				break;
			case ST_EXTARGS_TYPE::double_with_default:
			case ST_EXTARGS_TYPE::double_with_error:
			case ST_EXTARGS_TYPE::double_with_set_default:
			case ST_EXTARGS_TYPE::double_with_set_error:
				cout << setw(lvalue + 1) << setiosflags(ios::fixed) << extargs_double_value;
				break;
			case ST_EXTARGS_TYPE::str:
			case ST_EXTARGS_TYPE::str_with_set_default:
			case ST_EXTARGS_TYPE::str_with_set_error:
				/*if (extargs_string_value.empty())
					cout << setw(lvalue + 1) << "/";
				else*/
				cout << setw(lvalue + 1) << extargs_string_value;
				break;
			case ST_EXTARGS_TYPE::ipaddr_with_default:
			case ST_EXTARGS_TYPE::ipaddr_with_error:
				cout << setw(lvalue + 1) << get_str_ipaddr();
				break;
		}
	}
	else
		cout << setw(lvalue + 1) << '/';
	return 0;
}

const int args_analyse_tools::print_form_range_set(u_int lname, u_int ltype, u_int ldefault, u_int lexists, u_int lvalue, u_int lrs, const string* typen) const
{
	string tmp;
	switch (extargs_type)
	{
		case ST_EXTARGS_TYPE::int_with_default:
		case ST_EXTARGS_TYPE::int_with_error:
			tmp = "[" + to_string(extargs_int_min) + ".." + to_string(extargs_int_max) + "]";
			break;
		case ST_EXTARGS_TYPE::int_with_set_default:
		case ST_EXTARGS_TYPE::int_with_set_error:
			tmp = "";
			for (int* x = extargs_int_set; *x != INVALID_INT_VALUE_OF_SET; x++)
				if (x != extargs_int_set)
					tmp += "/" + to_string(*x);
				else
					tmp += to_string(*x);
			break;
			//to_string对double是什么效果？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？？
		case ST_EXTARGS_TYPE::double_with_default:
		case ST_EXTARGS_TYPE::double_with_error:
			tmp = "[" + to_string(extargs_double_min) + ".." + to_string(extargs_double_max) + "]";
			break;
		case ST_EXTARGS_TYPE::double_with_set_default:
		case ST_EXTARGS_TYPE::double_with_set_error:
			tmp = "";
			for (double* x = extargs_double_set; *x != INVALID_DOUBLE_VALUE_OF_SET; x++)
				if (x != extargs_double_set)
					tmp += "/" + to_string(*x);
				else
					tmp += to_string(*x);
			break;
		case ST_EXTARGS_TYPE::str_with_set_default:
		case ST_EXTARGS_TYPE::str_with_set_error:
			tmp = "";
			for (string* z = extargs_string_set; z->empty() == false; z++)
				if (z != extargs_string_set)
					tmp += "/" + *z;
				else
					tmp += *z;
			break;
		default:
			tmp = "/";
			break;
	}
	cout << setw(lrs + 1) << tmp;
	return 0;
}

const int args_analyse_tools::print_form(const u_int lname, const u_int ltype, const u_int ldefault, const u_int lexists, const u_int lvalue, const u_int lrs, const string* typen) const
{
	cout << setfill(' ') << ' ';//输出内容的准备工作
	//name
	cout << setw(lname + 1) << args_name;
	//type
	cout << setw(ltype + 1) << typen[(int)extargs_type];
	//default
	print_form_default(lname, ltype, ldefault, lexists, lvalue, lrs, typen);
	//exists
	cout << setw(lexists + 1) << existed();
	//value
	print_form_value(lname, ltype, ldefault, lexists, lvalue, lrs, typen);
	//range/set
	print_form_range_set(lname, ltype, ldefault, lexists, lvalue, lrs, typen);

	cout << endl;
	return 0;
}
/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const string args_analyse_tools::get_name() const
{
	return this->args_name;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：加!!后，只能是0/1
			已实现，不要动
 ***************************************************************************/
const int args_analyse_tools::existed() const
{
	return !!this->args_existed;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const int args_analyse_tools::get_int() const
{
	return this->extargs_int_value;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const double args_analyse_tools::get_double() const
{
	return this->extargs_double_value;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const string args_analyse_tools::get_string() const
{
	return this->extargs_string_value;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：已实现，不要动
 ***************************************************************************/
const unsigned int args_analyse_tools::get_ipaddr() const
{
	return this->extargs_ipaddr_value;
}

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：将 extargs_ipaddr_value 的值从 0x7f000001 转为 "127.0.0.1"
 ***************************************************************************/
const string args_analyse_tools::get_str_ipaddr() const
{
	return get_ipaddr_str(this->extargs_ipaddr_value);
}


/***************************************************************************
  函数名称：
  功    能：
  输入参数：follow_up_args：是否有后续参数
			0  ：无后续参数
			1  ：有后续参数
  返 回 值：
  说    明：友元函数
***************************************************************************/
int args_analyse_process(const int argc, const char* const *const argv, args_analyse_tools* const args, const int follow_up_args)
{
	int i;
	args_analyse_tools* p;

	for (i = 1; i <= argc - 1; i++)
	{
		if (strlen(argv[i])>=2&&argv[i][0] == '-' && argv[i][1] == '-')//为可变参数
		{
			for (p = args; (p->args_name != argv[i]) && p->extargs_type != ST_EXTARGS_TYPE::null; p++)
				;
			if (p->args_name == argv[i])//为合法的可变参数（会人为保证均以--开头）
			{
				if (p->existed() == 0)
					p->args_existed = 1;
				else
				{
					cout << "参数[" << p->args_name << "]重复." << endl;
					return -1;
				}

				if (p->extargs_num == 1)//有一个额外参数
				{
					if (i == argc - 1)//已经是最后一个参数
					{
						cout << "参数[" << p->args_name << "]的附加参数不足. ";
						p->print_prompt(1, 1, 1);
						return -1;
					}
					if (argv[i + 1][0] == '-' && argv[i + 1][1] == '-')//后面直接跟了另一个参数
					{
						cout << "参数[" << p->args_name << "]缺少附加参数. ";
						p->print_prompt(1, 1, 1);
						return -1;
					}

					//处理额外参数
					if(p->extra_para(argv[i + 1])!=0)
						return -1;
					i++;
					continue;
				}//end of 有附加参数的判断
			}//end of 为合法的可变参数（--开头且在列表中）
			else
			{
				cout << "参数[" << argv[i] << "]非法." << endl;
				return -1;
			}
		}//end of为可变参数（--开头）
		else//为固定参数
		{
			if (follow_up_args)
			{
				//if (follow_up_args!=argc-i)//需要的固定参数和实际给出的不一样
				//{
				//	cout << "除可变参数外，还给出了"<< argc - i<<"个固定参数，但本程序只需要一个固定参数" << endl;
				//	return -1;
				//}
				return i;
			}
			else
			{
				cout << "参数[" << argv[i] << "]格式非法(不是--开头的有效内容)." << endl;
				return -1;
			}
		}
	}//end of for (i = 1; i <= argc - 1; i++)
	//i=argc
	return i; //此句根据需要修改
}


/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：友元函数
***************************************************************************/
int args_analyse_print(const args_analyse_tools* const args)
{
	u_int lname = strlen("name"), ltype = strlen("type"), ldefault = strlen("default");
	u_int lexists = strlen("exists"), lvalue = strlen("value"), lrs = strlen("range/set");//lrs默认值是1的原因是有些类型的range/set没有，只需要打印一个斜杠
	args_analyse_tools* p = (args_analyse_tools*)args;
	string typen[15] = {"null",
						"Bool",
						"IntWithDefault",
						"IntWithError",
						"IntSETWithDefault",
						"IntSETWithError",
						"DoubleWithDefault",
						"DoubleWithError",
						"DoubleSETWithDefault",
						"DoubleSETWithError",
						"String",
						"StringSETWithDefault",
						"StringSETWithError",
						"IPAddrWithDefault",
						"IPAddrWithError",
	};
	//int rs_len[15] = { 0 };//1-15（共14个）ST_EXTARGS_TYPE类型的所对应的rs的长度
	
	for (;p->extargs_type != ST_EXTARGS_TYPE::null; p++)
		p->cal_width(lname, ltype, ldefault, lexists, lvalue, lrs, typen);

	int l = lname + ltype + ldefault + lexists + lvalue + lrs + 7;//7个空格
	

	//输出
	
	//输出表头
	cout << setw(l) << setfill('=') << "=" << endl;//第一行=
	cout << ' ' << setfill(' ') << setiosflags(ios::left);//输出表头的准备工作
	cout<<setw(lname + 1) << "name" << setw(ltype+1) << "type" << setw(ldefault+1) << "default" << setw(lexists+1) << "exists" << setw(lvalue+1) << "value" << setw(lrs+1) << "range/set" << endl;
	cout << setw(l) << setfill('=') << "=" << endl;//第二行=

	//输出内容
	for (p = (args_analyse_tools*)args; p->extargs_type != ST_EXTARGS_TYPE::null; p++)
		p->print_form(lname, ltype, ldefault, lexists, lvalue, lrs, typen);
	
	//输出表尾
	cout << setw(l) << setfill('=') << "=" << endl;//最后一行=
	cout << endl;

	//cout << resetiosflags(ios::left)<<setfill(' ');//恢复默认格式
	cout << resetiosflags(ios::fixed);
	return 0; //此句根据需要修改
}
#endif // !ENABLE_LIB_COMMON_TOOLS
