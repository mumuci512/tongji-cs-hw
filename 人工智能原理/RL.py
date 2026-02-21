import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
from collections import deque
import random

class DQN(nn.Module):
    """定义深度Q网络结构"""
    def __init__(self, input_size, output_size):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(input_size, 64),
            nn.ReLU(),
            nn.Linear(64, output_size)
        )
    
    def forward(self, x):
        return self.net(x)
    
# --------------------- 1. 强化学习环境定义 ---------------------
class RobotEnv:
    def __init__(self, matrix, start, end):
        self.matrix = np.array(matrix)  # 0: 可行区域, 1: 障碍
        self.start = start              # 起点坐标 (x, y)
        self.end = end                  # 终点坐标 (x, y)
        self.current_pos = start        # 当前位置
        self.rows, self.cols = self.matrix.shape
        
    def reset(self):
        """重置环境到初始状态"""
        self.current_pos = self.start
        return self._get_state()
    
    def step(self, action):
        """
        执行动作并返回新状态、奖励、是否终止
        action: 0:上, 1:下, 2:左, 3:右
        """
        x, y = self.current_pos
        dx = [-1, 1, 0, 0][action]
        dy = [0, 0, -1, 1][action]
        nx, ny = x + dx, y + dy
        
        # 检查是否越界或撞障碍
        if 0 <= nx < self.rows and 0 <= ny < self.cols and self.matrix[nx, ny] != 1:
            self.current_pos = (nx, ny)
        
        # 计算奖励
        reward = -0.1  # 每一步的小惩罚
        done = (self.current_pos == self.end)
        if done:
            reward = 10.0  # 到达终点的奖励
        elif self.matrix[x, y] == 1:
            reward = -5.0   # 撞障碍的惩罚
        
        return self._get_state(), reward, done
    
    def _get_state(self):
        """将状态转换为网络输入格式（一维向量）"""
        return np.array([self.current_pos[0], self.current_pos[1]])

# --------------------- 2. 经验回放缓冲区 ---------------------
class ReplayBuffer:
    def __init__(self, capacity):
        self.buffer = deque(maxlen=capacity)
    
    def push(self, state, action, reward, next_state, done):
        """存储单条经验"""
        self.buffer.append((state, action, reward, next_state, done))
    
    def sample(self, batch_size):
        """随机采样一个批次"""
        batch = random.sample(self.buffer, batch_size)
        states, actions, rewards, next_states, dones = zip(*batch)
        return (
            torch.FloatTensor(states),
            torch.LongTensor(actions),
            torch.FloatTensor(rewards),
            torch.FloatTensor(next_states),
            torch.FloatTensor(dones)
        )
    
    def __len__(self):
        return len(self.buffer)

# --------------------- 3. DQN智能体定义 ---------------------
class DQNAgent:
    def __init__(self, env, gamma=0.99, lr=1e-3):
        self.env = env
        self.gamma = gamma  # 折扣因子
        self.epsilon = 1.0  # 初始探索率
        self.epsilon_min = 0.01
        self.epsilon_decay = 0.995
        
        # 定义主网络和目标网络
        self.policy_net = DQN(input_size=2, output_size=4)  # 输入为坐标，输出为4个动作
        self.target_net = DQN(input_size=2, output_size=4)
        self.target_net.load_state_dict(self.policy_net.state_dict())
        
        self.optimizer = optim.Adam(self.policy_net.parameters(), lr=lr)
        self.loss_fn = nn.MSELoss()
        
        # 经验回放缓冲区
        self.memory = ReplayBuffer(capacity=10000)
    
    def select_action(self, state):
        """ε-greedy策略选择动作"""
        if random.random() < self.epsilon:
            return random.randint(0, 3)  # 随机探索
        else:
            with torch.no_grad():
                state_tensor = torch.FloatTensor(state).unsqueeze(0)
                q_values = self.policy_net(state_tensor)
                return q_values.argmax().item()
    
    def update_model(self, batch_size):
        """从经验回放中采样并更新网络"""
        if len(self.memory) < batch_size:
            return
        
        # 采样批次数据
        states, actions, rewards, next_states, dones = self.memory.sample(batch_size)
        
        # 计算当前Q值
        current_q = self.policy_net(states).gather(1, actions.unsqueeze(1))
        
        # 计算目标Q值
        with torch.no_grad():
            next_q = self.target_net(next_states).max(1)[0]
            target_q = rewards + self.gamma * next_q * (1 - dones)
        
        # 计算损失并反向传播
        loss = self.loss_fn(current_q.squeeze(), target_q)
        self.optimizer.zero_grad()
        loss.backward()
        self.optimizer.step()
        
        # 衰减ε
        self.epsilon = max(self.epsilon_min, self.epsilon * self.epsilon_decay)
    
    def update_target_net(self):
        """更新目标网络参数"""
        self.target_net.load_state_dict(self.policy_net.state_dict())

# --------------------- 4. 训练循环 ---------------------
def train_dqn(env, episodes=500, batch_size=32, target_update=10):
    agent = DQNAgent(env)
    
    for episode in range(episodes):
        state = env.reset()
        total_reward = 0
        done = False
        
        while not done:
            action = agent.select_action(state)
            next_state, reward, done = env.step(action)
            agent.memory.push(state, action, reward, next_state, done)
            state = next_state
            total_reward += reward
            
            # 更新主网络
            agent.update_model(batch_size)
        
        # 定期更新目标网络
        if episode % target_update == 0:
            agent.update_target_net()
        
        print(f"Episode {episode+1}, Total Reward: {total_reward:.2f}, Epsilon: {agent.epsilon:.3f}")
    
    return agent

# --------------------- 5. 测试训练结果 ---------------------
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
    start = (4, 0)
    end = (3, 4)
    matrix_clean = [[1 if cell == 1 else 0 for cell in row] for row in matrix]
    env = RobotEnv(matrix_clean, start, end)
    
    # 训练模型
    agent = train_dqn(env, episodes=200)
    
    # 测试训练后的策略
    state = env.reset()
    path = [state]
    done = False
    while not done:
        with torch.no_grad():
            state_tensor = torch.FloatTensor(state).unsqueeze(0)
            action = agent.policy_net(state_tensor).argmax().item()
        next_state, _, done = env.step(action)
        state = next_state
        path.append(state)
    
    print("最终路径:", path)