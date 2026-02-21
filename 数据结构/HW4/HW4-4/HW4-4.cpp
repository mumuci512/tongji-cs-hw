#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

// 拓扑排序函数
vector<int> topologicalSort(int k, const vector<pair<int, int>>& conditions) {
    vector<int> inDegree(k + 1, 0); // 入度
    vector<vector<int>> graph(k + 1); // 邻接表

    // 构建图
    for (const auto& condition : conditions) {
        int u = condition.first, v = condition.second;
        graph[u].push_back(v);
        inDegree[v]++;
    }

    // 拓扑排序
    queue<int> q;
    for (int i = 1; i <= k; ++i) {
        if (inDegree[i] == 0) q.push(i);
    }

    vector<int> order;
    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        order.push_back(curr);
        for (int neighbor : graph[curr]) {
            inDegree[neighbor]--;
            if (inDegree[neighbor] == 0) q.push(neighbor);
        }
    }

    // 如果排序的结果中没有包含所有节点，说明有环
    if (order.size() != k) return {};
    return order;
}

int main() {
    int k, n, m;
    cin >> k >> n >> m;

    vector<pair<int, int>> rowConditions, colConditions;

    // 读取 rowConditions
    for (int i = 0; i < n; ++i) {
        int above, below;
        cin >> above >> below;
        rowConditions.emplace_back(above, below);
    }

    // 读取 colConditions
    for (int i = 0; i < m; ++i) {
        int left, right;
        cin >> left >> right;
        colConditions.emplace_back(left, right);
    }

    // 进行拓扑排序
    vector<int> rowOrder = topologicalSort(k, rowConditions);
    vector<int> colOrder = topologicalSort(k, colConditions);

    // 如果无法排序，返回 -1
    if (rowOrder.empty() || colOrder.empty()) {
        cout << -1 << endl;
        return 0;
    }

    // 构造位置映射
    unordered_map<int, int> rowPos, colPos;
    for (int i = 0; i < k; ++i) {
        rowPos[rowOrder[i]] = i;
        colPos[colOrder[i]] = i;
    }

    // 构造矩阵
    vector<vector<int>> matrix(k, vector<int>(k, 0));
    for (int i = 1; i <= k; ++i) {
        int r = rowPos[i], c = colPos[i];
        matrix[r][c] = i;
    }

    // 输出矩阵
    for (const auto& row : matrix) {
        for (int i = 0; i < k; ++i) {
            if (i > 0) cout << " ";
            cout << row[i];
        }
        cout << endl;
    }

    return 0;
}
