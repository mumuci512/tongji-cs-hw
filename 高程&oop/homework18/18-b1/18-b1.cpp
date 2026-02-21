/* 2350222 盛曦 计科 */
#include <iostream>
#include <cmath>
using namespace std;

class Shape {
protected:
    //根据需要加入相应的成员，也可以为空
public:
    virtual void ShapeName() = 0; //此句不准动
    //根据需要加入相应的成员，也可以为空
	virtual double area() const = 0; //此句不准动
};

//此处给出五个类的定义及实现(成员函数采用体外实现形式)
const double pi = 3.14159;
//const double eps = 1e-6;

class Circle : public Shape 
{
private:
	double radius;
public:
    Circle(double r);
    virtual void ShapeName();
    virtual double area() const;
};
Circle::Circle(double r)
{
	radius = r;
}
void Circle::ShapeName()
{
	cout << "Circle" << endl;
}
double Circle::area() const
{
	if (radius <= 0)
		return 0;
	else
		return pi * radius * radius;
}

class Square : public Shape
{
private:
	double side;
public:
	Square(double s);
	virtual void ShapeName();
	virtual double area() const;
};
Square::Square(double s)
{
	side = s;
}
void Square::ShapeName()
{
	cout << "Square" << endl;
}
double Square::area() const
{
	if (side <= 0)
		return 0;
	else
		return side * side;
}

class Rectangle : public Shape
{
private:
	double length, width;
public:
	Rectangle(double l, double w);
	virtual void ShapeName();
	virtual double area() const;
};
Rectangle::Rectangle(double l, double w)
{
	length = l;
	width = w;
}
void Rectangle::ShapeName()
{
	cout << "Rectangle" << endl;
}
double Rectangle::area() const
{
	if (length <= 0 || width <= 0)
		return 0;
	else
		return length * width;
}

class Trapezoid : public Shape
{
private:
	double upper, lower, height;
public:
	Trapezoid(double u, double l, double h);
	virtual void ShapeName();
	virtual double area() const;
};
Trapezoid::Trapezoid(double u, double l, double h)
{
	upper = u;
	lower = l;
	height = h;
}
void Trapezoid::ShapeName()
{
	cout << "Trapezoid" << endl;
}
double Trapezoid::area() const
{
	if (upper <= 0 || lower <= 0 || height <= 0)
		return 0;
	else
		return (upper + lower) * height / 2;
}

class Triangle : public Shape
{
private:
	double a, b, c;
public:
	Triangle(double x, double y, double z);
	virtual void ShapeName();
	virtual double area() const;
};
Triangle::Triangle(double x, double y, double z)
{
	a = x;
	b = y;
	c = z;
}
void Triangle::ShapeName()
{
	cout << "Triangle" << endl;
}
double Triangle::area() const
{
	if (a <= 0 || b <= 0 || c <= 0 || a + b <= c || a + c <= b || b + c <= a)
		return 0;
	else
	{
		double p = (a + b + c) / 2;
		return sqrt(p * (p - a) * (p - b) * (p - c));
	}
}

/* -- 替换标记行 -- 本行不要做任何改动 -- 本行不要删除 -- 在本行的下面不要加入任何自己的语句，作业提交后从本行开始会被替换 -- 替换标记行 -- */

/***************************************************************************
  函数名称：
  功    能：
  输入参数：
  返 回 值：
  说    明：给出的是main函数的大致框架，允许进行微调或改变初值
***************************************************************************/
int main()
{
    if (1) {
        Circle    c1(5.2);           //半径（如果<=0，面积为0）
        Square    s1(5.2);           //边长（如果<=0，面积为0）
        Rectangle r1(5.2, 3.7);      //长、宽（如果任一<=0，面积为0）
        Trapezoid t1(2.3, 4.4, 3.8); //上底、下底、高（如果任一<=0，面积为0）
        Triangle  t2(3, 4, 5);       //三边长度（如果任一<=0或不构成三角形，面积为0）
        Shape* s[5] = { &c1, &s1, &r1, &t1, &t2 };

        int   i;
        for (i = 0; i < 5; i++) {
            s[i]->ShapeName(); //分别打印不同形状图形的名称（格式参考demo）
            cout << s[i]->area() << endl; //分别打印不同形状图形的面积（格式参考demo）
            cout << endl;
        }
    }

    if (1) {
        Circle    c1(-1);           //半径（如果<=0，面积为0）
        Square    s1(-1);           //边长（如果<=0，面积为0）
        Rectangle r1(5.2, -1);      //长、宽（如果任一<=0，面积为0）
        Trapezoid t1(2.3, -1, 3.8); //上底、下底、高（如果任一<=0，面积为0）
        Triangle  t2(3, 4, -1);       //三边长度（如果任一<=0或不构成三角形，面积为0）
        Shape* s[5] = { &c1, &s1, &r1, &t1, &t2 };

        cout << "============" << endl;
        int   i;
        for (i = 0; i < 5; i++) {
            s[i]->ShapeName(); //分别打印不同形状图形的名称（格式参考demo）
            cout << s[i]->area() << endl; //分别打印不同形状图形的面积（格式参考demo）
            cout << endl;
        }
    }

    return 0;
}
