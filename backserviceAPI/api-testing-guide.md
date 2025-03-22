# 数据结构与算法游戏 API 测试指南

本文档提供了如何测试数据结构与算法游戏后端API的详细指南。我们提供了两种测试方法：使用PowerShell脚本（适用于Windows）和Bash脚本（适用于Linux/macOS）。

## 前提条件

- 确保后端服务已启动并运行在 http://localhost:3000
- 对于Bash脚本测试，需要安装curl和jq
- 对于PowerShell脚本测试，确保PowerShell版本至少为5.1

## 测试重置功能

我们已提供两个专用脚本来测试所有数据结构和算法的重置功能：

- Windows: `test_reset_apis.ps1`
- Linux/macOS: `test_reset_apis.sh`

### 运行测试脚本

在Windows上：
```powershell
.\test_reset_apis.ps1
```

在Linux/macOS上：
```bash
chmod +x test_reset_apis.sh
./test_reset_apis.sh
```

## 手动测试API

以下是如何手动测试各种API的示例：

### 1. 链表操作API

#### 插入节点
```bash
# Bash
curl -X POST http://localhost:3000/api/games/linkedlist/insert -H "Content-Type: application/json" -d '{"value":10,"position":0}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/linkedlist/insert" -Method POST -ContentType "application/json" -Body '{"value":10,"position":0}'
```

#### 删除节点
```bash
# Bash
curl -X POST http://localhost:3000/api/games/linkedlist/delete -H "Content-Type: application/json" -d '{"position":0}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/linkedlist/delete" -Method POST -ContentType "application/json" -Body '{"position":0}'
```

#### 查找节点
```bash
# Bash
curl -X POST http://localhost:3000/api/games/linkedlist/find -H "Content-Type: application/json" -d '{"value":10}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/linkedlist/find" -Method POST -ContentType "application/json" -Body '{"value":10}'
```

#### 反转链表
```bash
# Bash
curl -X POST http://localhost:3000/api/games/linkedlist/reverse -H "Content-Type: application/json" -d '{}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/linkedlist/reverse" -Method POST -ContentType "application/json" -Body '{}'
```

#### 获取链表状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/linkedlist/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/linkedlist/state" -Method GET
```

#### 重置链表
```bash
# Bash
curl -X POST http://localhost:3000/api/games/linkedlist/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/linkedlist/reset" -Method POST
```

### 2. 队列操作API

#### 入队
```bash
# Bash
curl -X POST http://localhost:3000/api/games/queue/enqueue -H "Content-Type: application/json" -d '{"value":10}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/queue/enqueue" -Method POST -ContentType "application/json" -Body '{"value":10}'
```

#### 出队
```bash
# Bash
curl -X POST http://localhost:3000/api/games/queue/dequeue -H "Content-Type: application/json" -d '{}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/queue/dequeue" -Method POST -ContentType "application/json" -Body '{}'
```

#### 获取队首元素
```bash
# Bash
curl -X GET http://localhost:3000/api/games/queue/peek

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/queue/peek" -Method GET
```

#### 获取队列状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/queue/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/queue/state" -Method GET
```

#### 重置队列
```bash
# Bash
curl -X POST http://localhost:3000/api/games/queue/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/queue/reset" -Method POST
```

### 3. 栈操作API

#### 入栈
```bash
# Bash
curl -X POST http://localhost:3000/api/games/stack/push -H "Content-Type: application/json" -d '{"value":10}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/stack/push" -Method POST -ContentType "application/json" -Body '{"value":10}'
```

#### 出栈
```bash
# Bash
curl -X POST http://localhost:3000/api/games/stack/pop -H "Content-Type: application/json" -d '{}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/stack/pop" -Method POST -ContentType "application/json" -Body '{}'
```

#### 获取栈顶元素
```bash
# Bash
curl -X GET http://localhost:3000/api/games/stack/peek

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/stack/peek" -Method GET
```

#### 获取栈状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/stack/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/stack/state" -Method GET
```

#### 重置栈
```bash
# Bash
curl -X POST http://localhost:3000/api/games/stack/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/stack/reset" -Method POST
```

### 4. 二叉树操作API

#### 插入节点
```bash
# Bash
curl -X POST http://localhost:3000/api/games/binarytree/insert -H "Content-Type: application/json" -d '{"value":10}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/insert" -Method POST -ContentType "application/json" -Body '{"value":10}'
```

#### 前序遍历
```bash
# Bash
curl -X POST http://localhost:3000/api/games/binarytree/traversePreOrder -H "Content-Type: application/json" -d '{"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/traversePreOrder" -Method POST -ContentType "application/json" -Body '{"animate":true}'
```

#### 中序遍历
```bash
# Bash
curl -X POST http://localhost:3000/api/games/binarytree/traverseInOrder -H "Content-Type: application/json" -d '{"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/traverseInOrder" -Method POST -ContentType "application/json" -Body '{"animate":true}'
```

#### 后序遍历
```bash
# Bash
curl -X POST http://localhost:3000/api/games/binarytree/traversePostOrder -H "Content-Type: application/json" -d '{"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/traversePostOrder" -Method POST -ContentType "application/json" -Body '{"animate":true}'
```

#### 层序遍历
```bash
# Bash
curl -X POST http://localhost:3000/api/games/binarytree/traverseLevelOrder -H "Content-Type: application/json" -d '{"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/traverseLevelOrder" -Method POST -ContentType "application/json" -Body '{"animate":true}'
```

#### 获取二叉树状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/binarytree/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/state" -Method GET
```

