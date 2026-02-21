#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>

using namespace std;

// 计算距离不超过6的节点数量
int calculateSixDegree(int start, const vector<vector<int>>& adjList, int n) {
    vector<bool> visited(n + 1, false); // 标记节点是否访问
    queue<pair<int, int>> q;           // 队列存储 (节点, 当前距离)
    visited[start] = true;
    q.push({ start, 0 });
    int count = 1;                     // 包含自己

    while (!q.empty()) {
        int node = q.front().first;
        int distance = q.front().second;
        q.pop();

        if (distance == 6) continue;   // 超过6度停止扩展

        for (int neighbor : adjList[node]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push({ neighbor, distance + 1 });
                count++;
            }
        }
    }
    return count;
}

int main() {
    int n, m;
    cin >> n >> m;

    vector<vector<int>> adjList(n + 1); // 邻接表，节点从1到n
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adjList[u].push_back(v);
        adjList[v].push_back(u); // 无向图需要双向存储
    }

    // 对每个节点计算六度空间内的节点占比
    for (int i = 1; i <= n; ++i) {
        int count = calculateSixDegree(i, adjList, n);
        float percentage = (static_cast<float>(count) / n) * 100;
        cout << i << ": " << fixed << setprecision(2) << percentage << "%" << endl;
    }

    return 0;
}

