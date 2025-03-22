Page({
  data: {
    stack: [],
    currentTask: '当前任务: 入栈元素25，然后执行两次出栈操作',
    operationTypes: ['入栈 (Push)', '出栈 (Pop)', '查看栈顶 (Peek)'],
    operationTypeIndex: 0,
    elementValue: 25,
    operationHistory: [],
    operationResult: '',
    taskProgress: 0,
    taskCompleted: false
  },

  onLoad: function() {
    this.initStack();
  },

  // 初始化栈
  initStack: function() {
    const stack = [5, 10, 15, 20];
    const operationHistory = [
      '入栈: 5',
      '入栈: 10',
      '入栈: 15',
      '入栈: 20'
    ];
    this.setData({ stack, operationHistory });
  },

  // 操作类型改变
  onOperationTypeChange: function(e) {
    const operationTypeIndex = parseInt(e.detail.value);
    this.setData({ operationTypeIndex });
  },

  // 元素值输入
  onElementValueInput: function(e) {
    this.setData({
      elementValue: e.detail.value
    });
  },

  // 执行栈操作
  executeOperation: function() {
    const { operationTypeIndex, elementValue, stack } = this.data;
    
    // 转换为数字类型
    const elementValueNum = parseInt(elementValue);

    switch (operationTypeIndex) {
      case 0: // 入栈 (Push)
        if (isNaN(elementValueNum)) {
          this.showResult('请输入有效的元素值');
          return;
        }
        this.pushElement(elementValueNum);
        break;
      case 1: // 出栈 (Pop)
        this.popElement();
        break;
      case 2: // 查看栈顶 (Peek)
        this.peekElement();
        break;
    }

    // 检查任务完成情况
    this.checkTaskCompletion();
  },

  // 入栈操作
  pushElement: function(value) {
    const stack = [...this.data.stack, value];
    const operationHistory = [...this.data.operationHistory, `入栈: ${value}`];
    
    this.setData({ stack, operationHistory });
    this.showResult(`成功将元素 ${value} 入栈`);

    // 如果当前任务是入栈25，更新进度
    if (this.data.taskProgress === 0 && value === 25) {
      this.setData({
        taskProgress: 1 // 第一步完成
      });
    }
  },

  // 出栈操作
  popElement: function() {
    const stack = [...this.data.stack];
    
    if (stack.length === 0) {
      this.showResult('栈为空，无法执行出栈操作');
      return;
    }

    const poppedValue = stack.pop();
    const operationHistory = [...this.data.operationHistory, `出栈: ${poppedValue}`];
    
    this.setData({ stack, operationHistory });
    this.showResult(`成功将元素 ${poppedValue} 出栈`);

    // 如果当前任务是入栈25后执行两次出栈，更新进度
    if (this.data.taskProgress === 1 || this.data.taskProgress === 2) {
      this.setData({
        taskProgress: this.data.taskProgress + 1
      });
    }
  },

  // 查看栈顶元素
  peekElement: function() {
    const stack = this.data.stack;
    
    if (stack.length === 0) {
      this.showResult('栈为空，没有栈顶元素');
      return;
    }

    const topValue = stack[stack.length - 1];
    const operationHistory = [...this.data.operationHistory, `查看栈顶: ${topValue}`];
    
    this.setData({ operationHistory });
    this.showResult(`栈顶元素为: ${topValue}`);
  },

  // 显示操作结果
  showResult: function(message) {
    this.setData({
      operationResult: message
    });

    // 3秒后自动清除结果消息
    setTimeout(() => {
      this.setData({
        operationResult: ''
      });
    }, 3000);
  },

  // 检查任务完成情况
  checkTaskCompletion: function() {
    // 检查当前任务是否已完成
    if (this.data.taskProgress === 3 && !this.data.taskCompleted) {
      wx.showToast({
        title: '任务完成！',
        icon: 'success',
        duration: 2000
      });
      this.setData({ taskCompleted: true });
    }
  },
  
  // 重置栈
  resetStack: function() {
    const api = require('../../services/api').stack;
    
    // 显示加载提示
    wx.showLoading({
      title: '重置中...',
    });
    
    // 使用API重置栈状态
    api.reset().then(res => {
      if (res.success) {
        // 重置成功，重新初始化栈
        this.initStack();
        
        // 重置任务进度
        this.setData({
          taskProgress: 0,
          taskCompleted: false
        });
        
        wx.showToast({
          title: '栈已重置',
          icon: 'success',
          duration: 1500
        });
      } else {
        wx.showToast({
          title: '重置栈失败',
          icon: 'none',
          duration: 1500
        });
      }
    }).catch(err => {
      console.error('重置栈API调用失败:', err);
      wx.showToast({
        title: '网络错误，请重试',
        icon: 'none',
        duration: 1500
      });
    }).finally(() => {
      wx.hideLoading();
    });
  }
}); 