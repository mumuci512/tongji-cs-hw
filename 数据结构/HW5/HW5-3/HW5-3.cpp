#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

class Solution {
public:
    int solve(vector<vector<string>>& old_chart, vector<vector<string>>& new_chart) {
        int n = old_chart.size(), m = old_chart[0].size();
        unordered_map<string, pair<int, int>> position_map;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                position_map[old_chart[i][j]] = make_pair(i, j);
            }
        }

        int swaps = 0;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                while (old_chart[i][j] != new_chart[i][j]) {
                    swaps++;
                    pair<int, int> pos = position_map[new_chart[i][j]];
                    int x = pos.first;
                    int y = pos.second;
                    swap(old_chart[i][j], old_chart[x][y]);

                    position_map[old_chart[x][y]] = make_pair(x, y);
                    position_map[old_chart[i][j]] = make_pair(i, j);
                }
            }
        }

        return swaps;
    }
};
