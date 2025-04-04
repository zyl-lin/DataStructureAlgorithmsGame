Component({
  /**
   * 组件的属性列表
   */
  properties: {
    level: {
      type: Number,
      value: 1
    },
    useApi: {
      type: Boolean,
      value: true
    }
  },

  /**
   * 组件的初始数据
   */
  data: {
    // 算法类型
    greedyType: 'coin', // 'coin', 'activity', 'huffman'
    
    // API相关状态
    isApiMode: true,
    isLoading: false,
    apiError: '',
    animationSpeed: 5,
    showSpeedControl: false,
    
    // 可视化状态
    isVisualizing: false,
    isAutoPlaying: false,
    isCompleted: false,
    currentStepDescription: '',
    
    // 零钱兑换参数
    coinAmount: 63,
    availableCoins: [25, 10, 5, 1],
    selectedCoins: [true, true, true, true],
    coinSelections: [],
    remainingAmount: 0,
    coinResult: {
      count: 0,
      coins: []
    },
    
    // 活动安排参数
    activities: [
      { id: 0, start: 1, end: 4 },
      { id: 1, start: 3, end: 5 },
      { id: 2, start: 0, end: 6 },
      { id: 3, start: 5, end: 7 },
      { id: 4, start: 3, end: 9 },
      { id: 5, start: 6, end: 10 },
      { id: 6, start: 8, end: 11 }
    ],
    timelineHours: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12],
    sortedActivities: [],
    selectedActivities: [],
    activityResult: {
      count: 0,
      activities: []
    },
    
    // 哈夫曼编码参数
    huffmanText: "hello world",
    charFrequencies: [],
    huffmanNodes: [],
    huffmanResult: {
      originalLength: 0,
      compressedLength: 0,
      compressionRatio: 0
    },
    
    // 可视化步骤
    visualizationSteps: [],
    currentStepIndex: -1,
    
    // 自动播放计时器
    autoPlayTimer: null,
    autoPlaySpeed: 800, // 毫秒

    // 哈夫曼可视化容器尺寸
    huffmanContainerSize: {
      width: 300,
      height: 450,
      nodeSpacing: 40,
      verticalSpacing: 60,
      maxNodes: 20
    },
  },

  /**
   * 组件的生命周期
   */
  lifetimes: {
    attached() {
      // 根据level设置不同的初始化参数
      this.initLevelSettings();
      // 初始化哈夫曼容器尺寸
      this.initHuffmanContainerSize();
      // 监听屏幕旋转
      wx.onWindowResize(this.updateHuffmanContainerSize.bind(this));
    },
    detached() {
      // 清理定时器
      this.clearAutoPlayTimer();
      // 移除屏幕旋转监听
      wx.offWindowResize(this.updateHuffmanContainerSize);
    }
  },

  /**
   * 组件的方法列表
   */
  methods: {
    // 初始化关卡设置
    initLevelSettings() {
      const level = this.properties.level;
      
      // 根据关卡设置不同的算法和难度
      if (level === 1) {
        this.setData({
          greedyType: 'coin',
          coinAmount: 42
        });
      } else if (level === 2) {
        this.setData({
          greedyType: 'activity',
          activities: [
            { id: 0, start: 1, end: 3 },
            { id: 1, start: 2, end: 5 },
            { id: 2, start: 4, end: 7 },
            { id: 3, start: 6, end: 9 },
            { id: 4, start: 8, end: 10 }
          ]
        });
      } else if (level === 3) {
        this.setData({
          greedyType: 'huffman',
          huffmanText: "data structures and algorithms"
        });
      }
    },

    // 切换算法类型
    changeGreedyType(e) {
      const type = e.currentTarget.dataset.type;
      if (type === this.data.greedyType) return;
      
      // 重置状态
      this.resetVisualization();
      
      // 设置新的算法类型
      this.setData({
        greedyType: type
      });
    },

    // ======================= 输入参数处理 =======================
    
    // 零钱兑换金额变化
    onCoinAmountChange(e) {
      const value = parseInt(e.detail.value) || 0;
      const coinAmount = Math.max(value, 1);
      this.setData({ coinAmount });
    },

    // 切换硬币选择
    toggleCoin(e) {
      const index = e.currentTarget.dataset.index;
      const selectedCoins = [...this.data.selectedCoins];
      selectedCoins[index] = !selectedCoins[index];
      
      // 确保至少选择一种硬币
      if (selectedCoins.some(selected => selected)) {
        this.setData({ selectedCoins });
      }
    },

    // 活动信息变化
    onActivityChange(e) {
      const { index, field } = e.currentTarget.dataset;
      const value = parseInt(e.detail.value) || 0;
      const positiveValue = Math.max(value, 0);
      
      const activities = [...this.data.activities];
      activities[index][field] = positiveValue;
      
      // 确保结束时间大于开始时间
      if (field === 'start' && activities[index].start >= activities[index].end) {
        activities[index].end = activities[index].start + 1;
      } else if (field === 'end' && activities[index].end <= activities[index].start) {
        activities[index].start = Math.max(0, activities[index].end - 1);
      }
      
      this.setData({ activities });
    },

    // 添加活动
    addActivity() {
      const activities = [...this.data.activities];
      const lastActivity = activities[activities.length - 1];
      const newId = activities.length;
      const newStart = lastActivity ? lastActivity.end : 0;
      const newEnd = newStart + 2;
      
      activities.push({ id: newId, start: newStart, end: newEnd });
      this.setData({ activities });
    },

    // 删除活动
    removeActivity() {
      const activities = [...this.data.activities];
      if (activities.length <= 1) return;
      
      activities.pop();
      this.setData({ activities });
    },

    // 哈夫曼文本变化
    onHuffmanTextChange(e) {
      const huffmanText = e.detail.value || '';
      this.setData({ huffmanText });
    },

    // ======================= 可视化控制 =======================
    
    // 开始可视化
    startVisualization() {
      // 重置状态，避免循环调用reset API
      this.setData({ _skipApiReset: false }, () => {
        this.resetVisualization();
        
        if (this.data.isApiMode) {
          this.startApiVisualization();
        } else {
          // 使用原有的本地实现
          this.prepareVisualizationSteps();
          this.setData({
            isVisualizing: true,
            currentStepIndex: -1
          });
          this.nextStep();
        }
      });
    },

    // 下一步
    nextStep() {
      const { currentStepIndex, visualizationSteps } = this.data;
      const nextIndex = currentStepIndex + 1;
      
      if (nextIndex >= visualizationSteps.length) {
        // 所有步骤完成
        this.setData({
          isCompleted: true,
          currentStepDescription: '算法执行完成！'
        });
        
        // 停止自动播放
        this.clearAutoPlayTimer();
        return;
      }
      
      // 执行下一步
      const step = visualizationSteps[nextIndex];
      this.executeStep(step, nextIndex);
      
      // 更新当前步骤索引
      this.setData({
        currentStepIndex: nextIndex
      });
    },

    // 切换自动播放
    toggleAutoPlay() {
      const { isAutoPlaying } = this.data;
      
      if (isAutoPlaying) {
        // 停止自动播放
        this.clearAutoPlayTimer();
        this.setData({ isAutoPlaying: false });
      } else {
        // 开始自动播放
        this.setData({ isAutoPlaying: true });
        this.startAutoPlay();
      }
    },

    // 开始自动播放
    startAutoPlay() {
      // 清理现有定时器
      this.clearAutoPlayTimer();
      
      // 设置新定时器
      const timer = setInterval(() => {
        // 如果已完成或者不是自动播放状态，则停止
        if (this.data.isCompleted || !this.data.isAutoPlaying) {
          this.clearAutoPlayTimer();
          return;
        }
        
        // 执行下一步
        this.nextStep();
      }, this.data.autoPlaySpeed);
      
      // 保存定时器ID
      this.data.autoPlayTimer = timer;
    },

    // 清理自动播放定时器
    clearAutoPlayTimer() {
      if (this.data.autoPlayTimer) {
        clearInterval(this.data.autoPlayTimer);
        this.data.autoPlayTimer = null;
      }
    },

    // 重置可视化
    resetVisualization() {
      // 清理定时器
      this.clearAutoPlayTimer();
      
      // 如果是API模式，调用API重置，但避免循环调用
      if (this.data.isApiMode && !this.data._skipApiReset) {
        this.resetApi();
        return; // 提前返回，避免重复重置
      }
      
      // 重置状态
      this.setData({
        isVisualizing: false,
        isAutoPlaying: false,
        isCompleted: false,
        currentStepIndex: -1,
        currentStepDescription: '',
        visualizationSteps: [],
        
        // 重置算法特定数据
        coinSelections: [],
        remainingAmount: 0,
        coinResult: { count: 0, coins: [] },
        
        sortedActivities: [],
        selectedActivities: [],
        activityResult: { count: 0, activities: [] },
        
        charFrequencies: [],
        huffmanNodes: [],
        huffmanResult: {
          originalLength: 0,
          compressedLength: 0,
          compressionRatio: 0
        },
        
        // 添加API相关状态重置
        isLoading: false,
        apiError: '',
        _skipApiReset: false // 重置标记
      });
    },

    // 重置API状态
    resetApi() {
      if (!this.data.isApiMode) return;
      
      const api = require('../../services/api').greedy;
      this.setData({ isLoading: true, apiError: '' });
      
      api.reset()
        .then(() => {
          console.log('贪心算法状态重置成功');
          // 设置标记以避免循环调用
          this.setData({ _skipApiReset: true }, () => {
            this.resetVisualization();
          });
        })
        .catch(error => {
          console.error('重置贪心算法状态失败:', error);
          this.setData({
            apiError: '重置状态失败，请重试',
            isLoading: false
          });
        });
    },

    // ======================= 算法实现 =======================
    
    // 准备可视化步骤
    prepareVisualizationSteps() {
      const { greedyType } = this.data;
      
      if (greedyType === 'coin') {
        this.prepareCoinSteps();
      } else if (greedyType === 'activity') {
        this.prepareActivitySteps();
      } else if (greedyType === 'huffman') {
        this.prepareHuffmanSteps();
      }
    },

    // =================== 零钱兑换可视化 ===================
    
    // 准备零钱兑换步骤
    prepareCoinSteps() {
      const { coinAmount, availableCoins, selectedCoins } = this.data;
      const steps = [];
      
      // 获取选中的硬币
      const coins = availableCoins.filter((_, i) => selectedCoins[i]);
      
      // 初始化步骤
      steps.push({
        type: 'coin-init',
        description: `初始化零钱兑换问题，需要找零${coinAmount}元`,
        remainingAmount: coinAmount,
        coinSelections: []
      });
      
      // 对硬币排序
      coins.sort((a, b) => b - a);
      
      steps.push({
        type: 'coin-sort',
        description: `对硬币面值进行排序: ${coins.join(', ')}`,
        remainingAmount: coinAmount,
        coinSelections: []
      });
      
      // 贪心选择硬币
      let remaining = coinAmount;
      const selected = [];
      
      for (const coin of coins) {
        let count = 0;
        
        while (remaining >= coin) {
          // 选择当前硬币
          selected.push(coin);
          count++;
          remaining -= coin;
          
          // 添加步骤
          steps.push({
            type: 'coin-select',
            description: `选择面值为${coin}的硬币，剩余金额为${remaining}`,
            remainingAmount: remaining,
            coinSelections: selected.map((value, index) => ({
              value,
              status: index === selected.length - 1 ? 'current' : 'used'
            }))
          });
        }
        
        if (count > 0) {
          steps.push({
            type: 'coin-summary',
            description: `总共选择了${count}个面值为${coin}的硬币`,
            remainingAmount: remaining,
            coinSelections: selected.map(value => ({
              value,
              status: 'used'
            }))
          });
        }
      }
      
      // 最终结果
      steps.push({
        type: 'coin-result',
        description: `零钱兑换完成，总共使用了${selected.length}个硬币: ${selected.join(', ')}`,
        remainingAmount: 0,
        coinSelections: selected.map(value => ({
          value,
          status: 'used'
        })),
        result: {
          count: selected.length,
          coins: selected
        }
      });
      
      this.setData({
        visualizationSteps: steps,
        remainingAmount: coinAmount
      });
    },

    // ===================== 活动安排可视化 ======================
    
    // 准备活动安排步骤
    prepareActivitySteps() {
      const { activities, timelineHours } = this.data;
      const steps = [];
      
      // 计算最早和最晚时间，以便更新时间轴
      let minTime = Infinity, maxTime = -Infinity;
      for (const activity of activities) {
        minTime = Math.min(minTime, activity.start);
        maxTime = Math.max(maxTime, activity.end);
      }
      
      // 确保时间轴覆盖所有活动
      const hours = [...timelineHours];
      while (hours[0] > minTime) {
        hours.unshift(hours[0] - 1);
      }
      while (hours[hours.length - 1] < maxTime) {
        hours.push(hours[hours.length - 1] + 1);
      }
      
      // 计算每个活动的位置百分比
      const totalTime = hours[hours.length - 1] - hours[0];
      const activitiesWithPosition = activities.map(act => ({
        ...act,
        startPercent: ((act.start - hours[0]) / totalTime) * 100,
        widthPercent: ((act.end - act.start) / totalTime) * 100,
        status: ''
      }));
      
      // 初始化步骤
      steps.push({
        type: 'activity-init',
        description: `初始化活动安排问题，共有${activities.length}个活动`,
        hours,
        activities: activitiesWithPosition,
        selectedActivities: []
      });
      
      // 对活动按结束时间排序
      const sortedActivities = [...activitiesWithPosition].sort((a, b) => a.end - b.end);
      
      steps.push({
        type: 'activity-sort',
        description: '对活动按照结束时间从早到晚排序',
        hours,
        activities: sortedActivities,
        selectedActivities: []
      });
      
      // 贪心选择活动
      const selected = [];
      let lastEndTime = -1;
      
      for (let i = 0; i < sortedActivities.length; i++) {
        const current = sortedActivities[i];
        
        // 更新当前考虑的活动状态
        const activitiesWithStatus = sortedActivities.map((act, index) => ({
          ...act,
          status: index === i ? 'current' : act.status
        }));
        
        steps.push({
          type: 'activity-consider',
          description: `考虑活动${current.id + 1}，开始时间: ${current.start}，结束时间: ${current.end}`,
          hours,
          activities: activitiesWithStatus,
          selectedActivities: [...selected]
        });
        
        if (current.start >= lastEndTime) {
          // 选择当前活动
          selected.push(current);
          lastEndTime = current.end;
          
          // 更新活动状态
          for (let j = 0; j < sortedActivities.length; j++) {
            if (j === i) {
              sortedActivities[j].status = 'selected';
            } else if (sortedActivities[j].start < lastEndTime && sortedActivities[j].status !== 'selected') {
              sortedActivities[j].status = 'rejected';
            }
          }
          
          steps.push({
            type: 'activity-select',
            description: `选择活动${current.id + 1}，更新最晚结束时间为${lastEndTime}`,
            hours,
            activities: [...sortedActivities],
            selectedActivities: [...selected]
          });
        } else {
          // 拒绝当前活动
          sortedActivities[i].status = 'rejected';
          
          steps.push({
            type: 'activity-reject',
            description: `拒绝活动${current.id + 1}，因为它与已选活动冲突`,
            hours,
            activities: [...sortedActivities],
            selectedActivities: [...selected]
          });
        }
      }
      
      // 最终结果
      steps.push({
        type: 'activity-result',
        description: `活动安排完成，最多可安排${selected.length}个活动`,
        hours,
        activities: sortedActivities,
        selectedActivities: selected,
        result: {
          count: selected.length,
          activities: selected
        }
      });
      
      this.setData({
        visualizationSteps: steps,
        timelineHours: hours
      });
    },

    // ================ 哈夫曼编码可视化 ================
    
    // 准备哈夫曼编码步骤
    prepareHuffmanSteps() {
      const { huffmanText } = this.data;
      const steps = [];
      
      // 验证输入
      if (!huffmanText || huffmanText.trim() === '') {
        console.error('哈夫曼编码输入为空');
        this.setData({
          apiError: '请输入有效的文本内容',
          isLoading: false
        });
        return;
      }
      
      // 统计字符频率
      const frequencyMap = new Map();
      for (const char of huffmanText) {
        frequencyMap.set(char, (frequencyMap.get(char) || 0) + 1);
      }
      
      // 转换为数组形式
      const charFrequencies = Array.from(frequencyMap.entries()).map(([char, frequency]) => ({
        char,
        frequency,
        code: '',
        status: ''
      }));
      
      // 验证字符频率
      if (charFrequencies.length === 0) {
        console.error('没有有效的字符频率数据');
        this.setData({
          apiError: '文本解析错误，请检查输入',
          isLoading: false
        });
        return;
      }
      
      // 初始化步骤
      steps.push({
        type: 'huffman-init',
        description: '统计字符频率',
        charFrequencies,
        nodes: []
      });
      
      // 计算布局的基本参数 - 用于居中布局
      const { width: containerWidth, nodeSpacing, verticalSpacing } = this.data.huffmanContainerSize;
      const baseX = containerWidth / 2; // 中心点X坐标
      
      // 根据字符数量动态调整节点间距
      const charCount = charFrequencies.length;
      const actualNodeSpacing = Math.min(
        nodeSpacing,
        (containerWidth - 60) / (charCount + 1) // 减去左右边距，确保节点不会贴边
      );
      
      // 创建初始节点
      let nodes = charFrequencies.map((info, index) => {
        // 计算水平位置，使得节点分布在中心点的两侧
        const totalWidth = (charCount - 1) * actualNodeSpacing; // 所有节点占用的总宽度
        const startX = baseX - (totalWidth / 2); // 从中心点向左偏移一半总宽度
        const x = startX + (index * actualNodeSpacing); // 依次排列节点
        
        return {
          id: index,
          char: info.char,
          frequency: info.frequency,
          left: null,
          right: null,
          x: x,
          y: 40,
          status: '',
          parent: null
        };
      });
      
      // 验证初始节点
      if (nodes.length === 0) {
        console.error('无法创建初始哈夫曼树节点');
        this.setData({
          apiError: '无法生成哈夫曼树',
          isLoading: false
        });
        return;
      }
      
      steps.push({
        type: 'huffman-nodes',
        description: '创建初始哈夫曼树节点',
        charFrequencies,
        nodes: [...nodes]
      });
      
      // 确保我们至少有两个节点，否则无法构建哈夫曼树
      if (nodes.length < 2) {
        // 如果只有一个字符，为其分配编码"0"
        if (nodes.length === 1) {
          const singleChar = nodes[0];
          const updatedFrequencies = charFrequencies.map(info => ({
            ...info,
            code: "0",
            status: 'current'
          }));
          
          steps.push({
            type: 'huffman-result',
            description: `只有一个字符'${singleChar.char}'，分配编码为"0"`,
            charFrequencies: updatedFrequencies,
            nodes: nodes,
            result: {
              originalLength: huffmanText.length * 8,
              compressedLength: huffmanText.length,
              compressionRatio: 87.5 // (1 - 1/8) * 100
            }
          });
          
          this.setData({
            visualizationSteps: steps
          });
          return;
        }
        
        console.error('哈夫曼树节点数量不足');
        this.setData({
          apiError: '字符种类太少，无法构建哈夫曼树',
          isLoading: false
        });
        return;
      }
      
      // 记录所有创建的节点，用于后续引用
      let allNodesMap = {};
      nodes.forEach(node => {
        allNodesMap[node.id] = node;
      });
      
      // 构建哈夫曼树
      let nodeId = nodes.length;
      while (nodes.length > 1) {
        // 选择频率最小的两个节点
        nodes.sort((a, b) => a.frequency - b.frequency);
        
        const left = nodes[0];
        const right = nodes[1];
        
        // 更新节点状态
        const nodesWithStatus = nodes.map((node, index) => ({
          ...node,
          status: index < 2 ? 'current' : ''
        }));
        
        steps.push({
          type: 'huffman-select',
          description: `选择两个频率最小的节点: ${left.char ? `'${left.char}'` : '内部节点'}(${left.frequency}) 和 ${right.char ? `'${right.char}'` : '内部节点'}(${right.frequency})`,
          charFrequencies,
          nodes: nodesWithStatus
        });
        
        // 创建新节点 - 位置计算为两个子节点的中间位置
        const newNode = {
          id: nodeId++,
          char: null,
          frequency: left.frequency + right.frequency,
          left: left.id,
          right: right.id,
          // 水平位置取两个子节点的中点
          x: (left.x + right.x) / 2,
          // 垂直位置在子节点上方，但不超过容器高度
          y: Math.min(Math.max(left.y, right.y) + verticalSpacing, this.data.huffmanContainerSize.height - 60),
          status: 'current',
          parent: null
        };
        
        // 保存新节点到映射表
        allNodesMap[newNode.id] = newNode;
        
        // 更新子节点的父节点
        let updatedNodes = nodesWithStatus.map(node => {
          if (node.id === left.id || node.id === right.id) {
            return { ...node, parent: newNode.id, status: 'processed' };
          }
          return node;
        });
        
        // 添加新节点
        updatedNodes.push(newNode);
        
        steps.push({
          type: 'huffman-merge',
          description: `合并两个节点，创建新节点，频率为 ${newNode.frequency}`,
          charFrequencies,
          nodes: updatedNodes
        });
        
        // 移除已处理的节点，添加新节点
        nodes = nodes.slice(2);
        nodes.push(newNode);
      }
      
      // 根节点
      const root = nodes[0];
      
      // 生成哈夫曼编码时传递所有节点的映射
      this.generateHuffmanCodes(charFrequencies, steps, root.id, '', {}, allNodesMap);
      
      // 计算压缩效果
      const originalLength = huffmanText.length * 8; // 假设原始文本每个字符8位
      let compressedLength = 0;
      for (const char of huffmanText) {
        compressedLength += charFrequencies.find(info => info.char === char).code.length;
      }
      
      const compressionRatio = Math.round((1 - compressedLength / originalLength) * 100);
      
      // 最终结果
      steps.push({
        type: 'huffman-result',
        description: `哈夫曼编码完成，压缩率为 ${compressionRatio}%`,
        charFrequencies,
        nodes: nodes[0].status === 'current' ? nodes : nodes.map(node => ({
          ...node,
          status: node.id === root.id ? 'current' : node.status
        })),
        result: {
          originalLength,
          compressedLength,
          compressionRatio
        }
      });
      
      this.setData({
        visualizationSteps: steps
      });
    },

    // 递归生成哈夫曼编码
    generateHuffmanCodes(charFrequencies, steps, nodeId, code, visited, allNodesMap = null) {
      // 检查是否已访问该节点
      if (!visited) {
        visited = {};
      }
      if (visited[nodeId]) return;
      visited[nodeId] = true;
      
      // 从节点映射中获取节点
      let node;
      if (allNodesMap && allNodesMap[nodeId]) {
        node = allNodesMap[nodeId];
      } else {
        // 从最近的步骤中查找节点
        const recentSteps = steps.slice().reverse();
        for (const step of recentSteps) {
          if (step.nodes && step.nodes.length > 0) {
            node = step.nodes.find(n => n.id === nodeId);
            if (node) break;
          }
        }
      }
      
      // 如果找不到节点，返回
      if (!node) {
        console.error(`找不到ID为${nodeId}的节点`);
        return;
      }
      
      // 如果是叶子节点
      if (node.char !== null) {
        // 更新字符的哈夫曼编码
        const newFrequencies = charFrequencies.map(info => {
          if (info.char === node.char) {
            return { ...info, code, status: 'current' };
          }
          return info;
        });
        
        // 获取最近一次步骤中的所有节点
        let latestNodes = [];
        for (let i = steps.length - 1; i >= 0; i--) {
          if (steps[i].nodes && steps[i].nodes.length > 0) {
            latestNodes = steps[i].nodes;
            break;
          }
        }
        
        // 更新节点状态
        const newNodes = latestNodes.map(n => {
          if (n.id === nodeId) {
            return { ...n, status: 'current' };
          }
          return n;
        });
        
        steps.push({
          type: 'huffman-code',
          description: `字符 '${node.char}' 的哈夫曼编码为: ${code}`,
          charFrequencies: newFrequencies,
          nodes: newNodes
        });
        
        return;
      }
      
      // 递归处理左子树和右子树
      if (node.left !== null) {
        // 使用节点映射检查左子节点是否存在
        if (allNodesMap && allNodesMap[node.left]) {
          this.generateHuffmanCodes(charFrequencies, steps, node.left, code + '0', visited, allNodesMap);
        } else {
          console.error(`找不到左子节点ID为${node.left}的节点`);
        }
      }
      
      if (node.right !== null) {
        // 使用节点映射检查右子节点是否存在
        if (allNodesMap && allNodesMap[node.right]) {
          this.generateHuffmanCodes(charFrequencies, steps, node.right, code + '1', visited, allNodesMap);
        } else {
          console.error(`找不到右子节点ID为${node.right}的节点`);
        }
      }
    },

    // ===================== 步骤执行逻辑 ======================
    
    // 执行单个步骤
    executeStep(step, stepIndex) {
      // 更新步骤描述
      this.setData({
        currentStepDescription: step.description
      });
      
      // 根据步骤类型执行相应操作
      const type = step.type.split('-')[0];
      
      if (type === 'coin') {
        this.executeCoinStep(step);
      } else if (type === 'activity') {
        this.executeActivityStep(step);
      } else if (type === 'huffman') {
        this.executeHuffmanStep(step);
      }
    },

    // 执行零钱兑换步骤
    executeCoinStep(step) {
      const subType = step.type.split('-')[1];
      
      // 更新剩余金额
      if (step.remainingAmount !== undefined) {
        this.setData({
          remainingAmount: step.remainingAmount
        });
      }
      
      // 更新选择的硬币
      if (step.coinSelections) {
        this.setData({
          coinSelections: step.coinSelections
        });
      }
      
      // 更新结果
      if (subType === 'result' && step.result) {
        this.setData({
          coinResult: step.result
        });
      }
    },

    // 执行活动安排步骤
    executeActivityStep(step) {
      const subType = step.type.split('-')[1];
      
      // 更新时间轴
      if (step.hours) {
        this.setData({
          timelineHours: step.hours
        });
      }
      
      // 更新活动
      if (step.activities) {
        this.setData({
          sortedActivities: step.activities
        });
      }
      
      // 更新选择的活动
      if (step.selectedActivities) {
        this.setData({
          selectedActivities: step.selectedActivities
        });
      }
      
      // 更新结果
      if (subType === 'result' && step.result) {
        this.setData({
          activityResult: step.result
        });
      }
    },

    // 执行哈夫曼编码步骤
    executeHuffmanStep(step) {
      const subType = step.type.split('-')[1];
      
      // 更新字符频率
      if (step.charFrequencies) {
        this.setData({
          charFrequencies: step.charFrequencies
        });
      }
      
      // 更新节点
      if (step.nodes) {
        this.setData({
          huffmanNodes: step.nodes
        });
        
        // 绘制连线
        this.drawHuffmanLines(step.nodes);
      }
      
      // 更新结果
      if (subType === 'result' && step.result) {
        this.setData({
          huffmanResult: step.result
        });
      }
    },

    // 绘制哈夫曼树连线
    drawHuffmanLines(nodes) {
      // 使用canvas画线
      const query = this.createSelectorQuery();
      query.select('#huffman-canvas').fields({ node: true, size: true })
        .exec((res) => {
          if (!res[0] || !res[0].node) return;
          
          const canvas = res[0].node;
          const ctx = canvas.getContext('2d');
          
          // 设置canvas尺寸
          const dpr = wx.getSystemInfoSync().pixelRatio;
          canvas.width = res[0].width * dpr;
          canvas.height = res[0].height * dpr;
          ctx.scale(dpr, dpr);
          
          // 清空画布
          ctx.clearRect(0, 0, canvas.width, canvas.height);
          
          // 画连线
          ctx.strokeStyle = '#aaa';
          ctx.lineWidth = 2;
          
          // 节点半径 - 确保与CSS中的节点大小一致
          const nodeRadius = 30; // 修改为与CSS中的节点大小一致，即60rpx/2
          
          for (const node of nodes) {
            if (node.parent !== null) {
              const parent = nodes.find(n => n.id === node.parent);
              if (parent) {
                // 绘制从当前节点到父节点的线
                ctx.beginPath();
                ctx.moveTo(node.x, node.y);
                ctx.lineTo(parent.x, parent.y);
                ctx.stroke();
                
                // 添加 0/1 标签 - 位置计算使其处于线段中点附近
                const isLeft = parent.left === node.id;
                const labelX = (node.x + parent.x) / 2 + (isLeft ? -10 : 10);
                const labelY = (node.y + parent.y) / 2;
                
                ctx.fillStyle = '#333';
                ctx.font = '12px sans-serif';
                ctx.textAlign = 'center';
                ctx.textBaseline = 'middle';
                ctx.fillText(isLeft ? '0' : '1', labelX, labelY);
              }
            }
          }
        });
    },

    // API相关方法
    toggleApiMode: function(e) {
      if (this.data.isVisualizing || this.data.isLoading) return;
      
      const mode = e.currentTarget.dataset.mode;
      const isApiMode = mode === 'api';
      
      this.setData({ 
        isApiMode,
        apiError: '',
        showSpeedControl: isApiMode
      });
      
      // 切换到API模式时重置状态
      if (isApiMode) {
        this.resetApi();
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
    },

    // 修改API可视化方法
    startApiVisualization: function() {
      const api = require('../../services/api').greedy;
      const { greedyType, animationSpeed } = this.data;
      
      this.setData({ isLoading: true, apiError: '' });
      
      let apiMethod;
      let requestData = {};
      
      switch (greedyType) {
        case 'coin':
          apiMethod = api.coinChange;
          requestData = {
            amount: this.data.coinAmount,
            coins: this.data.availableCoins.filter((coin, index) => this.data.selectedCoins[index])
          };
          break;
        case 'activity':
          apiMethod = api.activitySelection;
          // 修改为后端期望的参数格式
          const start = [];
          const finish = [];
          this.data.activities.forEach(activity => {
            start.push(activity.start);
            finish.push(activity.end);
          });
          requestData = {
            start: start,
            finish: finish
          };
          break;
        case 'huffman':
          apiMethod = api.huffman;
          // 修改为后端期望的参数格式 - 计算字符频率
          const text = this.data.huffmanText;
          const charFreqMap = new Map();
          for (const char of text) {
            charFreqMap.set(char, (charFreqMap.get(char) || 0) + 1);
          }
          
          const chars = [];
          const freqs = [];
          charFreqMap.forEach((freq, char) => {
            chars.push(char);
            freqs.push(freq);
          });
          
          requestData = {
            chars: chars,
            freqs: freqs
          };
          break;
        default:
          this.setData({
            apiError: '不支持的算法类型',
            isLoading: false
          });
          return;
      }
      
      if (!apiMethod) {
        this.setData({
          apiError: `无法找到${greedyType}算法的API方法`,
          isLoading: false
        });
        return;
      }
      
      console.log(`发送${greedyType}算法请求:`, requestData);
      
      apiMethod(requestData, true, animationSpeed)
        .then(response => {
          if (response.success) {
            this.handleApiResponse(response.data);
          } else {
            this.setData({
              apiError: '算法执行失败',
              isLoading: false
            });
          }
        })
        .catch(error => {
          console.error('API请求失败:', error);
          this.setData({
            apiError: error.message || '网络请求失败',
            isLoading: false
          });
        });
    },

    // 修改API响应处理方法
    handleApiResponse: function(data) {
      if (data.animation && data.animation.frames) {
        const steps = this.convertApiFramesToSteps(data.animation.frames);
        this.setData({
          visualizationSteps: steps,
          isVisualizing: true,
          currentStepIndex: -1,
          isLoading: false
        });
        this.nextStep();
      } else {
        // 如果没有动画帧，直接显示结果
        this.updateVisualizationState(data.state);
        this.setData({
          isVisualizing: false,
          isCompleted: true,
          isLoading: false
        });
      }
    },

    // 更新可视化状态
    updateVisualizationState: function(state) {
      if (!state) return;
      
      switch (this.data.greedyType) {
        case 'coin':
          this.setData({
            coinSelections: state.selections || [],
            remainingAmount: state.remaining || 0,
            coinResult: {
              count: state.count || 0,
              coins: state.coins || []
            }
          });
          break;
        case 'activity':
          this.setData({
            sortedActivities: state.sorted || [],
            selectedActivities: state.selected || [],
            activityResult: {
              count: state.count || 0,
              activities: state.activities || []
            }
          });
          break;
        case 'huffman':
          this.setData({
            charFrequencies: state.frequencies || [],
            huffmanNodes: state.nodes || [],
            huffmanResult: {
              originalLength: state.originalLength || 0,
              compressedLength: state.compressedLength || 0,
              compressionRatio: state.compressionRatio || 0
            }
          });
          break;
      }
    },

    // 将API帧转换为可视化步骤
    convertApiFramesToSteps: function(frames) {
      return frames.map((frame, index) => {
        const step = {
          description: frame.description || '',
          type: this.data.greedyType
        };
        
        switch (this.data.greedyType) {
          case 'coin':
            step.selections = frame.selections || [];
            step.remaining = frame.remaining || 0;
            step.result = frame.result;
            break;
          case 'activity':
            step.sorted = frame.sorted || [];
            step.selected = frame.selected || [];
            step.result = frame.result;
            break;
          case 'huffman':
            step.frequencies = frame.frequencies || [];
            step.nodes = frame.nodes || [];
            step.result = frame.result;
            break;
        }
        
        return step;
      });
    },

    // 初始化哈夫曼容器尺寸
    initHuffmanContainerSize() {
      const systemInfo = wx.getSystemInfoSync();
      const screenWidth = systemInfo.windowWidth;
      const screenHeight = systemInfo.windowHeight;
      
      // 计算合适的容器尺寸
      const containerWidth = Math.min(screenWidth * 0.9, 600); // 最大宽度600px
      const containerHeight = Math.min(screenHeight * 0.4, 800); // 最大高度800px
      
      // 根据容器大小计算节点间距
      const nodeSpacing = this.calculateNodeSpacing(containerWidth);
      const verticalSpacing = this.calculateVerticalSpacing(containerHeight);
      
      this.setData({
        huffmanContainerSize: {
          width: containerWidth,
          height: containerHeight,
          nodeSpacing,
          verticalSpacing,
          maxNodes: Math.floor(containerWidth / nodeSpacing)
        }
      });
    },

    // 更新哈夫曼容器尺寸（用于屏幕旋转）
    updateHuffmanContainerSize() {
      this.initHuffmanContainerSize();
      // 如果正在可视化，则重新绘制
      if (this.data.isVisualizing && this.data.greedyType === 'huffman') {
        this.drawHuffmanLines(this.data.huffmanNodes);
      }
    },

    // 计算节点水平间距
    calculateNodeSpacing(containerWidth) {
      // 根据容器宽度动态计算合适的节点间距
      const baseSpacing = containerWidth / 10; // 基础间距
      return Math.min(Math.max(baseSpacing, 20), 50); // 限制在20-50px之间
    },

    // 计算节点垂直间距
    calculateVerticalSpacing(containerHeight) {
      // 根据容器高度动态计算合适的垂直间距
      const baseSpacing = containerHeight / 6; // 基础垂直间距
      return Math.min(Math.max(baseSpacing, 40), 80); // 限制在40-80px之间
    },
  }
}) 