const app = getApp();

Page({
  data: {
    currentLevel: 1,
    totalLevels: 3,
    traversalMode: 'inorder', // 默认为中序遍历
    treeData: null,
    correctSequence: [],
    tasks: [
      {
        id: 1,
        description: '按照当前选择的遍历方式，依次点击二叉树节点',
        tree: {
          value: 10,
          left: {
            value: 5,
            left: {
              value: 3,
              left: null,
              right: null
            },
            right: {
              value: 7,
              left: null,
              right: null
            }
          },
          right: {
            value: 15,
            left: {
              value: 12,
              left: null,
              right: null
            },
            right: null
          }
        },
        sequences: {
          preorder: [10, 5, 3, 7, 15, 12],
          inorder: [3, 5, 7, 10, 12, 15],
          postorder: [3, 7, 5, 12, 15, 10],
          levelorder: [10, 5, 15, 3, 7, 12]
        }
      },
      {
        id: 2,
        description: '使用三种遍历方式（前序、中序、后序）完成二叉树遍历',
        tree: {
          value: 20,
          left: {
            value: 10,
            left: {
              value: 5,
              left: null,
              right: null
            },
            right: {
              value: 15,
              left: null,
              right: null
            }
          },
          right: {
            value: 30,
            left: {
              value: 25,
              left: null,
              right: null
            },
            right: {
              value: 35,
              left: null,
              right: null
            }
          }
        },
        sequences: {
          preorder: [20, 10, 5, 15, 30, 25, 35],
          inorder: [5, 10, 15, 20, 25, 30, 35],
          postorder: [5, 15, 10, 25, 35, 30, 20],
          levelorder: [20, 10, 30, 5, 15, 25, 35]
        }
      },
      {
        id: 3,
        description: '挑战完整二叉树的所有遍历方式',
        tree: {
          value: 50,
          left: {
            value: 25,
            left: {
              value: 10,
              left: {
                value: 5,
                left: null,
                right: null
              },
              right: {
                value: 15,
                left: null,
                right: null
              }
            },
            right: {
              value: 35,
              left: {
                value: 30,
                left: null,
                right: null
              },
              right: {
                value: 40,
                left: null,
                right: null
              }
            }
          },
          right: {
            value: 75,
            left: {
              value: 60,
              left: {
                value: 55,
                left: null,
                right: null
              },
              right: {
                value: 65,
                left: null,
                right: null
              }
            },
            right: {
              value: 90,
              left: {
                value: 80,
                left: null,
                right: null
              },
              right: {
                value: 95,
                left: null,
                right: null
              }
            }
          }
        },
        sequences: {
          preorder: [50, 25, 10, 5, 15, 35, 30, 40, 75, 60, 55, 65, 90, 80, 95],
          inorder: [5, 10, 15, 25, 30, 35, 40, 50, 55, 60, 65, 75, 80, 90, 95],
          postorder: [5, 15, 10, 30, 40, 35, 25, 55, 65, 60, 80, 95, 90, 75, 50],
          levelorder: [50, 25, 75, 10, 35, 60, 90, 5, 15, 30, 40, 55, 65, 80, 95]
        }
      }
    ],
    completedTraversals: {},
    currentTask: null
  },

  onLoad: function() {
    this.initLevel(this.data.currentLevel);
  },

  initLevel: function(level) {
    const index = level - 1;
    if (index < 0 || index >= this.data.tasks.length) {
      return;
    }
    
    const task = this.data.tasks[index];
    const treeData = task.tree;
    const correctSequence = task.sequences[this.data.traversalMode];
    
    this.setData({
      currentLevel: level,
      currentTask: task,
      treeData: treeData,
      correctSequence: correctSequence
    });
  },

  onTraversalModeChange: function(e) {
    const { mode } = e.detail;
    
    // 更新遍历模式和正确序列
    this.setData({
      traversalMode: mode,
      correctSequence: this.data.currentTask.sequences[mode]
    });
  },

  onTraversalComplete: function(e) {
    const { mode, isCorrect } = e.detail;
    
    if (isCorrect) {
      // 更新已完成的遍历记录
      const taskId = this.data.currentTask.id;
      const completedTraversals = { ...this.data.completedTraversals };
      
      if (!completedTraversals[taskId]) {
        completedTraversals[taskId] = {};
      }
      
      completedTraversals[taskId][mode] = true;
      
      this.setData({
        completedTraversals
      });
      
      // 检查是否完成了当前关卡的所有遍历方式
      const { preorder, inorder, postorder, levelorder } = completedTraversals[taskId] || {};
      const allCompleted = preorder && inorder && postorder && levelorder;
      
      if (allCompleted) {
        setTimeout(() => {
          wx.showModal({
            title: '恭喜',
            content: '你已完成当前关卡的所有遍历方式！',
            confirmText: '下一关',
            cancelText: '留在当前',
            success: (res) => {
              if (res.confirm && this.data.currentLevel < this.data.totalLevels) {
                this.nextLevel();
              }
            }
          });
        }, 1000);
      } else {
        setTimeout(() => {
          wx.showToast({
            title: '遍历完成！尝试其他遍历方式',
            icon: 'none',
            duration: 2000
          });
        }, 1000);
      }
    }
  },

  resetLevel: function() {
    this.initLevel(this.data.currentLevel);
  },

  prevLevel: function() {
    if (this.data.currentLevel > 1) {
      this.initLevel(this.data.currentLevel - 1);
    }
  },

  nextLevel: function() {
    if (this.data.currentLevel < this.data.totalLevels) {
      this.initLevel(this.data.currentLevel + 1);
    }
  }
}) 