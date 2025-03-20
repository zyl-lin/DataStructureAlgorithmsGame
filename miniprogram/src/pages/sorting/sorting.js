Page({
  data: {
    currentLevel: 1,
    totalLevels: 3,
    currentAlgorithm: 'bubble',
    currentArray: [64, 34, 25, 12, 22, 11, 90],
    currentTask: null,
    algorithms: [
      { type: 'bubble', name: '冒泡排序', completed: false },
      { type: 'selection', name: '选择排序', completed: false },
      { type: 'insertion', name: '插入排序', completed: false },
      { type: 'quick', name: '快速排序', completed: false }
    ],
    tasks: [
      {
        id: 1,
        description: "理解冒泡排序和选择排序的工作原理，完成对 [64, 34, 25, 12, 22, 11, 90] 的排序",
        arrays: {
          default: [64, 34, 25, 12, 22, 11, 90]
        },
        targetAlgorithms: ['bubble', 'selection']
      },
      {
        id: 2,
        description: "使用插入排序和快速排序对 [38, 27, 43, 3, 9, 82, 10] 进行排序，观察速度差异",
        arrays: {
          default: [38, 27, 43, 3, 9, 82, 10]
        },
        targetAlgorithms: ['insertion', 'quick']
      },
      {
        id: 3,
        description: "尝试使用四种排序算法对更大的数据集 [55, 40, 93, 17, 28, 76, 39, 51, 62, 84, 12, 5] 进行排序，比较它们的性能",
        arrays: {
          default: [55, 40, 93, 17, 28, 76, 39, 51, 62, 84, 12, 5]
        },
        targetAlgorithms: ['bubble', 'selection', 'insertion', 'quick']
      }
    ]
  },

  onLoad: function () {
    this.initLevel(this.data.currentLevel);
  },

  // 初始化关卡
  initLevel: function (level) {
    const taskIndex = level - 1;
    if (taskIndex < 0 || taskIndex >= this.data.tasks.length) return;

    const task = this.data.tasks[taskIndex];
    const defaultArray = task.arrays.default;

    // 重置所有算法的完成状态
    const algorithms = this.data.algorithms.map(algo => {
      return { ...algo, completed: false };
    });

    this.setData({
      currentLevel: level,
      currentTask: task,
      currentArray: [...defaultArray],
      algorithms: algorithms,
      currentAlgorithm: task.targetAlgorithms[0] || 'bubble'
    });
  },

  // 排序完成事件处理
  onSortComplete: function (e) {
    const { algorithmType, isSorted } = e.detail;
    
    if (isSorted) {
      // 更新当前算法的完成状态
      const algorithms = this.data.algorithms.map(algo => {
        if (algo.type === algorithmType) {
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

  // 算法类型改变事件处理
  onAlgorithmChange: function (e) {
    this.setData({
      currentAlgorithm: e.detail.algorithmType
    });
  },

  // 重置当前关卡
  resetLevel: function () {
    this.initLevel(this.data.currentLevel);
  },

  // 上一关
  prevLevel: function () {
    if (this.data.currentLevel > 1) {
      this.initLevel(this.data.currentLevel - 1);
    }
  },

  // 下一关
  nextLevel: function () {
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
