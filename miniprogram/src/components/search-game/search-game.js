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
    currentAlgorithmInfo: null
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
      
      // 触发事件
      this.triggerEvent('searchStart', {
        searchType: this.data.searchType,
        targetValue: target
      });
    },

    // 生成线性搜索步骤
    generateLinearSearchSteps: function(target) {
      const array = this.data.originalArray;
      const steps = [];
      
      // 初始步骤
      steps.push({
        description: `开始在数组中线性搜索值 ${target}`,
        visualState: array.map((val, idx) => ({
          index: idx,
          status: ''
        }))
      });
      
      // 线性搜索的每一步
      for (let i = 0; i < array.length; i++) {
        const value = array[i];
        
        steps.push({
          description: `检查索引 ${i} 处的值: ${value}`,
          visualState: array.map((val, idx) => ({
            index: idx,
            status: idx < i ? 'checked' : idx === i ? 'current' : ''
          }))
        });
        
        if (value === target) {
          steps.push({
            description: `在索引 ${i} 处找到值 ${target}！`,
            visualState: array.map((val, idx) => ({
              index: idx,
              status: idx < i ? 'checked' : idx === i ? 'found' : ''
            })),
            result: {
              found: true,
              index: i,
              message: `在索引 ${i} 处找到值 ${target}！`
            }
          });
          break;
        }
        
        // 如果是最后一个元素仍未找到
        if (i === array.length - 1) {
          steps.push({
            description: `未找到值 ${target}`,
            visualState: array.map((val, idx) => ({
              index: idx,
              status: 'checked'
            })),
            result: {
              found: false,
              index: -1,
              message: `未找到值 ${target}`
            }
          });
        }
      }
      
      return steps;
    },

    // 生成二分搜索步骤
    generateBinarySearchSteps: function(target) {
      const array = this.data.originalArray;
      const steps = [];
      
      // 初始步骤
      steps.push({
        description: `开始在排序数组中二分搜索值 ${target}`,
        visualState: array.map((val, idx) => ({
          index: idx,
          status: ''
        }))
      });
      
      // 二分搜索的每一步
      let left = 0;
      let right = array.length - 1;
      
      while (left <= right) {
        const mid = Math.floor((left + right) / 2);
        const midValue = array[mid];
        
        // 当前状态：显示当前检查范围
        steps.push({
          description: `检查中间索引 ${mid} 处的值: ${midValue}，当前搜索范围: [${left}-${right}]`,
          visualState: array.map((val, idx) => ({
            index: idx,
            status: idx < left || idx > right ? 'checked' : 
                   idx === mid ? 'current' : 
                   (idx >= left && idx <= right) ? '' : ''
          }))
        });
        
        if (midValue === target) {
          steps.push({
            description: `在索引 ${mid} 处找到值 ${target}！`,
            visualState: array.map((val, idx) => ({
              index: idx,
              status: idx < left || idx > right ? 'checked' : 
                     idx === mid ? 'found' : 
                     (idx >= left && idx <= right) ? '' : ''
            })),
            result: {
              found: true,
              index: mid,
              message: `在索引 ${mid} 处找到值 ${target}！`
            }
          });
          break;
        } else if (midValue < target) {
          // 在右半部分搜索
          left = mid + 1;
          steps.push({
            description: `${midValue} < ${target}，继续在右半部分 [${left}-${right}] 搜索`,
            visualState: array.map((val, idx) => ({
              index: idx,
              status: idx < left ? 'checked' : 
                     (idx >= left && idx <= right) ? '' : 'checked'
            }))
          });
        } else {
          // 在左半部分搜索
          right = mid - 1;
          steps.push({
            description: `${midValue} > ${target}，继续在左半部分 [${left}-${right}] 搜索`,
            visualState: array.map((val, idx) => ({
              index: idx,
              status: idx <= right ? '' : 'checked'
            }))
          });
        }
        
        // 未找到
        if (left > right) {
          steps.push({
            description: `未找到值 ${target}，搜索范围已用尽`,
            visualState: array.map((val, idx) => ({
              index: idx,
              status: 'checked'
            })),
            result: {
              found: false,
              index: -1,
              message: `未找到值 ${target}`
            }
          });
        }
      }
      
      return steps;
    },

    // 应用搜索步骤
    applyStep: function(stepIndex) {
      if (stepIndex >= this.data.searchSteps.length) return;
      
      const step = this.data.searchSteps[stepIndex];
      const visualArray = this.data.visualArray.map((item, index) => {
        const state = step.visualState.find(s => s.index === index);
        return {
          value: item.value,
          status: state ? state.status : ''
        };
      });
      
      this.setData({
        currentStep: stepIndex,
        visualArray: visualArray,
        currentStepDescription: step.description
      });
      
      // 如果有搜索结果，则更新搜索结果
      if (step.result) {
        this.setData({
          isCompleted: true,
          isAutoPlaying: false,
          searchResult: step.result
        });
        
        // 清除自动播放定时器
        if (this.data.autoPlayInterval) {
          clearInterval(this.data.autoPlayInterval);
          this.setData({
            autoPlayInterval: null
          });
        }
        
        // 触发搜索完成事件
        this.triggerEvent('searchComplete', {
          searchType: this.data.searchType,
          result: step.result
        });
      }
    },

    // 下一步
    nextStep: function() {
      if (this.data.currentStep < this.data.searchSteps.length - 1) {
        this.applyStep(this.data.currentStep + 1);
      }
    },

    // 切换自动播放
    toggleAutoPlay: function() {
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
          if (this.data.currentStep < this.data.searchSteps.length - 1) {
            this.applyStep(this.data.currentStep + 1);
          } else {
            clearInterval(interval);
            this.setData({
              isAutoPlaying: false,
              autoPlayInterval: null
            });
          }
        }, 800);
        
        this.setData({
          isAutoPlaying: true,
          autoPlayInterval: interval
        });
      }
    },

    // 重置搜索
    resetSearch: function() {
      // 清除自动播放定时器
      if (this.data.autoPlayInterval) {
        clearInterval(this.data.autoPlayInterval);
      }
      
      // 重置状态
      this.initArray();
      this.setData({
        isSearching: false,
        isAutoPlaying: false,
        isCompleted: false,
        currentStep: 0,
        searchSteps: [],
        currentStepDescription: '请输入要搜索的值并点击"开始搜索"',
        searchResult: {
          found: false,
          index: -1,
          message: ''
        },
        autoPlayInterval: null
      });
    }
  }
}) 