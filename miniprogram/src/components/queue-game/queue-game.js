Component({
  properties: {
    queue: {
      type: Array,
      value: []
    },
    maxSize: {
      type: Number,
      value: 6
    },
    task: {
      type: Object,
      value: null
    }
  },
  
  data: {
    isQueueFull: false,
    isQueueEmpty: true,
    operationHistory: [],
    currentValue: ''
  },
  
  observers: {
    'queue': function(queue) {
      this.setData({
        isQueueFull: queue.length >= this.data.maxSize,
        isQueueEmpty: queue.length === 0
      });
    }
  },
  
  methods: {
    onEnqueue: function() {
      const value = this.data.currentValue.trim();
      if (!value) {
        wx.showToast({
          title: '请输入有效值',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.isQueueFull) {
        wx.showToast({
          title: '队列已满',
          icon: 'none'
        });
        return;
      }
      
      const newQueue = [...this.data.queue];
      newQueue.push(value);
      
      this.setData({
        currentValue: ''
      });
      
      this.addHistory(`入队: ${value}`);
      
      this.triggerEvent('operation', {
        type: 'enqueue',
        value: value,
        queue: newQueue
      });
    },
    
    onDequeue: function() {
      if (this.data.isQueueEmpty) {
        wx.showToast({
          title: '队列为空',
          icon: 'none'
        });
        return;
      }
      
      const newQueue = [...this.data.queue];
      const removed = newQueue.shift();
      
      this.addHistory(`出队: ${removed}`);
      
      this.triggerEvent('operation', {
        type: 'dequeue',
        value: removed,
        queue: newQueue
      });
    },
    
    onPeek: function() {
      if (this.data.isQueueEmpty) {
        wx.showToast({
          title: '队列为空',
          icon: 'none'
        });
        return;
      }
      
      wx.showToast({
        title: `队首元素: ${this.data.queue[0]}`,
        icon: 'none'
      });
      
      this.addHistory(`查看队首: ${this.data.queue[0]}`);
      
      this.triggerEvent('operation', {
        type: 'peek',
        value: this.data.queue[0],
        queue: this.data.queue
      });
    },
    
    addHistory: function(text) {
      const newHistory = [...this.data.operationHistory];
      newHistory.push(text);
      
      // 保留最近的10条记录
      if (newHistory.length > 10) {
        newHistory.shift();
      }
      
      this.setData({
        operationHistory: newHistory
      });
    },
    
    onValueInput: function(e) {
      this.setData({
        currentValue: e.detail.value
      });
    }
  }
}) 