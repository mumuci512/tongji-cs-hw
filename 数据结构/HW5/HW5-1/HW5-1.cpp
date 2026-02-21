#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> maxSubsequenceLength(const vector<int>& nums, const vector<int>& queries) {
    vector<int> sortedNums = nums;
    sort(sortedNums.begin(), sortedNums.end()); // 对nums排序，便于按和构建最长子序列

    vector<int> prefixSum(sortedNums.size() + 1, 0);
    for (size_t i = 0; i < sortedNums.size(); ++i) {
        prefixSum[i + 1] = prefixSum[i] + sortedNums[i]; // 构建前缀和
    }

    vector<int> answer;
    for (int query : queries) {
        // 在前缀和中找到满足 prefixSum[k] <= query 的最大 k
        auto it = upper_bound(prefixSum.begin(), prefixSum.end(), query);
        answer.push_back(it - prefixSum.begin() - 1); // 子序列长度为 k
    }

    return answer;
}

int main() {
    int n, m;
    cin >> n >> m;

    vector<int> nums(n);
    for (int i = 0; i < n; ++i) {
        cin >> nums[i];
    }

    vector<int> queries(m);
    for (int i = 0; i < m; ++i) {
        cin >> queries[i];
    }

    vector<int> result = maxSubsequenceLength(nums, queries);

    for (size_t i = 0; i < result.size(); ++i) {
        if (i > 0) cout << " ";
        cout << result[i];
    }
    cout << endl;

    return 0;
}
