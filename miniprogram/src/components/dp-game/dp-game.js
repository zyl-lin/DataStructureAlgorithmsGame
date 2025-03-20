Component({
  /**
   * 组件的属性列表
   */
  properties: {
    level: {
      type: Number,
      value: 1
    }
  },

  /**
   * 组件的初始数据
   */
  data: {
    // 算法类型
    dpType: 'fibonacci', // 'fibonacci', 'knapsack', 'lcs'
    
    // 可视化状态
    isVisualizing: false,
    isAutoPlaying: false,
    isCompleted: false,
    currentStepDescription: '',
    
    // 斐波那契数列参数
    fibN: 7,
    fibonacciNodes: [],
    fibonacciMemo: [],
    fibResult: null,
    
    // 背包问题参数
    knapsackCapacity: 10,
    knapsackItems: [
      { value: 4, weight: 3 },
      { value: 5, weight: 4 },
      { value: 10, weight: 7 },
      { value: 11, weight: 8 },
      { value: 13, weight: 9 }
    ],
    knapsackDpTable: [],
    currentKnapsackItem: null,
    selectedKnapsackItems: [],
    knapsackResult: null,
    
    // 最长公共子序列参数
    lcsStr1: 'ABCBDAB',
    lcsStr2: 'BDCABA',
    lcsDpTable: [],
    lcsHighlight1: [],
    lcsHighlight2: [],
    lcsResult: '',
    
    // 可视化步骤
    visualizationSteps: [],
    currentStepIndex: -1,
    
    // 自动播放计时器
    autoPlayTimer: null,
    autoPlaySpeed: 800 // 毫秒
  },

  /**
   * 组件的生命周期
   */
  lifetimes: {
    attached() {
      // 根据level设置不同的初始化参数
      this.initLevelSettings();
    },
    detached() {
      // 清理定时器
      this.clearAutoPlayTimer();
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
          dpType: 'fibonacci',
          fibN: 6
        });
      } else if (level === 2) {
        this.setData({
          dpType: 'knapsack',
          knapsackCapacity: 8,
          knapsackItems: [
            { value: 3, weight: 2 },
            { value: 4, weight: 3 },
            { value: 7, weight: 5 }
          ]
        });
      } else if (level === 3) {
        this.setData({
          dpType: 'lcs',
          lcsStr1: 'ABCD',
          lcsStr2: 'ACBD'
        });
      }
    },

    // 切换算法类型
    changeDpType(e) {
      const type = e.currentTarget.dataset.type;
      if (type === this.data.dpType) return;
      
      // 重置状态
      this.resetVisualization();
      
      // 设置新的算法类型
      this.setData({
        dpType: type
      });
    },

    // ======================= 输入参数处理 =======================
    
    // 斐波那契输入变化
    onFibNChange(e) {
      const value = parseInt(e.detail.value) || 0;
      // 限制范围，避免过大导致性能问题
      const fibN = Math.min(Math.max(value, 1), 15);
      this.setData({ fibN });
    },

    // 背包容量变化
    onKnapsackCapacityChange(e) {
      const value = parseInt(e.detail.value) || 0;
      const knapsackCapacity = Math.max(value, 1);
      this.setData({ knapsackCapacity });
    },

    // 背包物品属性变化
    onKnapsackItemChange(e) {
      const { index, field } = e.currentTarget.dataset;
      const value = parseInt(e.detail.value) || 0;
      const positiveValue = Math.max(value, 1);
      
      const knapsackItems = [...this.data.knapsackItems];
      knapsackItems[index][field] = positiveValue;
      
      this.setData({ knapsackItems });
    },

    // 添加背包物品
    addKnapsackItem() {
      const knapsackItems = [...this.data.knapsackItems];
      knapsackItems.push({ value: 5, weight: 4 });
      this.setData({ knapsackItems });
    },

    // 删除背包物品
    removeKnapsackItem() {
      const knapsackItems = [...this.data.knapsackItems];
      if (knapsackItems.length <= 1) return;
      
      knapsackItems.pop();
      this.setData({ knapsackItems });
    },

    // LCS字符串1变化
    onLcsStr1Change(e) {
      const lcsStr1 = e.detail.value || '';
      this.setData({ lcsStr1 });
    },

    // LCS字符串2变化
    onLcsStr2Change(e) {
      const lcsStr2 = e.detail.value || '';
      this.setData({ lcsStr2 });
    },

    // ======================= 可视化控制 =======================
    
    // 开始可视化
    startVisualization() {
      // 重置状态
      this.resetVisualization();
      
      // 准备可视化步骤
      this.prepareVisualizationSteps();
      
      // 开始可视化
      this.setData({
        isVisualizing: true,
        currentStepIndex: -1
      });
      
      // 执行第一步
      this.nextStep();
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
        
        // 显示最终结果
        this.showFinalResult();
        
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
      
      // 重置状态
      this.setData({
        isVisualizing: false,
        isAutoPlaying: false,
        isCompleted: false,
        currentStepIndex: -1,
        currentStepDescription: '',
        visualizationSteps: [],
        
        // 重置算法特定数据
        fibonacciNodes: [],
        fibonacciMemo: [],
        fibResult: null,
        
        knapsackDpTable: [],
        currentKnapsackItem: null,
        selectedKnapsackItems: [],
        knapsackResult: null,
        
        lcsDpTable: [],
        lcsHighlight1: [],
        lcsHighlight2: [],
        lcsResult: ''
      });
    },

    // ======================= 算法实现 =======================
    
    // 准备可视化步骤
    prepareVisualizationSteps() {
      const { dpType } = this.data;
      
      if (dpType === 'fibonacci') {
        this.prepareFibonacciSteps();
      } else if (dpType === 'knapsack') {
        this.prepareKnapsackSteps();
      } else if (dpType === 'lcs') {
        this.prepareLcsSteps();
      }
    },

    // =================== 斐波那契数列可视化 ===================
    
    // 准备斐波那契数列步骤
    prepareFibonacciSteps() {
      const { fibN } = this.data;
      const steps = [];
      const memo = Array(fibN + 1).fill().map(() => ({ value: undefined, status: '' }));
      
      // 初始化记忆化表格
      steps.push({
        type: 'fibonacci-init',
        description: '初始化记忆化数组，准备计算斐波那契数列',
        memo: JSON.parse(JSON.stringify(memo))
      });
      
      // 设置基础情况
      memo[0] = { value: 0, status: 'calculated' };
      memo[1] = { value: 1, status: 'calculated' };
      
      steps.push({
        type: 'fibonacci-base',
        description: '设置基础情况：F(0) = 0, F(1) = 1',
        memo: JSON.parse(JSON.stringify(memo))
      });
      
      // 生成计算树节点和步骤
      const nodeId = 0;
      const nodes = [];
      const rootNode = { id: nodeId, n: fibN, x: 300, y: 50, value: undefined, status: 'current', parent: null };
      nodes.push(rootNode);
      
      this.generateFibonacciSteps(fibN, memo, steps, nodes, rootNode);
      
      this.setData({
        visualizationSteps: steps,
        fibonacciMemo: memo
      });
    },

    // 递归生成斐波那契步骤
    generateFibonacciSteps(n, memo, steps, nodes, currentNode, nodeIdCounter = { value: 0 }) {
      // 如果是基础情况或已计算过
      if (n <= 1 || memo[n].value !== undefined) {
        const value = n <= 1 ? n : memo[n].value;
        
        steps.push({
          type: 'fibonacci-retrieve',
          description: `F(${n}) = ${value} ${n <= 1 ? '(基础情况)' : '(已计算过的值)'}`,
          n,
          value,
          nodeId: currentNode.id,
          nodes: JSON.parse(JSON.stringify(nodes))
        });
        
        // 更新当前节点值和状态
        const nodeIndex = nodes.findIndex(node => node.id === currentNode.id);
        if (nodeIndex !== -1) {
          nodes[nodeIndex].value = value;
          nodes[nodeIndex].status = 'calculated';
        }
        
        return value;
      }
      
      // 添加计算F(n-1)的步骤
      steps.push({
        type: 'fibonacci-compute',
        description: `计算F(${n}) = F(${n-1}) + F(${n-2})，首先计算F(${n-1})`,
        n,
        nodes: JSON.parse(JSON.stringify(nodes))
      });
      
      // 创建F(n-1)子节点
      nodeIdCounter.value++;
      const leftNodeId = nodeIdCounter.value;
      const leftNode = {
        id: leftNodeId,
        n: n - 1,
        x: currentNode.x - 80 / (n * 0.5),
        y: currentNode.y + 80,
        value: undefined,
        status: 'current',
        parent: currentNode.id
      };
      nodes.push(leftNode);
      
      // 计算F(n-1)
      const leftValue = this.generateFibonacciSteps(n - 1, memo, steps, nodes, leftNode, nodeIdCounter);
      
      // 添加计算F(n-2)的步骤
      steps.push({
        type: 'fibonacci-compute',
        description: `继续计算F(${n}) = F(${n-1}) + F(${n-2})，接下来计算F(${n-2})`,
        n,
        nodes: JSON.parse(JSON.stringify(nodes))
      });
      
      // 创建F(n-2)子节点
      nodeIdCounter.value++;
      const rightNodeId = nodeIdCounter.value;
      const rightNode = {
        id: rightNodeId,
        n: n - 2,
        x: currentNode.x + 80 / (n * 0.5),
        y: currentNode.y + 80,
        value: undefined,
        status: 'current',
        parent: currentNode.id
      };
      nodes.push(rightNode);
      
      // 计算F(n-2)
      const rightValue = this.generateFibonacciSteps(n - 2, memo, steps, nodes, rightNode, nodeIdCounter);
      
      // 计算F(n)并存入记忆化数组
      const result = leftValue + rightValue;
      memo[n] = { value: result, status: 'calculated' };
      
      steps.push({
        type: 'fibonacci-save',
        description: `计算得到F(${n}) = F(${n-1}) + F(${n-2}) = ${leftValue} + ${rightValue} = ${result}，并保存到记忆化数组`,
        n,
        value: result,
        memo: JSON.parse(JSON.stringify(memo)),
        nodeId: currentNode.id,
        nodes: JSON.parse(JSON.stringify(nodes))
      });
      
      // 更新当前节点值和状态
      const nodeIndex = nodes.findIndex(node => node.id === currentNode.id);
      if (nodeIndex !== -1) {
        nodes[nodeIndex].value = result;
        nodes[nodeIndex].status = 'calculated';
      }
      
      return result;
    },

    // ===================== 背包问题可视化 ======================
    
    // 准备背包问题步骤
    prepareKnapsackSteps() {
      const { knapsackItems, knapsackCapacity } = this.data;
      const n = knapsackItems.length;
      const W = knapsackCapacity;
      const steps = [];
      
      // 初始化DP表格
      const dp = Array(n + 1).fill().map(() => 
        Array(W + 1).fill().map(() => ({ value: undefined, status: '' }))
      );
      
      // 初始化步骤
      steps.push({
        type: 'knapsack-init',
        description: '初始化背包问题的动态规划表格',
        dp: JSON.parse(JSON.stringify(dp))
      });
      
      // 设置基础情况：第0行全为0（没有物品可选）
      for (let w = 0; w <= W; w++) {
        dp[0][w] = { value: 0, status: 'calculated' };
      }
      
      steps.push({
        type: 'knapsack-base',
        description: '设置基础情况：当没有物品可选时，最大价值为0',
        dp: JSON.parse(JSON.stringify(dp))
      });
      
      // 填充DP表格
      for (let i = 1; i <= n; i++) {
        const item = knapsackItems[i-1];
        
        steps.push({
          type: 'knapsack-item',
          description: `考虑物品${i}：价值=${item.value}，重量=${item.weight}`,
          currentItem: { index: i-1, ...item },
          dp: JSON.parse(JSON.stringify(dp))
        });
        
        for (let w = 0; w <= W; w++) {
          // 步骤：考虑当前容量
          steps.push({
            type: 'knapsack-cell',
            description: `考虑容量为${w}的情况`,
            row: i,
            col: w,
            currentItem: { index: i-1, ...item },
            dp: JSON.parse(JSON.stringify(dp))
          });
          
          if (item.weight > w) {
            // 物品太重，不能放入
            dp[i][w] = { value: dp[i-1][w].value, status: 'calculated' };
            
            steps.push({
              type: 'knapsack-too-heavy',
              description: `物品${i}重量(${item.weight})大于当前容量(${w})，不能放入背包，保持上一行的值：${dp[i][w].value}`,
              row: i,
              col: w,
              dp: JSON.parse(JSON.stringify(dp))
            });
          } else {
            // 可以选择放入或不放入
            const notTake = dp[i-1][w].value;
            const take = dp[i-1][w - item.weight].value + item.value;
            
            // 突出显示相关单元格
            const dpCopy = JSON.parse(JSON.stringify(dp));
            dpCopy[i-1][w].status = 'considered';
            dpCopy[i-1][w - item.weight].status = 'considered';
            
            steps.push({
              type: 'knapsack-choice',
              description: `物品${i}可以放入或不放入背包:
                不放入：取上一行同列的值 = ${notTake}
                放入：取上一行(容量-物品重量)列的值 + 物品价值 = ${dp[i-1][w - item.weight].value} + ${item.value} = ${take}`,
              row: i,
              col: w,
              take,
              notTake,
              dp: dpCopy
            });
            
            // 选择价值更大的选项
            dp[i][w] = { value: Math.max(notTake, take), status: 'calculated' };
            
            steps.push({
              type: 'knapsack-decision',
              description: `选择价值更大的选项：${Math.max(notTake, take)}
                ${take > notTake ? `选择放入物品${i}` : `选择不放入物品${i}`}`,
              row: i,
              col: w,
              decision: take > notTake ? 'take' : 'notTake',
              dp: JSON.parse(JSON.stringify(dp))
            });
          }
        }
      }
      
      // 回溯找出选择的物品
      const selectedItems = [];
      let i = n;
      let w = W;
      
      steps.push({
        type: 'knapsack-backtrack-start',
        description: '回溯找出最优解中选择的物品',
        dp: JSON.parse(JSON.stringify(dp))
      });
      
      // 回溯步骤
      while (i > 0 && w > 0) {
        if (dp[i][w].value !== dp[i-1][w].value) {
          // 选择了当前物品
          const item = knapsackItems[i-1];
          selectedItems.push({ index: i-1, ...item });
          
          // 突出显示回溯过程
          const dpCopy = JSON.parse(JSON.stringify(dp));
          dpCopy[i][w].status = 'current';
          dpCopy[i-1][w].status = 'considered';
          
          steps.push({
            type: 'knapsack-backtrack-item',
            description: `物品${i}被选中(价值=${item.value}, 重量=${item.weight})，因为当前格子值不等于上一行同列的值`,
            row: i,
            col: w,
            selectedItems: [...selectedItems],
            dp: dpCopy
          });
          
          w -= item.weight;
        } else {
          // 没有选择当前物品
          const dpCopy = JSON.parse(JSON.stringify(dp));
          dpCopy[i][w].status = 'current';
          dpCopy[i-1][w].status = 'considered';
          
          steps.push({
            type: 'knapsack-backtrack-skip',
            description: `物品${i}没有被选中，因为当前格子值等于上一行同列的值`,
            row: i,
            col: w,
            dp: dpCopy
          });
        }
        
        i--;
      }
      
      // 最终结果
      steps.push({
        type: 'knapsack-result',
        description: `背包问题求解完成，最大价值为：${dp[n][W].value}`,
        selectedItems,
        result: dp[n][W].value,
        dp: JSON.parse(JSON.stringify(dp))
      });
      
      this.setData({
        visualizationSteps: steps,
        knapsackDpTable: dp,
        knapsackResult: dp[n][W].value
      });
    },

    // ================ 最长公共子序列(LCS)可视化 ================
    
    // 准备LCS步骤
    prepareLcsSteps() {
      const { lcsStr1, lcsStr2 } = this.data;
      const m = lcsStr1.length;
      const n = lcsStr2.length;
      const steps = [];
      
      // 初始化DP表格
      const dp = Array(m + 1).fill().map(() => 
        Array(n + 1).fill().map(() => ({ value: undefined, status: '' }))
      );
      
      // 初始化步骤
      steps.push({
        type: 'lcs-init',
        description: '初始化最长公共子序列的动态规划表格',
        dp: JSON.parse(JSON.stringify(dp))
      });
      
      // 设置基础情况
      for (let i = 0; i <= m; i++) {
        dp[i][0] = { value: 0, status: 'calculated' };
      }
      for (let j = 0; j <= n; j++) {
        dp[0][j] = { value: 0, status: 'calculated' };
      }
      
      steps.push({
        type: 'lcs-base',
        description: '设置基础情况：当任一字符串为空时，LCS长度为0',
        dp: JSON.parse(JSON.stringify(dp))
      });
      
      // 初始化高亮数组
      const highlight1 = Array(m).fill(false);
      const highlight2 = Array(n).fill(false);
      
      // 填充DP表格
      for (let i = 1; i <= m; i++) {
        for (let j = 1; j <= n; j++) {
          // 当前考虑的字符
          const char1 = lcsStr1[i-1];
          const char2 = lcsStr2[j-1];
          
          steps.push({
            type: 'lcs-cell',
            description: `考虑字符 ${char1} (字符串1的第${i}个) 和 ${char2} (字符串2的第${j}个)`,
            row: i,
            col: j,
            char1,
            char2,
            dp: JSON.parse(JSON.stringify(dp))
          });
          
          if (char1 === char2) {
            // 字符匹配
            dp[i][j] = { value: dp[i-1][j-1].value + 1, status: 'calculated' };
            
            // 更新高亮
            const newHighlight1 = [...highlight1];
            const newHighlight2 = [...highlight2];
            newHighlight1[i-1] = true;
            newHighlight2[j-1] = true;
            
            steps.push({
              type: 'lcs-match',
              description: `字符匹配：${char1} = ${char2}，LCS长度加1，值为${dp[i][j].value}`,
              row: i,
              col: j,
              dp: JSON.parse(JSON.stringify(dp)),
              highlight1: newHighlight1,
              highlight2: newHighlight2
            });
          } else {
            // 字符不匹配，取上方和左方的最大值
            const leftValue = dp[i][j-1].value;
            const upValue = dp[i-1][j].value;
            
            // 突出显示相关单元格
            const dpCopy = JSON.parse(JSON.stringify(dp));
            dpCopy[i-1][j].status = 'considered';
            dpCopy[i][j-1].status = 'considered';
            
            steps.push({
              type: 'lcs-mismatch',
              description: `字符不匹配：${char1} ≠ ${char2}，取左侧(${leftValue})和上方(${upValue})的最大值`,
              row: i,
              col: j,
              dp: dpCopy
            });
            
            dp[i][j] = { value: Math.max(leftValue, upValue), status: 'calculated' };
            
            steps.push({
              type: 'lcs-decision',
              description: `选择最大值：${dp[i][j].value}`,
              row: i,
              col: j,
              dp: JSON.parse(JSON.stringify(dp))
            });
          }
        }
      }
      
      // 回溯找出LCS
      steps.push({
        type: 'lcs-backtrack-start',
        description: '回溯找出最长公共子序列',
        dp: JSON.parse(JSON.stringify(dp))
      });
      
      // 回溯过程
      let i = m;
      let j = n;
      let lcs = '';
      
      while (i > 0 && j > 0) {
        // 计算当前状态
        const dpCopy = JSON.parse(JSON.stringify(dp));
        dpCopy[i][j].status = 'current';
        
        if (lcsStr1[i-1] === lcsStr2[j-1]) {
          // 字符匹配，将字符加入LCS
          lcs = lcsStr1[i-1] + lcs;
          
          // 更新高亮
          const newHighlight1 = Array(m).fill(false);
          const newHighlight2 = Array(n).fill(false);
          newHighlight1[i-1] = true;
          newHighlight2[j-1] = true;
          
          steps.push({
            type: 'lcs-backtrack-match',
            description: `字符匹配：${lcsStr1[i-1]} 是LCS的一部分，当前LCS：${lcs}`,
            row: i,
            col: j,
            lcs,
            dp: dpCopy,
            highlight1: newHighlight1,
            highlight2: newHighlight2
          });
          
          i--;
          j--;
        } else if (dp[i-1][j].value >= dp[i][j-1].value) {
          // 向上移动
          dpCopy[i-1][j].status = 'considered';
          
          steps.push({
            type: 'lcs-backtrack-up',
            description: '向上移动，不添加字符',
            row: i,
            col: j,
            lcs,
            dp: dpCopy
          });
          
          i--;
        } else {
          // 向左移动
          dpCopy[i][j-1].status = 'considered';
          
          steps.push({
            type: 'lcs-backtrack-left',
            description: '向左移动，不添加字符',
            row: i,
            col: j,
            lcs,
            dp: dpCopy
          });
          
          j--;
        }
      }
      
      // 最终结果
      steps.push({
        type: 'lcs-result',
        description: `最长公共子序列计算完成，结果为：${lcs}，长度：${lcs.length}`,
        lcs,
        dp: JSON.parse(JSON.stringify(dp))
      });
      
      this.setData({
        visualizationSteps: steps,
        lcsDpTable: dp,
        lcsResult: lcs
      });
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
      
      if (type === 'fibonacci') {
        this.executeFibonacciStep(step);
      } else if (type === 'knapsack') {
        this.executeKnapsackStep(step);
      } else if (type === 'lcs') {
        this.executeLcsStep(step);
      }
    },

    // 执行斐波那契步骤
    executeFibonacciStep(step) {
      const subType = step.type.split('-')[1];
      
      if (subType === 'init' || subType === 'base') {
        // 更新记忆化数组
        this.setData({
          fibonacciMemo: step.memo
        });
      } else if (subType === 'compute' || subType === 'retrieve' || subType === 'save') {
        // 更新节点
        if (step.nodes) {
          this.setData({
            fibonacciNodes: step.nodes
          });
          
          // 绘制节点连线
          this.drawFibonacciLines(step.nodes);
        }
        
        // 更新记忆化数组
        if (step.memo) {
          this.setData({
            fibonacciMemo: step.memo
          });
        }
        
        // 更新结果
        if (subType === 'save' && step.n === this.data.fibN) {
          this.setData({
            fibResult: step.value
          });
        }
      }
    },

    // 执行背包问题步骤
    executeKnapsackStep(step) {
      const subType = step.type.split('-')[1];
      
      // 更新DP表格
      if (step.dp) {
        this.setData({
          knapsackDpTable: step.dp
        });
      }
      
      // 更新当前考虑的物品
      if (step.currentItem) {
        this.setData({
          currentKnapsackItem: step.currentItem
        });
      }
      
      // 更新选中的物品
      if (step.selectedItems) {
        this.setData({
          selectedKnapsackItems: step.selectedItems
        });
      }
      
      // 更新结果
      if (subType === 'result') {
        this.setData({
          knapsackResult: step.result
        });
      }
    },

    // 执行LCS步骤
    executeLcsStep(step) {
      const subType = step.type.split('-')[1];
      
      // 更新DP表格
      if (step.dp) {
        this.setData({
          lcsDpTable: step.dp
        });
      }
      
      // 更新字符高亮
      if (step.highlight1 && step.highlight2) {
        this.setData({
          lcsHighlight1: step.highlight1,
          lcsHighlight2: step.highlight2
        });
      }
      
      // 更新LCS结果
      if (step.lcs !== undefined) {
        this.setData({
          lcsResult: step.lcs
        });
      }
    },

    // 显示最终结果
    showFinalResult() {
      const { dpType, fibResult, knapsackResult, lcsResult } = this.data;
      
      if (dpType === 'fibonacci') {
        this.setData({
          fibResult
        });
      } else if (dpType === 'knapsack') {
        this.setData({
          knapsackResult
        });
      } else if (dpType === 'lcs') {
        this.setData({
          lcsResult
        });
      }
    },

    // 绘制斐波那契节点连线
    drawFibonacciLines(nodes) {
      // 使用canvas画线
      const query = this.createSelectorQuery();
      query.select('#fibonacci-canvas').fields({ node: true, size: true })
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
          
          for (const node of nodes) {
            if (node.parent !== null) {
              const parent = nodes.find(n => n.id === node.parent);
              if (parent) {
                ctx.beginPath();
                ctx.moveTo(parent.x + 50, parent.y + 50);
                ctx.lineTo(node.x + 50, node.y + 50);
                ctx.stroke();
              }
            }
          }
        });
    }
  }
}) 