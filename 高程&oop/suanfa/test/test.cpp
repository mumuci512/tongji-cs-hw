#include <iostream>
#include <vector>
#include <queue>
using namespace std;

template<class Type>
class EdgeNode {
public:
    Type weight;
    int u, v;
    bool operator>(const EdgeNode& other) const {
        return weight > other.weight;
    }
};

class UnionFind {
private:
    vector<int> parent;
public:
    UnionFind(int n) {
        parent.resize(n);
        for (int i = 0; i < n; ++i)
            parent[i] = i;
    }

    int Find(int x) {
        if (parent[x] != x)
            parent[x] = Find(parent[x]);
        return parent[x];
    }

    void Union(int x, int y) {
        int fx = Find(x);
        int fy = Find(y);
        if (fx != fy) parent[fy] = fx;
    }
};

template<class Type>
class Solution {
public:
    bool Kruskal(int n, int e, EdgeNode<Type> E[], EdgeNode<Type> T[]) {
        priority_queue<EdgeNode<Type>, vector<EdgeNode<Type>>, greater<>> H(E, E + e);

        UnionFind U(n);
        int k = 0;

        while (!H.empty() && k < n - 1) {
            EdgeNode<Type> x = H.top();
            H.pop();
            e--;

            int a = U.Find(x.u);
            int b = U.Find(x.v);
            if (a != b) {
                T[k++] = x;
                U.Union(a, b);
            }
        }

        return (k == n - 1);
    }
};
int main() {
    const int n = 4; // 顶点数
    const int e = 5; // 边数
    EdgeNode<int> edges[e] = {
        {1, 0, 1},
        {4, 0, 2},
        {2, 1, 2},
        {6, 1, 3},
        {3, 2, 3}
    };
    EdgeNode<int> mst[n - 1];

    Solution<int> sol;
    if (sol.Kruskal(n, e, edges, mst)) {
        cout << "MST 边集合如下：" << endl;
        for (int i = 0; i < n - 1; ++i)
            cout << mst[i].u << " - " << mst[i].v << " : 权重 = " << mst[i].weight << endl;
    }
    else {
        cout << "无法构造连通图" << endl;
    }

    return 0;
}