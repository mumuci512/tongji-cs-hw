import heapq
import matplotlib.pyplot as plt
import numpy as np

class Node:
    def __init__(self, position, g=0, h=0, parent=None):
        self.position = position
        self.g = g
        self.h = h
        self.f = g + h
        self.parent = parent

    def __lt__(self, other):
        return self.f < other.f

def heuristic(a, b):
    # 曼哈顿距离
    return abs(a[0] - b[0]) + abs(a[1] - b[1])

def astar(matrix, start, end, ax=None):
    rows = len(matrix)
    cols = len(matrix[0]) if rows > 0 else 0
    open_list = []
    heapq.heapify(open_list)
    closed_set = set()
    explored = []

    start_node = Node(start, g=0, h=heuristic(start, end))
    heapq.heappush(open_list, start_node)
    nodes = {start: start_node}

    while open_list:
        current_node = heapq.heappop(open_list)
        explored.append(current_node.position)

        # 动态展示探索过程
        if ax:
            x, y = current_node.position
            rect = plt.Rectangle((y-0.5, x-0.5), 1, 1, facecolor='gray', alpha=0.5)
            ax.add_patch(rect)
            plt.pause(0.01)

        if current_node.position == end:
            path = []
            while current_node:
                path.append(current_node.position)
                current_node = current_node.parent
            return path[::-1], explored  # 返回路径和探索过的节点

        closed_set.add(current_node.position)

        # 四方向移动
        directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
        for dx, dy in directions:
            x = current_node.position[0] + dx
            y = current_node.position[1] + dy
            if 0 <= x < rows and 0 <= y < cols and matrix[x][y] != 1:
                neighbor_pos = (x, y)
                if neighbor_pos in closed_set:
                    continue

                tentative_g = current_node.g + 1
                if neighbor_pos in nodes:
                    neighbor_node = nodes[neighbor_pos]
                    if tentative_g < neighbor_node.g:
                        neighbor_node.g = tentative_g
                        neighbor_node.f = tentative_g + neighbor_node.h
                        neighbor_node.parent = current_node
                        heapq.heappush(open_list, neighbor_node)
                else:
                    h = heuristic(neighbor_pos, end)
                    neighbor_node = Node(neighbor_pos, tentative_g, h, current_node)
                    heapq.heappush(open_list, neighbor_node)
                    nodes[neighbor_pos] = neighbor_node

    return None, explored  # 无路径

# 示例矩阵（0: 可行，1: 障碍，S: 起点，E: 终点）
matrix = [
    [0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 1, 1, 1, 0, 0, 0],
    [0, 1, 0, 'S', 1, 0, 0, 0],
    [0, 1, 0, 0, 1, 0, 0, 0],
    [0, 0, 0, 0, 1, 0, 0, 'E'],
    [0, 0, 0, 0, 1, 0, 0, 0],
    [0, 0, 0, 0, 1, 0, 0, 0],
    [0, 1, 1, 1, 1, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0]
]

# 解析起点和终点
start = None
end = None
for i in range(len(matrix)):
    for j in range(len(matrix[i])):
        if matrix[i][j] == 'S':
            start = (i, j)
        elif matrix[i][j] == 'E':
            end = (i, j)
if not start or not end:
    print("起点或终点未找到")
    exit()

# 清理矩阵中的S和E标记为0
matrix_clean = [[1 if cell == 1 else 0 for cell in row] for row in matrix]

# 可视化初始化
fig, ax = plt.subplots()
ax.set_xticks(np.arange(-0.5, len(matrix[0]), 1), minor=True)
ax.set_yticks(np.arange(-0.5, len(matrix), 1), minor=True)
ax.grid(which='minor', color='black', linestyle='-', linewidth=1)
ax.imshow(matrix_clean, cmap='Blues', interpolation='none')

# 标注起点和终点
ax.text(start[1], start[0], 'S', ha='center', va='center', color='green', fontsize=12)
ax.text(end[1], end[0], 'E', ha='center', va='center', color='red', fontsize=12)

# 执行A*算法并动态展示
path, explored = astar(matrix, start, end, ax)

# 绘制最终路径
if path:
    path_y = [p[1] for p in path]
    path_x = [p[0] for p in path]
    ax.plot(path_y, path_x, 'y-', linewidth=2, marker='o', markersize=8)
    print("找到路径：", path)
else:
    print("无可行路径")

plt.show()