#include <iostream>
#include <fstream>
#include <vector>
#include <climits>
#include <algorithm>

using namespace std;

int kadane(const vector<int>& arr) {
    int max_ending_here = arr[0];
    int max_so_far = arr[0];
    for (size_t i = 1; i < arr.size(); ++i) {
        max_ending_here = max(arr[i], max_ending_here + arr[i]);
        max_so_far = max(max_so_far, max_ending_here);
    }
    return max_so_far;
}

int maxSubmatrix(const vector<vector<int>>& matrix, int n, int p) {
    int max_sum = INT_MIN;
    for (int a = 0; a < n; ++a) {
        vector<int> row_sum(p, 0);
        for (int b = a; b < n; ++b) {
            for (int z = 0; z < p; ++z) {
                row_sum[z] += matrix[b][z];
            }
            int current_max = kadane(row_sum);
            if (current_max > max_sum) {
                max_sum = current_max;
            }
        }
    }
    return max_sum;
}

int main() {
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    int m, n, p;
    fin >> m >> n >> p;

    vector<vector<vector<int>>> cube(m, vector<vector<int>>(n, vector<int>(p)));
    for (int x = 0; x < m; ++x) {
        for (int y = 0; y < n; ++y) {
            for (int z = 0; z < p; ++z) {
                fin >> cube[x][y][z];
            }
        }
    }

    // x轴方向的前缀和
    vector<vector<vector<int>>> sum_x(n, vector<vector<int>>(p, vector<int>(m, 0)));
    for (int y = 0; y < n; ++y) {
        for (int z = 0; z < p; ++z) {
            sum_x[y][z][0] = cube[0][y][z];
            for (int x = 1; x < m; ++x) {
                sum_x[y][z][x] = sum_x[y][z][x - 1] + cube[x][y][z];
            }
        }
    }

    int max_total = INT_MIN;

    for (int i = 0; i < m; ++i) {
        for (int k = i; k < m; ++k) {
            vector<vector<int>> current_sum(n, vector<int>(p, 0));
            for (int y = 0; y < n; ++y) {
                for (int z = 0; z < p; ++z) {
                    if (i == 0) {
                        current_sum[y][z] = sum_x[y][z][k];
                    }
                    else {
                        current_sum[y][z] = sum_x[y][z][k] - sum_x[y][z][i - 1];
                    }
                }
            }
            int current_max = maxSubmatrix(current_sum, n, p);
            if (current_max > max_total) {
                max_total = current_max;
            }
        }
    }

    fout << max_total << endl;

    fin.close();
    fout.close();
    return 0;
}