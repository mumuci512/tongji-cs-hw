#include <iostream>
#include <unordered_map>
#include <vector>
#include <queue>
using namespace std;

class Solution {
public:
    int amountOfTime(int n, int start, vector<pair<int, int>>& nodes) {
        // Step 1: Build the adjacency list
        unordered_map<int, vector<int>> edges;
        for (int i = 0; i < n; ++i) {
            int left = nodes[i].first;
            int right = nodes[i].second;

            if (left != -1) {
                edges[i].push_back(left);
                edges[left].push_back(i);
            }
            if (right != -1) {
                edges[i].push_back(right);
                edges[right].push_back(i);
            }
        }

        // Step 2: Perform BFS starting from 'start' node
        unordered_map<int, bool> infected; // Tracks infected nodes
        queue<pair<int, int>> que; // Pair of (node, time)
        que.push({ start, 0 });
        int maxTime = 0;

        while (!que.empty()) {
            int currentNode = que.front().first;
            int time = que.front().second;
            que.pop();

            // Mark the current node as infected
            infected[currentNode] = true;
            maxTime = max(maxTime, time);

            // Spread infection to neighbors
            for (int neighbor : edges[currentNode]) {
                if (!infected[neighbor]) {
                    infected[neighbor] = true;
                    que.push({ neighbor, time + 1 });
                }
            }
        }

        return maxTime;
    }
};

int main() {
    int n, start;
    cin >> n >> start;
    vector<pair<int, int>> nodes(n);

    for (int i = 0; i < n; ++i) {
        int left, right;
        cin >> left >> right;
        nodes[i] = { left, right };
    }

    Solution sol;
    int result = sol.amountOfTime(n, start, nodes);
    cout << result << endl;
    return 0;
}