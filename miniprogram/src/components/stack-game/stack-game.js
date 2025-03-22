Component({
  properties: {
    stack: {
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
    isStackFull: false,
    isStackEmpty: true,
    operationHistory: [],
    currentValue: '',
    isLoading: false,
    apiError: '',
    animationSpeed: 5,
    showSpeedControl: false
  },
  
  observers: {
    'stack': function(stack) {
      this.setData({
        isStackFull: stack.length >= this.data.maxSize,
        isStackEmpty: stack.length === 0
      });
    }
  },
  
  methods: {
    onPush: async function() {
      const value = this.data.currentValue.trim();
      if (!value) {
        wx.showToast({
          title: '请输入有效值',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.isStackFull) {
        wx.showToast({
          title: '栈已满',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          const api = require('../../services/api').stack;
          const result = await api.push(
            { value },
            true,
            this.data.animationSpeed
          );
          
          if (result.success) {
            this.setData({ currentValue: '' });
            this.addHistory(`入栈: ${value}`);
          }
        } catch (error) {
          this.setData({
            apiError: '入栈失败: ' + (error.message || '未知错误')
          });
        } finally {
          this.setData({ isLoading: false });
        }
      } else {
        const newStack = [...this.data.stack];
        newStack.push(value);
        
        this.setData({
          currentValue: ''
        });
        
        this.addHistory(`入栈: ${value}`);
        
        this.triggerEvent('operation', {
          type: 'push',
          value: value,
          stack: newStack
        });
      }
    },
    
    onPop: async function() {
      if (this.data.isStackEmpty) {
        wx.showToast({
          title: '栈为空',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          const api = require('../../services/api').stack;
          const result = await api.pop(
            true,
            this.data.animationSpeed
          );
          
          if (result.success) {
            this.addHistory(`出栈: ${result.data.value}`);
          }
        } catch (error) {
          this.setData({
            apiError: '出栈失败: ' + (error.message || '未知错误')
          });
        } finally {
          this.setData({ isLoading: false });
        }
      } else {
        const newStack = [...this.data.stack];
        const removed = newStack.pop();
        
        this.addHistory(`出栈: ${removed}`);
        
        this.triggerEvent('operation', {
          type: 'pop',
          value: removed,
          stack: newStack
        });
      }
    },
    
    onPeek: async function() {
      if (this.data.isStackEmpty) {
        wx.showToast({
          title: '栈为空',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          const api = require('../../services/api').stack;
          const result = await api.peek();
          
          if (result.success) {
            wx.showToast({
              title: `栈顶元素: ${result.data.value}`,
              icon: 'none'
            });
          }
        } catch (error) {
          this.setData({
            apiError: '查看栈顶失败: ' + (error.message || '未知错误')
          });
        } finally {
          this.setData({ isLoading: false });
        }
      } else {
        const topElement = this.data.stack[this.data.stack.length - 1];
        wx.showToast({
          title: `栈顶元素: ${topElement}`,
          icon: 'none'
        });
      }
    },
    
    // 重置栈方法
    resetStack: async function() {
      if (this.data.isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          const api = require('../../services/api').stack;
          const result = await api.reset();
          
          if (result.success) {
            this.triggerEvent('operation', {
              type: 'reset',
              stack: []
            });
            
            this.setData({
              operationHistory: [],
              currentValue: ''
            });
            
            wx.showToast({
              title: '栈已重置',
              icon: 'success',
              duration: 1500
            });
          }
        } catch (error) {
          console.error('重置栈API调用失败:', error);
          this.setData({
            apiError: '重置栈失败: ' + (error.message || '未知错误')
          });
        } finally {
          this.setData({ isLoading: false });
        }
      } else {
        this.triggerEvent('operation', {
          type: 'reset',
          stack: []
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
        this.resetStack();
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