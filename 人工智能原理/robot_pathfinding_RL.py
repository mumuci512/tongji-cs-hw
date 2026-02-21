import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
from collections import deque
import random

class DQN(nn.Module):
    def __init__(self, input_size, output_size):
        super().__init__()
        self.net = nn.Sequential(
            nn.Linear(input_size, 32),
            nn.ReLU(),
            nn.Linear(32, output_size)
        )
    
    def forward(self, x):
        return self.net(x)
    
class RobotEnv:
    def __init__(self, matrix, start, end):
        self.matrix = np.array(matrix) 
        self.start = start           
        self.end = end               
        self.current_pos = start    
        self.rows, self.cols = self.matrix.shape
        
    def reset(self):
        self.current_pos = self.start
        return self._get_state()
    
    def step(self, action):
        x, y = self.current_pos
        dx = [-1, 1, 0, 0][action]
        dy = [0, 0, -1, 1][action]
        nx, ny = x + dx, y + dy
        
        if 0 <= nx < self.rows and 0 <= ny < self.cols and self.matrix[nx, ny] != 1:
            self.current_pos = (nx, ny)
        
        reward = -0.1 # 一步的惩罚
        done = (self.current_pos == self.end)
        if done:
            reward = 10.0 # 达到终点的奖励
        elif self.matrix[x, y] == 1:
            reward = -5.0 # 撞障碍的惩罚
        else:
            distance_old = abs(x - self.end[0]) + abs(y - self.end[1])  # 原曼哈顿距离
            distance_new = abs(nx - self.end[0]) + abs(ny - self.end[1])
            reward += (distance_old - distance_new) * 0.2  # 靠近终点奖励
        
        return self._get_state(), reward, done
    
    def _get_state(self):
        return np.array([self.current_pos[0], self.current_pos[1]])

class ReplayBuffer:
    def __init__(self, capacity):
        self.buffer = deque(maxlen=capacity)
    
    def push(self, state, action, reward, next_state, done):
        # 强制转换数据类型
        self.buffer.append((
            np.array(state, dtype=np.float32),   # 状态转为float32
            np.array(action, dtype=np.int64),    # 动作转为int64
            np.array(reward, dtype=np.float32),
            np.array(next_state, dtype=np.float32),
            np.array(done, dtype=np.float32)
        ))
    
    def sample(self, batch_size):
        batch = random.sample(self.buffer, batch_size)
        states, actions, rewards, next_states, dones = zip(*batch)
        
        # 优化：使用np.stack合并数组
        states = np.stack(states, axis=0)        # shape: (batch_size, 2)
        actions = np.array(actions, dtype=np.int64) 
        rewards = np.array(rewards, dtype=np.float32)
        next_states = np.stack(next_states, axis=0)
        dones = np.array(dones, dtype=np.float32)
        
        return (
            torch.from_numpy(states),            # 自动转为FloatTensor
            torch.from_numpy(actions),          # 自动转为LongTensor
            torch.from_numpy(rewards),
            torch.from_numpy(next_states),
            torch.from_numpy(dones)
        )
    
    def __len__(self):
        return len(self.buffer)

class DQNAgent:
    def __init__(self, env, gamma=0.99, lr=1e-3):
        self.env = env
        self.gamma = gamma 
        self.epsilon = 1.0 
        self.epsilon_min = 0.01
        self.epsilon_decay = 0.995
        
        self.policy_net = DQN(input_size=2, output_size=4) # 输入坐标，输出4个动作
        self.target_net = DQN(input_size=2, output_size=4)
        self.target_net.load_state_dict(self.policy_net.state_dict())
        
        self.optimizer = optim.Adam(self.policy_net.parameters(), lr=lr)
        self.loss_fn = nn.MSELoss()
        
        self.memory = ReplayBuffer(capacity=10000)
    
    def select_action(self, state):
        if random.random() < self.epsilon:
            return random.randint(0, 3)  
        else:
            with torch.no_grad():
                state_tensor = torch.FloatTensor(state).unsqueeze(0)
                q_values = self.policy_net(state_tensor)
                return q_values.argmax().item()
    
    def update_model(self, batch_size): # 采样并更新网络
        if len(self.memory) < batch_size:
            return
        
        states, actions, rewards, next_states, dones = self.memory.sample(batch_size)# 采样批次数据
        
        current_q = self.policy_net(states).gather(1, actions.unsqueeze(1)) # 计算当前Q值
        
        with torch.no_grad(): # 计算目标Q值
            next_q = self.target_net(next_states).max(1)[0]
            target_q = rewards + self.gamma * next_q * (1 - dones)
        
        loss = self.loss_fn(current_q.squeeze(), target_q)
        self.optimizer.zero_grad()
        loss.backward() # 损失反向传播
        self.optimizer.step()
        
        self.epsilon = max(self.epsilon_min, self.epsilon * self.epsilon_decay) # 减epsilon
    
    def update_target_net(self):
        self.target_net.load_state_dict(self.policy_net.state_dict())

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
            
            agent.update_model(batch_size)
        
        if episode % target_update == 0: # 定期更新
            agent.update_target_net()
        
        print(f"Episode {episode+1}, Total Reward: {total_reward:.2f}, Epsilon: {agent.epsilon:.3f}")
    
    return agent

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
    matrix1 = [[1 if cell == 1 else 0 for cell in row] for row in matrix]
    env = RobotEnv(matrix1, start, end)
    
    agent = train_dqn(env, episodes=50, batch_size=128, target_update=20)
    
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