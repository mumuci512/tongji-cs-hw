/* 计科 2350222 盛曦 */
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

struct student {
	int  no;   //学号，不考虑0开头 
	char name[9];  //姓名，最长4个汉字（无生僻字，均为双字节GB汉字） 
	int  score;   //成绩，不考虑小数点 
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
		p = new(nothrow) struct student;
		if (p == NULL)
			return -1;
		infile >> p->no >> p->name >> p->score;
		if (p->no == 9999999)
		{
			delete p;
			break;
		}
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
		//fout << head->no << " " << head->name << " " << head->score << endl;
		cout << head->no << " " << head->name << " " << head->score << endl;
		q = head->next;
		delete head;
		head = q;
	}
	//fout.close();
	return 0;
}