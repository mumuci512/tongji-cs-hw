/* 计科 2350222 盛曦 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

struct student {
	int  no;   //学号，不考虑0开头 
	char name[9]; //姓名，最长4个汉字（无生僻字，均为双字节GB汉字） 
	int  score;  //成绩，不考虑小数点 
	int  rank;  //名次 
};

void bubble_sort(struct student* p, int n, int no_score_rank)//no_score_rank为1，表示排no，为2，表示排score,为3，表示rank
{
	int i, j;
	struct student t;
	for (i = 0; i < n - 1; i++)
	{
		for (j = 0; j < n - (i + 1); j++)
		{
			if (((no_score_rank == 1) && ((p + j)->no > (p + j + 1)->no)) || ((no_score_rank == 2) && ((p + j)->score < (p + j + 1)->score))|| ((no_score_rank == 3)&& (((p + j)->rank > (p + j + 1)->rank)|| (((p + j)->rank == (p + j + 1)->rank) && ((p + j)->no > (p + j + 1)->no)))))
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

	/*FILE* infile;
	infile = fopen("student.txt", "r");
	if (infile == NULL) {
		printf("打开文件失败\n");
		return -1;
	}

	fscanf(infile,"%u", &n);*/
	scanf("%u", &n);

	struct student* p = (struct student*)calloc(n, sizeof(struct student));
	if (p == NULL)
	{
		free(p);
		return -1;
	}
	struct student* head = p;//记录这段内存的开始

	unsigned int i;
	for (i = 0; i < n; i++, p++)
		scanf("%d %s %d", &p->no, p->name, &p->score);
	/*	fscanf(infile, "%d %s %d", &p->no, p->name, &p->score);

	fclose(infile);*/

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
	bubble_sort(p, n, 3);//以排名排序

	/*printf("结果输出到了r.txt里\n");
	FILE* fout;
	fout = fopen("r.txt", "w");
	if (fout == NULL) {
		printf("打开文件失败\n");
		return -1;
	}*/
	//fprintf(fout, "名次输出，c方式\n");
	for (i = 0, p = head; i < n; i++, p++)
		printf("%d %s %d %d\n", p->no, p->name, p->score, p->rank);
	/*	fprintf(fout, "%d %s %d %d\n", p->no, p->name, p->score, p->rank);
	fclose(fout);*/

	//free(p);
	free(head);
	return 0;
}