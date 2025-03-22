/**
 * API服务模块
 * 定义与后端交互的各种API接口调用函数
 */

const { get, post } = require('../utils/request');

/**
 * 二叉树游戏API部分
 */
const binaryTreeApi = {
  // 获取二叉树状态
  getState: () => {
    return get('/api/games/binarytree/state');
  },

  // 前序遍历操作
  traversePreOrder: (data = {}, animate = false, speed = 5) => {
    let url = '/api/games/binarytree/traversePreOrder';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 中序遍历操作
  traverseInOrder: (data = {}, animate = false, speed = 5) => {
    let url = '/api/games/binarytree/traverseInOrder';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 后序遍历操作
  traversePostOrder: (data = {}, animate = false, speed = 5) => {
    let url = '/api/games/binarytree/traversePostOrder';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 层序遍历操作
  traverseLevelOrder: (data = {}, animate = false, speed = 5) => {
    let url = '/api/games/binarytree/traverseLevelOrder';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 插入节点操作
  insert: (data = {}, animate = false, speed = 5) => {
    let url = '/api/games/binarytree/insert';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 重置二叉树状态
  reset: () => {
    return post('/api/games/binarytree/reset');
  }
};

/**
 * 队列游戏API部分
 */
const queueApi = {
  // 获取队列状态
  getState: () => {
    return get('/api/games/queue/state');
  },

  // 入队操作
  enqueue: (data, animate = false, speed = 5) => {
    let url = '/api/games/queue/enqueue';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 出队操作
  dequeue: (animate = false, speed = 5) => {
    let url = '/api/games/queue/dequeue';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url);
  },

  // 查看队首元素
  peek: () => {
    return get('/api/games/queue/peek');
  },

  // 重置队列状态
  reset: () => {
    return post('/api/games/queue/reset');
  }
};

/**
 * 栈游戏API部分
 */
const stackApi = {
  // 获取栈状态
  getState: () => {
    return get('/api/games/stack/state');
  },

  // 入栈操作
  push: (data, animate = false, speed = 5) => {
    let url = '/api/games/stack/push';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 出栈操作
  pop: (animate = false, speed = 5) => {
    let url = '/api/games/stack/pop';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url);
  },

  // 查看栈顶元素
  peek: () => {
    return get('/api/games/stack/peek');
  },

  // 重置栈状态
  reset: () => {
    return post('/api/games/stack/reset');
  }
};

/**
 * 图游戏API部分
 */
const graphApi = {
  // 获取图状态
  getState: () => {
    return get('/api/games/graph/state');
  },

  // DFS遍历
  traverseDFS: (data, animate = false, speed = 5) => {
    let url = '/api/games/graph/traverseDFS';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // BFS遍历
  traverseBFS: (data, animate = false, speed = 5) => {
    let url = '/api/games/graph/traverseBFS';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 路径查找
  findPath: (data, animate = false, speed = 5) => {
    let url = '/api/games/graph/findPath';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 重置图遍历状态
  reset: () => {
    return post('/api/games/graph/reset');
  }
};

/**
 * 排序游戏API部分
 */
const sortingApi = {
  // 获取排序状态
  getState: () => {
    return get('/api/games/sorting/state');
  },

  // 冒泡排序
  bubbleSort: (data, animate = false, speed = 5, step = false) => {
    let url = '/api/games/sorting/bubbleSort';
    const params = [];
    if (animate) params.push(`animate=true&speed=${speed}`);
    if (step) params.push('step=true');
    if (params.length > 0) {
      url += `?${params.join('&')}`;
    }
    return post(url, data);
  },

  // 选择排序
  selectionSort: (data, animate = false, speed = 5, step = false) => {
    let url = '/api/games/sorting/selectionSort';
    const params = [];
    if (animate) params.push(`animate=true&speed=${speed}`);
    if (step) params.push('step=true');
    if (params.length > 0) {
      url += `?${params.join('&')}`;
    }
    return post(url, data);
  },

  // 插入排序
  insertionSort: (data, animate = false, speed = 5, step = false) => {
    let url = '/api/games/sorting/insertionSort';
    const params = [];
    if (animate) params.push(`animate=true&speed=${speed}`);
    if (step) params.push('step=true');
    if (params.length > 0) {
      url += `?${params.join('&')}`;
    }
    return post(url, data);
  },

  // 快速排序
  quickSort: (data, animate = false, speed = 5, step = false) => {
    let url = '/api/games/sorting/quickSort';
    const params = [];
    if (animate) params.push(`animate=true&speed=${speed}`);
    if (step) params.push('step=true');
    if (params.length > 0) {
      url += `?${params.join('&')}`;
    }
    return post(url, data);
  },

  // 重置排序状态
  reset: () => {
    return post('/api/games/sorting/reset');
  }
};

/**
 * 迷宫游戏API部分
 */
const mazeApi = {
  // 获取迷宫状态
  getState: () => {
    return get('/api/games/maze/state');
  },

  // 生成新迷宫
  generate: (params = {}) => {
    return get('/api/games/maze/generate', params);
  },

  // DFS求解迷宫
  solveDFS: (data = {}, animate = false, speed = 5) => {
    let url = '/api/games/maze/solveDFS';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // BFS求解迷宫
  solveBFS: (data = {}, animate = false, speed = 5) => {
    let url = '/api/games/maze/solveBFS';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // A*求解迷宫
  solveAStar: (data = {}, animate = false, speed = 5) => {
    let url = '/api/games/maze/solveAStar';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },
  
  // 重置迷宫状态
  reset: () => {
    return post('/api/games/maze/reset');
  }
};

/**
 * 链表游戏API部分
 */
const linkedListApi = {
  // 获取链表状态
  getState: () => {
    return get('/api/games/linkedlist/state');
  },

  // 插入节点操作
  insert: (data, animate = false, speed = 5) => {
    let url = '/api/games/linkedlist/insert';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 删除节点操作
  delete: (data, animate = false, speed = 5) => {
    let url = '/api/games/linkedlist/delete';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 查找节点操作
  find: (data, animate = false, speed = 5) => {
    let url = '/api/games/linkedlist/find';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 链表反转操作
  reverse: (animate = false, speed = 5) => {
    let url = '/api/games/linkedlist/reverse';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url);
  },

  // 重置链表状态
  reset: () => {
    return post('/api/games/linkedlist/reset');
  }
};

/**
 * 搜索游戏API部分
 */
const searchApi = {
  // 获取搜索状态
  getState: () => {
    return get('/api/games/search/state');
  },

  // 线性搜索
  linear: (data, animate = false, speed = 5) => {
    let url = '/api/games/search/linear';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 二分搜索
  binary: (data, animate = false, speed = 5) => {
    let url = '/api/games/search/binary';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },
  
  // 重置搜索状态
  reset: () => {
    return post('/api/games/search/reset');
  }
};

/**
 * 动态规划游戏API部分
 */
const dpApi = {
  // 获取DP状态
  getState: () => {
    return get('/api/games/dp/state');
  },

  // 斐波那契数列
  fibonacci: (data, animate = false, speed = 5) => {
    let url = '/api/games/dp/fibonacci';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 背包问题
  knapsack: (data, animate = false, speed = 5) => {
    let url = '/api/games/dp/knapsack';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 最长公共子序列
  lcs: (data, animate = false, speed = 5) => {
    let url = '/api/games/dp/lcs';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },
  
  // 重置DP状态
  reset: () => {
    return post('/api/games/dp/reset');
  }
};

/**
 * 贪心算法游戏API部分
 */
const greedyApi = {
  // 获取贪心算法状态
  getState: () => {
    return get('/api/games/greedy/state');
  },

  // 零钱兑换
  coinChange: (data, animate = false, speed = 5) => {
    let url = '/api/games/greedy/coinChange';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 活动安排
  activitySelection: (data, animate = false, speed = 5) => {
    let url = '/api/games/greedy/activitySelection';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },

  // 哈夫曼编码
  huffman: (data, animate = false, speed = 5) => {
    let url = '/api/games/greedy/huffman';
    if (animate) {
      url += `?animate=true&speed=${speed}`;
    }
    return post(url, data);
  },
  
  // 重置贪心算法状态
  reset: () => {
    return post('/api/games/greedy/reset');
  }
};

// 导出所有API
module.exports = {
  binaryTree: binaryTreeApi,
  queue: queueApi,
  stack: stackApi,
  graph: graphApi,
  sorting: sortingApi,
  maze: mazeApi,
  linkedList: linkedListApi,
  search: searchApi,
  dp: dpApi,
  greedy: greedyApi
}; 