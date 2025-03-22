# 数据结构与算法游戏API使用示例

本文档提供了主要数据结构和算法API的使用示例，帮助开发者理解如何调用API并处理响应结果。

## 通用查询参数

大多数API支持以下查询参数：

- `animate=true|false` - 是否返回动画帧序列
- `speed=1-10` - 动画速度（1最慢，10最快）
- `step=true|false` - 是否单步执行

## 链表操作示例

### 插入节点

```javascript
// 请求
fetch('/api/games/linkedlist/insert', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ value: 42, position: 0, animate: true })
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "nodes": [
        { "id": 1, "value": 42, "next": null }
      ],
      "head": 1,
      "tail": 1,
      "length": 1
    },
    "animation": {
      "frames": [
        {
          "nodes": [{ "id": 1, "value": 42, "next": null, "status": "active" }],
          "head": 1,
          "tail": 1,
          "description": "创建新节点 42"
        },
        {
          "nodes": [{ "id": 1, "value": 42, "next": null, "status": "normal" }],
          "head": 1,
          "tail": 1,
          "description": "将新节点设为链表头"
        }
      ],
      "totalSteps": 2,
      "speed": 5
    }
  }
}
```

### 重置链表

```javascript
// 请求
fetch('/api/games/linkedlist/reset', {
  method: 'POST'
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "nodes": [],
      "head": null,
      "tail": null,
      "length": 0
    }
  }
}
```

## 二叉树操作示例

### 插入节点

```javascript
// 请求
fetch('/api/games/binarytree/insert', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ value: 50, animate: true })
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "nodes": [
        { "id": 1, "value": 50, "x": 400, "y": 50 }
      ],
      "edges": [],
      "root": 1,
      "tree": {
        "id": 1,
        "value": 50,
        "x": 400,
        "y": 50,
        "left": null,
        "right": null
      }
    },
    "animation": {
      "frames": [
        {
          "nodes": [{ "id": 1, "value": 50, "x": 400, "y": 50, "status": "active" }],
          "edges": [],
          "description": "创建根节点 50"
        },
        {
          "nodes": [{ "id": 1, "value": 50, "x": 400, "y": 50, "status": "normal" }],
          "edges": [],
          "description": "插入完成"
        }
      ],
      "totalSteps": 2,
      "speed": 5
    }
  }
}
```

### 中序遍历

```javascript
// 请求 
fetch('/api/games/binarytree/traverseInOrder', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ animate: true })
})

// 响应 (假设树已包含节点50, 30, 70)
{
  "success": true,
  "data": {
    "state": {
      "nodes": [
        { "id": 1, "value": 50, "x": 400, "y": 50 },
        { "id": 2, "value": 30, "x": 200, "y": 120 },
        { "id": 3, "value": 70, "x": 600, "y": 120 }
      ],
      "edges": [
        { "source": 1, "target": 2, "position": "left" },
        { "source": 1, "target": 3, "position": "right" }
      ],
      "root": 1,
      "tree": {
        "id": 1,
        "value": 50,
        "x": 400,
        "y": 50,
        "left": {
          "id": 2,
          "value": 30,
          "x": 200,
          "y": 120,
          "left": null,
          "right": null
        },
        "right": {
          "id": 3,
          "value": 70,
          "x": 600,
          "y": 120,
          "left": null,
          "right": null
        }
      },
      "traversal": {
        "result": [30, 50, 70],
        "type": "inorder"
      }
    },
    "animation": {
      "frames": [
        {
          "nodes": [
            { "id": 1, "value": 50, "x": 400, "y": 50, "status": "visiting" },
            { "id": 2, "value": 30, "x": 200, "y": 120, "status": "normal" },
            { "id": 3, "value": 70, "x": 600, "y": 120, "status": "normal" }
          ],
          "edges": [
            { "source": 1, "target": 2, "position": "left", "status": "normal" },
            { "source": 1, "target": 3, "position": "right", "status": "normal" }
          ],
          "description": "访问根节点 50"
        },
        // 更多动画帧...
      ],
      "totalSteps": 7,
      "speed": 5
    }
  }
}
```

## 图操作示例

### DFS遍历

```javascript
// 请求
fetch('/api/games/graph/traverseDFS', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ startVertex: "A", animate: true })
})

// 响应 (假设图已包含顶点A, B, C和相应的边)
{
  "success": true,
  "data": {
    "state": {
      "vertices": [
        { "id": "A", "x": 100, "y": 100 },
        { "id": "B", "x": 200, "y": 100 },
        { "id": "C", "x": 150, "y": 200 }
      ],
      "edges": [
        { "source": "A", "target": "B" },
        { "source": "A", "target": "C" },
        { "source": "B", "target": "C" }
      ],
      "traversal": {
        "result": ["A", "B", "C"],
        "type": "dfs"
      }
    },
    "animation": {
      "frames": [
        {
          "vertices": [
            { "id": "A", "x": 100, "y": 100, "status": "visiting" },
            { "id": "B", "x": 200, "y": 100, "status": "normal" },
            { "id": "C", "x": 150, "y": 200, "status": "normal" }
          ],
          "edges": [
            { "source": "A", "target": "B", "status": "normal" },
            { "source": "A", "target": "C", "status": "normal" },
            { "source": "B", "target": "C", "status": "normal" }
          ],
          "description": "从顶点 A 开始深度优先搜索"
        },
        // 更多动画帧...
      ],
      "totalSteps": 6,
      "speed": 5
    }
  }
}
```

## 排序算法示例

### 冒泡排序

