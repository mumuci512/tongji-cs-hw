import heapq
from typing import Tuple, List, Set, Optional, Dict, Union
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import matplotlib.patches as patches
from matplotlib.lines import Line2D as L2D
import numpy as np
from collections import deque

location = Tuple[int, int]
Matrix = List[List[Union[int, str]]]

class Node:
    def __init__(self, location: location, g: float = 0, h: float = 0, parent: Optional['Node'] = None):
        self.location = location
        self.g = g
        self.h = h
        self.f = g + h
        self.parent = parent

    def __lt__(self, other):
        return self.f < other.f
    
    def __eq__(self, other):
        return isinstance(other, Node) and self.location == other.location
    
    def __hash__(self):
        return hash(self.location)

class PriorityQueue:
    def __init__(self):
        self.elements: list[tuple[float, Node]] = []
        self.loc_dic: Dict[location, Node] = {}  # 坐标字典,便于查找某个坐标是否在队列中

    def empty(self) -> bool:
        return not self.elements
    
    def put(self, item: Node):
        # 只有新节点或更优节点才加入
        if (item.location not in self.loc_dic) or (item.g < self.loc_dic[item.location].g):
            heapq.heappush(self.elements, (item.f, item))
            self.loc_dic[item.location] = item

    def get(self) -> Node:
        while self.elements:
            _, node = heapq.heappop(self.elements)
            # 若节点存在于字典中，才弹出，并且从字典中抹除
            if node.location in self.loc_dic and self.loc_dic[node.location] == node:
                del self.loc_dic[node.location]
                return node
        raise IndexError("PriorityQueue is empty")

def heuristic(a: location, b: location) -> float: # 曼哈顿距离
    return abs(a[0] - b[0]) + abs(a[1] - b[1]) 

def reconstruct_path(node: Node) -> List[location]:
    path = []
    while node:
        path.append(node.location)
        node = node.parent
    return path[::-1]

def neighbor_loc(current: location, matrix: Matrix) -> List[location]:
    rows = len(matrix)
    cols = len(matrix[0]) if rows > 0 else 0
    neighbors = []

    dir = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    for dx, dy in dir:
        x = current[0] + dx
        y = current[1] + dy
        if 0 <= x < rows and 0 <= y < cols and matrix[x][y] != 1:
            neighbors.append((x, y))
    return neighbors

def draw_node_in_open_list(node: Node, ax: plt.Axes, drawn_lines: Dict[location, L2D]=None, color=None, TF_line=None, TF_text=None) -> None:
    x=node.location[0]
    y=node.location[1]

    if color: # 若指定颜色，则画方块
        ax.add_patch(patches.Rectangle(
            (y-0.5, x-0.5), 1, 1, facecolor=color, alpha=1))
    
    if TF_text: # 若指定文本，则画文本
        prompt="f={:.2f}\ng={:.2f}\nh={:.2f}".format(node.f, node.g, node.h)
        ax.text(y, x, prompt, ha='center', va='center', fontsize=6)

    if TF_line: # 画线
        line, =ax.plot((y, node.parent.location[1]), (x, node.parent.location[0]), 
                'g-', linewidth=2, marker='o', markersize=8)
        drawn_lines[node.location] = line # 记录已画的线段

    plt.pause(0.6)

"""A*算法 
参数:
    matrix: 二维矩阵 (0=可行, 1=障碍, 2=起点, 3=终点)
    weight: 权重矩阵 (与matrix同维度)
    start: 起点坐标 (row, col)
    end: 终点坐标 (row, col)
    ax: 可视化坐标轴
        
返回:
    路径 或 None
"""
def Astar(matrix: Matrix, weight: Matrix, start_loc: location, end_loc: location, ax: plt.Axes) -> Optional[List[location]]:
    start = Node(start_loc, g=0, h=heuristic(start_loc, end_loc))
    open_list = PriorityQueue()
    open_list.put(start)
    closed_set = set()

    drawn_lines: Dict[location, L2D] = {} # 记录已画的线段

    while not open_list.empty():
        current_node = open_list.get()

        if current_node.location == end_loc:
            return reconstruct_path(current_node)

        draw_node_in_open_list(current_node, ax, color = 'purple', TF_text=True) # 绘制当前节点

        neighbors= neighbor_loc(current_node.location, matrix)
        for neighbor_pos in neighbors:
            if neighbor_pos in closed_set:
                continue
                
            wei = weight[neighbor_pos[0]][neighbor_pos[1]]
            if neighbor_pos in open_list.loc_dic:
                neighbor_node = open_list.loc_dic[neighbor_pos]
                if neighbor_node.g>current_node.g+wei: # 若新节点更优，则覆盖旧节点
                    neighbor_node.g = current_node.g + wei
                    neighbor_node.parent = current_node
                    neighbor_node.f = neighbor_node.g + neighbor_node.h
                    drawn_lines[neighbor_node.location].remove() # 删除旧节点的线段
                    open_list.put(neighbor_node) # 会自动覆盖旧节点，因为字典中旧节点已不复存在
                    draw_node_in_open_list(neighbor_node, ax, drawn_lines=drawn_lines, TF_line=True, TF_text=True) # 绘制当前节点
            else: # 如果open_list中没有这个节点
                neighbor_node = Node(neighbor_pos, current_node.g + wei, heuristic(neighbor_pos, end_loc), current_node)
                open_list.put(neighbor_node)
                draw_node_in_open_list(neighbor_node, ax, drawn_lines=drawn_lines, color='yellow', TF_line=True, TF_text=True) # 绘制当前节点      

        closed_set.add(current_node.location)
        draw_node_in_open_list(current_node, ax, color = 'red', TF_text=True) # 当前节点改为红色，表示已压入closed_set

    return None # 没有路径

