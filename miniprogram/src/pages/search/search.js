Page({
  data: {
    currentLevel: 1,
    totalLevels: 3,
    currentArray: [],
    currentTask: null,
    algorithms: [
      { type: 'linear', name: '线性搜索', completed: false },
      { type: 'binary', name: '二分搜索', completed: false }
    ],
    tasks: [
      {
        id: 1,
        description: "使用线性搜索在未排序数组 [17, 3, 75, 42, 83, 90, 25, 56] 中查找指定元素",
        array: [17, 3, 75, 42, 83, 90, 25, 56],
        isSorted: false,
        defaultSearchType: 'linear',
        targetAlgorithms: ['linear']
      },
      {
        id: 2,
        description: "使用二分搜索在已排序数组 [3, 17, 25, 42, 56, 75, 83, 90] 中查找指定元素",
        array: [3, 17, 25, 42, 56, 75, 83, 90],
        isSorted: true,
        defaultSearchType: 'binary',
        targetAlgorithms: ['binary']
      },
      {
        id: 3,
        description: "比较线性搜索和二分搜索在大型已排序数组中的性能差异",
        array: [4, 8, 12, 16, 23, 28, 32, 37, 41, 45, 50, 55, 62, 67, 73, 78, 84, 90, 95, 99],
        isSorted: true,
        defaultSearchType: 'linear',
        targetAlgorithms: ['linear', 'binary']
      }
    ]
  },

  onLoad: function() {
    this.initLevel(this.data.currentLevel);
  },

  // 初始化关卡
  initLevel: function(level) {
    const taskIndex = level - 1;
    if (taskIndex < 0 || taskIndex >= this.data.tasks.length) return;

    const task = this.data.tasks[taskIndex];
    
    // 重置所有算法的完成状态
    const algorithms = this.data.algorithms.map(algo => {
      return { ...algo, completed: false };
    });

    this.setData({
      currentLevel: level,
      currentTask: task,
      currentArray: task.array.slice(),
      algorithms: algorithms
    });
  },

  // 搜索完成事件处理
  onSearchComplete: function(e) {
    const { searchType, result } = e.detail;
    
    if (result && (result.found || !result.found)) {
      // 更新当前算法的完成状态
      const algorithms = this.data.algorithms.map(algo => {
        if (algo.type === searchType) {
          return { ...algo, completed: true };
        }
        return algo;
      });

      this.setData({ algorithms });

      // 检查当前关卡所有目标算法是否都已完成
      const currentTask = this.data.currentTask;
      const targetAlgorithms = currentTask.targetAlgorithms;
      const allCompleted = targetAlgorithms.every(type => {
        const algorithm = algorithms.find(a => a.type === type);
        return algorithm && algorithm.completed;
      });

      if (allCompleted) {
        wx.showToast({
          title: '恭喜！本关卡完成',
          icon: 'success',
          duration: 2000
        });
      }
    }
  },

  // 重置当前关卡
  resetLevel: function() {
    this.initLevel(this.data.currentLevel);
  },

  // 上一关
  prevLevel: function() {
    if (this.data.currentLevel > 1) {
      this.initLevel(this.data.currentLevel - 1);
    }
  },

  // 下一关
  nextLevel: function() {
    if (this.data.currentLevel < this.data.totalLevels) {
      this.initLevel(this.data.currentLevel + 1);
    } else {
      wx.showToast({
        title: '已经是最后一关',
        icon: 'none',
        duration: 2000
      });
    }
  }
}); 