#### 重置二叉树
```bash
# Bash
curl -X POST http://localhost:3000/api/games/binarytree/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/reset" -Method POST
```

### 5. 图操作API

#### DFS遍历
```bash
# Bash
curl -X POST http://localhost:3000/api/games/graph/traverseDFS -H "Content-Type: application/json" -d '{"startNodeId":1,"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/graph/traverseDFS" -Method POST -ContentType "application/json" -Body '{"startNodeId":1,"animate":true}'
```

#### BFS遍历
```bash
# Bash
curl -X POST http://localhost:3000/api/games/graph/traverseBFS -H "Content-Type: application/json" -d '{"startNodeId":1,"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/graph/traverseBFS" -Method POST -ContentType "application/json" -Body '{"startNodeId":1,"animate":true}'
```

#### 寻找路径
```bash
# Bash
curl -X POST http://localhost:3000/api/games/graph/findPath -H "Content-Type: application/json" -d '{"startNodeId":1,"endNodeId":5,"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/graph/findPath" -Method POST -ContentType "application/json" -Body '{"startNodeId":1,"endNodeId":5,"animate":true}'
```

#### 获取图状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/graph/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/graph/state" -Method GET
```

#### 重置图
```bash
# Bash
curl -X POST http://localhost:3000/api/games/graph/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/graph/reset" -Method POST
```

### 6. 排序算法API

#### 冒泡排序
```bash
# Bash
curl -X POST http://localhost:3000/api/games/sorting/bubbleSort -H "Content-Type: application/json" -d '{"array":[5,3,8,1,2],"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/sorting/bubbleSort" -Method POST -ContentType "application/json" -Body '{"array":[5,3,8,1,2],"animate":true}'
```

#### 选择排序
```bash
# Bash
curl -X POST http://localhost:3000/api/games/sorting/selectionSort -H "Content-Type: application/json" -d '{"array":[5,3,8,1,2],"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/sorting/selectionSort" -Method POST -ContentType "application/json" -Body '{"array":[5,3,8,1,2],"animate":true}'
```

#### 插入排序
```bash
# Bash
curl -X POST http://localhost:3000/api/games/sorting/insertionSort -H "Content-Type: application/json" -d '{"array":[5,3,8,1,2],"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/sorting/insertionSort" -Method POST -ContentType "application/json" -Body '{"array":[5,3,8,1,2],"animate":true}'
```

#### 快速排序
```bash
# Bash
curl -X POST http://localhost:3000/api/games/sorting/quickSort -H "Content-Type: application/json" -d '{"array":[5,3,8,1,2],"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/sorting/quickSort" -Method POST -ContentType "application/json" -Body '{"array":[5,3,8,1,2],"animate":true}'
```

#### 获取排序状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/sorting/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/sorting/state" -Method GET
```

#### 重置排序
```bash
# Bash
curl -X POST http://localhost:3000/api/games/sorting/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/sorting/reset" -Method POST
```

### 7. 搜索算法API

#### 线性搜索
```bash
# Bash
curl -X POST http://localhost:3000/api/games/search/linear -H "Content-Type: application/json" -d '{"array":[10,20,30,40,50],"target":30,"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/search/linear" -Method POST -ContentType "application/json" -Body '{"array":[10,20,30,40,50],"target":30,"animate":true}'
```

#### 二分搜索
```bash
# Bash
curl -X POST http://localhost:3000/api/games/search/binary -H "Content-Type: application/json" -d '{"array":[10,20,30,40,50],"target":30,"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/search/binary" -Method POST -ContentType "application/json" -Body '{"array":[10,20,30,40,50],"target":30,"animate":true}'
```

