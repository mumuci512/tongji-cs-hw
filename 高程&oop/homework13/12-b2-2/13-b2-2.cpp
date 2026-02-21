/* 计科 2350222 盛曦 */
#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

struct student {
	int  no;   //学号，不考虑0开头 
	char name[9]; //姓名，最长4个汉字（无生僻字，均为双字节GB汉字） 
	int  score;  //成绩，不考虑小数点 
	int  rank;  //名次 
};

void bubble_sort(struct student* p, int n, int no_or_score)//no_or_score为1，表示排no，为2，表示排score
{
	int i, j;
	struct student t;
	for (i = 0; i < n - 1; i++)
	{
		for (j = 0; j < n - (i + 1); j++)
		{
			if (((no_or_score == 1) && ((p + j)->no > (p + j + 1)->no)) || ((no_or_score == 2) && ((p + j)->score < (p + j + 1)->score)))
			{
				t = *(p + j);
				*(p + j) = *(p + j + 1);
				*(p + j + 1) = t;
			}
		}
	}
}

int main()
{
	unsigned int n;//学生人数

	/*ifstream infile;
	infile.open("student.txt", ios::in);
	if (infile.is_open() == 0) {
		cout << "打开文件失败" << endl;
		return -1;
	}

	infile>>n;*/
	cin >> n;

	struct student* p = new(nothrow) struct student[n];
	if (p == NULL)
	{
		delete p;
		return -1;
	}
	struct student* head = p;//记录这段内存的开始

	unsigned int i;
	for (i = 0; i < n; i++, p++)
		cin >> p->no >> p->name >> p->score;
	/*	infile>>p->no>>p->name>>p->score;

	infile.close();*/

	p = head;
	bubble_sort(p, n, 2);//以成绩排序
	for (i = 0, p->rank = 1; i < n - 1; i++, p++)//计算名次
	{
		if ((p + 1)->score == p->score)
			(p + 1)->rank = p->rank;
		else
			(p + 1)->rank = i + 2;
	}
	p = head;
	bubble_sort(p, n, 1);//以学号排序

	/*cout<<"结果输出到了r.txt里"<<endl;
	ofstream  fout;
	fout.open("r.txt", ios::out);
	if (fout.is_open() == 0) {
		cout << "打开文件失败" << endl;
		return -1;
	}*/
	//fout << "学号输出，c++方式" << endl;
	for (i = 0, p = head; i < n; i++, p++)
		cout << p->no << " " << p->name << " " << p->score << " " << p->rank << endl;
	/*	fout << p->no << " " << p->name << " " << p->score << " " << p->rank << endl;
	fout.close();*/

	delete[]head;
	return 0;
}