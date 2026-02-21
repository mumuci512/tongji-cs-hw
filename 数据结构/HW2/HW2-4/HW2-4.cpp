#include <iostream>
using namespace std;

// 队列元素类型为 pair<int, int>，用于存储矩阵坐标
typedef pair<int, int> QElemType;

// 队列类定义
class SqQueue {
private:
    QElemType* base;  // 队列存储的数组
    int front;  // 队头指针
    int rear;   // 队尾指针
    int queueSize;  // 队列的大小

public:
    // 构造函数，初始化队列
    SqQueue(int initSize = 1000) {
        base = new QElemType[initSize];  // 使用 new 分配内存
        front = rear = 0;  // 初始化时队头队尾指针都指向0
        queueSize = initSize;  // 设置初始大小
    }
    // 析构函数，释放队列内存
    ~SqQueue() {
        delete[] base;  // 释放内存
        base = nullptr;
        front = rear = 0;
        queueSize = 0;
    }
    // 判断队列是否为空
    bool isEmpty() const {
        return front == rear;
    }

    // 入队操作，将元素e加入队列
    void enqueue(QElemType e) {
        base[rear] = e;  // 将元素放入队尾
        rear = (rear + 1) % queueSize;  // 队尾指针后移，循环队列
    }

    // 出队操作，将队头元素取出
    void dequeue(QElemType& e) {
        e = base[front];  // 取出队头元素
        front = (front + 1) % queueSize;  // 队头指针后移，循环队列
    }
};

// 动态分配矩阵
int** allocateMatrix(int n, int m) {
    int** matrix = new int* [n];  // 分配 n 个指向 int 类型的指针（行指针）
    for (int i = 0; i < n; i++) {
        matrix[i] = new int[m];  // 为每一行分配 m 个 int 类型的数组
    }
    return matrix;
}

// 动态分配 visited 矩阵
bool** allocateVisited(int n, int m) {
    bool** visited = new bool* [n];  // 分配n个指向bool类型的指针（行指针）
    for (int i = 0; i < n; i++) {
        visited[i] = new bool[m];  // 为每一行分配m个bool类型的数组
        for (int j = 0; j < m; j++) {
            visited[i][j] = false;  // 初始化为false
        }
    }
    return visited;
}

// 释放矩阵的动态内存
void freeMatrix(int** matrix, int n) {
    for (int i = 0; i < n; i++) {
        delete[] matrix[i];  // 释放每一行的内存
    }
    delete[] matrix;  // 释放行指针数组
}

// 释放 visited 的动态内存
void freeVisited(bool** visited, int n) {
    for (int i = 0; i < n; i++) {
        delete[] visited[i];  // 释放每一行的内存
    }
    delete[] visited;  // 释放行指针数组
}

const int dir[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };  // 上下左右方向

// 使用队列进行广度优先搜索（BFS）
int bfs(int x, int y, int** matrix, bool** visited, int n, int m, SqQueue& Q) {
    Q.enqueue({ x, y });
    visited[x][y] = true;
    bool isEdgeRegion = true;  //假定该区域为边缘无效区域bool isEdgeRegion = true

    while (!Q.isEmpty()) {
        QElemType frontElem;
        Q.dequeue(frontElem);//每次循环取出队头元素
        int currX = frontElem.first;
        int currY = frontElem.second;
        if(!(currX == 0 || currX == n - 1 || currY == 0 || currY == m - 1))
            isEdgeRegion = false;//检测到不在矩阵边缘的元素，则将isEdgeRegion置为false
        for (int i = 0; i < 4; i++) { //依次取四个方向
            int newX = currX + dir[i][0];
            int newY = currY + dir[i][1];

            //判断是否有未被标记的1
            if (newX >= 0 && newX < n && newY >= 0 && newY < m && !visited[newX][newY] && matrix[newX][newY] == 1) {
                Q.enqueue({ newX, newY }); //把未被标记的1的坐标放入队列
                visited[newX][newY] = true; //标记当前元素
            }
        }
    }
    return isEdgeRegion;//返回是否为边缘无效区域
}

int main() {
    int n, m;
    cin >> n >> m;

    // 使用 new 动态分配 matrix 和 visited
    int** matrix = allocateMatrix(n, m);
    bool** visited = allocateVisited(n, m);

    // 输入矩阵数据
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> matrix[i][j];
        }
    }

    SqQueue Q;  // 定义队列对象
    int regionCount = 0;  // 记录区域数目

    // 遍历矩阵
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            // 如果找到一个未访问的1，表示找到新的区域
            if (matrix[i][j] == 1 && !visited[i][j]) {
                bool isEdgeRegion=bfs(i, j, matrix, visited, n, m, Q);  // 从该点进行BFS
                if(!isEdgeRegion)
                    regionCount++;; // 区域数加1
            }
        }
    }

    // 输出区域数
    cout << regionCount << endl;

    // 释放动态分配的内存
    freeMatrix(matrix, n);
    freeVisited(visited, n);

    return 0;
}
