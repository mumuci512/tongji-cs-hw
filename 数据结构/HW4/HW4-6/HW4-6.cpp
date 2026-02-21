#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>

using namespace std;

const int INF = INT_MAX;

struct Edge {
    int to, weight;
};

void dijkstra(int start, const vector<vector<Edge>>& graph, vector<int>& dist) {
    dist.assign(graph.size(), INF);
    dist[start] = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({ 0, start });

    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d > dist[u]) continue;

        for (const Edge& edge : graph[u]) {
            int v = edge.to;
            int w = edge.weight;
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                pq.push({ dist[v], v });
            }
        }
    }
}

int main() {
    int N, M;
    cin >> N >> M;

    vector<vector<Edge>> graph(N + 1);

    for (int i = 0; i < M; ++i) {
        int x, y, w;
        cin >> x >> y >> w;
        graph[x].push_back({ y, w });
        graph[y].push_back({ x, w });
    }

    int H, R;
    cin >> H >> R;

    vector<int> grass_points(H);
    for (int i = 0; i < H; ++i) {
        cin >> grass_points[i];
    }

    vector<int> start(R), end(R);
    for (int j = 0; j < R; ++j) {
        cin >> start[j] >> end[j];
    }

    vector<int> dist_to_grass(N + 1, INF);
    for (int grass_point : grass_points) {
        vector<int> temp_dist;
        dijkstra(grass_point, graph, temp_dist);
        for (int i = 1; i <= N; ++i) {
            dist_to_grass[i] = min(dist_to_grass[i], temp_dist[i]);
        }
    }

    for (int j = 0; j < R; ++j) {
        vector<int> dist_from_start, dist_from_end;
        dijkstra(start[j], graph, dist_from_start);
        dijkstra(end[j], graph, dist_from_end);

        int min_distance = INF;
        for (int grass_point : grass_points) {
            int total_distance = dist_from_start[grass_point] + dist_from_end[grass_point];
            min_distance = min(min_distance, total_distance);
        }

        cout << min_distance << endl;
    }

    return 0;
}
