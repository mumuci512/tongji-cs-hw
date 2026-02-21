/* 计科 2350222 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

struct student {
	int  no;   //学号，不考虑0开头 
	char name[9];  //姓名，最长4个汉字（无生僻字，均为双字节GB汉字） 
	int  score;   //成绩，不考虑小数点 
	struct student* next;
};

int main()
{
	struct student *head = NULL,*p=NULL,*q=NULL;

	FILE* infile;
	infile = fopen("list.txt", "r");
	if (infile == NULL) {
		printf("打开文件失败\n");
		return -1;
	}

	int i = 0;//记录申请到了几个节点
	while(1)
	{
		if (i > 0)
			q = p;
		p = (struct student*)malloc(sizeof(struct student));
		if (p == NULL)
			return -1;
		fscanf(infile,"%d %s %d", &p->no, p->name, &p->score);
		if (p->no == 9999999)
		{
			free(p);
			break;
		}
		if (i == 0)
			head = p;
		else
			q->next = p;
		p->next = NULL;
		i++;
	}
	fclose(infile);

	/*printf("结果输出到了r.txt里\n");
	FILE* fout;
	fout = fopen("r.txt", "w");
	if (fout == NULL) {
		printf("打开文件失败\n");
		return -1;
	}*/
	while(head)
	{
		printf( "%d %s %d\n", head->no, head->name, head->score);
		q = head->next;
		free(head);
		head = q;
	}
	//fclose(fout);
	return 0;
}