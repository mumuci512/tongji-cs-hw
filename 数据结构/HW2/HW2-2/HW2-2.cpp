#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// 函数计算布尔表达式结果
bool evaluateBooleanExpression(const string& expression) {
    stack<bool> operands;
    stack<char> operators;

    auto applyOperator = [](stack<bool>& operands, char op) {
        bool right = operands.top(); operands.pop();
        bool left = operands.top(); operands.pop();
        bool result;
        switch (op) {
            case '&': result = left && right; break;
            case '|': result = left || right; break;
        }
        operands.push(result);
        };

    auto applyNotOperator = [](stack<bool>& operands) {
        bool operand = operands.top(); operands.pop();
        operands.push(!operand);
        };

    for (size_t i = 0; i < expression.size(); ++i) {
        char ch = expression[i];
        if (isspace(ch)) continue; // 跳过空格

        if (ch == 'V') {
            operands.push(true);
        }
        else if (ch == 'F') {
            operands.push(false);
        }
        else if (ch == '(') {
            operators.push(ch);
        }
        else if (ch == ')') {
            while (!operators.empty() && operators.top() != '(') {
                char op = operators.top(); operators.pop();
                if (op == '!') {
                    applyNotOperator(operands);
                }
                else {
                    applyOperator(operands, op);
                }
            }
            operators.pop(); // 移除 '('
        }
        else if (ch == '&' || ch == '|' || ch == '!') {
            while (!operators.empty() &&
                ((ch == '&' || ch == '|') && operators.top() == '!') ||
                ((ch == '|') && operators.top() == '&')) {
                char op = operators.top(); operators.pop();
                if (op == '!') {
                    applyNotOperator(operands);
                }
                else {
                    applyOperator(operands, op);
                }
            }
            operators.push(ch);
        }
    }

    while (!operators.empty()) {
        char op = operators.top(); operators.pop();
        if (op == '!') {
            applyNotOperator(operands);
        }
        else {
            applyOperator(operands, op);
        }
    }

    return operands.top();
}

int main() {
    string line;
    vector<string> expressions;
    int expressionCount = 0;

    // 读取所有表达式直到EOF
    while (getline(cin, line)) {
        expressions.push_back(line);
    }

    for (const string& expression : expressions) {
        bool result = evaluateBooleanExpression(expression);
        cout << "Expression " << ++expressionCount << ": " << (result ? "V" : "F") << endl;
    }

    return 0;
}
