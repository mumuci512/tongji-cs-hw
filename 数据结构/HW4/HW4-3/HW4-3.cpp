#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// 并查集实现
class UnionFind {
public:
    vector<int> parent, rank;

    UnionFind(int n) {
        parent.resize(n + 1);
        rank.resize(n + 1, 0);
        for (int i = 1; i <= n; ++i) {
            parent[i] = i;
        }
    }

    // 查找根节点
    int find(int x) {
        if (x != parent[x]) {
            parent[x] = find(parent[x]); // 路径压缩
        }
        return parent[x];
    }

    // 合并两个集合
    bool unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX == rootY) return false;

        if (rank[rootX] > rank[rootY]) {
            parent[rootY] = rootX;
        }
        else if (rank[rootX] < rank[rootY]) {
            parent[rootX] = rootY;
        }
        else {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
        return true;
    }
};

struct Edge {
    int u, v, weight;
    Edge(int u, int v, int w) : u(u), v(v), weight(w) {}
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

int main() {
    int n;
    cin >> n;

    // 读取村庄之间的距离
    vector<Edge> edges;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            int weight;
            cin >> weight;
            if (i < j) { // 避免重复边
                edges.emplace_back(i, j, weight);
            }
        }
    }

    // 初始化并查集
    UnionFind uf(n);

    // 读取已修建的道路
    int m;
    cin >> m;
    for (int i = 0; i < m; ++i) {
        int a, b;
        cin >> a >> b;
        uf.unite(a, b); // 已修建的道路直接合并
    }

    // 对所有边按权重排序
    sort(edges.begin(), edges.end());

    // Kruskal 算法求最小生成树
    int mstWeight = 0;
    for (const auto& edge : edges) {
        if (uf.unite(edge.u, edge.v)) {
            mstWeight += edge.weight;
        }
    }

    cout << mstWeight << endl;
    return 0;
}


