from kanren import run, var, lall,membero
from kanren.goals import goalify

N=8

# 因为kanren库中没有找到不等于(neq)的goal constructors，所以自己定义一个
def neq(a,b):
    return a!=b

# 无法直接对var变量进行计算，因此只能通过函数来判断两个皇后是否在斜线上
'''
* @brief 判断第i、j行的皇后是否在斜线上
* @param q1：queen[i],即第i行的纵坐标
* @param q2：queen[j],即第j行的纵坐标
* @param i：第i行
* @param j：第j行
* @return true:在同一条写线上 false:不在同一条斜线上
'''
def is_diagonal(q1,q2,i,j):
    return abs(q1-q2)==abs(i-j)

# 每个元素的值用于记录8个皇后的纵坐标；每个元素的下标为横坐标，范围在0-7，互不相同
queens=[var() for _ in range(N)]

'''
* @brief 求出八皇后问题解
* @param
* @return 八皇后问题解
'''
def eight_queens():
    # 列表queens内的8个元素应符合相同约束，因此使用列表组合这些重复性的约束条件，相当于逻辑与（lall）

    # 每个皇后的纵坐标在0-7之间
    constraints_domain=[membero(q,range(N))for q in queens]
    # 任意两个皇后的纵坐标不能相等
    constraints_col_neq=[(goalify(neq),(queens[i],queens[j]),True) for i in range(N) for j in range(i+1,N)]  
    # 任意两个皇后不能在同一斜线上
    constraints_diagonal_neq = [(goalify(is_diagonal),(queens[i],queens[j],i,j),False)for i in range(N) for j in range(i+1,N)]   
    
    # 组合所有约束
    constraints=constraints_domain+constraints_col_neq+constraints_diagonal_neq

    # 返回结果
    return run(0,queens,lall(*constraints))

# 输出结果
print(eight_queens())