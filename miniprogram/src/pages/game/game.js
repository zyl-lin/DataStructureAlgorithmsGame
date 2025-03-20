const app = getApp()

Page({
  data: {
    gameId: null,
    gameName: '',
    gameType: '',
    currentLevel: 1,
    totalLevels: 10,
    levelCompleted: false,
    currentTask: '',
    showHint: false,
    currentHint: '',
    operations: [],
    nodes: [],
    maze: [],
    player: { x: 0, y: 0 },
    array: [],
    queue: [],
    tree: null,
    graph: null,
    progressData: {}
  },

  onLoad(options) {
    const { id, name } = options
    this.setData({
      gameId: parseInt(id),
      gameName: name
    })
    this.initGame()
    
    // 获取用户游戏进度
    this.getGameProgress()
  },
  
  // 获取游戏进度
  getGameProgress() {
    // 从本地存储获取游戏进度数据
    const progressData = wx.getStorageSync('gameProgress') || {}
    const gameProgress = progressData[this.data.gameId] || { currentLevel: 1, completedLevels: [] }
    
    this.setData({
      currentLevel: gameProgress.currentLevel,
      progressData: progressData
    })
  },
  
  // 保存游戏进度
  saveGameProgress() {
    let progressData = this.data.progressData
    
    // 确保有当前游戏的进度记录
    if (!progressData[this.data.gameId]) {
      progressData[this.data.gameId] = { 
        currentLevel: this.data.currentLevel, 
        completedLevels: [] 
      }
    }
    
    // 如果完成了当前关卡，记录到已完成列表中
    if (this.data.levelCompleted && 
        !progressData[this.data.gameId].completedLevels.includes(this.data.currentLevel)) {
      progressData[this.data.gameId].completedLevels.push(this.data.currentLevel)
    }
    
    // 更新当前关卡
    progressData[this.data.gameId].currentLevel = this.data.currentLevel
    
    // 保存到本地存储
    wx.setStorageSync('gameProgress', progressData)
    
    // 记录学习时间和进度
    this.recordLearningActivity()
  },
  
  // 记录学习活动
  recordLearningActivity() {
    // 获取当前活动记录
    const learningHistory = wx.getStorageSync('learningHistory') || []
    
    // 创建新记录
    const newRecord = {
      type: 'game',
      gameId: this.data.gameId,
      gameName: this.data.gameName,
      gameType: this.data.gameType,
      level: this.data.currentLevel,
      date: new Date().toISOString(),
      durationMinutes: 5 // 假设平均花费5分钟
    }
    
    // 添加到记录
    learningHistory.unshift(newRecord)
    
    // 如果记录过多，只保留最近100条
    if (learningHistory.length > 100) {
      learningHistory.splice(100)
    }
    
    // 保存记录
    wx.setStorageSync('learningHistory', learningHistory)
  },

  initGame() {
    // 重置关卡完成状态
    this.setData({
      levelCompleted: false
    })
    
    // 根据游戏类型初始化不同的游戏
    switch (this.data.gameId) {
      case 1: // 链表操作
        this.initLinkedListGame()
        break
      case 2: // 栈与队列
        this.initQueueGame()
        break
      case 3: // 迷宫寻路
        this.initMazeGame()
        break
      case 4: // 排序可视化
        this.initSortingGame()
        break
      case 5: // 二叉树遍历
        this.initBinaryTreeGame()
        break
      case 6: // 图遍历
        this.initGraphGame()
        break
    }
  },

  initLinkedListGame() {
    const level = this.data.currentLevel
    let nodes = []
    let task = ''
    let hint = ''
    
    if (level === 1) {
      nodes = [
        { id: 1, value: 10, next: 2 },
        { id: 2, value: 20, next: 3 },
        { id: 3, value: 30, next: null }
      ]
      task = '在链表尾部添加一个新节点'
      hint = '点击"插入节点"按钮，然后选择插入位置为链表尾部，并输入节点值'
    } else if (level === 2) {
      nodes = [
        { id: 1, value: 5, next: 2 },
        { id: 2, value: 10, next: 3 },
        { id: 3, value: 15, next: 4 },
        { id: 4, value: 20, next: null }
      ]
      task = '删除值为15的节点'
      hint = '点击"删除节点"按钮，然后选择要删除的节点（值为15）'
    } else if (level === 3) {
      nodes = [
        { id: 1, value: 8, next: 2 },
        { id: 2, value: 12, next: 3 },
        { id: 3, value: 16, next: 4 },
        { id: 4, value: 20, next: 5 },
        { id: 5, value: 24, next: null }
      ]
      task = '将链表反转'
      hint = '点击"反转链表"按钮，系统会自动反转整个链表'
    }
    
    this.setData({
      gameType: 'linkedList',
      nodes: nodes,
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: '插入节点' },
        { id: 2, name: '删除节点' },
        { id: 3, name: '查找节点' },
        { id: 4, name: '反转链表' }
      ]
    })
  },
  
  initQueueGame() {
    const level = this.data.currentLevel
    let queue = []
    let task = ''
    let hint = ''
    
    if (level === 1) {
      queue = []
      task = '添加5个元素到队列中'
      hint = '点击"入队"按钮，依次添加5个元素到队列'
    } else if (level === 2) {
      queue = [10, 20, 30, 40, 50]
      task = '从队列中移除3个元素'
      hint = '点击"出队"按钮3次，观察先进先出的特性'
    } else if (level === 3) {
      queue = [5, 10, 15, 20]
      task = '入队3个元素，然后出队2个元素'
      hint = '先点击"入队"按钮3次，然后点击"出队"按钮2次'
    }
    
    this.setData({
      gameType: 'queue',
      queue: queue,
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: '入队' },
        { id: 2, name: '出队' },
        { id: 3, name: '查看队首' }
      ]
    })
  },

  initMazeGame() {
    const level = this.data.currentLevel
    let maze
    let player = { x: 0, y: 0 }
    let task = ''
    let hint = ''
    
    // 根据不同关卡生成不同的迷宫
    if (level === 1) {
      maze = [
        [0, 0, 1, 1, 1],
        [1, 0, 0, 0, 1],
        [1, 1, 1, 0, 1],
        [1, 0, 0, 0, 0],
        [1, 1, 1, 1, 0]
      ]
      task = '使用深度优先搜索(DFS)找到迷宫出口'
      hint = '选择DFS算法，然后点击开始，观察算法如何找到路径'
    } else if (level === 2) {
      maze = [
        [0, 0, 0, 1, 1],
        [1, 1, 0, 0, 1],
        [1, 0, 1, 0, 0],
        [1, 0, 1, 1, 0],
        [1, 0, 0, 0, 0]
      ]
      task = '使用广度优先搜索(BFS)找到迷宫出口'
      hint = '选择BFS算法，然后点击开始，观察算法如何找到最短路径'
    } else if (level === 3) {
      maze = [
        [0, 0, 1, 1, 1, 1, 1],
        [1, 0, 0, 0, 0, 0, 1],
        [1, 1, 1, 1, 1, 0, 1],
        [1, 0, 0, 0, 0, 0, 1],
        [1, 0, 1, 1, 1, 1, 1],
        [1, 0, 0, 0, 0, 0, 0],
        [1, 1, 1, 1, 1, 1, 0]
      ]
      task = '使用A*算法找到迷宫出口'
      hint = '选择A*算法，然后点击开始，观察算法如何找到最优路径'
    }
    
    this.setData({
      gameType: 'maze',
      maze: maze,
      player: player,
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: 'DFS' },
        { id: 2, name: 'BFS' },
        { id: 3, name: 'A*' }
      ]
    })
  },

  initSortingGame() {
    const level = this.data.currentLevel
    let array
    let task = ''
    let hint = ''
    
    if (level === 1) {
      array = [64, 34, 25, 12, 22, 11, 90]
      task = '用冒泡排序对数组进行排序'
      hint = '选择冒泡排序算法，然后点击开始，观察元素如何交换位置'
    } else if (level === 2) {
      array = [38, 27, 43, 3, 9, 82, 10]
      task = '用快速排序对数组进行排序'
      hint = '选择快速排序算法，然后点击开始，观察分区操作如何进行'
    } else if (level === 3) {
      array = [5, 2, 4, 6, 1, 3, 9, 7, 10, 8]
      task = '比较不同排序算法的效率'
      hint = '分别选择不同排序算法，观察它们的执行过程和所需步骤数'
    }
    
    this.setData({
      gameType: 'sorting',
      array: array,
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: '冒泡排序' },
        { id: 2, name: '选择排序' },
        { id: 3, name: '插入排序' },
        { id: 4, name: '快速排序' }
      ]
    })
  },
  
  initBinaryTreeGame() {
    const level = this.data.currentLevel
    let tree
    let task = ''
    let hint = ''
    
    if (level === 1) {
      tree = {
        value: 10,
        left: {
          value: 5,
          left: { value: 3, left: null, right: null },
          right: { value: 7, left: null, right: null }
        },
        right: {
          value: 15,
          left: { value: 12, left: null, right: null },
          right: { value: 18, left: null, right: null }
        }
      }
      task = '执行二叉树的前序遍历'
      hint = '选择前序遍历，然后点击开始。前序遍历的顺序是：根-左-右'
    } else if (level === 2) {
      tree = {
        value: 8,
        left: {
          value: 4,
          left: { value: 2, left: null, right: null },
          right: { value: 6, left: null, right: null }
        },
        right: {
          value: 12,
          left: { value: 10, left: null, right: null },
          right: { value: 14, left: null, right: null }
        }
      }
      task = '执行二叉树的中序遍历'
      hint = '选择中序遍历，然后点击开始。中序遍历的顺序是：左-根-右'
    } else if (level === 3) {
      tree = {
        value: 20,
        left: {
          value: 10,
          left: { value: 5, left: null, right: null },
          right: { value: 15, left: null, right: null }
        },
        right: {
          value: 30,
          left: { value: 25, left: null, right: null },
          right: { value: 35, left: null, right: null }
        }
      }
      task = '执行二叉树的后序遍历'
      hint = '选择后序遍历，然后点击开始。后序遍历的顺序是：左-右-根'
    }
    
    this.setData({
      gameType: 'binaryTree',
      tree: tree,
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: '前序遍历' },
        { id: 2, name: '中序遍历' },
        { id: 3, name: '后序遍历' },
        { id: 4, name: '层序遍历' }
      ]
    })
  },
  
  initGraphGame() {
    const level = this.data.currentLevel
    let graph
    let task = ''
    let hint = ''
    
    if (level === 1) {
      graph = {
        nodes: [
          { id: 'A', x: 100, y: 100 },
          { id: 'B', x: 200, y: 50 },
          { id: 'C', x: 300, y: 100 },
          { id: 'D', x: 200, y: 200 }
        ],
        edges: [
          { source: 'A', target: 'B' },
          { source: 'A', target: 'C' },
          { source: 'A', target: 'D' },
          { source: 'B', target: 'C' },
          { source: 'D', target: 'C' }
        ]
      }
      task = '从节点A开始执行深度优先搜索'
      hint = '选择DFS算法，然后选择起始节点A，观察遍历顺序'
    } else if (level === 2) {
      graph = {
        nodes: [
          { id: '1', x: 100, y: 100 },
          { id: '2', x: 200, y: 50 },
          { id: '3', x: 300, y: 100 },
          { id: '4', x: 200, y: 200 },
          { id: '5', x: 350, y: 200 }
        ],
        edges: [
          { source: '1', target: '2' },
          { source: '1', target: '4' },
          { source: '2', target: '3' },
          { source: '3', target: '5' },
          { source: '4', target: '3' },
          { source: '4', target: '5' }
        ]
      }
      task = '从节点1开始执行广度优先搜索'
      hint = '选择BFS算法，然后选择起始节点1，观察遍历顺序'
    } else if (level === 3) {
      graph = {
        nodes: [
          { id: 'S', x: 50, y: 150 },
          { id: 'A', x: 150, y: 50 },
          { id: 'B', x: 150, y: 250 },
          { id: 'C', x: 250, y: 50 },
          { id: 'D', x: 250, y: 250 },
          { id: 'E', x: 350, y: 150 }
        ],
        edges: [
          { source: 'S', target: 'A', weight: 1 },
          { source: 'S', target: 'B', weight: 2 },
          { source: 'A', target: 'C', weight: 3 },
          { source: 'B', target: 'D', weight: 1 },
          { source: 'C', target: 'E', weight: 2 },
          { source: 'D', target: 'E', weight: 1 }
        ]
      }
      task = '找出从S到E的最短路径'
      hint = '选择最短路径算法，然后选择起点S和终点E，观察算法如何找出最短路径'
    }
    
    this.setData({
      gameType: 'graph',
      graph: graph,
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: 'DFS' },
        { id: 2, name: 'BFS' },
        { id: 3, name: '最短路径' }
      ]
    })
  },

  onNodeTap(e) {
    const { node } = e.detail
    // 处理节点点击事件
    console.log('Node tapped:', node)
  },

  onOperation(e) {
    const { operation, data } = e.detail
    // 处理操作事件
    console.log('Operation:', operation, 'Data:', data)
  },

  onMove(e) {
    const { direction } = e.detail
    // 处理移动事件
    console.log('Move direction:', direction)
  },

  onAlgorithmSelect(e) {
    const { algorithm } = e.detail
    // 处理算法选择事件
    console.log('Algorithm selected:', algorithm)
  },

  onSort(e) {
    const { algorithm } = e.detail
    // 处理排序事件
    console.log('Sort algorithm:', algorithm)
  },
  
  onNodeSelect(e) {
    const { nodeId } = e.detail
    // 处理节点选择事件
    console.log('Node selected:', nodeId)
  },
  
  onTraversal(e) {
    const { type, path } = e.detail
    // 处理遍历事件
    console.log('Traversal type:', type, 'Path:', path)
  },
  
  onEnqueue(e) {
    const { value } = e.detail
    // 处理入队事件
    console.log('Enqueued value:', value)
  },
  
  onDequeue(e) {
    // 处理出队事件
    console.log('Dequeued')
  },

  onOperationTap(e) {
    const { operation } = e.currentTarget.dataset
    // 处理操作按钮点击事件
    console.log('Operation button tapped:', operation)
  },
  
  onLevelComplete(e) {
    // 标记关卡完成
    this.setData({
      levelCompleted: true
    })
    
    // 保存游戏进度
    this.saveGameProgress()
    
    // 提示用户已完成关卡
    wx.showToast({
      title: '关卡完成！',
      icon: 'success'
    })
  },

  prevLevel() {
    if (this.data.currentLevel > 1) {
      this.setData({
        currentLevel: this.data.currentLevel - 1
      })
      this.initGame()
    }
  },

  nextLevel() {
    if (this.data.currentLevel < this.data.totalLevels && this.data.levelCompleted) {
      this.setData({
        currentLevel: this.data.currentLevel + 1
      })
      this.initGame()
      
      // 保存游戏进度
      this.saveGameProgress()
    }
  },

  showHint() {
    this.setData({
      showHint: true
    })
  },

  closeHint() {
    this.setData({
      showHint: false
    })
  },
  
  onUnload() {
    // 页面卸载时保存游戏进度
    this.saveGameProgress()
  }
}) 