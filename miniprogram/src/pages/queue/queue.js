const app = getApp();

Page({
  data: {
    queue: [],
    maxSize: 6,
    currentLevel: 1,
    tasks: [
      {
        id: 1,
        description: '入队元素 5、10、15，然后执行一次出队操作',
        operations: ['enqueue', 'enqueue', 'enqueue', 'dequeue'],
        values: ['5', '10', '15', null],
        completed: false
      },
      {
        id: 2,
        description: '入队元素 20、25、30，然后执行查看队首操作，最后执行两次出队操作',
        operations: ['enqueue', 'enqueue', 'enqueue', 'peek', 'dequeue', 'dequeue'],
        values: ['20', '25', '30', null, null, null],
        completed: false
      },
      {
        id: 3,
        description: '创建一个具有5个元素的队列，然后全部出队',
        operations: ['enqueue', 'enqueue', 'enqueue', 'enqueue', 'enqueue', 'dequeue', 'dequeue', 'dequeue', 'dequeue', 'dequeue'],
        values: ['1', '2', '3', '4', '5', null, null, null, null, null],
        completed: false
      }
    ],
    currentTaskIndex: 0,
    expectedOperations: [],
    performedOperations: [],
  },

  onLoad: function() {
    this.initLevel(this.data.currentLevel);
  },

  initLevel: function(level) {
    // 关卡索引从0开始，而level从1开始
    const index = level - 1;
    if (index < 0 || index >= this.data.tasks.length) {
      return;
    }
    
    this.setData({
      currentLevel: level,
      currentTaskIndex: index,
      queue: [],
      expectedOperations: this.generateExpectedOperations(this.data.tasks[index]),
      performedOperations: []
    });
  },

  generateExpectedOperations: function(task) {
    const operations = [];
    for (let i = 0; i < task.operations.length; i++) {
      operations.push({
        type: task.operations[i],
        value: task.values[i]
      });
    }
    return operations;
  },

  onQueueOperation: function(e) {
    const operation = e.detail;
    const { type, value, queue } = operation;
    
    // 更新队列
    this.setData({
      queue: queue
    });
    
    // 记录操作
    const performedOperations = this.data.performedOperations;
    performedOperations.push({
      type: type,
      value: value
    });
    
    this.setData({
      performedOperations: performedOperations
    });
    
    // 检查任务是否完成
    this.checkTaskCompletion();
  },

  checkTaskCompletion: function() {
    const { expectedOperations, performedOperations, currentTaskIndex, tasks } = this.data;
    
    // 如果已经完成的操作数量小于期望的操作数量，任务未完成
    if (performedOperations.length < expectedOperations.length) {
      return;
    }
    
    // 检查已完成的操作是否符合期望
    let allMatch = true;
    for (let i = 0; i < expectedOperations.length; i++) {
      const expected = expectedOperations[i];
      const performed = performedOperations[i];
      
      if (expected.type !== performed.type) {
        allMatch = false;
        break;
      }
      
      // 对于入队操作，检查值是否匹配
      if (expected.type === 'enqueue' && expected.value !== performed.value) {
        allMatch = false;
        break;
      }
    }
    
    if (allMatch) {
      // 任务完成，更新状态
      const updatedTasks = [...this.data.tasks];
      updatedTasks[currentTaskIndex].completed = true;
      
      this.setData({
        tasks: updatedTasks
      });
      
      wx.showToast({
        title: '任务完成！',
        icon: 'success'
      });
      
      // 如果是最后一个任务，显示关卡完成消息
      if (currentTaskIndex === tasks.length - 1) {
        setTimeout(() => {
          wx.showModal({
            title: '恭喜',
            content: '你已完成所有队列操作任务！',
            showCancel: false
          });
        }, 1500);
      } else {
        // 否则，提示用户进入下一关
        setTimeout(() => {
          wx.showModal({
            title: '任务完成',
            content: '进入下一关？',
            confirmText: '确定',
            cancelText: '留在当前',
            success: (res) => {
              if (res.confirm) {
                this.nextLevel();
              }
            }
          });
        }, 1500);
      }
    }
  },

  resetLevel: function() {
    this.initLevel(this.data.currentLevel);
    
    wx.showToast({
      title: '已重置当前关卡',
      icon: 'none'
    });
  },

  prevLevel: function() {
    if (this.data.currentLevel > 1) {
      this.initLevel(this.data.currentLevel - 1);
    }
  },

  nextLevel: function() {
    if (this.data.currentLevel < this.data.tasks.length) {
      this.initLevel(this.data.currentLevel + 1);
    }
  }
}) 