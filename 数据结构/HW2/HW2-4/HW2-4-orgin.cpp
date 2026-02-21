#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// 定义四个方向：上、下、左、右
int directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

void bfs(int x, int y, vector<vector<int>>& matrix, vector<vector<bool>>& visited, int n, int m) {
    queue<pair<int, int>> q;
    q.push({ x, y });
    visited[x][y] = true;

    while (!q.empty()) {
        pair<int, int> current = q.front();
        int cx = current.first;
        int cy = current.second;

        q.pop();

        for (int i = 0; i < 4; i++) {
            int nx = cx + directions[i][0];
            int ny = cy + directions[i][1];

            // 检查是否在边界内，并且该位置是1且未访问
            if (nx >= 0 && nx < n && ny >= 0 && ny < m && matrix[nx][ny] == 1 && !visited[nx][ny]) {
                visited[nx][ny] = true;
                q.push({ nx, ny });
            }
        }
    }
}

int countRegions(vector<vector<int>>& matrix, int n, int m) {
    vector<vector<bool>> visited(n, vector<bool>(m, false));
    int regionCount = 0;

    // 遍历每个位置，若是1且未访问，则进行一次BFS/DFS
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (matrix[i][j] == 1 && !visited[i][j]) {
                // 判断该区域是否完全位于内部
                bool isValidRegion = true;
                queue<pair<int, int>> q;
                q.push({ i, j });
                visited[i][j] = true;

                while (!q.empty()) {
                    pair<int, int> current = q.front();
                    int cx = current.first;
                    int cy = current.second;
                    q.pop();

                    for (int k = 0; k < 4; k++) {
                        int nx = cx + directions[k][0];
                        int ny = cy + directions[k][1];

                        if (nx >= 0 && nx < n && ny >= 0 && ny < m && matrix[nx][ny] == 1 && !visited[nx][ny]) {
                            visited[nx][ny] = true;
                            q.push({ nx, ny });
                        }

                        // 如果该位置是边界上的1，则该区域不计入
                        if (nx < 0 || nx >= n || ny < 0 || ny >= m) {
                            isValidRegion = false;
                        }
                    }
                }

                // 如果这个区域是有效的，则计数
                if (isValidRegion) {
                    regionCount++;
                }
            }
        }
    }

    return regionCount;
}

int main() {
    int n, m;
    cin >> n >> m;

    vector<vector<int>> matrix(n, vector<int>(m));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> matrix[i][j];
        }
    }

    int result = countRegions(matrix, n, m);
    cout << result << endl;

    return 0;
}
