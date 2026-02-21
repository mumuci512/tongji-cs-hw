#include <iostream>
#include <stack>
#include <string>
#include <algorithm>

using namespace std;

pair<int, int> longestValidParentheses(const string& s) {
    int maxLength = 0, startIndex = -1; // 初始化最大长度和起始位置
    stack<int> st; // 栈中保存的是括号的索引
    st.push(-1); // 初始化边界，处理从开头的有效括号子串

    for (int i = 0; i < s.length(); ++i) {
        if (s[i] == '(') {
            st.push(i); // 将'('的索引压入栈
        }
        else {
            st.pop(); // 处理')'，尝试匹配之前的'('
            if (st.empty()) {
                st.push(i); // 如果栈为空，记录新的起始点
            }
            else {
                // 计算匹配长度，并更新最大长度和起始位置
                int length = i - st.top();
                if (length > maxLength) {
                    maxLength = length;
                    startIndex = st.top() + 1; // 记录起始位置
                }
            }
        }
    }

    // 如果字符串为空或没有有效括号子串，返回长度0，起始位置0
    return { maxLength, maxLength == 0 ? 0 : startIndex };
}

int main() {
    string s;
    cin >> s;

    pair<int, int> result = longestValidParentheses(s);
    cout << result.first << " " << result.second << endl;

    return 0;
}
