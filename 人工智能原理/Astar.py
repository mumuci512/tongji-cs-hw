import heapq
from typing import Tuple, List, Dict, Set, Optional, TypeVar, Iterator, Protocol
Location = TypeVar('Location')  # 位置类型泛型
T = TypeVar('T')                # 泛型类型参数

GridLocation = Tuple[int, int]

class Graph(Protocol):
    def neighbors(self, id: Location) -> list[Location]: pass

class PriorityQueue:
    def __init__(self):
        self.elements: list[tuple[float, T]] = []
    
    def empty(self) -> bool:
        return not self.elements
    
    def put(self, item: T, priority: float):
        heapq.heappush(self.elements, (priority, item))
    
    def get(self) -> T:
        return heapq.heappop(self.elements)[1]

class SquareGrid:
    def __init__(self, width: int, height: int):
        self.width = width
        self.height = height
        self.walls: list[GridLocation] = []
    
    def in_bounds(self, id: GridLocation) -> bool:
        (x, y) = id
        return 0 <= x < self.width and 0 <= y < self.height
    
    def passable(self, id: GridLocation) -> bool:
        return id not in self.walls
    
    def neighbors(self, id: GridLocation) -> Iterator[GridLocation]:
        (x, y) = id
        neighbors = [(x+1, y), (x-1, y), (x, y-1), (x, y+1)] # E W N S
        # see "Ugly paths" section for an explanation:
        if (x + y) % 2 == 0: neighbors.reverse() # S N W E
        results = filter(self.in_bounds, neighbors)
        results = filter(self.passable, results)
        return results

class WeightedGraph(Graph):
    def cost(self, from_id: Location, to_id: Location) -> float: pass

class GridWithWeights(SquareGrid):
    def __init__(self, width: int, height: int):
        super().__init__(width, height)
        self.weights: dict[GridLocation, float] = {}
    
    def cost(self, from_node: GridLocation, to_node: GridLocation) -> float:
        return self.weights.get(to_node, 1)

def heuristic(a: GridLocation, b: GridLocation) -> float:
    (x1, y1) = a
    (x2, y2) = b
    return abs(x1 - x2) + abs(y1 - y2)

def a_star_search(graph: WeightedGraph, start: Location, goal: Location):
    frontier = PriorityQueue() # 存储待探索的节点，按优先级（总代价 f(n) = g(n) + h(n)）排序。
    frontier.put(start, 0)
    came_from: dict[Location, Optional[Location]] = {}
    cost_so_far: dict[Location, float] = {}
    came_from[start] = None
    cost_so_far[start] = 0
    
    while not frontier.empty():
        current: Location = frontier.get() # 当前可接触的节点中，最优先的节点为下一节点（current）
        
        if current == goal:
            break
        
        for my_next in graph.neighbors(current):
            new_cost = cost_so_far[current] + graph.cost(current, my_next)
            if my_next not in cost_so_far or new_cost < cost_so_far[my_next]:
                cost_so_far[my_next] = new_cost
                priority = new_cost + heuristic(my_next, goal)
                frontier.put(my_next, priority)
                came_from[my_next] = current
    
    return came_from, cost_so_far