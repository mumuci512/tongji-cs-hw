/* 计科 2350222 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <time.h>
//#include <windows.h>
using namespace std;

struct student {
	int  no;   //报名号 
	char* name;  //姓名
	char*  school;   //毕业学校
	struct student* next;
};

int main()
{
	srand((unsigned int)(time(0)));

	int n, m;//招生人数，报名总人数,n<=m

	ifstream infile;
	infile.open("stulist.txt", ios::in);
	if (infile.is_open() == 0) {
		cout << "打开文件失败" << endl;
		return -1;
	}

	infile >> n >> m;

	struct student* head = NULL, * p = NULL, * q = NULL;
	int i;//记录申请到了几个节点
	for(i=0;i<m;i++)
	{
		if (i > 0)
			q = p;

		//申请整个结构体
		p = new(nothrow) struct student;
		if (p == NULL)
			return -1;

		if (i == 0)
			head = p;
		else
			q->next = p;

		cout << "请输入第" << i + 1 << "个学生的基本信息" << endl;

		//读取报名号
		infile >> p->no;

		//申请姓名
		char t[20];
		infile >> t;
		p->name = new(nothrow) char[strlen(t) + 1];
		if (p->name == NULL)
			return -1;
		strcpy(p->name, t);

		//申请成绩
		char t2[20];
		infile >> t2;
		p->school = new(nothrow) char[strlen(t2)+1];
		if (p->school == NULL)
			return -1;
		strcpy(p->school, t2);

		p->next = NULL;
	}
	infile.close();

	int j,ran;
	int* pselect = new(nothrow) int[m] {0};//用于记录已经选中的学生，0表示未选中，1表示选中
	if (pselect == NULL)
	{
		delete[] pselect;
		return -1;
	}
	for (j = 0; j < n; j++)//抽取n个人
	{
		do
		{
			ran = rand() % m;
		} while (pselect[ran] == 1);
		pselect[ran] = 1;
	}

	cout << "随机抽中的学生输出在result.txt中" << endl;
	ofstream  fout;
	fout.open("result.txt", ios::out);
	if (fout.is_open() == 0) {
		cout << "打开文件失败" << endl;
		return -1;
	}

	for (i = 0; i < m; i++)
	{
		if (pselect[i] == 1)
		{
		/*	fout << (head + i)->no << " " << (head + i)->name << " " << (head + i)->school << endl;*/
			struct student* temp = head;
			for (int k = 0; k <i; k++)
				temp = temp->next;
			fout << temp->no << " " << temp->name << " " << temp->school << endl;
		}
	}

	fout.close();

	while (head)
	{
		q = head->next;
		delete []head->name;
		delete []head->school;
		delete head;
		head = q;
	}
	delete []pselect;
	return 0;
}
/*
应该是申请连续空间当数组用，还是申请链式空间？那种方法好？
申请链式空间更好。
因为每个学生的名字和学校都是字符串，长度不一样，申请链式空间意味着可以改变申请到的每个结构体的大小，可以按需申请名字和学校的空间。
假如申请连续空间当数组用，那么必须预知名字和学校的最大长度，按最大长度申请，会浪费许多空间，虽然可以节省指针域的空间，但是这个节省微不足道。
*/