const app = getApp()

Page({
  data: {
    gameId: null,
    gameName: '',
    gameType: '',
    currentLevel: 1,
    totalLevels: 10,
    showHint: false,
    currentHint: '',
    operations: [],
    nodes: [],
    maze: [],
    player: { x: 0, y: 0 },
    array: []
  },

  onLoad(options) {
    const { id, name } = options
    this.setData({
      gameId: parseInt(id),
      gameName: name
    })
    this.initGame()
  },

  initGame() {
    // 根据游戏类型初始化不同的游戏
    switch (this.data.gameId) {
      case 1: // 链表操作
        this.initLinkedListGame()
        break
      case 3: // 迷宫寻路
        this.initMazeGame()
        break
      case 4: // 排序可视化
        this.initSortingGame()
        break
    }
  },

  initLinkedListGame() {
    this.setData({
      gameType: 'linkedList',
      operations: [
        { id: 1, name: '插入节点' },
        { id: 2, name: '删除节点' },
        { id: 3, name: '查找节点' },
        { id: 4, name: '反转链表' }
      ],
      nodes: [
        { id: 1, value: 1, next: 2 },
        { id: 2, value: 2, next: 3 },
        { id: 3, value: 3, next: null }
      ]
    })
  },

  initMazeGame() {
    this.setData({
      gameType: 'maze',
      maze: this.generateMaze(),
      player: { x: 0, y: 0 }
    })
  },

  initSortingGame() {
    this.setData({
      gameType: 'sorting',
      array: this.generateRandomArray(10),
      operations: [
        { id: 1, name: '冒泡排序' },
        { id: 2, name: '快速排序' },
        { id: 3, name: '归并排序' }
      ]
    })
  },

  generateMaze() {
    // 生成一个简单的迷宫
    return [
      [1, 1, 1, 1, 1],
      [1, 0, 0, 0, 1],
      [1, 0, 1, 0, 1],
      [1, 0, 0, 0, 1],
      [1, 1, 1, 1, 1]
    ]
  },

  generateRandomArray(length) {
    return Array.from({ length }, () => Math.floor(Math.random() * 100))
  },

  onNodeTap(e) {
    const { node } = e.detail
    // 处理节点点击事件
  },

  onOperation(e) {
    const { operation } = e.detail
    // 处理操作事件
  },

  onMove(e) {
    const { direction } = e.detail
    // 处理移动事件
  },

  onAlgorithmSelect(e) {
    const { algorithm } = e.detail
    // 处理算法选择事件
  },

  onSort(e) {
    const { algorithm } = e.detail
    // 处理排序事件
  },

  onOperationTap(e) {
    const { operation } = e.currentTarget.dataset
    // 处理操作按钮点击事件
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
    if (this.data.currentLevel < this.data.totalLevels) {
      this.setData({
        currentLevel: this.data.currentLevel + 1
      })
      this.initGame()
    }
  },

  showHint() {
    this.setData({
      showHint: true,
      currentHint: '这是一个提示信息'
    })
  },

  closeHint() {
    this.setData({
      showHint: false
    })
  }
}) 