def merge_path(node, front_visited, back_visited) -> List[location]:
    path = []
    current = node
    while current:
        path.append(current)
        current = front_visited[current]
    path = path[::-1]
    current = back_visited[node]
    while current:
        path.append(current)
        current = back_visited[current]
    return path
"""双向BFS算法 
参数:
    matrix: 二维矩阵 (0=可行, 1=障碍, 2=起点, 3=终点)
    start: 起点坐标 (row, col)
    end: 终点坐标 (row, col)
    ax: 可视化坐标轴
        
返回:
    路径 或 None
"""
def bidirectional_bfs(matrix, start, end, ax: plt.Axes) -> Optional[List[location]]:
    front_queue = deque([start])
    back_queue = deque([end])
    front_visited = {start: None} # 前驱
    back_visited = {end: None}

    while front_queue and back_queue:
        # 正向
        current = front_queue.popleft()
        ax.add_patch(patches.Rectangle(
                    (current[1]-0.5, current[0]-0.5), 1, 1, facecolor='purple', alpha=1))
        plt.pause(0.6)
        if current in back_visited:
            return merge_path(current, front_visited, back_visited)
        
        neighbors = neighbor_loc(current, matrix)
        for neighbor in neighbors:
            if neighbor not in front_visited:
                front_visited[neighbor] = current
                front_queue.append(neighbor)
                ax.add_patch(patches.Rectangle(
                            (neighbor[1]-0.5, neighbor[0]-0.5), 1, 1, facecolor='yellow', alpha=1))
                ax.plot((neighbor[1], current[1]), (neighbor[0], current[0]), 
                        'g-', linewidth=2, marker='o', markersize=8)
                plt.pause(0.6)

        ax.add_patch(patches.Rectangle(
                    (current[1]-0.5, current[0]-0.5), 1, 1, facecolor='red', alpha=1))
        
        # 反向
        current = back_queue.popleft()
        ax.add_patch(patches.Rectangle(
                    (current[1]-0.5, current[0]-0.5), 1, 1, facecolor='purple', alpha=1))
        plt.pause(0.6)
        if current in front_visited:
            return merge_path(current, front_visited, back_visited)
        for neighbor in neighbor_loc(current, matrix):
            if neighbor not in back_visited:
                back_visited[neighbor] = current
                back_queue.append(neighbor)
                ax.add_patch(patches.Rectangle(
                            (neighbor[1]-0.5, neighbor[0]-0.5), 1, 1, facecolor='yellow', alpha=1))
                ax.plot((neighbor[1], current[1]), (neighbor[0], current[0]), 
                        'g-', linewidth=2, marker='o', markersize=8)                
                plt.pause(0.6)
        ax.add_patch(patches.Rectangle(
                    (current[1]-0.5, current[0]-0.5), 1, 1, facecolor='red', alpha=1))
        
    return None

def draw_init(matrix:Matrix, start:location=None, end:location=None) -> Tuple[plt.Figure, plt.Axes]:
    fig, ax = plt.subplots()

    ax.set_xticks(np.arange(-0.5, len(matrix[0]), 1), minor=True)
    ax.set_yticks(np.arange(-0.5, len(matrix), 1), minor=True)

    ax.grid(which='minor', color='black', linestyle='-', linewidth=1)

    colors = ['white', 'blue', 'red', 'grey'] # 0:可行，1:障碍，2:起点，3:终点
    cmap = ListedColormap(colors)

    ax.imshow(matrix, cmap=cmap, interpolation='none')

    ax.set_xticks(np.arange(len(matrix[0])))
    ax.set_yticks(np.arange(len(matrix)))

    ax.text(start[1], start[0], 'S', ha='center', va='center', fontsize=12)
    ax.text(end[1], end[0], 'E', ha='center', va='center', fontsize=12)

    return fig, ax


if __name__ == "__main__":
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
    weight: Matrix = [[1 for _ in row] for row in matrix] 

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

    # S和E已经赋值给start和end，将其在矩阵中改为数字2,3,便于绘图处理
    matrix1 = [[2 if item == 'S' else 3 if item == 'E' else item for item in row] for row in matrix]

    fig, ax = draw_init(matrix1, start, end)
    path = Astar(matrix1, weight, start, end, ax)
    #path = bidirectional_bfs(matrix1, start, end, ax)
    if(path == None):
        print("没有路径")
    else:
        path_y = [p[1] for p in path]
        path_x = [p[0] for p in path]
        ax.plot(path_y, path_x, 'w-', linewidth=2, marker='o', markersize=8)
        print(path)

    #plt.show()

    plt.pause(50)
