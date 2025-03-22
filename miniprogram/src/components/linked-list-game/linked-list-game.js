Component({
  properties: {
    title: {
      type: String,
      value: '单链表游戏'
    },
    description: {
      type: String,
      value: '练习单链表的基本操作'
    },
    level: {
      type: Number,
      value: 1
    }
  },

  data: {
    nodes: [],
    isCompleted: false,
    useApi: true, // 是否使用API模式
    loading: false, // 加载状态
    animationSpeed: 5, // 动画速度(1-10)
    inputValue: '', // 输入的节点值
    inputPosition: '', // 输入的位置
    operationResult: '', // 操作结果
    animationFrames: [], // 动画帧
    currentFrame: 0, // 当前帧
    isPlaying: false, // 是否正在播放动画
    currentOperation: '', // 当前操作类型
    searchValue: '', // 查找的值
    highlightIndex: -1, // 高亮的节点索引
    animationTimer: null, // 动画定时器
    targetFound: false, // 是否找到目标节点
    apiError: '', // API错误信息
    showSpeedControl: false, // 是否显示速度控制器
  },

  lifetimes: {
    attached: function() {
      // 组件加载时初始化
      this.initLevel();
    },
    detached: function() {
      // 组件卸载时清除定时器
      if (this.data.animationTimer) {
        clearTimeout(this.data.animationTimer);
      }
    }
  },

  methods: {
    // 初始化关卡
    initLevel: function() {
      const level = this.properties.level;
      let nodes = [];
      
      // 根据关卡生成不同的链表
      switch(level) {
        case 1:
          nodes = [
            { value: 5 },
            { value: 8 },
            { value: 12 },
            { value: 15 },
            { value: 20 }
          ];
          break;
        case 2:
          nodes = [
            { value: 3 },
            { value: 7 },
            { value: 10 },
            { value: 15 },
            { value: 22 },
            { value: 30 }
          ];
          break;
        case 3:
          nodes = [
            { value: 6 },
            { value: 12 },
            { value: 18 },
            { value: 24 }
          ];
          break;
        default:
          nodes = [
            { value: 5 },
            { value: 10 },
            { value: 15 }
          ];
      }
      
      this.setData({ 
        nodes, 
        isCompleted: false,
        inputValue: '',
        inputPosition: '',
        operationResult: '',
        highlightIndex: -1,
        searchValue: '',
        targetFound: false
      });

      // 如果使用API，则从服务器获取链表状态
      if (this.data.useApi) {
        this.fetchLinkedListState();
      }
    },

    // 获取链表状态
    fetchLinkedListState: function() {
      const api = require('../../services/api').linkedList;
      
      this.setData({ loading: true });
      
      api.getState().then(res => {
        if (res.success && res.data && res.data.state) {
          this.setData({
            nodes: res.data.state.nodes || [],
            loading: false
          });
        } else {
          // 如果API调用失败，使用本地数据
          wx.showToast({
            title: '获取链表状态失败，使用本地数据',
            icon: 'none',
            duration: 2000
          });
          this.setData({ loading: false, useApi: false });
        }
      }).catch(err => {
        console.error('获取链表状态失败:', err);
        wx.showToast({
          title: '网络错误，使用本地数据',
          icon: 'none',
          duration: 2000
        });
        this.setData({ loading: false, useApi: false });
      });
    },

    // 切换速度控制显示状态
    toggleSpeedControl: function() {
      this.setData({
        showSpeedControl: !this.data.showSpeedControl
      });
    },

    // 重置链表API状态
    resetListApi: function() {
      if (!this.data.useApi) return;
      
      const api = require('../../services/api').linkedList;
      this.setData({ loading: true, apiError: '' });
      
      api.reset()
        .then(() => {
          console.log('链表状态重置成功');
          this.initLevel();
        })
        .catch(error => {
          console.error('重置链表状态失败:', error);
          this.setData({
            apiError: '重置状态失败，请重试'
          });
        })
        .finally(() => {
          this.setData({ loading: false });
        });
    },

    // 重置链表
    resetList: function() {
      // 如果正在播放动画，先停止
      if (this.data.animationTimer) {
        clearTimeout(this.data.animationTimer);
      }
      
      // 如果在API模式下，调用API重置
      if (this.data.useApi) {
        this.resetListApi();
        return;
      }
      
      // 本地重置
      this.initLevel();
    },

    // 修改切换API模式方法
    toggleApiMode: function() {
      // 如果正在播放动画，不允许切换模式
      if (this.data.isPlaying || this.data.loading) return;
      
      const newApiMode = !this.data.useApi;
      this.setData({ 
        useApi: newApiMode,
        apiError: '',
        showSpeedControl: newApiMode
      });
      
      // 如果切换到API模式，重置状态并获取链表状态
      if (newApiMode) {
        this.resetListApi();
      } else {
        // 切换到本地模式，直接重置
        this.initLevel();
      }
      
      wx.showToast({
        title: newApiMode ? '已切换至API模式' : '已切换至本地模式',
        icon: 'none'
      });
    },

    // 修改插入节点方法
    insertNode: async function() {
      const value = this.data.inputValue.trim();
      const position = parseInt(this.data.inputPosition);
      
      if (!value) {
        wx.showToast({
          title: '请输入节点值',
          icon: 'none'
        });
        return;
      }
      
      if (isNaN(position) || position < 0) {
        wx.showToast({
          title: '请输入有效的位置',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.useApi) {
        this.setData({ loading: true, apiError: '' });
        
        try {
          const api = require('../../services/api').linkedList;
          const result = await api.insert({
            value,
            position
          }, true, this.data.animationSpeed);
          
          if (result.success) {
            if (result.data.animation && result.data.animation.frames) {
              this.setData({
                animationFrames: result.data.animation.frames,
                currentFrame: 0,
                isPlaying: true
              });
              this.playAnimation();
            } else {
              this.setData({
                nodes: result.data.state.nodes || [],
                operationResult: '插入成功'
              });
            }
          } else {
            this.setData({
              apiError: '插入失败'
            });
          }
        } catch (error) {
          console.error('API请求失败:', error);
          this.setData({
            apiError: error.message || '网络请求失败'
          });
        } finally {
          this.setData({ loading: false });
        }
      } else {
        // 本地插入逻辑保持不变
        // ... existing local insert code ...
      }
    },

    // 调整动画速度
    changeSpeed: function(e) {
      const speed = e.detail.value;
      this.setData({ animationSpeed: speed });
    },

    // 输入节点值
    onValueInput: function(e) {
      this.setData({ inputValue: e.detail.value });
    },

    // 输入位置
    onPositionInput: function(e) {
      this.setData({ inputPosition: e.detail.value });
    },

    // 输入查找值
    onSearchInput: function(e) {
      this.setData({ searchValue: e.detail.value });
    },

    // 播放动画
    playAnimation: function() {
      if (this.data.animationFrames.length === 0 || this.data.currentFrame >= this.data.animationFrames.length) {
        this.setData({ isPlaying: false });
        return;
      }
      
      const frame = this.data.animationFrames[this.data.currentFrame];
      
      // 更新链表状态
      this.setData({
        nodes: frame.nodes || this.data.nodes,
        highlightIndex: frame.highlightIndex !== undefined ? frame.highlightIndex : -1,
        currentFrame: this.data.currentFrame + 1
      });
      
      // 延迟播放下一帧
      this.data.animationTimer = setTimeout(() => {
        this.playAnimation();
      }, 1000 / this.data.animationSpeed);
    },

    // 开始游戏
    onStart: function() {
      // 触发开始游戏事件
      this.triggerEvent('gamestart', { level: this.properties.level });
      
      // 这里可以添加游戏开始的动画或其他效果
      wx.showToast({
        title: `第${this.properties.level}关开始`,
        icon: 'none',
        duration: 1500
      });
    },

    // 下一关
    onNextLevel: function() {
      // 触发下一关事件
      this.triggerEvent('nextlevel', { 
        currentLevel: this.properties.level,
        nextLevel: this.properties.level + 1
      });
    },

    // 标记当前关卡完成
    markCompleted: function() {
      this.setData({ isCompleted: true });
      
      wx.showToast({
        title: '关卡完成！',
        icon: 'success',
        duration: 2000
      });
      
      // 触发关卡完成事件
      this.triggerEvent('levelcompleted', { level: this.properties.level });
    },

    // 删除节点
    deleteNode: function() {
      const position = parseInt(this.data.inputPosition);
      
      // 检查输入有效性
      if (isNaN(position) || position < 0 || position >= this.data.nodes.length) {
        wx.showToast({
          title: `位置应在0-${this.data.nodes.length - 1}之间`,
          icon: 'none',
          duration: 1500
        });
        return;
      }
      
      // 清除旧状态
      this.setData({
        highlightIndex: -1,
        operationResult: '',
        targetFound: false
      });
      
      if (this.data.useApi) {
        // 使用API删除节点
        this.deleteNodeApi(position);
      } else {
        // 使用本地逻辑删除节点
        this.deleteNodeLocal(position);
      }
    },

    // API删除节点
    deleteNodeApi: function(position) {
      const api = require('../../services/api').linkedList;
      this.setData({ loading: true, currentOperation: 'delete' });
      
      api.delete({ position }, true, this.data.animationSpeed).then(res => {
        if (res.success) {
          // 如果返回包含动画帧
          if (res.data.animation && res.data.animation.frames) {
            this.setData({
              animationFrames: res.data.animation.frames,
              currentFrame: 0,
              isPlaying: true,
              loading: false
            });
            this.playAnimation();
          } else {
            // 没有动画，直接更新状态
            this.setData({
              nodes: res.data.state.nodes || this.data.nodes,
              loading: false,
              operationResult: `成功删除位置${position}的节点`
            });
          }
        } else {
          wx.showToast({
            title: '删除节点失败',
            icon: 'none',
            duration: 1500
          });
          this.setData({ loading: false });
        }
      }).catch(err => {
        console.error('删除节点API调用失败:', err);
        wx.showToast({
          title: '网络错误，请重试',
          icon: 'none',
          duration: 1500
        });
        this.setData({ loading: false });
      });
    },

    // 本地删除节点
    deleteNodeLocal: function(position) {
      // 创建动画帧
      const animationFrames = [];
      const newNodes = [...this.data.nodes];
      
      // 添加当前状态作为第一帧
      animationFrames.push({
        nodes: JSON.parse(JSON.stringify(newNodes)),
        highlightIndex: -1
      });
      
      // 遍历到删除位置
      for (let i = 0; i < position; i++) {
        animationFrames.push({
          nodes: JSON.parse(JSON.stringify(newNodes)),
          highlightIndex: i
        });
      }
      
      // 高亮要删除的节点
      animationFrames.push({
        nodes: JSON.parse(JSON.stringify(newNodes)),
        highlightIndex: position
      });
      
      // 删除节点
      const deletedValue = newNodes[position].value;
      newNodes.splice(position, 1);
      
      // 添加删除后的状态
      animationFrames.push({
        nodes: JSON.parse(JSON.stringify(newNodes)),
        highlightIndex: -1
      });
      
      this.setData({
        animationFrames,
        currentFrame: 0,
        isPlaying: true,
        currentOperation: 'delete'
      });
      
      this.playAnimation();
      
      // 播放完成后更新操作结果
      setTimeout(() => {
        this.setData({
          operationResult: `成功删除位置${position}的节点，值为${deletedValue}`
        });
      }, animationFrames.length * (1000 / this.data.animationSpeed));
    },

    // 查找节点
    findNode: function() {
      const value = parseInt(this.data.searchValue);
      
      // 检查输入有效性
      if (isNaN(value)) {
        wx.showToast({
          title: '请输入有效的查找值',
          icon: 'none',
          duration: 1500
        });
        return;
      }
      
      // 清除旧状态
      this.setData({
        highlightIndex: -1,
        operationResult: '',
        targetFound: false
      });
      
      if (this.data.useApi) {
        // 使用API查找节点
        this.findNodeApi(value);
      } else {
        // 使用本地逻辑查找节点
        this.findNodeLocal(value);
      }
    },

    // API查找节点
    findNodeApi: function(value) {
      const api = require('../../services/api').linkedList;
      this.setData({ loading: true, currentOperation: 'find' });
      
      api.find({ value }, true, this.data.animationSpeed).then(res => {
        if (res.success) {
          // 如果返回包含动画帧
          if (res.data.animation && res.data.animation.frames) {
            this.setData({
              animationFrames: res.data.animation.frames,
              currentFrame: 0,
              isPlaying: true,
              loading: false,
              targetFound: res.data.state.found || false
            });
            this.playAnimation();
          } else {
            // 没有动画，直接更新状态
            this.setData({
              loading: false,
              targetFound: res.data.state.found || false,
              highlightIndex: res.data.state.position || -1,
              operationResult: res.data.state.found 
                ? `在位置${res.data.state.position}找到值为${value}的节点` 
                : `未找到值为${value}的节点`
            });
          }
        } else {
          wx.showToast({
            title: '查找节点失败',
            icon: 'none',
            duration: 1500
          });
          this.setData({ loading: false });
        }
      }).catch(err => {
        console.error('查找节点API调用失败:', err);
        wx.showToast({
          title: '网络错误，请重试',
          icon: 'none',
          duration: 1500
        });
        this.setData({ loading: false });
      });
    },

    // 本地查找节点
    findNodeLocal: function(value) {
      // 创建动画帧
      const animationFrames = [];
      const nodes = [...this.data.nodes];
      let found = false;
      let position = -1;
      
      // 添加当前状态作为第一帧
      animationFrames.push({
        nodes: JSON.parse(JSON.stringify(nodes)),
        highlightIndex: -1
      });
      
      // 遍历查找
      for (let i = 0; i < nodes.length; i++) {
        animationFrames.push({
          nodes: JSON.parse(JSON.stringify(nodes)),
          highlightIndex: i
        });
        
        if (nodes[i].value === value) {
          found = true;
          position = i;
          break;
        }
      }
      
      // 如果找到，添加一个额外的帧来强调
      if (found) {
        animationFrames.push({
          nodes: JSON.parse(JSON.stringify(nodes)),
          highlightIndex: position
        });
      }
      
      // 添加完成状态
      animationFrames.push({
        nodes: JSON.parse(JSON.stringify(nodes)),
        highlightIndex: found ? position : -1
      });
      
      this.setData({
        animationFrames,
        currentFrame: 0,
        isPlaying: true,
        currentOperation: 'find',
        targetFound: found
      });
      
      this.playAnimation();
      
      // 播放完成后更新操作结果
      setTimeout(() => {
        this.setData({
          operationResult: found 
            ? `在位置${position}找到值为${value}的节点` 
            : `未找到值为${value}的节点`,
          highlightIndex: found ? position : -1
        });
      }, animationFrames.length * (1000 / this.data.animationSpeed));
    },

    // 反转链表
    reverseList: function() {
      // 清除旧状态
      this.setData({
        highlightIndex: -1,
        operationResult: '',
        targetFound: false
      });
      
      if (this.data.useApi) {
        // 使用API反转链表
        this.reverseListApi();
      } else {
        // 使用本地逻辑反转链表
        this.reverseListLocal();
      }
    },

    // API反转链表
    reverseListApi: function() {
      const api = require('../../services/api').linkedList;
      this.setData({ loading: true, currentOperation: 'reverse' });
      
      api.reverse(true, this.data.animationSpeed).then(res => {
        if (res.success) {
          // 如果返回包含动画帧
          if (res.data.animation && res.data.animation.frames) {
            this.setData({
              animationFrames: res.data.animation.frames,
              currentFrame: 0,
              isPlaying: true,
              loading: false
            });
            this.playAnimation();
          } else {
            // 没有动画，直接更新状态
            this.setData({
              nodes: res.data.state.nodes || this.data.nodes,
              loading: false,
              operationResult: '链表反转成功'
            });
          }
        } else {
          wx.showToast({
            title: '链表反转失败',
            icon: 'none',
            duration: 1500
          });
          this.setData({ loading: false });
        }
      }).catch(err => {
        console.error('链表反转API调用失败:', err);
        wx.showToast({
          title: '网络错误，请重试',
          icon: 'none',
          duration: 1500
        });
        this.setData({ loading: false });
      });
    },

    // 本地反转链表
    reverseListLocal: function() {
      // 创建动画帧
      const animationFrames = [];
      const originalNodes = [...this.data.nodes];
      
      // 添加当前状态作为第一帧
      animationFrames.push({
        nodes: JSON.parse(JSON.stringify(originalNodes)),
        highlightIndex: -1
      });
      
      // 反转操作
      const reversedNodes = [];
      for (let i = originalNodes.length - 1; i >= 0; i--) {
        reversedNodes.push(originalNodes[i]);
        
        // 添加每一步的状态
        animationFrames.push({
          nodes: JSON.parse(JSON.stringify(reversedNodes)),
          highlightIndex: reversedNodes.length - 1
        });
      }
      
      // 添加完成状态
      animationFrames.push({
        nodes: JSON.parse(JSON.stringify(reversedNodes)),
        highlightIndex: -1
      });
      
      this.setData({
        animationFrames,
        currentFrame: 0,
        isPlaying: true,
        currentOperation: 'reverse'
      });
      
      this.playAnimation();
      
      // 播放完成后更新操作结果
      setTimeout(() => {
        this.setData({
          operationResult: '链表反转成功'
        });
      }, animationFrames.length * (1000 / this.data.animationSpeed));
    },
  }
}); 