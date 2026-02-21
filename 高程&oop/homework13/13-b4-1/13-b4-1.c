/* 计科 2350222 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
	int* no;      //学号，不考虑0开头 
	char* name;    //姓名，无生僻字，均为双字节GB汉字 
	int* score;   //成绩，不考虑小数点 
	struct student* next;
};

int main()
{
	struct student* head = NULL, * p = NULL, * q = NULL;

	FILE* infile;
	infile = fopen("list.txt", "r");
	if (infile == NULL) {
		printf("打开文件失败\n");
		return -1;
	}

	int i = 0;//记录申请到了几个节点
	while (1)
	{
		if (i > 0)
			q = p;
		
		//申请整个结构体
		p = (struct student*)malloc(sizeof(struct student));
		if (p == NULL)
			return -1;

		//申请学号
		p->no = (int*)malloc(sizeof(int));
		if (p->no == NULL)
			return -1;
		fscanf(infile, "%d", p->no);

		if (*p->no == 9999999)
		{
			free(p->no);
			free(p);
			break;
		}

		//申请姓名
		char t[9];
		fscanf(infile, "%s", t);
		p->name = (char*)malloc((strlen(t)+1) * sizeof(char));
		if (p->name == NULL)
			return -1;
		strcpy(p->name, t);

		//申请成绩
		p->score = (int*)malloc(sizeof(int));
		if (p->score == NULL)
			return -1;
		fscanf(infile, "%d", p->score);

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
	while (head)
	{
		printf( "%d %s %d\n", *head->no, head->name, *head->score);
		q = head->next;
		free(head->no);
		free(head->name);
		free(head->score);
		free(head);
		head = q;
	}
	//fclose(fout);
	return 0;
}