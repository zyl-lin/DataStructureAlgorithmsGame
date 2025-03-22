Component({
  properties: {
    array: {
      type: Array,
      value: []
    },
    isApiMode: {
      type: Boolean,
      value: false
    }
  },

  data: {
    selectedAlgorithm: 'bubble',
    displayArray: [],
    originalArray: [],
    arraySize: 20,
    sortingSpeed: 5,
    showValues: true,
    isSorting: false,
    comparisons: 0,
    swaps: 0,
    sortingStatus: '准备就绪',
    algorithmInfo: '冒泡排序是一种简单的排序算法，它重复地遍历要排序的数列，一次比较两个元素，如果他们的顺序错误就把他们交换过来。',
    sortingInterval: null,
    sortingSteps: [],
    currentStep: 0,
    apiError: '',
    isLoading: false
  },

  lifetimes: {
    attached() {
      // 初始化数组
      this.generateNewArray();
    }
  },

  methods: {
    // 选择排序算法
    selectAlgorithm(e) {
      const algorithm = e.currentTarget.dataset.algorithm;
      this.setData({
        selectedAlgorithm: algorithm,
        algorithmInfo: this.getAlgorithmInfo(algorithm)
      });
    },
    
    // 获取算法说明
    getAlgorithmInfo(algorithm) {
      switch (algorithm) {
        case 'bubble':
          return '冒泡排序是一种简单的排序算法，它重复地遍历要排序的数列，一次比较两个元素，如果他们的顺序错误就把他们交换过来。';
        case 'insertion':
          return '插入排序是一种简单直观的排序算法，它的工作原理是通过构建有序序列，对于未排序数据，在已排序序列中从后向前扫描，找到相应位置并插入。';
        case 'quick':
          return '快速排序是一种分治的排序算法，它将一个数组分成两个子数组，将两部分独立地排序。';
        case 'merge':
          return '归并排序是一种分治的排序算法，它将两个已经排序的序列合并成一个序列。';
        default:
          return '';
      }
    },
    
    // 生成新数组
    generateNewArray() {
      const size = this.data.arraySize;
      const newArray = [];
      
      for (let i = 0; i < size; i++) {
        newArray.push(Math.floor(Math.random() * 100) + 1);
      }
      
      // 根据数组值计算高度
      const displayArray = newArray.map(value => {
        return {
          value,
          height: (value / 100) * 280 + 20, // 确保最小高度为20
          color: '#4CAF50'
        };
      });
      
      this.setData({
        originalArray: [...newArray],
        displayArray,
        comparisons: 0,
        swaps: 0,
        sortingStatus: '准备就绪',
        currentStep: 0,
        sortingSteps: []
      });
      
      this.triggerEvent('arrayGenerated', { array: newArray });
    },
    
    // 开始排序
    startSorting() {
      if (this.data.isSorting) return;
      
      this.setData({
        isSorting: true,
        sortingStatus: '排序中...',
        comparisons: 0,
        swaps: 0,
        // 重置显示数组，恢复原始颜色
        displayArray: this.data.displayArray.map(item => ({ ...item, color: '#4CAF50' }))
      });
      
      // 根据选择的算法生成排序步骤
      this.generateSortingSteps();
      
      // 开始可视化排序过程
      this.startVisualization();
    },
    
    // 停止排序
    stopSorting() {
      if (this.data.sortingInterval) {
        clearInterval(this.data.sortingInterval);
      }
      
      this.setData({
        isSorting: false,
        sortingStatus: '已停止',
        sortingInterval: null
      });
    },
    
    // 生成排序步骤
    generateSortingSteps() {
      const algorithm = this.data.selectedAlgorithm;
      const array = [...this.data.originalArray];
      
      let steps = [];
      let comparisons = 0;
      let swaps = 0;
      
      switch (algorithm) {
        case 'bubble':
          ({ steps, comparisons, swaps } = this.bubbleSortSteps(array));
          break;
        case 'insertion':
          ({ steps, comparisons, swaps } = this.insertionSortSteps(array));
          break;
        case 'quick':
          ({ steps, comparisons, swaps } = this.quickSortSteps(array));
          break;
        case 'merge':
          ({ steps, comparisons, swaps } = this.mergeSortSteps(array));
          break;
      }
      
      this.setData({
        sortingSteps: steps,
        currentStep: 0
      });
    },
    
    // 开始可视化排序过程
    startVisualization() {
      const speed = 1100 - this.data.sortingSpeed * 100; // 转换速度值：1-10 -> 1000-100ms
      
      const interval = setInterval(() => {
        if (this.data.currentStep >= this.data.sortingSteps.length) {
          // 排序完成
          this.stopSorting();
          this.setData({
            sortingStatus: '排序完成'
          });
          return;
        }
        
        const step = this.data.sortingSteps[this.data.currentStep];
        
        // 更新显示
        this.visualizeStep(step);
        
        // 移动到下一步
        this.setData({
          currentStep: this.data.currentStep + 1,
          comparisons: step.comparisons,
          swaps: step.swaps
        });
      }, speed);
      
      this.setData({
        sortingInterval: interval
      });
    },
    
    // 可视化单个排序步骤
    visualizeStep(step) {
      const displayArray = [...this.data.displayArray];
      
      // 重置所有元素的颜色
      displayArray.forEach(item => {
        item.color = '#4CAF50';
      });
      
      // 标记当前比较的元素
      if (step.comparing) {
        step.comparing.forEach(index => {
          if (displayArray[index]) {
            displayArray[index].color = '#2196F3'; // 蓝色表示正在比较
          }
        });
      }
      
      // 标记正在交换的元素
      if (step.swapping) {
        step.swapping.forEach(index => {
          if (displayArray[index]) {
            displayArray[index].color = '#FF9800'; // 橙色表示正在交换
          }
        });
      }
      
      // 标记已排序的元素
      if (step.sorted) {
        step.sorted.forEach(index => {
          if (displayArray[index]) {
            displayArray[index].color = '#8BC34A'; // 浅绿色表示已排序
          }
        });
      }
      
      // 更新数组值和高度
      if (step.array) {
        step.array.forEach((value, index) => {
          if (displayArray[index]) {
            displayArray[index].value = value;
            displayArray[index].height = (value / 100) * 280 + 20;
          }
        });
      }
      
      this.setData({
        displayArray
      });
    },
    
    // 冒泡排序步骤生成
    bubbleSortSteps(array) {
      const arr = [...array];
      const steps = [];
      let comparisons = 0;
      let swaps = 0;
      
      const n = arr.length;
      
      for (let i = 0; i < n; i++) {
        let swapped = false;
        
        for (let j = 0; j < n - i - 1; j++) {
          // 添加比较步骤
          comparisons++;
          steps.push({
            array: [...arr],
            comparing: [j, j + 1],
            sorted: Array.from({ length: n - i }, (_, idx) => n - 1 - idx),
            comparisons,
            swaps
          });
          
          if (arr[j] > arr[j + 1]) {
            // 交换元素
            [arr[j], arr[j + 1]] = [arr[j + 1], arr[j]];
            swapped = true;
            swaps++;
            
            // 添加交换步骤
            steps.push({
              array: [...arr],
              swapping: [j, j + 1],
              sorted: Array.from({ length: n - i }, (_, idx) => n - 1 - idx),
              comparisons,
              swaps
            });
          }
        }
        
        // 如果没有交换，说明数组已经排序完成
        if (!swapped) break;
      }
      
      // 最终排序完成的步骤
      steps.push({
        array: [...arr],
        sorted: Array.from({ length: n }, (_, idx) => idx),
        comparisons,
        swaps
      });
      
      return { steps, comparisons, swaps };
    },
    
    // 插入排序步骤生成
    insertionSortSteps(array) {
      const arr = [...array];
      const steps = [];
      let comparisons = 0;
      let swaps = 0;
      
      const n = arr.length;
      
      for (let i = 1; i < n; i++) {
        const key = arr[i];
        let j = i - 1;
        
        // 比较关键元素与已排序部分
        while (j >= 0) {
          comparisons++;
          
          steps.push({
            array: [...arr],
            comparing: [j, i],
            sorted: Array.from({ length: i }, (_, idx) => idx),
            comparisons,
            swaps
          });
          
          if (arr[j] > key) {
            arr[j + 1] = arr[j];
            swaps++;
            
            steps.push({
              array: [...arr],
              swapping: [j, j + 1],
              sorted: Array.from({ length: i }, (_, idx) => idx),
              comparisons,
              swaps
            });
            
            j--;
          } else {
            break;
          }
        }
        
        arr[j + 1] = key;
        
        // 插入步骤
        if (j + 1 !== i) {
          steps.push({
            array: [...arr],
            swapping: [j + 1],
            sorted: Array.from({ length: i + 1 }, (_, idx) => idx),
            comparisons,
            swaps
          });
        }
      }
      
      // 最终排序完成的步骤
      steps.push({
        array: [...arr],
        sorted: Array.from({ length: n }, (_, idx) => idx),
        comparisons,
        swaps
      });
      
      return { steps, comparisons, swaps };
    },
    
    // 快速排序步骤生成
    quickSortSteps(array) {
      const arr = [...array];
      const steps = [];
      let comparisons = 0;
      let swaps = 0;
      
      const quickSort = (arr, low, high, sortedIndices = []) => {
        if (low < high) {
          const pivotIndex = partition(arr, low, high, sortedIndices);
          quickSort(arr, low, pivotIndex - 1, [...sortedIndices, pivotIndex]);
          quickSort(arr, pivotIndex + 1, high, [...sortedIndices, pivotIndex]);
        } else if (low === high) {
          sortedIndices.push(low);
        }
      };
      
      const partition = (arr, low, high, sortedIndices) => {
        const pivot = arr[high];
        let i = low - 1;
        
        // 添加选择基准元素的步骤
        steps.push({
          array: [...arr],
          comparing: [high],
          sorted: [...sortedIndices],
          comparisons,
          swaps
        });
        
        for (let j = low; j < high; j++) {
          comparisons++;
          
          // 添加比较步骤
          steps.push({
            array: [...arr],
            comparing: [j, high],
            sorted: [...sortedIndices],
            comparisons,
            swaps
          });
          
          if (arr[j] <= pivot) {
            i++;
            [arr[i], arr[j]] = [arr[j], arr[i]];
            swaps++;
            
            // 添加交换步骤
            steps.push({
              array: [...arr],
              swapping: [i, j],
              sorted: [...sortedIndices],
              comparisons,
              swaps
            });
          }
        }
        
        [arr[i + 1], arr[high]] = [arr[high], arr[i + 1]];
        swaps++;
        
        // 添加交换基准元素的步骤
        steps.push({
          array: [...arr],
          swapping: [i + 1, high],
          sorted: [...sortedIndices, i + 1],
          comparisons,
          swaps
        });
        
        return i + 1;
      };
      
      quickSort(arr, 0, arr.length - 1);
      
      // 最终排序完成的步骤
      steps.push({
        array: [...arr],
        sorted: Array.from({ length: arr.length }, (_, idx) => idx),
        comparisons,
        swaps
      });
      
      return { steps, comparisons, swaps };
    },
    
    // 归并排序步骤生成
    mergeSortSteps(array) {
      const arr = [...array];
      const steps = [];
      let comparisons = 0;
      let swaps = 0;
      
      const mergeSort = (arr, start, end) => {
        if (start < end) {
          const mid = Math.floor((start + end) / 2);
          
          mergeSort(arr, start, mid);
          mergeSort(arr, mid + 1, end);
          
          merge(arr, start, mid, end);
        }
      };
      
      const merge = (arr, start, mid, end) => {
        const leftArr = arr.slice(start, mid + 1);
        const rightArr = arr.slice(mid + 1, end + 1);
        
        let i = 0, j = 0, k = start;
        
        while (i < leftArr.length && j < rightArr.length) {
          comparisons++;
          
          // 添加比较步骤
          steps.push({
            array: [...arr],
            comparing: [start + i, mid + 1 + j],
            sorted: [],
            comparisons,
            swaps
          });
          
          if (leftArr[i] <= rightArr[j]) {
            arr[k] = leftArr[i];
            i++;
          } else {
            arr[k] = rightArr[j];
            j++;
            swaps++;
          }
          
          k++;
          
          // 添加合并步骤
          steps.push({
            array: [...arr],
            swapping: [k - 1],
            sorted: [],
            comparisons,
            swaps
          });
        }
        
        while (i < leftArr.length) {
          arr[k] = leftArr[i];
          i++;
          k++;
          
          // 添加剩余左侧元素的步骤
          steps.push({
            array: [...arr],
            swapping: [k - 1],
            sorted: [],
            comparisons,
            swaps
          });
        }
        
        while (j < rightArr.length) {
          arr[k] = rightArr[j];
          j++;
          k++;
          
          // 添加剩余右侧元素的步骤
          steps.push({
            array: [...arr],
            swapping: [k - 1],
            sorted: [],
            comparisons,
            swaps
          });
        }
      };
      
      mergeSort(arr, 0, arr.length - 1);
      
      // 最终排序完成的步骤
      steps.push({
        array: [...arr],
        sorted: Array.from({ length: arr.length }, (_, idx) => idx),
        comparisons,
        swaps
      });
      
      return { steps, comparisons, swaps };
    },
    
    // 改变数组大小
    onArraySizeChange(e) {
      this.setData({
        arraySize: e.detail.value
      });
      this.generateNewArray();
    },
    
    // 改变排序速度
    onSortingSpeedChange(e) {
      this.setData({
        sortingSpeed: e.detail.value
      });
    },
    
    // 显示/隐藏数值
    onShowValuesChange(e) {
      this.setData({
        showValues: e.detail.value
      });
    },
    
    // 重置排序状态
    resetSort() {
      if (this.data.isSorting) {
        this.stopSorting();
      }

      if (this.data.isApiMode) {
        this.resetSortApi();
      }
      
      // 重置本地状态
      this.setData({
        displayArray: this.data.displayArray.map(item => ({ ...item, color: '#4CAF50' })),
        comparisons: 0,
        swaps: 0,
        sortingStatus: '已重置',
        currentStep: 0,
        sortingSteps: [],
        apiError: '',
        isLoading: false
      });
      
      // 触发重置事件
      this.triggerEvent('reset');
    },
    
    // 重置API状态
    resetSortApi() {
      const api = require('../../services/api');
      this.setData({ isLoading: true });
      
      api.sorting.reset()
        .then(() => {
          console.log('排序状态重置成功');
        })
        .catch(error => {
          console.error('重置排序状态失败:', error);
          this.setData({
            apiError: '重置状态失败，请重试'
          });
        })
        .finally(() => {
          this.setData({ isLoading: false });
        });
    }
  }
}) 