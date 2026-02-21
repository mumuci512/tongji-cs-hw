#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>

using namespace std;

void dfs(int v, const vector<vector<int>>& adjList, vector<bool>& visited, vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int neighbor : adjList[v]) {
        if (!visited[neighbor]) {
            dfs(neighbor, adjList, visited, component);
        }
    }
}

void bfs(int start, const vector<vector<int>>& adjList, vector<bool>& visited, vector<int>& component) {
    queue<int> q;
    visited[start] = true;
    q.push(start);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        component.push_back(v);
        for (int neighbor : adjList[v]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
}

void printComponents(const vector<vector<int>>& components) {
    for (const auto& component : components) {
        cout << "{";
        for (size_t i = 0; i < component.size(); ++i) {
            cout << component[i];
            if (i < component.size() - 1) {
                cout << " ";
            }
        }
        cout << "}";
    }
    cout << endl;
}

int main() {
    int n, m;
    cin >> n >> m;

    vector<vector<int>> adjList(n); // 邻接表
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adjList[u].push_back(v);
        adjList[v].push_back(u); // 无向图需要双向存储
    }

    // 确保邻接表中每个顶点的邻居是有序的
    for (int i = 0; i < n; ++i) {
        sort(adjList[i].begin(), adjList[i].end());
    }

    vector<bool> visited(n, false);
    vector<vector<int>> dfsComponents;

    // 使用DFS找连通分量
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            vector<int> component;
            dfs(i, adjList, visited, component);
            dfsComponents.push_back(component);
        }
    }

    visited.assign(n, false);
    vector<vector<int>> bfsComponents;

    // 使用BFS找连通分量
    for (int i = 0; i < n; ++i) {
        if (!visited[i]) {
            vector<int> component;
            bfs(i, adjList, visited, component); // 修正此处，传递 visited 参数
            bfsComponents.push_back(component);
        }
    }

    // 输出DFS结果
    printComponents(dfsComponents);
    // 输出BFS结果
    printComponents(bfsComponents);

    return 0;
}


