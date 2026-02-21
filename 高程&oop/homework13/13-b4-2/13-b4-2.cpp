/* 计科 2350222 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
using namespace std;

struct student {
	int* no;      //学号，不考虑0开头 
	char* name;    //姓名，无生僻字，均为双字节GB汉字 
	int* score;   //成绩，不考虑小数点 
	struct student* next;
};

int main()
{
	struct student* head = NULL, * p = NULL, * q = NULL;

	ifstream infile;
	infile.open("list.txt", ios::in);
	if (infile.is_open() == 0) {
		cout << "打开文件失败" << endl;
		return -1;
	}

	int i = 0;//记录申请到了几个节点
	while (1)
	{
		if (i > 0)
			q = p;

		//申请整个结构体
		p = new(nothrow) struct student;
		if (p == NULL)
			return -1;

		//申请学号
		p->no = new(nothrow) int;
		if (p->no == NULL)
			return -1;
		infile >> *p->no;

		if (*p->no == 9999999)
		{
			delete p->no;
			delete p;
			break;
		}

		//申请姓名
		char t[9];
		infile >> t;
		p->name = new(nothrow) char[strlen(t) + 1];
		if (p->name == NULL)
			return -1;
		strcpy(p->name, t);

		//申请成绩
		p->score = new(nothrow) int;
		if (p->score == NULL)
			return -1;
		infile >> *p->score;

		if (i == 0)
			head = p;
		else
			q->next = p;
		p->next = NULL;
		i++;
	}
	infile.close();

	/*cout << "结果输出到了r.txt里" << endl;
	ofstream  fout;
	fout.open("r.txt", ios::out);
	if (fout.is_open() == 0) {
		cout << "打开文件失败" << endl;
		return -1;
	}*/
	while (head)
	{
		cout << *head->no << " " << head->name << " " << *head->score << endl;
		q = head->next;
		delete head->no;
		delete []head->name;
		delete head->score;
		delete head;
		head = q;
	}
	//fout.close();
	return 0;
}