Component({
  properties: {
    // 树的数据，遵循特定格式的对象，例如：{ value: 10, left: { value: 5, ... }, right: { value: 15, ... } }
    treeData: {
      type: Object,
      value: null
    },
    // 当前遍历模式: 'preorder', 'inorder', 'postorder', 'levelorder'
    traversalMode: {
      type: String,
      value: 'inorder'
    },
    // 当前任务
    task: {
      type: Object,
      value: null
    },
    // 正确的遍历序列
    correctSequence: {
      type: Array,
      value: []
    }
  },

  data: {
    userSequence: [], // 用户的遍历序列
    selectedNodes: [], // 已选择的节点
    highlightedNodes: [], // 当前高亮显示的节点
    isTraversalComplete: false, // 遍历是否完成
    showFeedback: false, // 是否显示反馈信息
    feedbackMessage: '', // 反馈信息
    isCorrect: false, // 是否正确
    treeDepth: 0, // 树的深度
    treeLayout: [], // 树的布局数据，用于渲染
    tempLayoutData: {}, // 临时布局数据
    animationStep: 0 // 动画步骤
  },

  observers: {
    'treeData': function(treeData) {
      if (treeData) {
        // 计算树的深度和布局
        const depth = this.calculateTreeDepth(treeData);
        this.setData({
          treeDepth: depth,
          treeLayout: this.generateTreeLayout(treeData, depth),
          userSequence: [],
          selectedNodes: [],
          highlightedNodes: [],
          isTraversalComplete: false,
          showFeedback: false
        });
      }
    },
    'traversalMode': function() {
      // 切换遍历模式时，重置状态
      this.resetTraversal();
    },
    'correctSequence': function() {
      // 当正确序列改变时，重置状态
      this.resetTraversal();
    }
  },

  methods: {
    // 计算树的深度
    calculateTreeDepth: function(node, currentDepth = 0) {
      if (!node) return currentDepth - 1;
      
      const leftDepth = this.calculateTreeDepth(node.left, currentDepth + 1);
      const rightDepth = this.calculateTreeDepth(node.right, currentDepth + 1);
      
      return Math.max(leftDepth, rightDepth);
    },
    
    // 生成树的布局数据
    generateTreeLayout: function(root, depth) {
      const layout = [];
      
      // 初始化布局数组
      for (let i = 0; i <= depth; i++) {
        layout.push([]);
      }
      
      // 递归填充布局数组
      this.fillLayoutArray(root, 0, layout);
      
      return layout;
    },
    
    // 递归填充布局数组
    fillLayoutArray: function(node, level, layout) {
      if (!node) return;
      
      layout[level].push({
        value: node.value,
        id: node.id || `node-${node.value}-${level}-${layout[level].length}`,
        hasLeft: !!node.left,
        hasRight: !!node.right
      });
      
      this.fillLayoutArray(node.left, level + 1, layout);
      this.fillLayoutArray(node.right, level + 1, layout);
    },
    
    // 重置遍历状态
    resetTraversal: function() {
      this.setData({
        userSequence: [],
        selectedNodes: [],
        highlightedNodes: [],
        isTraversalComplete: false,
        showFeedback: false,
        feedbackMessage: '',
        isCorrect: false,
        animationStep: 0
      });
    },
    
    // 处理节点点击事件
    onNodeTap: function(e) {
      const nodeId = e.currentTarget.dataset.id;
      const nodeValue = e.currentTarget.dataset.value;
      
      // 如果已经完成遍历，不做任何操作
      if (this.data.isTraversalComplete) return;
      
      // 检查是否已经选择了该节点
      if (this.data.selectedNodes.includes(nodeId)) {
        wx.showToast({
          title: '已选择该节点',
          icon: 'none'
        });
        return;
      }
      
      // 添加到用户序列和已选择节点列表
      const userSequence = [...this.data.userSequence, nodeValue];
      const selectedNodes = [...this.data.selectedNodes, nodeId];
      
      this.setData({
        userSequence,
        selectedNodes,
        highlightedNodes: [nodeId]
      });
      
      // 检查是否完成了遍历
      if (userSequence.length === this.data.correctSequence.length) {
        this.checkTraversal();
      }
    },
    
    // 检查遍历序列是否正确
    checkTraversal: function() {
      const { userSequence, correctSequence } = this.data;
      
      // 检查用户序列是否与正确序列匹配
      const isCorrect = JSON.stringify(userSequence) === JSON.stringify(correctSequence);
      
      let feedbackMessage = isCorrect 
        ? '恭喜，遍历序列正确！' 
        : '遍历序列不正确，请重试';
      
      this.setData({
        isTraversalComplete: true,
        showFeedback: true,
        feedbackMessage,
        isCorrect
      });
      
      // 触发完成事件
      this.triggerEvent('traversalComplete', {
        mode: this.data.traversalMode,
        sequence: userSequence,
        isCorrect
      });
    },
    
    // 展示正确的遍历动画
    showCorrectTraversal: function() {
      const { traversalMode } = this.data;
      
      // 重置状态
      this.setData({
        userSequence: [],
        selectedNodes: [],
        highlightedNodes: [],
        animationStep: 0
      });
      
      // 开始动画
      this.animateTraversal();
    },
    
    // 动画展示遍历过程
    animateTraversal: function() {
      const { correctSequence, animationStep } = this.data;
      
      if (animationStep >= correctSequence.length) {
        // 动画结束
        this.setData({
          isTraversalComplete: true,
          showFeedback: true,
          feedbackMessage: '动画展示完成',
          isCorrect: true
        });
        return;
      }
      
      // 找到对应的节点ID
      const nodeId = this.findNodeIdByValue(correctSequence[animationStep]);
      
      // 更新数据
      this.setData({
        userSequence: this.data.userSequence.concat(correctSequence[animationStep]),
        selectedNodes: this.data.selectedNodes.concat(nodeId),
        highlightedNodes: [nodeId],
        animationStep: animationStep + 1
      });
      
      // 延迟继续动画
      setTimeout(() => {
        this.animateTraversal();
      }, 800);
    },
    
    // 根据节点值查找节点ID
    findNodeIdByValue: function(value) {
      // 遍历布局数据查找对应的节点
      for (let level = 0; level < this.data.treeLayout.length; level++) {
        for (let i = 0; i < this.data.treeLayout[level].length; i++) {
          const node = this.data.treeLayout[level][i];
          if (node.value === value) {
            return node.id;
          }
        }
      }
      return null;
    },
    
    // 重新开始遍历
    restart: function() {
      this.resetTraversal();
    },
    
    // 改变遍历模式
    changeTraversalMode: function(e) {
      const mode = e.currentTarget.dataset.mode;
      this.triggerEvent('modeChange', { mode });
    }
  }
}) 