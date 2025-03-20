Page({
  data: {
    currentLevel: 1,
    totalLevels: 3,
    traversalMode: 'dfs', // 'dfs' 或 'bfs'
    startNodeIndex: 0,
    graphData: null,
    correctSequence: [],
    allTraversalsComplete: {
      dfs: false,
      bfs: false
    },
    tasks: [
      {
        id: 1,
        description: "在这个简单的图中，尝试使用DFS和BFS遍历所有节点",
        allowStartNodeChange: true,
        graph: {
          nodes: ["A", "B", "C", "D", "E"],
          edges: [
            [1, 2], // A 连接到 B, C
            [0, 3], // B 连接到 A, D
            [0, 4], // C 连接到 A, E
            [1, 4], // D 连接到 B, E
            [2, 3]  // E 连接到 C, D
          ]
        },
        sequences: {
          dfs: {
            0: ["A", "B", "D", "E", "C"], // 从A开始的DFS序列
            1: ["B", "A", "C", "E", "D"], // 从B开始的DFS序列
            2: ["C", "A", "B", "D", "E"], // 从C开始的DFS序列
            3: ["D", "B", "A", "C", "E"], // 从D开始的DFS序列
            4: ["E", "C", "A", "B", "D"]  // 从E开始的DFS序列
          },
          bfs: {
            0: ["A", "B", "C", "D", "E"], // 从A开始的BFS序列
            1: ["B", "A", "D", "C", "E"], // 从B开始的BFS序列
            2: ["C", "A", "E", "B", "D"], // 从C开始的BFS序列
            3: ["D", "B", "E", "A", "C"], // 从D开始的BFS序列
            4: ["E", "C", "D", "A", "B"]  // 从E开始的BFS序列
          }
        }
      },
      {
        id: 2,
        description: "在这个网状图中，寻找不同遍历方式的路径",
        allowStartNodeChange: true,
        graph: {
          nodes: ["A", "B", "C", "D", "E", "F"],
          edges: [
            [1, 2, 3], // A 连接到 B, C, D
            [0, 2, 4], // B 连接到 A, C, E
            [0, 1, 5], // C 连接到 A, B, F
            [0, 4, 5], // D 连接到 A, E, F
            [1, 3, 5], // E 连接到 B, D, F
            [2, 3, 4]  // F 连接到 C, D, E
          ]
        },
        sequences: {
          dfs: {
            0: ["A", "B", "C", "F", "E", "D"], // 从A开始的DFS序列
            1: ["B", "A", "C", "F", "E", "D"], // 从B开始的DFS序列
            2: ["C", "A", "B", "E", "F", "D"], // 从C开始的DFS序列
            3: ["D", "A", "B", "C", "F", "E"], // 从D开始的DFS序列
            4: ["E", "B", "A", "C", "F", "D"], // 从E开始的DFS序列
            5: ["F", "C", "A", "B", "E", "D"]  // 从F开始的DFS序列
          },
          bfs: {
            0: ["A", "B", "C", "D", "E", "F"], // 从A开始的BFS序列
            1: ["B", "A", "C", "E", "D", "F"], // 从B开始的BFS序列
            2: ["C", "A", "B", "F", "D", "E"], // 从C开始的BFS序列
            3: ["D", "A", "E", "F", "B", "C"], // 从D开始的BFS序列
            4: ["E", "B", "D", "F", "A", "C"], // 从E开始的BFS序列
            5: ["F", "C", "D", "E", "A", "B"]  // 从F开始的BFS序列
          }
        }
      },
      {
        id: 3,
        description: "这个树状图具有明显不同的DFS和BFS遍历路径",
        allowStartNodeChange: false,
        graph: {
          nodes: ["A", "B", "C", "D", "E", "F", "G", "H"],
          edges: [
            [1, 2, 3], // A 连接到 B, C, D
            [0, 4, 5], // B 连接到 A, E, F
            [0, 6],    // C 连接到 A, G
            [0, 7],    // D 连接到 A, H
            [1],       // E 连接到 B
            [1],       // F 连接到 B
            [2],       // G 连接到 C
            [3]        // H 连接到 D
          ]
        },
        sequences: {
          dfs: {
            0: ["A", "B", "E", "F", "C", "G", "D", "H"] // 从A开始的DFS序列
          },
          bfs: {
            0: ["A", "B", "C", "D", "E", "F", "G", "H"] // 从A开始的BFS序列
          }
        }
      }
    ]
  },

  onLoad: function() {
    this.initLevel(this.data.currentLevel);
  },

  // 根据关卡初始化图形数据和正确的遍历序列
  initLevel: function(level) {
    const taskIndex = level - 1;
    if (taskIndex < 0 || taskIndex >= this.data.tasks.length) return;

    const task = this.data.tasks[taskIndex];
    const startIndex = this.data.startNodeIndex;
    const mode = this.data.traversalMode;

    this.setData({
      currentLevel: level,
      graphData: task.graph,
      task: task,
      correctSequence: task.sequences[mode][startIndex] || []
    });
  },

  // 遍历模式改变时更新正确的遍历序列
  onTraversalModeChange: function(e) {
    const mode = e.detail.mode;
    const task = this.data.tasks[this.data.currentLevel - 1];
    const startIndex = this.data.startNodeIndex;

    this.setData({
      traversalMode: mode,
      correctSequence: task.sequences[mode][startIndex] || []
    });
  },

  // 起始节点改变时更新正确的遍历序列
  onStartNodeChange: function(e) {
    const startIndex = e.detail.index;
    const task = this.data.tasks[this.data.currentLevel - 1];
    const mode = this.data.traversalMode;

    this.setData({
      startNodeIndex: startIndex,
      correctSequence: task.sequences[mode][startIndex] || []
    });
  },

  // 遍历完成的处理函数
  onTraversalComplete: function(e) {
    const { mode, isCorrect } = e.detail;
    if (isCorrect) {
      // 更新当前模式的完成状态
      const allTraversalsComplete = { ...this.data.allTraversalsComplete };
      allTraversalsComplete[mode] = true;

      // 如果两种遍历方式都完成了，提示用户进入下一关
      const allComplete = allTraversalsComplete.dfs && allTraversalsComplete.bfs;
      
      this.setData({ allTraversalsComplete });

      if (allComplete) {
        wx.showToast({
          title: '恭喜！你已完成所有遍历方式',
          icon: 'success',
          duration: 2000
        });
      }
    }
  },

  // 重置当前关卡
  resetLevel: function() {
    this.setData({
      allTraversalsComplete: {
        dfs: false,
        bfs: false
      },
      startNodeIndex: 0,
      traversalMode: 'dfs'
    }, () => {
      this.initLevel(this.data.currentLevel);
    });
  },

  // 前往上一关
  prevLevel: function() {
    if (this.data.currentLevel > 1) {
      this.setData({
        allTraversalsComplete: {
          dfs: false,
          bfs: false
        },
        startNodeIndex: 0,
        traversalMode: 'dfs'
      }, () => {
        this.initLevel(this.data.currentLevel - 1);
      });
    }
  },

  // 前往下一关
  nextLevel: function() {
    if (this.data.currentLevel < this.data.totalLevels) {
      this.setData({
        allTraversalsComplete: {
          dfs: false,
          bfs: false
        },
        startNodeIndex: 0,
        traversalMode: 'dfs'
      }, () => {
        this.initLevel(this.data.currentLevel + 1);
      });
    } else {
      wx.showToast({
        title: '恭喜！你已完成所有关卡',
        icon: 'success',
        duration: 2000
      });
    }
  }
}) 