```javascript
// 请求
fetch('/api/games/sorting/bubbleSort', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ 
    array: [5, 3, 8, 4, 2], 
    animate: true 
  })
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "array": [2, 3, 4, 5, 8],
      "originalArray": [5, 3, 8, 4, 2],
      "algorithm": "bubbleSort",
      "comparisons": 10,
      "swaps": 4,
      "completed": true
    },
    "animation": {
      "frames": [
        {
          "array": [5, 3, 8, 4, 2],
          "comparing": [0, 1],
          "description": "比较 5 和 3"
        },
        {
          "array": [3, 5, 8, 4, 2],
          "swapping": [0, 1],
          "description": "交换 5 和 3"
        },
        // 更多动画帧...
      ],
      "totalSteps": 14,
      "speed": 5
    }
  }
}
```

## 搜索算法示例

### 二分搜索

```javascript
// 请求
fetch('/api/games/search/binary', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ 
    array: [2, 3, 5, 7, 11, 13, 17], 
    target: 11,
    animate: true 
  })
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "array": [2, 3, 5, 7, 11, 13, 17],
      "target": 11,
      "found": true,
      "index": 4,
      "comparisons": 3,
      "algorithm": "binary"
    },
    "animation": {
      "frames": [
        {
          "array": [2, 3, 5, 7, 11, 13, 17],
          "left": 0,
          "right": 6,
          "mid": 3,
          "comparing": 7,
          "description": "比较中间元素 7 与目标值 11"
        },
        // 更多动画帧...
      ],
      "totalSteps": 3,
      "speed": 5
    }
  }
}
```

## 迷宫算法示例

### 生成迷宫

```javascript
// 请求
fetch('/api/games/maze/generate?width=10&height=10', {
  method: 'GET'
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "maze": [
        [1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
        [1, 0, 0, 0, 1, 0, 0, 0, 0, 1],
        // ...更多迷宫数据
      ],
      "width": 10,
      "height": 10,
      "start": { "x": 1, "y": 1 },
      "end": { "x": 8, "y": 8 },
      "algorithm": "dfs",
      "solved": false,
      "path": []
    }
  }
}
```

### 解迷宫 (BFS)

```javascript
// 请求
fetch('/api/games/maze/solveBFS', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ animate: true })
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "maze": [
        // ...迷宫数据
      ],
      "width": 10,
      "height": 10,
      "start": { "x": 1, "y": 1 },
      "end": { "x": 8, "y": 8 },
      "algorithm": "bfs",
      "solved": true,
      "path": [
        { "x": 1, "y": 1 },
        { "x": 2, "y": 1 },
        // ...路径数据
      ]
    },
    "animation": {
      "frames": [
        // ...动画帧数据
      ],
      "totalSteps": 24,
      "speed": 5
    }
  }
}
```

## 动态规划示例

### 斐波那契数列

```javascript
// 请求
fetch('/api/games/dp/fibonacci', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ 
    n: 6,
    animate: true 
  })
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "algorithm": "fibonacci",
      "input": 6,
      "result": 8,
      "table": [0, 1, 1, 2, 3, 5, 8],
      "calls": 6
    },
    "animation": {
      "frames": [
        {
          "table": [0, 1],
          "calculating": 2,
          "description": "计算 F(2) = F(1) + F(0)"
        },
        // 更多动画帧...
      ],
      "totalSteps": 6,
      "speed": 5
    }
  }
}
```

## 贪心算法示例

### 零钱兑换

```javascript
// 请求
fetch('/api/games/greedy/coinChange', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ 
    amount: 63,
    coins: [25, 10, 5, 1],
    animate: true 
  })
})

// 响应
{
  "success": true,
  "data": {
    "state": {
      "algorithm": "coinChange",
      "amount": 63,
      "coins": [25, 10, 5, 1],
      "result": {
        "25": 2,
        "10": 1,
        "5": 0,
        "1": 3
      },
      "totalCoins": 6
    },
    "animation": {
      "frames": [
        {
          "remainingAmount": 63,
          "currentCoin": 25,
          "description": "尝试使用面值 25 的硬币"
        },
        {
          "remainingAmount": 38,
          "coinsUsed": { "25": 1 },
          "description": "使用 1 个面值为 25 的硬币，剩余金额 38"
        },
        // 更多动画帧...
      ],
      "totalSteps": 8,
      "speed": 5
    }
  }
}
```

## 错误处理示例

当API请求出错时，会返回以下格式的错误响应：

```javascript
// 错误响应示例
{
  "success": false,
  "error": {
    "message": "无效的输入参数",
    "code": 400,
    "details": "数组不能为空",
    "module": "sorting"
  }
}
```

常见错误代码：
- 400: 错误的请求参数
- 404: 资源不存在
- 500: 服务器内部错误

## 使用curl测试API

以下是使用curl命令测试API的示例：

```bash
# 重置二叉树
curl -X POST http://localhost:3000/api/games/binarytree/reset

# 插入节点
curl -X POST http://localhost:3000/api/games/binarytree/insert \
  -H "Content-Type: application/json" \
  -d '{"value": 50}'

# 获取状态
curl http://localhost:3000/api/games/binarytree/state

# 前序遍历
curl -X POST http://localhost:3000/api/games/binarytree/traversePreOrder \
  -H "Content-Type: application/json" \
  -d '{"animate": true}'
```

## 使用PowerShell测试API

```powershell
# 重置二叉树
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/reset" -Method POST

# 插入节点
$body = @{
    value = 50
} | ConvertTo-Json
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/insert" -Method POST -Body $body -ContentType "application/json"

# 获取状态
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/state" -Method GET

# 前序遍历
$body = @{
    animate = $true
} | ConvertTo-Json
Invoke-WebRequest -Uri "http://localhost:3000/api/games/binarytree/traversePreOrder" -Method POST -Body $body -ContentType "application/json"
``` 