#### 获取搜索状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/search/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/search/state" -Method GET
```

#### 重置搜索
```bash
# Bash
curl -X POST http://localhost:3000/api/games/search/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/search/reset" -Method POST
```

### 8. 迷宫算法API

#### 生成迷宫
```bash
# Bash
curl -X GET http://localhost:3000/api/games/maze/generate

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/maze/generate" -Method GET
```

#### DFS解迷宫
```bash
# Bash
curl -X POST http://localhost:3000/api/games/maze/solveDFS -H "Content-Type: application/json" -d '{"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/maze/solveDFS" -Method POST -ContentType "application/json" -Body '{"animate":true}'
```

#### BFS解迷宫
```bash
# Bash
curl -X POST http://localhost:3000/api/games/maze/solveBFS -H "Content-Type: application/json" -d '{"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/maze/solveBFS" -Method POST -ContentType "application/json" -Body '{"animate":true}'
```

#### A*解迷宫
```bash
# Bash
curl -X POST http://localhost:3000/api/games/maze/solveAStar -H "Content-Type: application/json" -d '{"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/maze/solveAStar" -Method POST -ContentType "application/json" -Body '{"animate":true}'
```

#### 获取迷宫状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/maze/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/maze/state" -Method GET
```

#### 重置迷宫
```bash
# Bash
curl -X POST http://localhost:3000/api/games/maze/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/maze/reset" -Method POST
```

### 9. 动态规划算法API

#### 斐波那契数列
```bash
# Bash
curl -X POST http://localhost:3000/api/games/dp/fibonacci -H "Content-Type: application/json" -d '{"n":10,"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/dp/fibonacci" -Method POST -ContentType "application/json" -Body '{"n":10,"animate":true}'
```

#### 背包问题
```bash
# Bash
curl -X POST http://localhost:3000/api/games/dp/knapsack -H "Content-Type: application/json" -d '{"weights":[1,3,4,5],"values":[1,4,5,7],"capacity":7,"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/dp/knapsack" -Method POST -ContentType "application/json" -Body '{"weights":[1,3,4,5],"values":[1,4,5,7],"capacity":7,"animate":true}'
```

#### 最长公共子序列
```bash
# Bash
curl -X POST http://localhost:3000/api/games/dp/lcs -H "Content-Type: application/json" -d '{"str1":"ABCDGH","str2":"AEDFHR","animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/dp/lcs" -Method POST -ContentType "application/json" -Body '{"str1":"ABCDGH","str2":"AEDFHR","animate":true}'
```

#### 获取动态规划状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/dp/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/dp/state" -Method GET
```

#### 重置动态规划
```bash
# Bash
curl -X POST http://localhost:3000/api/games/dp/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/dp/reset" -Method POST
```

### 10. 贪心算法API

#### 零钱兑换
```bash
# Bash
curl -X POST http://localhost:3000/api/games/greedy/coinChange -H "Content-Type: application/json" -d '{"coins":[1,5,10,25],"amount":36,"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/greedy/coinChange" -Method POST -ContentType "application/json" -Body '{"coins":[1,5,10,25],"amount":36,"animate":true}'
```

#### 活动安排
```bash
# Bash
curl -X POST http://localhost:3000/api/games/greedy/activitySelection -H "Content-Type: application/json" -d '{"start":[1,3,0,5,8,5],"finish":[2,4,6,7,9,9],"animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/greedy/activitySelection" -Method POST -ContentType "application/json" -Body '{"start":[1,3,0,5,8,5],"finish":[2,4,6,7,9,9],"animate":true}'
```

#### 哈夫曼编码
```bash
# Bash
curl -X POST http://localhost:3000/api/games/greedy/huffman -H "Content-Type: application/json" -d '{"text":"BCAADDDCCACACAC","animate":true}'

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/greedy/huffman" -Method POST -ContentType "application/json" -Body '{"text":"BCAADDDCCACACAC","animate":true}'
```

#### 获取贪心算法状态
```bash
# Bash
curl -X GET http://localhost:3000/api/games/greedy/state

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/greedy/state" -Method GET
```

#### 重置贪心算法
```bash
# Bash
curl -X POST http://localhost:3000/api/games/greedy/reset

# PowerShell
Invoke-WebRequest -Uri "http://localhost:3000/api/games/greedy/reset" -Method POST
```

## 使用动画参数

大多数算法操作都支持动画参数，可以通过以下方式使用：

```bash
# 带动画的操作，速度为8
curl -X POST http://localhost:3000/api/games/sorting/bubbleSort -H "Content-Type: application/json" -d '{"array":[5,3,8,1,2],"animate":true,"speed":8}'

# 单步执行模式
curl -X POST http://localhost:3000/api/games/sorting/bubbleSort -H "Content-Type: application/json" -d '{"array":[5,3,8,1,2],"step":true}'
```

## 响应格式

所有API返回的标准JSON响应格式如下：

```json
{
  "success": true,
  "data": {
    "state": {
      // 操作后的最新状态
    },
    "animation": {
      "frames": [
        // 动画帧序列，仅当animate=true时提供
      ],
      "totalSteps": 5,
      "speed": 5
    }
  }
}
```

## 故障排除

1. 如果遇到连接错误，请确保后端服务正在运行并监听3000端口
2. 如果收到400错误，可能是请求格式不正确或缺少必要参数
3. 如果收到500错误，服务器可能遇到内部错误，请查看服务器日志获取更多信息

如需更多帮助，请查看完整的API设计文档或联系开发团队。 