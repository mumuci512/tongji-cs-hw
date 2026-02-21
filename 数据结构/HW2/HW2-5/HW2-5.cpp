#include <iostream>
#include <deque>
#include <queue>
#include <string>

using namespace std;

class MaxQueue {
private:
    queue<int> q;            // 用于存储实际的队列元素
    deque<int> maxDeque;     // 用于维护最大值的辅助双端队列
    int capacity;            // 队列的容量

public:
    MaxQueue(int n) : capacity(n) {}

    void enqueue(int value) {
        if (q.size() == capacity) {
            cout << "Queue is Full" << endl;
            return;
        }
        q.push(value);

        // 更新辅助队列，保持递减
        while (!maxDeque.empty() && maxDeque.back() < value) {
            maxDeque.pop_back();
        }
        maxDeque.push_back(value);
    }

    void dequeue() {
        if (q.empty()) {
            cout << "Queue is Empty" << endl;
            return;
        }
        int front = q.front();
        q.pop();
        cout << front << endl;

        // 如果出队的元素是当前最大值，辅助队列也要更新
        if (!maxDeque.empty() && maxDeque.front() == front) {
            maxDeque.pop_front();
        }
    }

    void getMax() {
        if (q.empty()) {
            cout << "Queue is Empty" << endl;
        }
        else {
            cout << maxDeque.front() << endl;  // 辅助队列的队首就是最大值
        }
    }

    void quit() {
        while (!q.empty()) {
            cout << q.front() << " ";
            q.pop();
        }
        cout << endl;
    }
};

int main() {
    int n;
    cin >> n;

    MaxQueue mq(n);
    string command;

    while (cin >> command) {
        if (command == "enqueue") {
            int value;
            cin >> value;
            mq.enqueue(value);
        }
        else if (command == "dequeue") {
            mq.dequeue();
        }
        else if (command == "max") {
            mq.getMax();
        }
        else if (command == "quit") {
            mq.quit();
            break;
        }
    }

    return 0;
}
