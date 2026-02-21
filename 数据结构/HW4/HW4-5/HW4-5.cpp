#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

struct Course {
    int time;                // 修完该课程所需的学时
    vector<int> prerequisites; // 该课程的前置课程
};

int main() {
    int n;
    cin >> n;

    vector<Course> courses(n + 1);
    vector<int> indegree(n + 1, 0);
    vector<vector<int>> adj(n + 1);

    // 输入课程信息
    for (int i = 1; i <= n; ++i) {
        int ti, ci;
        cin >> ti >> ci;
        courses[i].time = ti;
        for (int j = 0; j < ci; ++j) {
            int prereq;
            cin >> prereq;
            courses[i].prerequisites.push_back(prereq);
            adj[prereq].push_back(i);
            ++indegree[i];
        }
    }

    // 计算最早完成时间
    vector<int> earliest(n + 1, 0);
    queue<int> q;
    for (int i = 1; i <= n; ++i) {
        if (indegree[i] == 0) {
            q.push(i);
            earliest[i] = courses[i].time;
        }
    }

    while (!q.empty()) {
        int curr = q.front();
        q.pop();
        for (int next : adj[curr]) {
            earliest[next] = max(earliest[next], earliest[curr] + courses[next].time);
            if (--indegree[next] == 0) {
                q.push(next);
            }
        }
    }

    // 计算毕业所需的最短时间
    int graduationTime = *max_element(earliest.begin(), earliest.end());

    // 检查学时增加1是否影响毕业时间
    vector<int> results(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        if (earliest[i] + 1 > graduationTime) {
            results[i] = 1;
        }
        else {
            // 模拟增加学时的影响
            int newTime = earliest[i] + 1;
            int newGraduationTime = 0;

            // 更新所有后续节点的最早完成时间
            vector<int> tempEarliest = earliest;
            queue<int> tempQueue;
            tempQueue.push(i);
            tempEarliest[i] = newTime;

            while (!tempQueue.empty()) {
                int curr = tempQueue.front();
                tempQueue.pop();
                for (int next : adj[curr]) {
                    if (tempEarliest[next] < tempEarliest[curr] + courses[next].time) {
                        tempEarliest[next] = tempEarliest[curr] + courses[next].time;
                        tempQueue.push(next);
                    }
                }
            }

            newGraduationTime = *max_element(tempEarliest.begin(), tempEarliest.end());
            if (newGraduationTime > graduationTime) {
                results[i] = 1;
            }
        }
    }

    // 输出结果
    for (int i = 1; i <= n; ++i) {
        cout << earliest[i] << " " << results[i] << endl;
    }

    return 0;
}
