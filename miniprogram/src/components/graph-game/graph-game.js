Component({
  properties: {
    // 图的数据，形式为邻接表，例如：{ nodes: ["A", "B", "C"], edges: [[1, 2], [0, 2], [0, 1]] }
    graphData: {
      type: Object,
      value: null
    },
    // 当前遍历模式: 'dfs', 'bfs'
    traversalMode: {
      type: String,
      value: 'dfs'
    },
    // 起始节点的索引
    startNodeIndex: {
      type: Number,
      value: 0
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
    selectedNodes: [], // 已选择的节点索引
    highlightedNodes: [], // 当前高亮显示的节点索引
    highlightedEdges: [], // 当前高亮显示的边索引
    isTraversalComplete: false, // 遍历是否完成
    showFeedback: false, // 是否显示反馈信息
    feedbackMessage: '', // 反馈信息
    isCorrect: false, // 是否正确
    nodePositions: [], // 节点位置数据，用于渲染
    edgeData: [], // 边的数据，用于渲染
    animationStep: 0, // 动画步骤
    canvasContext: null, // Canvas绘图上下文
    canvasWidth: 300, // Canvas宽度
    canvasHeight: 200, // Canvas高度
    nodeRadius: 20, // 节点半径
    nodeColor: '#4caf50', // 默认节点颜色
    selectedNodeColor: '#2196f3', // 已选择节点颜色
    highlightNodeColor: '#f44336', // 高亮节点颜色
    edgeColor: '#999', // 默认边颜色
    highlightEdgeColor: '#4caf50', // 高亮边颜色
    firstRender: true // 是否首次渲染
  },

  lifetimes: {
    attached: function() {
      this.initCanvas();
    },
    detached: function() {
      // 清理资源
      this.data.canvasContext = null;
    }
  },

  observers: {
    'graphData': function(graphData) {
      if (graphData) {
        this.generateGraphLayout();
        this.resetTraversal();
      }
    },
    'traversalMode, startNodeIndex': function() {
      // 切换遍历模式或起始节点时，重置状态
      this.resetTraversal();
    }
  },

  methods: {
    // 初始化Canvas
    initCanvas: function() {
      const query = this.createSelectorQuery();
      query.select('#graph-canvas')
        .fields({ node: true, size: true })
        .exec((res) => {
          if (res[0]) {
            const canvas = res[0].node;
            const ctx = canvas.getContext('2d');
            
            // 设置Canvas尺寸
            const dpr = wx.getSystemInfoSync().pixelRatio;
            canvas.width = res[0].width * dpr;
            canvas.height = res[0].height * dpr;
            ctx.scale(dpr, dpr);
            
            this.setData({
              canvasContext: ctx,
              canvasWidth: res[0].width,
              canvasHeight: res[0].height
            });
            
            if (this.data.graphData) {
              this.generateGraphLayout();
              this.drawGraph();
            }
          }
        });
    },
    
    // 生成图的布局（节点位置）
    generateGraphLayout: function() {
      const { graphData, canvasWidth, canvasHeight, nodeRadius } = this.data;
      if (!graphData || !graphData.nodes) return;
      
      const nodeCount = graphData.nodes.length;
      const nodePositions = [];
      const edgeData = [];
      
      // 计算节点位置 - 使用圆形布局
      if (nodeCount <= 8) {
        // 圆形布局适合节点较少的情况
        const centerX = canvasWidth / 2;
        const centerY = canvasHeight / 2;
        const radius = Math.min(canvasWidth, canvasHeight) / 2 - nodeRadius * 2;
        
        for (let i = 0; i < nodeCount; i++) {
          const angle = (2 * Math.PI * i) / nodeCount;
          const x = centerX + radius * Math.cos(angle);
          const y = centerY + radius * Math.sin(angle);
          nodePositions.push({ x, y });
        }
      } else {
        // 网格布局适合节点较多的情况
        const cols = Math.ceil(Math.sqrt(nodeCount));
        const rows = Math.ceil(nodeCount / cols);
        const cellWidth = canvasWidth / (cols + 1);
        const cellHeight = canvasHeight / (rows + 1);
        
        let index = 0;
        for (let row = 0; row < rows; row++) {
          for (let col = 0; col < cols; col++) {
            if (index < nodeCount) {
              const x = (col + 1) * cellWidth;
              const y = (row + 1) * cellHeight;
              nodePositions.push({ x, y });
              index++;
            }
          }
        }
      }
      
      // 生成边的数据
      if (graphData.edges) {
        for (let i = 0; i < graphData.edges.length; i++) {
          for (let j = 0; j < graphData.edges[i].length; j++) {
            const targetIndex = graphData.edges[i][j];
            // 避免重复边
            if (targetIndex > i) {
              edgeData.push({
                source: i,
                target: targetIndex
              });
            }
          }
        }
      }
      
      this.setData({ nodePositions, edgeData });
      
      // 在下一个渲染周期绘制图
      if (this.data.canvasContext) {
        this.drawGraph();
      }
    },
    
    // 绘制图
    drawGraph: function() {
      const { 
        canvasContext: ctx, canvasWidth, canvasHeight, 
        nodePositions, edgeData, graphData,
        nodeRadius, nodeColor, selectedNodeColor, highlightNodeColor,
        edgeColor, highlightEdgeColor, selectedNodes, highlightedNodes, highlightedEdges
      } = this.data;
      
      if (!ctx || !graphData || !graphData.nodes) return;
      
      // 清空Canvas
      ctx.clearRect(0, 0, canvasWidth, canvasHeight);
      
      // 绘制边
      for (let i = 0; i < edgeData.length; i++) {
        const edge = edgeData[i];
        const sourcePos = nodePositions[edge.source];
        const targetPos = nodePositions[edge.target];
        
        ctx.beginPath();
        ctx.moveTo(sourcePos.x, sourcePos.y);
        ctx.lineTo(targetPos.x, targetPos.y);
        
        // 检查是否为高亮显示的边
        const isHighlighted = highlightedEdges.some(e => 
          (e.source === edge.source && e.target === edge.target) || 
          (e.source === edge.target && e.target === edge.source)
        );
        
        ctx.strokeStyle = isHighlighted ? highlightEdgeColor : edgeColor;
        ctx.lineWidth = isHighlighted ? 3 : 2;
        ctx.stroke();
      }
      
      // 绘制节点
      for (let i = 0; i < nodePositions.length; i++) {
        const pos = nodePositions[i];
        const isSelected = selectedNodes.includes(i);
        const isHighlighted = highlightedNodes.includes(i);
        
        // 绘制圆形
        ctx.beginPath();
        ctx.arc(pos.x, pos.y, nodeRadius, 0, 2 * Math.PI);
        
        // 根据状态设置颜色
        if (isHighlighted) {
          ctx.fillStyle = highlightNodeColor;
        } else if (isSelected) {
          ctx.fillStyle = selectedNodeColor;
        } else {
          ctx.fillStyle = nodeColor;
        }
        
        ctx.fill();
        
        // 绘制节点文本
        ctx.fillStyle = 'white';
        ctx.font = 'bold 14px Arial';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.fillText(graphData.nodes[i], pos.x, pos.y);
      }
    },
    
    // 重置遍历状态
    resetTraversal: function() {
      this.setData({
        userSequence: [],
        selectedNodes: [],
        highlightedNodes: [],
        highlightedEdges: [],
        isTraversalComplete: false,
        showFeedback: false,
        feedbackMessage: '',
        isCorrect: false,
        animationStep: 0
      }, () => {
        if (this.data.canvasContext) {
          this.drawGraph();
        }
      });
    },
    
    // 处理节点点击事件
    onNodeTap: function(e) {
      const { x, y } = e.detail;
      const nodeIndex = this.hitTest(x, y);
      
      // 如果未点击到节点或已经完成遍历，不做任何操作
      if (nodeIndex === -1 || this.data.isTraversalComplete) return;
      
      const { graphData } = this.data;
      
      // 检查是否已经选择了该节点
      if (this.data.selectedNodes.includes(nodeIndex)) {
        wx.showToast({
          title: '已选择该节点',
          icon: 'none'
        });
        return;
      }
      
      // 检查是否为合法的下一个节点
      if (!this.isValidNextNode(nodeIndex)) {
        wx.showToast({
          title: '请按照遍历顺序选择节点',
          icon: 'none'
        });
        return;
      }
      
      // 添加到用户序列和已选择节点列表
      const userSequence = [...this.data.userSequence, graphData.nodes[nodeIndex]];
      const selectedNodes = [...this.data.selectedNodes, nodeIndex];
      
      // 更新高亮边
      const highlightedEdges = [...this.data.highlightedEdges];
      if (selectedNodes.length > 1) {
        const lastNode = selectedNodes[selectedNodes.length - 2];
        highlightedEdges.push({
          source: lastNode,
          target: nodeIndex
        });
      }
      
      this.setData({
        userSequence,
        selectedNodes,
        highlightedNodes: [nodeIndex],
        highlightedEdges
      }, () => {
        this.drawGraph();
        
        // 检查是否完成了遍历
        if (userSequence.length === this.data.correctSequence.length) {
          this.checkTraversal();
        }
      });
    },
    
    // 检查是否为合法的下一个节点
    isValidNextNode: function(nodeIndex) {
      const { selectedNodes, graphData, traversalMode, startNodeIndex } = this.data;
      
      // 如果还没有选择节点，第一个节点必须是起始节点
      if (selectedNodes.length === 0) {
        return nodeIndex === startNodeIndex;
      }
      
      // 获取最后一个选择的节点
      const lastNode = selectedNodes[selectedNodes.length - 1];
      
      // 检查是否存在边连接
      const neighbors = graphData.edges[lastNode] || [];
      return neighbors.includes(nodeIndex);
    },
    
    // 点击canvas时的坐标转换和命中测试
    onCanvasTap: function(e) {
      const { x, y } = e.detail;
      this.onNodeTap({
        detail: {
          x: x,
          y: y
        }
      });
    },
    
    // 节点命中测试
    hitTest: function(x, y) {
      const { nodePositions, nodeRadius } = this.data;
      
      for (let i = 0; i < nodePositions.length; i++) {
        const node = nodePositions[i];
        const dist = Math.sqrt(Math.pow(x - node.x, 2) + Math.pow(y - node.y, 2));
        
        if (dist <= nodeRadius) {
          return i; // 返回命中的节点索引
        }
      }
      
      return -1; // 未命中任何节点
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
      // 重置状态
      this.setData({
        userSequence: [],
        selectedNodes: [],
        highlightedNodes: [],
        highlightedEdges: [],
        animationStep: 0
      }, () => {
        this.drawGraph();
        
        // 开始动画
        this.animateTraversal();
      });
    },
    
    // 动画展示遍历过程
    animateTraversal: function() {
      const { correctSequence, animationStep, graphData } = this.data;
      
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
      
      // 查找当前节点的索引
      const nodeName = correctSequence[animationStep];
      const nodeIndex = graphData.nodes.indexOf(nodeName);
      
      if (nodeIndex === -1) return;
      
      // 更新数据
      const userSequence = this.data.userSequence.concat([nodeName]);
      const selectedNodes = this.data.selectedNodes.concat([nodeIndex]);
      
      // 更新高亮边
      const highlightedEdges = [...this.data.highlightedEdges];
      if (selectedNodes.length > 1) {
        const lastNode = selectedNodes[selectedNodes.length - 2];
        highlightedEdges.push({
          source: lastNode,
          target: nodeIndex
        });
      }
      
      this.setData({
        userSequence,
        selectedNodes,
        highlightedNodes: [nodeIndex],
        highlightedEdges,
        animationStep: animationStep + 1
      }, () => {
        this.drawGraph();
        
        // 延迟继续动画
        setTimeout(() => {
          this.animateTraversal();
        }, 800);
      });
    },
    
    // 重新开始遍历
    restart: function() {
      this.resetTraversal();
    },
    
    // 更改遍历模式
    changeTraversalMode: function(e) {
      const newMode = e.currentTarget.dataset.mode;
      if (this.data.traversalMode !== newMode) {
        this.setData({
          traversalMode: newMode
        });
        
        // 显示提示
        wx.showToast({
          title: newMode === 'dfs' ? '已切换到深度优先遍历' : '已切换到广度优先遍历',
          icon: 'none',
          duration: 1500
        });
        
        // 重置遍历状态
        this.resetTraversal();
      }
    },
    
    // 更改起始节点
    changeStartNode: function(e) {
      const index = parseInt(e.currentTarget.dataset.index);
      this.triggerEvent('startNodeChange', { index });
    }
  }
}) 