/* 2350222 盛曦 计科 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

double definite_integration(double (*p)(double),double low,double high,int n)
{
	double step = (high - low) / n;
	double value = 0;
	int i;
	for (i = 0; i <n; i++)
		value += p(high - step * i);
	return value * step;
}

int main()
{
	int n;
	double low, high, value;

	printf("请输入sinxdx的下限、上限及区间划分数量\n");
	scanf("%lf %lf %d", &low, &high, &n);
	value = definite_integration(sin,low,high,n); //sin为math.h中已有函数 
	printf("sinxdx[%g~%g/n=%d] : %g\n", low, high, n, value);

	printf("请输入cosxdx的下限、上限及区间划分数量\n");
	scanf("%lf %lf %d", &low, &high, &n);
	value = definite_integration(cos, low, high, n); //cos为math.h中已有函数 
	printf("cosxdx[%g~%g/n=%d] : %g\n", low, high, n, value);

	printf("请输入e^xdx的下限、上限及区间划分数量\n");
	scanf("%lf %lf %d", &low, &high, &n);
	value = definite_integration(exp, low, high, n); //exp为math.中已有函数 
	printf("e^xdx[%g~%g/n=%d] : %g\n", low, high, n, value);

	return 0;
}