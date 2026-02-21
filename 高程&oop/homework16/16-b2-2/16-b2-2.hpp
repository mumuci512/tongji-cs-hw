/* 2350222 盛曦 计科 */
#include <iostream>
#include <string>
using namespace std;

/* 宏定义的值会在0/1之间任意更改 */
#define OPERATOR_IS_MENBER_FUNCTION		1	//运算符的重载用成员函数方式实现（如果值为0，表示用友元方式实现）

template <typename T, int ROW, int COL>
class matrix {
private:
	T value[ROW][COL];
	//除此之外不允许再添加数据成员
public:
	matrix(const T(*p)[COL]);	//构造函数，用数组初始化矩阵

#if OPERATOR_IS_MENBER_FUNCTION
	//给出+的成员函数实现方式的声明
	matrix operator+(const matrix& m);
#else
	//给出+的友元函数实现方式的声明
	template <typename T1, int ROW1, int COL1>
	friend matrix<T1, ROW1, COL1> operator+(const matrix<T1, ROW1, COL1>& m1, const matrix<T1, ROW1, COL1>& m2);
#endif

	/* ----根据需要添加其它成员函数的定义和友元函数的声明，数量不限---- */
	matrix();
	template <typename T1, int ROW1, int COL1>
	friend ostream& operator<<(ostream& out, const matrix<T1,ROW1,COL1>& m);
	template <typename T1, int ROW1, int COL1>
	friend istream& operator>>(istream& in, matrix<T1, ROW1, COL1>& m);
};

/* ---- 给出成员函数的体外实现/友元函数的实现，其中+的实现有成员和友元两种形式，要求放在下面的预编译中 ---- */
template <typename T, int ROW, int COL>
matrix<T,ROW,COL>::matrix()
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
			value[i][j] = T();//T类型默认构造函数
	}
}

template <typename T, int ROW, int COL>
matrix<T, ROW, COL>::matrix(const T(*p)[COL])
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
			value[i][j] = p[i][j];
	}
}

#if OPERATOR_IS_MENBER_FUNCTION
//给出+的成员函数方式的体外实现
template <typename T, int ROW, int COL>
matrix<T, ROW, COL> matrix<T, ROW, COL>::operator+(const matrix& m)
{
	matrix<T, ROW, COL> temp = m;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
			temp.value[i][j] += value[i][j];
	}
	return temp;
}
#else
//给出+的友元函数方式的体外实现
template <typename T, int ROW, int COL>
matrix<T, ROW, COL> operator+(const matrix<T, ROW, COL>& m1, const matrix<T, ROW, COL>& m2)
{
	matrix<T, ROW, COL> temp = m1;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
			temp.value[i][j] += m2.value[i][j];
	}
	return temp;
}
#endif

template <typename T, int ROW, int COL>
ostream& operator<<(ostream& out, const matrix<T, ROW, COL>& m)
{
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
			out << m.value[i][j] << ' ';
		out << endl;
	}
	return out;
}

template <typename T, int ROW, int COL>
istream& operator>>(istream& in, matrix<T, ROW, COL>& m)
{
	if (in.fail() == 1)
		return in;
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			in >> m.value[i][j];
			if (in.fail() == 1)
			{
				m.value[i][j] = T();
				//根据demo，不需要这两行
				/*in.clear();
				cin.ignore(1410065407, '\n');*/
				return in;
			}
		}
	}
	return in;
}