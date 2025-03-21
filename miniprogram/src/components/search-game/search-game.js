const api = require('../../services/api');

Component({
  properties: {
    // 搜索的数组
    dataArray: {
      type: Array,
      value: []
    },
    // 是否已排序（二分查找需要已排序数组）
    isSorted: {
      type: Boolean,
      value: false
    },
    // 初始搜索算法类型
    initialSearchType: {
      type: String,
      value: 'linear' // 'linear' 或 'binary'
    },
    // 是否使用API（默认使用API）
    useApi: {
      type: Boolean,
      value: true
    }
  },

  data: {
    searchType: 'linear', // 当前搜索算法类型
    visualArray: [], // 可视化数组，包含状态信息
    originalArray: [], // 原始数组
    targetValue: '', // 目标搜索值
    isSearching: false, // 是否正在搜索
    isAutoPlaying: false, // 是否自动播放
    isCompleted: false, // 搜索是否完成
    currentStep: 0, // 当前搜索步骤
    searchSteps: [], // 搜索步骤数据
    currentStepDescription: '请输入要搜索的值并点击"开始搜索"', // 当前步骤描述
    searchResult: {
      found: false,
      index: -1,
      message: ''
    },
    autoPlayInterval: null, // 自动播放的定时器
    algorithmInfo: {
      linear: {
        timeComplexity: 'O(n)',
        spaceComplexity: 'O(1)',
        useCases: '适用于未排序或较小的数据集'
      },
      binary: {
        timeComplexity: 'O(log n)',
        spaceComplexity: 'O(1)',
        useCases: '仅适用于已排序的数据集，但查找效率更高'
      }
    },
    currentAlgorithmInfo: null,
    isApiLoading: false, // API请求加载状态
    apiError: '', // API错误信息
    animationSpeed: 5, // 动画速度设置
    showSpeedControl: false, // 是否显示速度控制器
  },

  lifetimes: {
    attached: function() {
      // 初始化
      this.setData({
        searchType: this.properties.initialSearchType,
        currentAlgorithmInfo: this.data.algorithmInfo[this.properties.initialSearchType]
      });
      this.initArray();
    }
  },

  observers: {
    'dataArray': function(newVal) {
      if (newVal && newVal.length > 0) {
        this.initArray();
      }
    },
    'isSorted': function(newVal) {
      // 如果数组已排序但当前是二分搜索，自动启用二分搜索
      if (newVal && this.data.searchType === 'linear') {
        this.setData({
          searchType: 'binary',
          currentAlgorithmInfo: this.data.algorithmInfo.binary
        });
      }
    }
  },

  methods: {
    // 初始化数组
    initArray: function() {
      const array = this.properties.dataArray.slice();
      const visualArray = array.map(value => ({
        value,
        status: '' // 初始状态为空
      }));
      
      this.setData({
        originalArray: array,
        visualArray: visualArray
      });
    },

    // 改变搜索算法类型
    changeSearchType: function(e) {
      const type = e.currentTarget.dataset.type;
      
      // 如果是二分搜索，确保数组已排序
      if (type === 'binary' && !this.properties.isSorted) {
        wx.showToast({
          title: '二分搜索需要排序数组',
          icon: 'none',
          duration: 2000
        });
        return;
      }
      
      this.setData({
        searchType: type,
        currentAlgorithmInfo: this.data.algorithmInfo[type]
      });
      
      // 如果正在搜索中，重置搜索
      if (this.data.isSearching) {
        this.resetSearch();
      }
    },

    // 目标值改变
    onTargetValueChange: function(e) {
      this.setData({
        targetValue: e.detail.value
      });
    },

    // 开始搜索
    startSearch: function() {
      if (!this.data.targetValue) return;
      
      const target = parseInt(this.data.targetValue);
      
      // 如果使用API，则调用API进行搜索
      if (this.properties.useApi) {
        this.startSearchApi(target);
      } else {
        // 本地搜索逻辑（现有代码）
        let steps = [];
        
        if (this.data.searchType === 'linear') {
          steps = this.generateLinearSearchSteps(target);
        } else if (this.data.searchType === 'binary') {
          steps = this.generateBinarySearchSteps(target);
        }
        
        this.setData({
          isSearching: true,
          isCompleted: false,
          currentStep: 0,
          searchSteps: steps,
          currentStepDescription: '开始搜索...'
        });
        
        // 应用第一步
        this.applyStep(0);
      }
      
      // 触发事件
      this.triggerEvent('searchStart', {
        searchType: this.data.searchType,
        targetValue: target
      });
    },

    // 使用API开始搜索
    startSearchApi: function(target) {
      // 设置加载状态
      this.setData({
        isApiLoading: true,
        apiError: '',
        isSearching: true,
        isCompleted: false
      });
      
      // 准备请求数据
      const requestData = {
        array: this.data.originalArray,
        target: target
      };
      
      // 根据搜索类型选择API
      let apiMethod;
      if (this.data.searchType === 'linear') {
        apiMethod = api.search.linear;
      } else {
        apiMethod = api.search.binary;
      }
      
      // 调用API
      apiMethod(requestData, true, this.data.animationSpeed)
        .then(response => {
          console.log('搜索API返回:', response);
          
          if (response.success) {
            // 处理API返回的数据
            this.handleApiResponse(response.data, target);
          } else {
            this.setData({
              apiError: '搜索操作失败',
              isApiLoading: false
            });
            
            wx.showToast({
              title: '搜索操作失败',
              icon: 'none'
            });
          }
        })
        .catch(error => {
          console.error('API请求失败:', error);
          this.setData({
            apiError: error.message || '网络请求失败',
            isApiLoading: false
          });
          
          wx.showToast({
            title: '网络请求失败',
            icon: 'none'
          });
        });
    },
    
    // 处理API返回的搜索数据
    handleApiResponse: function(data, target) {
      // 处理API返回的数据并转换为步骤格式
      let steps = [];
      
      // 如果有动画帧
      if (data.animation && data.animation.frames) {
        steps = this.convertApiFramesToSteps(data.animation.frames, target);
      } else {
        // 如果没有动画帧，创建简单结果
        const result = data.state;
        const found = result.found;
        const index = result.index;
        
        steps = [{
          description: found ? 
            `在索引 ${index} 处找到值 ${target}！` : 
            `未找到值 ${target}！`,
          visualArray: this.data.visualArray.map((item, i) => {
            const status = found && i === index ? 'found' : '';
            return { ...item, status };
          }),
          result: {
            found,
            index,
            message: found ? 
              `在索引 ${index} 处找到值 ${target}！` : 
              `未找到值 ${target}！`
          }
        }];
      }
      
      this.setData({
        isApiLoading: false,
        searchSteps: steps,
        currentStep: 0
      });
      
      // 应用第一步
      this.applyStep(0);
    },
    
    // 将API帧转换为搜索步骤
    convertApiFramesToSteps: function(frames, target) {
      return frames.map((frame, index) => {
        // 根据帧数据转换为可视化数组
        const visualArray = frame.array.map((value, i) => ({
          value: value,
          status: this.getStatusFromFrame(frame, i)
        }));
        
        // 创建步骤描述
        let description = '';
        if (index === 0) {
          description = '开始搜索...';
        } else if (index === frames.length - 1) {
          description = frame.found ? 
            `在索引 ${frame.targetIndex} 处找到值 ${target}！` : 
            `未找到值 ${target}！`;
        } else if (this.data.searchType === 'linear') {
          description = `检查索引 ${frame.currentIndex}: 值 ${frame.array[frame.currentIndex]}`;
        } else { // binary
          description = `检查中间索引 ${frame.mid}: 值 ${frame.array[frame.mid]}`;
          if (frame.comparison) {
            description += ` (${frame.comparison})`;
          }
        }
        
        return {
          description,
          visualArray,
          result: index === frames.length - 1 ? {
            found: frame.found,
            index: frame.targetIndex || -1,
            message: frame.found ? 
              `在索引 ${frame.targetIndex} 处找到值 ${target}！` : 
              `未找到值 ${target}！`
          } : null
        };
      });
    },
    
    // 从帧数据获取元素状态
    getStatusFromFrame: function(frame, index) {
      if (frame.found && index === frame.targetIndex) {
        return 'found';
      }
      
      if (index === frame.currentIndex || index === frame.mid) {
        return 'current';
      }
      
      if (frame.checkedIndices && frame.checkedIndices.includes(index)) {
        return 'checked';
      }
      
      return '';
    },

    // 调整动画速度
    setAnimationSpeed: function(e) {
      this.setData({
        animationSpeed: e.detail.value
      });
    },

    // 切换显示速度控制器
    toggleSpeedControl: function() {
      this.setData({
        showSpeedControl: !this.data.showSpeedControl
      });
    },

    // 重置搜索状态
    resetSearch: function() {
      // 清除自动播放定时器
      if (this.data.autoPlayInterval) {
        clearInterval(this.data.autoPlayInterval);
      }
      
      // 重置可视化数组状态
      const visualArray = this.data.originalArray.map(value => ({
        value,
        status: ''
      }));
      
      this.setData({
        isSearching: false,
        isAutoPlaying: false,
        isCompleted: false,
        currentStep: 0,
        searchSteps: [],
        visualArray: visualArray,
        currentStepDescription: '请输入要搜索的值并点击"开始搜索"',
        searchResult: {
          found: false,
          index: -1,
          message: ''
        },
        autoPlayInterval: null,
        apiError: ''
      });
      
      // 触发重置事件
      this.triggerEvent('searchReset');
    },
    
    // 应用指定步骤的状态
    applyStep: function(stepIndex) {
      if (stepIndex >= this.data.searchSteps.length) {
        return;
      }
      
      const step = this.data.searchSteps[stepIndex];
      
      this.setData({
        visualArray: step.visualArray,
        currentStepDescription: step.description,
        currentStep: stepIndex
      });
      
      // 如果是最后一步或者步骤包含结果
      if (stepIndex === this.data.searchSteps.length - 1 || step.result) {
        this.setData({
          isCompleted: true,
          searchResult: step.result || {
            found: false,
            index: -1,
            message: '搜索完成'
          }
        });
        
        // 停止自动播放
        if (this.data.isAutoPlaying) {
          this.toggleAutoPlay();
        }
        
        // 触发搜索完成事件
        this.triggerEvent('searchComplete', {
          searchType: this.data.searchType,
          result: this.data.searchResult
        });
      }
    },
    
    // 下一步
    nextStep: function() {
      if (this.data.isCompleted || this.data.isApiLoading) {
        return;
      }
      
      const nextStepIndex = this.data.currentStep + 1;
      
      if (nextStepIndex < this.data.searchSteps.length) {
        this.applyStep(nextStepIndex);
      }
    },
    
    // 切换自动播放
    toggleAutoPlay: function() {
      if (this.data.isCompleted || this.data.isApiLoading) {
        return;
      }
      
      if (this.data.isAutoPlaying) {
        // 停止自动播放
        if (this.data.autoPlayInterval) {
          clearInterval(this.data.autoPlayInterval);
        }
        
        this.setData({
          isAutoPlaying: false,
          autoPlayInterval: null
        });
      } else {
        // 开始自动播放
        const interval = setInterval(() => {
          const nextStepIndex = this.data.currentStep + 1;
          
          if (nextStepIndex < this.data.searchSteps.length) {
            this.applyStep(nextStepIndex);
          } else {
            // 如果已经是最后一步，停止自动播放
            clearInterval(interval);
            this.setData({
              isAutoPlaying: false,
              autoPlayInterval: null
            });
          }
        }, 1000 / this.data.animationSpeed);
        
        this.setData({
          isAutoPlaying: true,
          autoPlayInterval: interval
        });
      }
    },

    // 生成线性搜索步骤
    generateLinearSearchSteps: function(target) {
      const array = this.data.originalArray;
      const steps = [];
      let found = false;
      let foundIndex = -1;
      
      // 初始步骤
      steps.push({
        description: '开始线性搜索...',
        visualArray: this.data.visualArray.map(item => ({
          ...item,
          status: ''
        }))
      });
      
      // 线性搜索各步骤
      for (let i = 0; i < array.length; i++) {
        const currentArray = this.data.visualArray.map((item, index) => ({
          ...item,
          status: index < i ? 'checked' : (index === i ? 'current' : '')
        }));
        
        steps.push({
          description: `检查索引 ${i}: 值 ${array[i]}`,
          visualArray: currentArray
        });
        
        if (array[i] === target) {
          found = true;
          foundIndex = i;
          break;
        }
      }
      
      // 最终结果步骤
      const finalArray = this.data.visualArray.map((item, index) => ({
        ...item,
        status: found && index === foundIndex ? 'found' : (index <= foundIndex ? 'checked' : '')
      }));
      
      steps.push({
        description: found ? 
          `在索引 ${foundIndex} 处找到值 ${target}！` : 
          `未找到值 ${target}！`,
        visualArray: finalArray,
        result: {
          found,
          index: foundIndex,
          message: found ? 
            `在索引 ${foundIndex} 处找到值 ${target}！` : 
            `未找到值 ${target}！`
        }
      });
      
      return steps;
    },
    
    // 生成二分搜索步骤
    generateBinarySearchSteps: function(target) {
      const array = this.data.originalArray;
      const steps = [];
      let found = false;
      let foundIndex = -1;
      
      // 初始步骤
      steps.push({
        description: '开始二分搜索...',
        visualArray: this.data.visualArray.map(item => ({
          ...item,
          status: ''
        }))
      });
      
      // 二分搜索步骤
      let left = 0;
      let right = array.length - 1;
      
      while (left <= right) {
        const mid = Math.floor((left + right) / 2);
        const currentValue = array[mid];
        
        // 标记当前检查的区域
        const currentArray = this.data.visualArray.map((item, index) => {
          let status = '';
          if (index < left || index > right) {
            status = 'checked'; // 已排除区域
          } else if (index === mid) {
            status = 'current'; // 当前检查位置
          }
          return { ...item, status };
        });
        
        steps.push({
          description: `检查中间索引 ${mid}: 值 ${currentValue}`,
          visualArray: currentArray
        });
        
        if (currentValue === target) {
          found = true;
          foundIndex = mid;
          break;
        } else if (currentValue < target) {
          steps.push({
            description: `${currentValue} < ${target}，向右搜索`,
            visualArray: currentArray
          });
          left = mid + 1;
        } else {
          steps.push({
            description: `${currentValue} > ${target}，向左搜索`,
            visualArray: currentArray
          });
          right = mid - 1;
        }
      }
      
      // 最终结果步骤
      const finalArray = this.data.visualArray.map((item, index) => ({
        ...item,
        status: found && index === foundIndex ? 'found' : 'checked'
      }));
      
      steps.push({
        description: found ? 
          `在索引 ${foundIndex} 处找到值 ${target}！` : 
          `未找到值 ${target}！`,
        visualArray: finalArray,
        result: {
          found,
          index: foundIndex,
          message: found ? 
            `在索引 ${foundIndex} 处找到值 ${target}！` : 
            `未找到值 ${target}！`
        }
      });
      
      return steps;
    },

    // 切换API模式
    toggleApiMode: function(e) {
      // 如果正在搜索或API请求加载中，不允许切换模式
      if (this.data.isSearching || this.data.isApiLoading) return;
      
      const mode = e.currentTarget.dataset.mode;
      const useApi = mode === 'api';
      
      this.setData({ 
        useApi: useApi 
      });
      
      wx.showToast({
        title: useApi ? '已切换到API模式' : '已切换到本地模式',
        icon: 'none',
        duration: 1500
      });
      
      // 重置搜索状态
      this.resetSearch();
    },
  }
}) 