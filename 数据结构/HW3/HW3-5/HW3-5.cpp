#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

// 全局数组来保存每个节点的父节点信息
const int MAXN = 1000;
int parent[MAXN + 1];

int findParent(int x, int y){
    int allParentX[MAXN + 1] = { x };
    int allParentY[MAXN + 1] = { y };
    int n;
    int num_of_parent_x, num_of_parent_y;

    for (num_of_parent_x =1,n = x; parent[n] != -1; num_of_parent_x++) {
		allParentX[num_of_parent_x] = parent[n];
		n = parent[n];
    }
    for (num_of_parent_y = 1, n = y; parent[n] != -1; num_of_parent_y++) {
        allParentY[num_of_parent_y] = parent[n];
        n = parent[n];
    }

	for (int i = 0; i < num_of_parent_x; i++) {
		for (int j = 0; j < num_of_parent_y; j++) {
			if (allParentX[i] == allParentY[j]) {
				return allParentX[i];
			}
		}
	}
}

int main() {
    int T;
    cin >> T;

    for (int t = 1; t <= T; t++) {
        int N, M;
        cin >> N >> M;

        for (int i = 1; i <= N; ++i) { 
            parent[i] = -1;
        }

        // 读取边的信息
        for (int i = 0; i < N - 1; ++i) {
            int a, b;
            cin >> a >> b;
            parent[b] = a;
        }

        for (int i = 0; i < M; i++) {
            int x, y;
            cin >> x >> y;
            cout << findParent(x, y) << endl;
        }
    }

    return 0;
}
