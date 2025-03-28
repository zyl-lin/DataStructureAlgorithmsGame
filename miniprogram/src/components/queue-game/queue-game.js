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
    },
    isApiMode: {
      type: Boolean,
      value: false
    }
  },
  
  data: {
    isQueueFull: false,
    isQueueEmpty: true,
    operationHistory: [],
    currentValue: '',
    isLoading: false,
    apiError: '',
    animationSpeed: 5,
    showSpeedControl: false
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
    onEnqueue: async function() {
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
      
      if (this.data.isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          const api = require('../../services/api').queue;
          const result = await api.enqueue(
            { value },
            true,
            this.data.animationSpeed
          );
          
          if (result.success) {
            this.setData({ currentValue: '' });
            this.addHistory(`入队: ${value}`);
          }
        } catch (error) {
          this.setData({
            apiError: '入队失败: ' + (error.message || '未知错误')
          });
        } finally {
          this.setData({ isLoading: false });
        }
      } else {
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
      }
    },
    
    onDequeue: async function() {
      if (this.data.isQueueEmpty) {
        wx.showToast({
          title: '队列为空',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          const api = require('../../services/api').queue;
          const result = await api.dequeue(
            true,
            this.data.animationSpeed
          );
          
          if (result.success) {
            this.addHistory(`出队: ${result.data.value}`);
          }
        } catch (error) {
          this.setData({
            apiError: '出队失败: ' + (error.message || '未知错误')
          });
        } finally {
          this.setData({ isLoading: false });
        }
      } else {
        const newQueue = [...this.data.queue];
        const removed = newQueue.shift();
        
        this.addHistory(`出队: ${removed}`);
        
        this.triggerEvent('operation', {
          type: 'dequeue',
          value: removed,
          queue: newQueue
        });
      }
    },
    
    onPeek: async function() {
      if (this.data.isQueueEmpty) {
        wx.showToast({
          title: '队列为空',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          const api = require('../../services/api').queue;
          const result = await api.peek();
          
          if (result.success) {
            wx.showToast({
              title: `队首元素: ${result.data.value}`,
              icon: 'none'
            });
          }
        } catch (error) {
          this.setData({
            apiError: '查看队首失败: ' + (error.message || '未知错误')
          });
        } finally {
          this.setData({ isLoading: false });
        }
      } else {
        const frontElement = this.data.queue[0];
        wx.showToast({
          title: `队首元素: ${frontElement}`,
          icon: 'none'
        });
      }
    },
    
    // 重置队列方法
    resetQueue: async function() {
      if (this.data.isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          const api = require('../../services/api').queue;
          const result = await api.reset();
          
          if (result.success) {
            this.triggerEvent('operation', {
              type: 'reset',
              queue: []
            });
            
            this.setData({
              operationHistory: [],
              currentValue: ''
            });
            
            wx.showToast({
              title: '队列已重置',
              icon: 'success',
              duration: 1500
            });
          }
        } catch (error) {
          console.error('重置队列API调用失败:', error);
          this.setData({
            apiError: '重置队列失败: ' + (error.message || '未知错误')
          });
        } finally {
          this.setData({ isLoading: false });
        }
      } else {
        this.triggerEvent('operation', {
          type: 'reset',
          queue: []
        });
        
        this.setData({
          operationHistory: [],
          currentValue: ''
        });
      }
    },
    
    // 添加操作历史
    addHistory: function(operation) {
      const history = [...this.data.operationHistory];
      history.push(operation);
      this.setData({
        operationHistory: history
      });
    },
    
    // 处理输入值变化
    onValueInput: function(e) {
      this.setData({
        currentValue: e.detail.value
      });
    },
    
    // 切换API模式
    toggleApiMode: function(e) {
      if (this.data.isLoading) return;
      
      const isApiMode = e.currentTarget.dataset.mode === 'api';
      if (isApiMode === this.data.isApiMode) return;
      
      this.setData({
        isApiMode,
        apiError: '',
        showSpeedControl: isApiMode
      });
      
      // 切换到API模式时重置状态
      if (isApiMode) {
        this.resetQueue();
      }
      
      wx.showToast({
        title: `已切换至${isApiMode ? 'API' : '本地'}模式`,
        icon: 'none'
      });
    },
    
    // 设置动画速度
    setAnimationSpeed: function(e) {
      this.setData({
        animationSpeed: e.detail.value
      });
    },
    
    // 切换速度控制显示状态
    toggleSpeedControl: function() {
      this.setData({
        showSpeedControl: !this.data.showSpeedControl
      });
    }
  }
}) 