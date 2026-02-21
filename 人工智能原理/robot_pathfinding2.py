import heapq
import matplotlib.pyplot as plt
import numpy as np
from typing import Tuple, List, Dict, Set, Optional

# ======================
# 类型定义
# ======================
GridLocation = Tuple[int, int]
Matrix = List[List[int]]

# ======================
# 数据结构
# ======================
class Node:
    def __init__(self, position: GridLocation, g: float = 0, h: float = 0, parent: Optional['Node'] = None):
        self.position = position
        self.g = g      # 实际成本
        self.h = h      # 启发式估计
        self.f = g + h  # 总成本
        self.parent = parent

    def __lt__(self, other: 'Node') -> bool:
        return self.f < other.f

# ======================
# 工具函数
# ======================
def heuristic(a: GridLocation, b: GridLocation) -> float:
    return abs(a[0] - b[0]) + abs(a[1] - b[1])

# 从终点节点回溯路径
def reconstruct_path(current_node: Node) -> List[GridLocation]:
    path = []
    while current_node:
        path.append(current_node.position)
        current_node = current_node.parent
    return path[::-1]  # 反转得到起点到终点的路径

# 解析矩阵并清理数据
def parse_matrix(matrix: Matrix) -> Tuple[GridLocation, GridLocation, Matrix]:
    start = None
    end = None
    clean_matrix = []
    
    for i, row in enumerate(matrix):
        clean_row = []
        for j, val in enumerate(row):
            if val == 'S':
                start = (i, j)
                clean_row.append(0)
            elif val == 'E':
                end = (i, j)
                clean_row.append(0)
            else:
                clean_row.append(1 if val == 1 else 0)
        clean_matrix.append(clean_row)
    
    if not start or not end:
        raise ValueError("矩阵中必须包含S(起点)和E(终点)")
    return start, end, clean_matrix

# ======================
# 核心算法
# ======================
def astar(
    matrix: Matrix,
    start: GridLocation,
    end: GridLocation,
    ax: Optional[plt.Axes] = None,
    visualize_interval: int = 10
) -> Tuple[Optional[List[GridLocation]], List[GridLocation]]:
    """A* 路径搜索算法
    
    参数:
        matrix: 二维矩阵 (0=可行, 1=障碍)
        start: 起点坐标 (row, col)
        end: 终点坐标 (row, col)
        ax: 可视化坐标轴 (可选)
        visualize_interval: 可视化更新间隔
        
    返回:
        (路径, 探索过的节点) 或 (None, 探索过的节点)
    """
    # 初始化设置
    rows = len(matrix)
    cols = len(matrix[0]) if rows > 0 else 0
    open_heap: List[Node] = []
    closed_set: Set[GridLocation] = set()
    node_map: Dict[GridLocation, Node] = {}
    explored: List[GridLocation] = []
    visualization_counter = 0
    
    # 初始节点
    start_node = Node(start, g=0, h=heuristic(start, end))
    heapq.heappush(open_heap, start_node)
    node_map[start] = start_node
    
    # 四方向移动向量
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    
    while open_heap:
        current_node = heapq.heappop(open_heap)
        explored.append(current_node.position)
        
        # 动态可视化（降低更新频率）
        if ax and visualization_counter % visualize_interval == 0:
            x, y = current_node.position
            rect = plt.Rectangle((y-0.5, x-0.5), 1, 1, 
                               facecolor='gray', alpha=0.3)
            ax.add_patch(rect)
            plt.pause(0.001)
        visualization_counter += 1
        
        # 终止条件检查
        if current_node.position == end:
            return reconstruct_path(current_node), explored
            
        closed_set.add(current_node.position)
        
        # 探索邻居节点
        for dx, dy in directions:
            x, y = current_node.position[0] + dx, current_node.position[1] + dy
            
            # 边界和障碍物检查
            if not (0 <= x < rows and 0 <= y < cols):
                continue
            if matrix[x][y] == 1:
                continue
                
            neighbor_pos = (x, y)
            
            # 跳过已关闭节点
            if neighbor_pos in closed_set:
                continue
                
            # 计算新成本
            tentative_g = current_node.g + 1
            
            # 更新或创建节点
            if neighbor_pos in node_map:
                neighbor_node = node_map[neighbor_pos]
                if tentative_g < neighbor_node.g:
                    neighbor_node.g = tentative_g
                    neighbor_node.f = tentative_g + neighbor_node.h
                    neighbor_node.parent = current_node
                    heapq.heappush(open_heap, neighbor_node)  # 重新插入更新后的节点
            else:
                h = heuristic(neighbor_pos, end)
                neighbor_node = Node(
                    position=neighbor_pos,
                    g=tentative_g,
                    h=h,
                    parent=current_node
                )
                heapq.heappush(open_heap, neighbor_node)
                node_map[neighbor_pos] = neighbor_node
                
    return None, explored  # 无路径

# ======================
# 可视化与测试
# ======================
def visualize_results(
    matrix: Matrix,
    path: Optional[List[GridLocation]],
    explored: List[GridLocation],
    start: GridLocation,
    end: GridLocation
):
    """可视化最终结果"""
    fig, ax = plt.subplots()
    
    # 绘制网格
    ax.set_xticks(np.arange(-0.5, len(matrix[0]), 1), minor=True)
    ax.set_yticks(np.arange(-0.5, len(matrix), 1), minor=True)
    ax.grid(which='minor', color='black', linestyle='-', linewidth=1)
    ax.imshow(matrix, cmap='Blues', interpolation='none')
    
    # 标注起终点
    ax.text(start[1], start[0], 'S', ha='center', va='center', 
           color='green', fontsize=12, weight='bold')
    ax.text(end[1], end[0], 'E', ha='center', va='center',
           color='red', fontsize=12, weight='bold')
    
    # 绘制路径
    if path:
        path_y = [p[1] for p in path]
        path_x = [p[0] for p in path]
        ax.plot(path_y, path_x, 'y-', linewidth=2, marker='o', 
               markersize=8, markeredgecolor='black')
        
    plt.show()

# ======================
# 示例使用
# ======================
if __name__ == "__main__":
    # 示例矩阵配置
    original_matrix = [
        [0, 0, 0, 1, 0],
        [0, 1, 0, 1, 0],
        [0, 1, 0, 0, 0],
        [0, 0, 0, 1, 'E'],
        ['S', 1, 0, 0, 0]
    ]
    
    try:
        # 数据预处理
        start_pos, end_pos, clean_matrix = parse_matrix(original_matrix)
        
        # 执行算法（关闭实时可视化以提高速度）
        path, explored = astar(clean_matrix, start_pos, end_pos)
        
        # 结果输出
        if path:
            print(f"找到路径 (长度: {len(path)}):")
            for step in path:
                print(f"→ {step}", end=' ')
            print("\n探索节点数:", len(explored))
        else:
            print("无可行路径")
            
        # 最终可视化
        visualize_results(clean_matrix, path, explored, start_pos, end_pos)
        
    except ValueError as e:
        print(f"错误: {e}")