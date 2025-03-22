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
      case 7: // 搜索算法
        this.initSearchGame()
        break
      case 8: // 动态规划
        this.initDPGame()
        break
      case 9: // 贪心算法
        this.initGreedyGame()
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
      currentHint: hint
    })
  },

  initMazeGame() {
    const level = this.data.currentLevel
    let maze
    let player = { x: 0, y: 0 }
    let task = ''
    let hint = ''
    
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
      currentHint: hint
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
      currentHint: hint
    })
  },
  
  initGraphGame() {
    const level = this.data.currentLevel
    let graph
    let task = ''
    let hint = ''
    
    if (level === 1) {
      graph = {
        nodes: ['A', 'B', 'C', 'D'],
        edges: [
          [1, 2], // A连接到B和C
          [0, 2], // B连接到A和C
          [0, 1, 3], // C连接到A、B和D
          [2]  // D连接到C
        ]
      }
      task = '从节点A开始执行深度优先搜索'
      hint = '选择DFS算法，然后选择起始节点A，观察遍历顺序'
    } else if (level === 2) {
      graph = {
        nodes: ['1', '2', '3', '4', '5'],
        edges: [
          [1, 3], // 1连接到2和4
          [0, 2], // 2连接到1和3
          [1, 4], // 3连接到2和5
          [0, 2, 4], // 4连接到1、3和5
          [2, 3]  // 5连接到3和4
        ]
      }
      task = '从节点1开始执行广度优先搜索'
      hint = '选择BFS算法，然后选择起始节点1，观察遍历顺序'
    } else if (level === 3) {
      graph = {
        nodes: ['S', 'A', 'B', 'C', 'D', 'E'],
        edges: [
          [1, 2], // S连接到A和B
          [0, 3], // A连接到S和C
          [0, 4], // B连接到S和D
          [1, 5], // C连接到A和E
          [2, 5], // D连接到B和E
          [3, 4]  // E连接到C和D
        ]
      }
      task = '找出从S到E的最短路径'
      hint = '选择最短路径算法，然后选择起点S和终点E，观察算法如何找出最短路径'
    }
    
    this.setData({
      gameType: 'graph',
      graph: graph,
      traversalMode: 'dfs',
      startNodeIndex: 0,
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: 'DFS' },
        { id: 2, name: 'BFS' },
        { id: 3, name: '最短路径' }
      ]
    })
  },

  // 搜索算法游戏初始化
  initSearchGame() {
    const level = this.data.currentLevel
    let array
    let task = ''
    let hint = ''
    
    if (level === 1) {
      array = [15, 23, 8, 42, 4, 16, 55]
      task = '使用线性搜索找出值为16的元素'
      hint = '选择线性搜索算法，然后设置目标值为16，观察搜索过程'
    } else if (level === 2) {
      array = [4, 8, 15, 16, 23, 42, 55]
      task = '使用二分搜索找出值为23的元素'
      hint = '选择二分搜索算法，然后设置目标值为23，观察搜索过程'
    } else if (level === 3) {
      array = [2, 5, 8, 12, 16, 23, 38, 56, 72, 91]
      task = '比较线性搜索和二分搜索的效率差异'
      hint = '分别使用线性搜索和二分搜索查找同一个元素，比较所需的步骤数'
    }
    
    this.setData({
      gameType: 'search',
      array: array,
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: '线性搜索' },
        { id: 2, name: '二分搜索' }
      ]
    })
  },
  
  // 动态规划游戏初始化
  initDPGame() {
    const level = this.data.currentLevel
    let task = ''
    let hint = ''
    
    if (level === 1) {
      task = '计算斐波那契数列第10项'
      hint = '选择斐波那契算法，设置n=10，观察动态规划如何优化计算'
    } else if (level === 2) {
      task = '解决背包问题'
      hint = '添加物品并设置背包容量，观察动态规划如何求解最优解'
    } else if (level === 3) {
      task = '求解最长公共子序列问题'
      hint = '输入两个字符串，使用动态规划计算它们的最长公共子序列'
    }
    
    this.setData({
      gameType: 'dp',
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: '斐波那契数列' },
        { id: 2, name: '背包问题' },
        { id: 3, name: '最长公共子序列' }
      ]
    })
  },
  
  // 贪心算法游戏初始化
  initGreedyGame() {
    const level = this.data.currentLevel
    let task = ''
    let hint = ''
    
    if (level === 1) {
      task = '使用贪心算法解决零钱兑换问题'
      hint = '设置需要兑换的金额和可用的硬币面值，观察贪心策略'
    } else if (level === 2) {
      task = '使用贪心算法解决活动安排问题'
      hint = '安排一系列活动，使尽可能多的活动能够进行'
    } else if (level === 3) {
      task = '使用贪心算法生成哈夫曼编码'
      hint = '输入字符及其频率，观察哈夫曼编码的构建过程'
    }
    
    this.setData({
      gameType: 'greedy',
      currentTask: task,
      currentHint: hint,
      operations: [
        { id: 1, name: '零钱兑换' },
        { id: 2, name: '活动安排' },
        { id: 3, name: '哈夫曼编码' }
      ]
    })
  },

  // 对不同游戏组件的事件回调统一处理
  onNodeTap(e) {
    console.log('Node tap:', e.detail)
  },
  
  onOperation(e) {
    console.log('Operation:', e.detail)
  },
  
  onMove(e) {
    console.log('Move:', e.detail)
  },
  
  onAlgorithmSelect(e) {
    console.log('Algorithm selected:', e.detail)
  },
  
  onSort(e) {
    console.log('Sort event:', e.detail)
  },
  
  onNodeSelect(e) {
    console.log('Node selected:', e.detail)
  },
  
  onTraversal(e) {
    console.log('Traversal:', e.detail)
  },
  
  onEnqueue(e) {
    console.log('Enqueue:', e.detail)
  },
  
  onDequeue(e) {
    console.log('Dequeue:', e.detail)
  },
  
  onOperationTap(e) {
    console.log('Operation tap:', e.detail)
  },
  
  // 搜索相关事件
  onSearchTypeChange(e) {
    console.log('Search type changed:', e.detail)
  },
  
  onTargetValueChange(e) {
    console.log('Target value changed:', e.detail)
  },
  
  // 动态规划相关事件
  onDPTypeChange(e) {
    console.log('DP type changed:', e.detail)
  },
  
  // 贪心算法相关事件
  onGreedyTypeChange(e) {
    console.log('Greedy type changed:', e.detail)
  },
  
  // 关卡完成处理
  onLevelComplete(e) {
    console.log('Level completed:', e.detail)
    
    // 各游戏类型可能有不同的完成条件，这里统一处理
    let isCompleted = false
    
    switch (this.data.gameType) {
      case 'linkedList':
      case 'maze':
      case 'queue':
      case 'binaryTree':
        isCompleted = e.detail.success === true
        break
      case 'sorting':
        isCompleted = e.detail.sorted === true
        break
      case 'graph':
        isCompleted = e.detail.completed === true
        break
      case 'search':
        isCompleted = e.detail.found !== undefined // 无论是否找到元素，只要搜索完成即算完成
        break
      case 'dp':
      case 'greedy':
        isCompleted = e.detail.completed === true
        break
      default:
        isCompleted = false
    }
    
    if (isCompleted) {
      // 显示完成提示
      wx.showToast({
        title: '关卡完成！',
        icon: 'success',
        duration: 2000
      })
      
      // 更新关卡完成状态
      this.setData({
        levelCompleted: true
      })
      
      // 保存游戏进度
      this.saveGameProgress()
    }
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
    
    // 如果当前是二叉树游戏，重置服务端的二叉树状态
    if (this.data.gameType === 'binaryTree') {
      const api = require('../../services/api')
      api.binaryTree.reset().catch(error => {
        console.error('重置二叉树状态失败:', error)
      })
    }
    
    // 如果当前是图遍历游戏，重置服务端的图遍历状态
    if (this.data.gameType === 'graph') {
      try {
        const api = require('../../services/api');
        api.graph.reset()
          .then(() => {
            console.log('重置图API状态成功');
          })
          .catch(err => {
            console.error('重置图API状态失败', err);
          });
      } catch (error) {
        console.error('图API重置出错:', error);
      }
    }
    
    // 如果当前是排序游戏，重置服务端的排序状态
    if (this.data.gameType === 'sorting') {
      const api = require('../../services/api')
      api.sorting.reset().catch(error => {
        console.error('重置排序状态失败:', error)
      })
    }
    
    // 如果当前是动态规划游戏，重置服务端的动态规划状态
    if (this.data.gameType === 'dp') {
      const api = require('../../services/api')
      api.dp.reset().catch(error => {
        console.error('重置动态规划状态失败:', error)
      })
    }
    
    // 如果当前是贪心算法游戏，重置服务端的贪心算法状态
    if (this.data.gameType === 'greedy') {
      const api = require('../../services/api')
      api.greedy.reset().catch(error => {
        console.error('重置贪心算法状态失败:', error)
      })
    }
    
    // 如果当前是搜索游戏，重置服务端的搜索状态
    if (this.data.gameType === 'search') {
      const api = require('../../services/api')
      api.search.reset().catch(error => {
        console.error('重置搜索状态失败:', error)
      })
    }
    
    // 如果当前是迷宫游戏，重置服务端的迷宫状态
    if (this.data.gameType === 'maze') {
      const api = require('../../services/api')
      api.maze.reset().catch(error => {
        console.error('重置迷宫状态失败:', error)
      })
    }
  }
}) 