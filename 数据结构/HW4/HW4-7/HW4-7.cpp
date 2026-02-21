#include <iostream>
#include <vector>
#include <stack>

using namespace std;

const int N = 6; //顶点数
int map[N][N] = {
    {0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 1},
    {0, 1, 0, 1, 0, 1},
    {0, 1, 1, 0, 1, 1},
    {0, 0, 0, 1, 0, 1},
    {0, 1, 1, 1, 1, 0}
};
vector<int> path; //存储欧拉路径的向量

//查找欧拉路径的函数
void findEulerPath(int start) {
    stack<int> stk; //用栈来存储顶点
    stk.push(start); //将起始顶点压入栈中

    while (!stk.empty()) {
        int u = stk.top(); //获取栈顶元素
        bool found = false; //标记是否找到相邻的未访问边

        //遍历所有顶点，寻找与当前顶点相连的边
        for (int v = 1; v < N; v++) {
            if (map[u][v] == 1) { //如果找到一条边
                map[u][v] = map[v][u] = 0; //删除边
                stk.push(v); //将相邻顶点压入栈中
                found = true; //标记找到边
                break; //退出循环
            }
        }

        if (!found) { //如果没有找到相邻的未访问边
            path.push_back(u); //将当前顶点加入路径
            stk.pop(); //弹出栈顶元素
        }
    }
}

int main() {
    int start = 1; //从顶点1开始
    findEulerPath(start); //查找欧拉路径

    //输出欧拉路径
    for (int i = path.size() - 1; i >= 0; i--) {
        cout << path[i];
    }

    return 0;
}
