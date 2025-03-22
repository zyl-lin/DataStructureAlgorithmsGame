# 数据结构与算法游戏API结构总览

## API基本结构

所有API都遵循以下基本模式：
- `/api/games/{数据结构或算法类型}/{操作}`

## API分类表

| 分类 | 数据结构/算法 | 操作类型 | API端点 | 请求方法 | 说明 |
|------|--------------|----------|---------|----------|------|
| **数据结构** | 链表 | 插入 | `/api/games/linkedlist/insert` | POST | 插入节点 |
| | | 删除 | `/api/games/linkedlist/delete` | POST | 删除节点 |
| | | 查找 | `/api/games/linkedlist/find` | POST | 查找节点 |
| | | 反转 | `/api/games/linkedlist/reverse` | POST | 反转链表 |
| | | 获取状态 | `/api/games/linkedlist/state` | GET | 获取当前链表状态 |
| | | 重置 | `/api/games/linkedlist/reset` | POST | 重置链表状态 |
| | 队列 | 入队 | `/api/games/queue/enqueue` | POST | 入队操作 |
| | | 出队 | `/api/games/queue/dequeue` | POST | 出队操作 |
| | | 查看队首 | `/api/games/queue/peek` | GET | 查看队首元素 |
| | | 获取状态 | `/api/games/queue/state` | GET | 获取当前队列状态 |
| | | 重置 | `/api/games/queue/reset` | POST | 重置队列状态 |
| | 栈 | 入栈 | `/api/games/stack/push` | POST | 入栈操作 |
| | | 出栈 | `/api/games/stack/pop` | POST | 出栈操作 |
| | | 查看栈顶 | `/api/games/stack/peek` | GET | 查看栈顶元素 |
| | | 获取状态 | `/api/games/stack/state` | GET | 获取当前栈状态 |
| | | 重置 | `/api/games/stack/reset` | POST | 重置栈状态 |
| | 二叉树 | 插入 | `/api/games/binarytree/insert` | POST | 插入节点 |
| | | 前序遍历 | `/api/games/binarytree/traversePreOrder` | POST | 前序遍历二叉树 |
| | | 中序遍历 | `/api/games/binarytree/traverseInOrder` | POST | 中序遍历二叉树 |
| | | 后序遍历 | `/api/games/binarytree/traversePostOrder` | POST | 后序遍历二叉树 |
| | | 层序遍历 | `/api/games/binarytree/traverseLevelOrder` | POST | 层序遍历二叉树 |
| | | 获取状态 | `/api/games/binarytree/state` | GET | 获取当前二叉树状态 |
| | | 重置 | `/api/games/binarytree/reset` | POST | 重置二叉树状态 |
| | 图 | DFS遍历 | `/api/games/graph/traverseDFS` | POST | 深度优先遍历图 |
| | | BFS遍历 | `/api/games/graph/traverseBFS` | POST | 广度优先遍历图 |
| | | 查找路径 | `/api/games/graph/findPath` | POST | 查找两节点间路径 |
| | | 获取状态 | `/api/games/graph/state` | GET | 获取当前图状态 |
| | | 重置 | `/api/games/graph/reset` | POST | 重置图状态 |
| **算法** | 排序 | 冒泡排序 | `/api/games/sorting/bubbleSort` | POST | 执行冒泡排序 |
| | | 选择排序 | `/api/games/sorting/selectionSort` | POST | 执行选择排序 |
| | | 插入排序 | `/api/games/sorting/insertionSort` | POST | 执行插入排序 |
| | | 快速排序 | `/api/games/sorting/quickSort` | POST | 执行快速排序 |
| | | 获取状态 | `/api/games/sorting/state` | GET | 获取当前排序状态 |
| | | 重置 | `/api/games/sorting/reset` | POST | 重置排序状态 |
| | 搜索 | 线性搜索 | `/api/games/search/linear` | POST | 执行线性搜索 |
| | | 二分搜索 | `/api/games/search/binary` | POST | 执行二分搜索 |
| | | 获取状态 | `/api/games/search/state` | GET | 获取当前搜索状态 |
| | | 重置 | `/api/games/search/reset` | POST | 重置搜索状态 |
| | 迷宫 | 生成迷宫 | `/api/games/maze/generate` | GET | 生成新迷宫 |
| | | DFS解迷宫 | `/api/games/maze/solveDFS` | POST | 使用DFS解迷宫 |
| | | BFS解迷宫 | `/api/games/maze/solveBFS` | POST | 使用BFS解迷宫 |
| | | A*解迷宫 | `/api/games/maze/solveAStar` | POST | 使用A*解迷宫 |
| | | 获取状态 | `/api/games/maze/state` | GET | 获取当前迷宫状态 |
| | | 重置 | `/api/games/maze/reset` | POST | 重置迷宫状态 |
| | 动态规划 | 斐波那契 | `/api/games/dp/fibonacci` | POST | 计算斐波那契数列 |
| | | 背包问题 | `/api/games/dp/knapsack` | POST | 执行背包问题 |
| | | 最长公共子序列 | `/api/games/dp/lcs` | POST | 执行最长公共子序列 |
| | | 获取状态 | `/api/games/dp/state` | GET | 获取当前DP状态 |
| | | 重置 | `/api/games/dp/reset` | POST | 重置DP状态 |
| | 贪心算法 | 零钱兑换 | `/api/games/greedy/coinChange` | POST | 执行零钱兑换 |
| | | 活动安排 | `/api/games/greedy/activitySelection` | POST | 执行活动安排 |
| | | 哈夫曼编码 | `/api/games/greedy/huffman` | POST | 执行哈夫曼编码 |
| | | 获取状态 | `/api/games/greedy/state` | GET | 获取当前贪心算法状态 |
| | | 重置 | `/api/games/greedy/reset` | POST | 重置贪心算法状态 |

## 通用参数

大多数POST请求支持以下通用参数：

| 参数名 | 类型 | 说明 |
|--------|------|------|
| animate | boolean | 控制是否返回动画帧序列，默认为false |
| speed | number | 当animate=true时控制动画速度，取值1-10，默认为5 |
| step | boolean | 是否单步执行，默认为false |

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

## API设计原则

1. **一致性**: 所有API都遵循相同的设计风格和响应格式
2. **直观性**: API路径结构反映了操作的层次关系
3. **功能完整性**: 每种数据结构和算法都提供完整的操作集
4. **状态管理**: 所有类型都提供状态查询和重置功能
5. **可视化支持**: 所有算法操作都支持动画和单步执行模式

这种设计使前端能够灵活地构建交互式的数据结构和算法可视化界面，同时保持代码的可维护性和一致性。 