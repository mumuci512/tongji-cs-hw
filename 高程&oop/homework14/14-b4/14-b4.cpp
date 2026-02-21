/* 2350222 盛曦 计科 */
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
using namespace std;

typedef struct param
{
	//参数名称
	char l;
	char n;
	char t;
	//附加参数个数
	int ln;
	int nn;
	int tn;
	//附加参数上下限
	int l1;
	int l2;
	int n1;
	int n2;
	int t1;
	int t2;
	//附加参数默认值
	int l0;
	int n0;
	int t0;
}PARAM;

int max(int a, int b, int c)
{
	int t = a > b ? a : b;
	return t > c ? t : c;
}

int usage(const char* const procname,PARAM* p)
{
	int w1 = (p->l1==0?0:(int)log10(p->l1))+1+ (p->l2 == 0 ? 0 : (int)log10(p->l2))+1 + 5;
	int w2 = (p->n1 == 0 ? 0 : (int)log10(p->n1)) + 1 + (p->n2 == 0 ? 0 : (int)log10(p->n2)) + 1 + 5;
	int w3 = (p->t1 == 0 ? 0 : (int)log10(p->t1)) + 1 + (p->t2 == 0 ? 0 : (int)log10(p->t2)) + 1 + 5;
	int w = max(w1, w2, w3);//“范围”一栏的宽度

	cout << "Usage: " << procname << " [-"<<p->l<<" 大小][-"<<p->n<<" 数量][-"<<p->t<<"] IP地址" << endl;
	cout << "       =============="<<setw(w)<<setfill('=')<<'='<<"========" << endl;
	cout << "       参数 附加参数 范围"<<setw(w-4)<<setfill(' ')<<' '<<"默认值" << endl;
	cout << "       ==============" << setw(w) << setfill('=') << '=' << "========" << endl;
	cout << setfill(' ') << setiosflags(ios::left);
	cout << "       -" << setw(4) << p->l << setw(9) << p->ln << "[" << p->l1 << ".." << p->l2 << "]" << setw(w - w1+1) << ' ' << p->l0 << endl;
	cout << "       -" << setw(4) << p->n << setw(9) << p->nn << "[" << p->n1 << ".." << p->n2 << "]" << setw(w - w2+1) << ' ' << p->n0 << endl;
	cout << "       -" << setw(4) << p->t << setw(9) << p->tn << "[" << p->t1 << ".." << p->t2 << "]" << setw(w - w3+1) << ' ' << p->t0 << endl;
	cout << "       ==================================" << endl;
	cout << resetiosflags(ios::left);
	return 0;
}

int param(int argc,char** argv,const PARAM* p,int i,int& L,char l_n_t)
{
	int x0, x1, x2,n;//x0:默认值	x1:下限	x2:上限		n:附加参数个数
	switch (l_n_t)
	{
		case'l':
			x0 = p->l0;
			x1 = p->l1;
			x2 = p->l2;
			n = p->ln;
			break;
		case 'n':
			x0 = p->n0;
			x1 = p->n1;
			x2 = p->n2;
			n = p->nn;
			break;
		case 't':
			x0 = p->t0;
			x1 = p->t1;
			x2 = p->t2;
			n = p->tn;
			break;
	}
	int j;

	//判断后续参数个数是否合法
	if(n>0)
	{
		for(j=0;j<n;j++)
		{
			if (argv[i + 1+j][0] == '-' || i + 1+j == argc - 1)
			{
				cout << "参数" << argv[i] << "没有后续参数" << endl;
				return 0;
			}
		}
	}
	else//若不需要附加参数且后面没有附加参数，则置默认值
	{
		return -1;//-1代表出函数后不需要i++
		//if (argv[i + 1][0] == '-' || i + 1 == argc - 1)
		//{
		//	L = x0;
		//	return -1;//-1代表出函数后不需要i++
		//}
		//else
		//{
		//	L = 1;
		//	return 0;
		//}
	}

	//判断后续参数范围是否合法
	for (j = 0; argv[i+1][j] != '\0'; j++)
	{
		if (argv[i + 1][j] < '0' || argv[i + 1][j]>'9')
		{
			L = x0;
			break;
		}
	}
	if (argv[i + 1][j] == '\0')
	{
		L = atoi(argv[i + 1]);
		if (L<x1 || L>x2)
			L = x0;
	}
	return 1;
}

int main(int argc, char** argv)
{
	PARAM p{ 'l','n','t',1,1,0,32,64000,1,1024,0,1,64,4,0 };
	if (argc -1==0)
	{
		usage(argv[0],&p);
		return 0;
	}

	//IP地址判断
	int i,j;
	for (i = 1; i < argc; i++)//先找到ip地址
	{
		if (strchr(argv[i], '.') != NULL)
			break;
	}
	if (i != argc-1)//没有找到IP地址or不是最后一项
	{
		cout << "IP地址错误" << endl;
		return 0;
	}
	int s;//记录每二个点之间的数字
	int dotn=0;//记录点的个数
	for (i = 0; ; i++)
	{
		if ((argv[argc - 1][i] < '0' || argv[argc - 1][i]>'9')&& argv[argc - 1][i]!='.' && argv[argc - 1][i] != '\0')
		{
			cout << "IP地址错误" << endl;
			return 0;
		}
		if (argv[argc - 1][i] == '.'|| argv[argc - 1][i] == '\0')
		{
			for (s = 0, j = i-1; argv[argc - 1][j] != '.'&&j>=0; j--)
				s += (argv[argc - 1][j] - '0') * (int)pow(10, i - j-1);
			if (j == i - 1)//没有执行上方累加循环
			{
				cout << "IP地址错误" << endl;
				return 0;
			}
			if (s < 0 || s>255)
			{
				cout << "IP地址错误" << endl;
				return 0;
			}
			if (argv[argc - 1][i] == '\0')
				break;
			else
				dotn++;
		}
	}
	if (dotn != 3)
	{
		cout << "IP地址错误" << endl;
		return 0;
	}

	//判断其余参数
	int L=p.l0, N=p.n0, T=p.t0;
	if (argv[1][0] != '-'&&argc-1>1)
	{
		cout << "参数" << argv[1] << "不是以-开头的合法参数" << endl;
		return 0;
	}
	for (i = 1; i < argc-1; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strlen(argv[i]) != 2)
			{
				cout << "参数" << argv[i] << "不存在" << endl;
				return 0;
			}
			if(argv[i][1]==p.l)
			{
				if (param(argc, argv, &p, i, L, argv[i][1]) == 0)
					return 0;
				i++;
			}
			else if (argv[i][1] == p.n)
			{
				if (param(argc, argv, &p, i, N, argv[i][1]) == 0)
					return 0;
				i++;
			}
			else if (argv[i][1] == p.t)
			{
				T = 1;
				if (param(argc, argv, &p, i, T, argv[i][1]) == 1)
					i++;
			}
			else
			{
				cout << "参数" << argv[i] << "不存在" << endl;
				return 0;
			}
		}
	}

	cout << "参数检查通过" << endl;
	cout << "-" << p.l <<" 参数：" << L << endl;
	cout << "-" << p.n << " 参数：" << N << endl;
	cout << "-" << p.t << " 参数：" << T << endl;
	cout << "IP地址：" << argv[argc - 1] << endl;
	return 0;
}