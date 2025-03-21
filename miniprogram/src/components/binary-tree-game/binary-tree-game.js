const api = require('../../services/api');

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
    },
    useApi: {
      type: Boolean,
      value: true
    },
    animationSpeed: {
      type: Number,
      value: 5
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
    animationStep: 0, // 动画步骤
    isLoading: false, // 是否正在加载API数据
    animationFrames: [], // 遍历动画帧
    currentFrameIndex: 0, // 当前动画帧索引
    apiError: '',
    isApiMode: true,
    showSpeedControl: false,
    nodeValueInput: '', // 节点值输入
    hasNodes: false, // 是否有节点
    currentTreeState: null, // 当前二叉树状态
    levelWidths: [], // 每层的宽度
    insertHistory: [], // 存储插入的节点历史记录
    traversalResult: null, // 存储遍历结果
    nodeConnections: [] // 存储节点之间的连接信息
  },

  observers: {
    'treeData': function(treeData) {
      if (treeData) {
        // 计算树的深度和布局
        const depth = this.calculateTreeDepth(treeData);
        const layout = this.generateTreeLayout(treeData, depth);
        // 计算节点连接
        const connections = this.calculateNodeConnections(treeData, layout, depth);
        
        this.setData({
          treeDepth: depth,
          treeLayout: layout,
          nodeConnections: connections,
          userSequence: [],
          selectedNodes: [],
          highlightedNodes: [],
          isTraversalComplete: false,
          showFeedback: false,
          hasNodes: true,
          currentTreeState: treeData
        });
      } else {
        this.setData({
          hasNodes: false,
          currentTreeState: null,
          nodeConnections: []
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

  lifetimes: {
    attached: function() {
      // 当组件实例进入页面节点树时执行
      if (this.data.isApiMode) {
        // 先重置服务端状态，再获取当前二叉树状态
        api.binaryTree.reset().then(() => {
          console.log('成功重置二叉树状态');
          // 重置后再获取当前状态（应该是空的）
          this.fetchBinaryTreeState();
        }).catch(error => {
          console.error('重置二叉树状态失败:', error);
          // 即使重置失败，也尝试获取当前状态
          this.fetchBinaryTreeState();
        });
      }
      
      // 初始化插入历史
      this.setData({
        insertHistory: []
      });
    },
    
    detached: function() {
      // 当组件实例从页面节点树移除时执行
      if (this.data.isApiMode) {
        // 重置服务端的二叉树状态
        api.binaryTree.reset().catch(error => {
          console.error('重置二叉树状态失败:', error);
        });
      }
    }
  },

  methods: {
    // 计算节点连接线信息
    calculateNodeConnections: function(node, layout, depth) {
      const connections = [];
      if (!node || !layout || layout.length === 0) return connections;
      
      // 定义初始坐标和层宽
      const baseX = 500;
      const baseY = 50;
      const baseWidth = 800; // 与后端保持一致的宽度
      
      // 单位转换因子 - 将计算像素转换为rpx单位
      // 一般情况下，微信小程序中2rpx = 1px，但这取决于设备
      const unitConversionFactor = 0.5; // 像素到rpx的转换因子，调整为0.75以缩短连接线
      
      console.log('[树连接计算] 初始参数 - 根节点位置:(x=' + baseX + ', y=' + baseY + '), 基础层宽:' + baseWidth + ', 单位转换因子:' + unitConversionFactor);
      
      // 创建节点坐标映射
      const nodeCoords = new Map();
      
      // 使用层序遍历计算所有节点的坐标
      const calculateCoordinates = (rootNode) => {
        if (!rootNode) return;
        
        const queue = [{node: rootNode, level: 0, position: 0, parentX: baseX, parentY: baseY}];
        
        // 设置根节点坐标
        nodeCoords.set(`node-${rootNode.value}-0-0`, {x: baseX, y: baseY});
        console.log('[节点坐标] 根节点 value=' + rootNode.value + ', 位置:(x=' + baseX + ', y=' + baseY + ')');
        
        while (queue.length > 0) {
          const {node, level, position, parentX, parentY} = queue.shift();
          
          // 计算当前节点坐标
          const currentLevel = level;
          const currentPos = position;
          const nodeId = `node-${node.value}-${currentLevel}-${currentPos}`;
          
          // 已经计算过的节点不需要重复计算
          if (!nodeCoords.has(nodeId)) {
            nodeCoords.set(nodeId, {x: parentX, y: parentY});
            console.log('[节点坐标] 节点ID:' + nodeId + ', 位置:(x=' + parentX + ', y=' + parentY + ')');
          }
          
          // 处理左子节点
          if (node.left) {
            const childLevel = level + 1;
            const childPos = position * 2;
            const childId = `node-${node.left.value}-${childLevel}-${childPos}`;
            
            // 计算当前层级
            const nodeLevel = childLevel;
            // 计算水平间距 - 与后端保持一致
            const horizontalOffset = Math.max(100, Math.floor(baseWidth / Math.pow(2, nodeLevel)));
            // 计算子节点坐标
            const childX = parentX - horizontalOffset;
            const childY = baseY + nodeLevel * 100; // 与后端保持一致的垂直距离
            
            console.log('[连接计算-左] 父节点:' + node.value + ', 子节点:' + node.left.value + 
                        ', 层级:' + childLevel + ', 位置:' + childPos);
            console.log('[连接计算-左] 水平偏移量:' + horizontalOffset + 
                        ', 计算公式: Math.max(100, ' + baseWidth + ' / Math.pow(2, ' + nodeLevel + '))');
            
            // 保存子节点坐标
            nodeCoords.set(childId, {x: childX, y: childY});
            console.log('[节点坐标] 左子节点ID:' + childId + ', 位置:(x=' + childX + ', y=' + childY + ')');
            
            // 计算连接线长度
            const dx = childX - parentX;
            const dy = childY - parentY;
            const pixelLength = Math.sqrt(Math.pow(dx, 2) + Math.pow(dy, 2));
            // 应用单位转换因子，转换为rpx
            const length = pixelLength * unitConversionFactor;
            
            console.log('[连接长度-左] 水平距离:' + dx + ', 垂直距离:' + dy + 
                        ', 像素长度:' + pixelLength.toFixed(2) + 
                        ', rpx长度:' + length.toFixed(2) + 
                        ', 计算公式: Math.sqrt(' + dx + '² + ' + dy + '²) * ' + unitConversionFactor);
            
            // 添加连接信息
            connections.push({
              from: nodeId,
              to: childId,
              type: 'left',
              length: length,
              fromX: parentX,
              fromY: parentY,
              toX: childX,
              toY: childY
            });
            
            // 将左子节点加入队列
            queue.push({
              node: node.left, 
              level: childLevel, 
              position: childPos,
              parentX: childX,
              parentY: childY
            });
          }
          
          // 处理右子节点
          if (node.right) {
            const childLevel = level + 1;
            const childPos = position * 2 + 1;
            const childId = `node-${node.right.value}-${childLevel}-${childPos}`;
            
            // 计算当前层级
            const nodeLevel = childLevel;
            // 计算水平间距 - 与后端保持一致
            const horizontalOffset = Math.max(100, Math.floor(baseWidth / Math.pow(2, nodeLevel)));
            // 计算子节点坐标
            const childX = parentX + horizontalOffset;
            const childY = baseY + nodeLevel * 100; // 与后端保持一致的垂直距离
            
            console.log('[连接计算-右] 父节点:' + node.value + ', 子节点:' + node.right.value + 
                        ', 层级:' + childLevel + ', 位置:' + childPos);
            console.log('[连接计算-右] 水平偏移量:' + horizontalOffset + 
                        ', 计算公式: Math.max(100, ' + baseWidth + ' / Math.pow(2, ' + nodeLevel + '))');
            
            // 保存子节点坐标
            nodeCoords.set(childId, {x: childX, y: childY});
            console.log('[节点坐标] 右子节点ID:' + childId + ', 位置:(x=' + childX + ', y=' + childY + ')');
            
            // 计算连接线长度
            const dx = childX - parentX;
            const dy = childY - parentY;
            const pixelLength = Math.sqrt(Math.pow(dx, 2) + Math.pow(dy, 2));
            // 应用单位转换因子，转换为rpx
            const length = pixelLength * unitConversionFactor;
            
            console.log('[连接长度-右] 水平距离:' + dx + ', 垂直距离:' + dy + 
                        ', 像素长度:' + pixelLength.toFixed(2) + 
                        ', rpx长度:' + length.toFixed(2) + 
                        ', 计算公式: Math.sqrt(' + dx + '² + ' + dy + '²) * ' + unitConversionFactor);
            
            // 添加连接信息
            connections.push({
              from: nodeId,
              to: childId,
              type: 'right',
              length: length,
              fromX: parentX,
              fromY: parentY,
              toX: childX,
              toY: childY
            });
            
            // 将右子节点加入队列
            queue.push({
              node: node.right, 
              level: childLevel, 
              position: childPos,
              parentX: childX,
              parentY: childY
            });
          }
        }
      };
      
      // 从根节点开始计算坐标
      calculateCoordinates(node);
      
      // 确保connections中有唯一的连接记录
      const uniqueConnections = connections.reduce((acc, conn) => {
        const key = `${conn.from}-${conn.to}`;
        if (!acc.find(c => `${c.from}-${c.to}` === key)) {
          acc.push(conn);
        }
        return acc;
      }, []);
      
      console.log('[连接总结] 总共生成连接数量:' + uniqueConnections.length);
      uniqueConnections.forEach((conn, index) => {
        console.log(`[连接详情 #${index+1}] 从:${conn.from} 到:${conn.to}, 类型:${conn.type}, 长度:${conn.length.toFixed(2)}rpx`);
      });
      
      return uniqueConnections;
    },

    // 获取当前二叉树状态
    async fetchBinaryTreeState() {
      try {
        this.setData({ isLoading: true, apiError: '' });
        const result = await api.binaryTree.getState();
        
        if (result && result.data) {
          // 处理新的API响应格式，包含state.tree, state.nodes, state.edges
          if (result.data.state && result.data.state.tree) {
            // 使用tree字段作为树状态
            const treeState = result.data.state.tree;
            const depth = this.calculateTreeDepth(treeState);
            const layout = this.generateTreeLayout(treeState, depth);
            const connections = this.calculateNodeConnections(treeState, layout, depth);
            
            // 更新树的显示
            this.setData({
              currentTreeState: treeState,
              hasNodes: true,
              treeData: treeState,
              treeDepth: depth,
              treeLayout: layout,
              nodeConnections: connections,
              isLoading: false
            });
            
            // 如果API返回了插入历史，更新本地历史
            if (result.data.insertHistory) {
              this.setData({
                insertHistory: result.data.insertHistory
              });
            }
          } 
          // 兼容原有格式，检查state是否直接就是树对象
          else if (result.data.state && Object.keys(result.data.state).length > 0 && 
                  (result.data.state.value !== undefined || result.data.state.id !== undefined)) {
            const treeState = result.data.state;
            const depth = this.calculateTreeDepth(treeState);
            const layout = this.generateTreeLayout(treeState, depth);
            const connections = this.calculateNodeConnections(treeState, layout, depth);
            
            // 更新树的显示
            this.setData({
              currentTreeState: treeState,
              hasNodes: true,
              treeData: treeState,
              treeDepth: depth,
              treeLayout: layout,
              nodeConnections: connections,
              isLoading: false
            });
            
            // 如果API返回了插入历史，更新本地历史
            if (result.data.insertHistory) {
              this.setData({
                insertHistory: result.data.insertHistory
              });
            }
          } else {
            // 处理空状态或无效状态
            console.log('二叉树状态为空或无效，重置为空树');
            this.setData({
              hasNodes: false,
              currentTreeState: null,
              treeData: null,
              treeDepth: 0,
              treeLayout: [],
              nodeConnections: [],
              isLoading: false,
              insertHistory: []
            });
          }
        } else {
          // 处理空响应
          this.setData({
            hasNodes: false,
            currentTreeState: null,
            treeData: null,
            treeDepth: 0,
            treeLayout: [],
            nodeConnections: [],
            isLoading: false,
            insertHistory: []
          });
        }
      } catch (error) {
        console.error('获取二叉树状态失败:', error);
        this.setData({
          isLoading: false,
          apiError: '获取二叉树状态失败: ' + (error.message || '未知错误'),
          // 在出错时也清空状态
          hasNodes: false,
          currentTreeState: null,
          treeData: null,
          treeDepth: 0,
          treeLayout: [],
          nodeConnections: []
        });
      }
    },

    // 插入节点
    async insertNode() {
      const { nodeValueInput, isApiMode, animationSpeed } = this.data;
      
      if (!nodeValueInput) return;
      
      const nodeValue = parseInt(nodeValueInput);
      
      if (isNaN(nodeValue)) {
        wx.showToast({
          title: '请输入有效数字',
          icon: 'none'
        });
        return;
      }
      
      // 将节点值添加到历史记录
      const insertHistory = [...this.data.insertHistory, nodeValue];
      this.setData({ insertHistory });
      
      if (isApiMode) {
        try {
          this.setData({ isLoading: true, apiError: '' });
          
          const result = await api.binaryTree.insert(
            { value: nodeValue },
            true,  // 启用动画
            animationSpeed
          );
          
          if (result && result.data) {
            if (result.data.animation && result.data.animation.frames) {
              // 处理动画帧
              this.setData({
                animationFrames: result.data.animation.frames,
                currentFrameIndex: 0
              });
              
              // 播放动画
              this.playInsertAnimation();
            }
            
            // 更新树状态
            if (result.data.state) {
              // 处理新的API响应格式，包含state.tree, state.nodes, state.edges
              if (result.data.state.tree) {
                const treeState = result.data.state.tree;
                const depth = this.calculateTreeDepth(treeState);
                const layout = this.generateTreeLayout(treeState, depth);
                const connections = this.calculateNodeConnections(treeState, layout, depth);
                
                this.setData({
                  currentTreeState: treeState,
                  treeData: treeState,
                  treeDepth: depth,
                  treeLayout: layout,
                  nodeConnections: connections,
                  hasNodes: true,
                  nodeValueInput: '',
                  isLoading: false
                });
              } 
              // 兼容原有格式
              else if (Object.keys(result.data.state).length > 0 && 
                    (result.data.state.value !== undefined || result.data.state.id !== undefined)) {
                const treeState = result.data.state;
                const depth = this.calculateTreeDepth(treeState);
                const layout = this.generateTreeLayout(treeState, depth);
                const connections = this.calculateNodeConnections(treeState, layout, depth);
                
                this.setData({
                  currentTreeState: treeState,
                  treeData: treeState,
                  treeDepth: depth,
                  treeLayout: layout,
                  nodeConnections: connections,
                  hasNodes: true,
                  nodeValueInput: '',
                  isLoading: false
                });
              } else {
                this.setData({ 
                  nodeValueInput: '',
                  isLoading: false 
                });
              }
            } else {
              this.setData({ 
                nodeValueInput: '',
                isLoading: false 
              });
            }
          } else {
            this.setData({ isLoading: false });
          }
        } catch (error) {
          this.setData({
            isLoading: false,
            apiError: '插入节点失败: ' + (error.message || '未知错误')
          });
          console.error('插入节点失败:', error);
        }
      } else {
        // 本地插入节点逻辑
        const newTree = this.insertNodeToTree(this.data.currentTreeState || {}, nodeValue);
        const depth = this.calculateTreeDepth(newTree);
        const layout = this.generateTreeLayout(newTree, depth);
        const connections = this.calculateNodeConnections(newTree, layout, depth);
        
        this.setData({
          currentTreeState: newTree,
          treeData: newTree,
          treeDepth: depth,
          treeLayout: layout,
          nodeConnections: connections,
          hasNodes: true,
          nodeValueInput: ''
        });
      }
    },

    // 本地插入节点到二叉树
    insertNodeToTree(tree, value) {
      if (!tree || Object.keys(tree).length === 0) {
        return { value, left: null, right: null };
      }
      
      // 使用层序遍历方式插入节点
      const newTree = JSON.parse(JSON.stringify(tree)); // 创建树的深拷贝
      const queue = [newTree]; // 创建队列，从根节点开始
      
      while (queue.length > 0) {
        const node = queue.shift(); // 取出队列中的第一个节点
        
        // 如果左子节点为空，将新节点插入到左侧
        if (!node.left) {
          node.left = { value, left: null, right: null };
          return newTree;
        } 
        // 如果右子节点为空，将新节点插入到右侧
        else if (!node.right) {
          node.right = { value, left: null, right: null };
          return newTree;
        }
        // 如果左右子节点都不为空，将它们加入队列继续搜索
        else {
          queue.push(node.left);
          queue.push(node.right);
        }
      }
      
      // 理论上不会执行到这里，因为总会找到空位置
      return newTree;
    },

    // 播放插入动画
    playInsertAnimation() {
      const { animationFrames, currentFrameIndex } = this.data;
      
      // 如果没有动画帧或已经播放完毕，退出
      if (!animationFrames || currentFrameIndex >= animationFrames.length) {
        // 动画结束，确保最终状态正确
        if (this.data.currentTreeState) {
          const depth = this.calculateTreeDepth(this.data.currentTreeState);
          const layout = this.generateTreeLayout(this.data.currentTreeState, depth);
          const connections = this.calculateNodeConnections(this.data.currentTreeState, layout, depth);
          
          this.setData({
            treeDepth: depth,
            treeLayout: layout,
            nodeConnections: connections,
            isLoading: false
          });
        }
        return;
      }
      
      // 获取当前帧
      const frame = animationFrames[currentFrameIndex];
      
      // 处理帧内容更新
      if (frame) {
        // 处理新的API响应格式，优先使用tree结构
        let treeState = null;
        
        if (frame.state && frame.state.tree) {
          // 新API格式，使用state.tree
          treeState = frame.state.tree;
        } else if (frame.state && (frame.state.value !== undefined || frame.state.id !== undefined)) {
          // 兼容原有格式，直接使用state对象
          treeState = frame.state;
        }
        
        if (treeState) {
          const frameDepth = this.calculateTreeDepth(treeState);
          const layout = this.generateTreeLayout(treeState, frameDepth);
          const connections = this.calculateNodeConnections(treeState, layout, frameDepth);
          
          this.setData({
            currentTreeState: treeState,
            treeDepth: frameDepth,
            treeLayout: layout,
            nodeConnections: connections,
            highlightedNodes: frame.highlight || []
          });
        }
      }
      
      // 延迟播放下一帧
      const frameDelay = 1000 / (this.data.animationSpeed || 5);
      setTimeout(() => {
        this.setData({ currentFrameIndex: currentFrameIndex + 1 });
        this.playInsertAnimation();
      }, frameDelay);
    },

    // 计算树的深度
    calculateTreeDepth: function(node, currentDepth = 0) {
      if (!node) return currentDepth - 1;
      
      const leftDepth = this.calculateTreeDepth(node.left, currentDepth + 1);
      const rightDepth = this.calculateTreeDepth(node.right, currentDepth + 1);
      
      return Math.max(leftDepth, rightDepth);
    },
    
    // 计算每层的宽度
    calculateLevelWidths(depth) {
      const widths = [];
      for (let i = 0; i <= depth; i++) {
        widths.push(Math.pow(2, i));
      }
      return widths;
    },

    // 生成树的布局数据
    generateTreeLayout: function(root, depth) {
      // 限制最大深度为4层，防止显示溢出
      const maxDepth = Math.min(depth, 4);
      const layout = [];
      
      // 初始化布局数组
      for (let i = 0; i <= maxDepth; i++) {
        layout[i] = new Array(Math.pow(2, i)).fill(null);
      }
      
      // 递归填充布局数组
      const fillLayout = (node, level, position) => {
        if (!node || level > maxDepth) return;
        
        // 在对应位置存储节点信息
        layout[level][position] = {
          value: node.value,
          id: `node-${node.value}-${level}-${position}`,
          hasLeft: !!node.left,
          hasRight: !!node.right
        };
        
        // 递归处理左右子树
        if (node.left) {
          fillLayout(node.left, level + 1, position * 2);
        }
        if (node.right) {
          fillLayout(node.right, level + 1, position * 2 + 1);
        }
      };
      
      // 从根节点开始填充布局
      if (root) {
        fillLayout(root, 0, 0);
      }
      
      return layout;
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
        animationStep: 0,
        isLoading: false,
        animationFrames: [],
        currentFrameIndex: 0,
        apiError: '',
        traversalResult: null
      });
    },
    
    // 处理节点点击事件
    onNodeTap: function(e) {
      if (!this.data.hasNodes) return;
      
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
      if (!this.data.hasNodes) return;
      
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
    
    // 切换遍历模式
    changeTraversalMode: function(e) {
      if (!this.data.hasNodes) {
        wx.showToast({
          title: '请先插入节点',
          icon: 'none'
        });
        return;
      }
      
      const mode = e.currentTarget.dataset.mode;
      
      if (mode === this.data.traversalMode) return;
      
      this.setData({
        traversalMode: mode
      });
      
      // 重置状态
      this.resetTraversal();
      
      // 触发遍历模式变更事件
      this.triggerEvent('traversalModeChange', {
        mode
      });
    },
    
    // 切换API模式
    toggleApiMode: function(e) {
      if (this.data.isLoading) return;
      
      const isApiMode = e.currentTarget.dataset.mode === 'api';
      
      if (isApiMode === this.data.isApiMode) return;
      
      this.setData({
        isApiMode,
        apiError: ''
      });
      
      // 如果切换到API模式，先重置后端状态，再获取当前二叉树状态
      if (isApiMode) {
        this.setData({ isLoading: true });
        api.binaryTree.reset().then(() => {
          console.log('切换至API模式: 成功重置二叉树状态');
          return this.fetchBinaryTreeState();
        }).catch(error => {
          console.error('切换至API模式: 重置二叉树状态失败:', error);
          return this.fetchBinaryTreeState();
        }).finally(() => {
          this.setData({ isLoading: false });
        });
      } else {
        // 切换到本地模式，如果当前有状态，保持状态
        // 如果没有状态，则保持空状态
        if (!this.data.hasNodes) {
          this.setData({
            currentTreeState: null,
            treeData: null,
            treeDepth: 0,
            treeLayout: [],
            hasNodes: false,
            insertHistory: []
          });
        }
      }
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
    
    // 重新开始
    restart: function() {
      this.resetTraversal();
      
      // 重置插入历史
      this.setData({
        insertHistory: []
      });
      
      // 如果是API模式，重置后端状态并重新获取二叉树状态
      if (this.data.isApiMode) {
        this.setData({ isLoading: true });
        api.binaryTree.reset().then(() => {
          console.log('重启: 成功重置二叉树状态');
          // 重置后，获取空的二叉树状态
          return this.fetchBinaryTreeState();
        }).catch(error => {
          console.error('重启: 重置二叉树状态失败:', error);
          // 即使重置失败，也尝试获取当前状态
          return this.fetchBinaryTreeState();
        }).finally(() => {
          this.setData({ isLoading: false });
        });
      } else {
        // 本地模式下，直接清空树状态
        this.setData({
          currentTreeState: null,
          treeData: null,
          treeDepth: 0,
          treeLayout: [],
          hasNodes: false
        });
      }
    },
    
    // 开始遍历
    startTraversal: async function() {
      if (!this.data.hasNodes) {
        wx.showToast({
          title: '请先插入节点',
          icon: 'none'
        });
        return;
      }
      
      if (this.data.isApiMode) {
        await this.startApiTraversal();
      } else {
        this.showCorrectTraversal();
      }
    },
    
    // 开始API遍历
    async startApiTraversal() {
      const { traversalMode, animationSpeed } = this.data;
      
      try {
        this.resetTraversal();
        this.setData({ isLoading: true, apiError: '' });
        
        let result;
        
        switch (traversalMode) {
          case 'preorder':
            result = await api.binaryTree.traversePreOrder({}, true, animationSpeed);
            break;
          case 'inorder':
            result = await api.binaryTree.traverseInOrder({}, true, animationSpeed);
            break;
          case 'postorder':
            result = await api.binaryTree.traversePostOrder({}, true, animationSpeed);
            break;
          case 'levelorder':
            result = await api.binaryTree.traverseLevelOrder({}, true, animationSpeed);
            break;
          default:
            throw new Error('不支持的遍历方式');
        }
        
        if (result && result.data) {
          // 保存遍历结果
          let traversalSequence = null;
          
          // 处理新的API响应格式中的traversal.result字段
          if (result.data.state && result.data.state.traversal && result.data.state.traversal.result) {
            traversalSequence = result.data.state.traversal.result;
          }
          // 兼容原有的traversalSequence字段
          else if (result.data.traversalSequence) {
            traversalSequence = result.data.traversalSequence;
          }
          
          // 处理遍历序列
          if (traversalSequence) {
            // 检查返回的序列是否为数组，如果是字符串则需要转换
            if (typeof traversalSequence === 'string') {
              // 尝试将字符串转换为数组，处理可能的格式如 "1, 2, 3" 或 "[1,2,3]"
              try {
                // 先尝试将其作为JSON解析
                traversalSequence = JSON.parse(traversalSequence);
              } catch (e) {
                // 如果JSON解析失败，则尝试按逗号分隔
                traversalSequence = traversalSequence.split(',').map(item => {
                  // 移除可能的空格和非数字字符
                  const num = parseInt(item.trim().replace(/[^\d]/g, ''));
                  return isNaN(num) ? item.trim() : num;
                });
              }
            }
            
            this.setData({
              traversalResult: traversalSequence
            });
          }
          
          if (result.data.animation && result.data.animation.frames) {
            // 处理动画帧
            this.setData({
              animationFrames: result.data.animation.frames,
              currentFrameIndex: 0
            });
            
            // 播放动画
            this.playTraversalAnimation();
          } else {
            // 没有动画帧，直接显示结果
            if (traversalSequence) {
              this.setData({
                userSequence: traversalSequence,
                isTraversalComplete: true,
                showFeedback: true,
                feedbackMessage: '遍历完成',
                isCorrect: true,
                isLoading: false
              });
            } else {
              this.setData({ isLoading: false });
            }
          }
        } else {
          this.setData({ isLoading: false });
        }
      } catch (error) {
        this.setData({
          isLoading: false,
          apiError: '遍历失败: ' + (error.message || '未知错误')
        });
        console.error('遍历失败:', error);
      }
    },
    
    // 播放遍历动画
    playTraversalAnimation() {
      const { animationFrames, currentFrameIndex } = this.data;
      
      if (!animationFrames || animationFrames.length === 0 || currentFrameIndex >= animationFrames.length) {
        // 动画结束，直接使用当前状态更新UI
        this.setData({ 
          isLoading: false,
          isTraversalComplete: true,
          showFeedback: true,
          feedbackMessage: '遍历完成',
          isCorrect: true
        });
        return;
      }
      
      // 应用当前帧
      const currentFrame = animationFrames[currentFrameIndex];
      
      // 如果有当前帧的遍历序列，更新到traversalResult
      let traversalSequence = null;
      
      // 处理新的API响应格式中的traversal.result字段
      if (currentFrame.state && currentFrame.state.traversal && currentFrame.state.traversal.result) {
        traversalSequence = currentFrame.state.traversal.result;
      }
      // 兼容原有的traversalSequence字段
      else if (currentFrame.traversalSequence) {
        traversalSequence = currentFrame.traversalSequence;
      }
      
      if (traversalSequence) {
        // 处理可能是字符串的遍历序列
        if (typeof traversalSequence === 'string') {
          try {
            traversalSequence = JSON.parse(traversalSequence);
          } catch (e) {
            traversalSequence = traversalSequence.split(',').map(item => {
              const num = parseInt(item.trim().replace(/[^\d]/g, ''));
              return isNaN(num) ? item.trim() : num;
            });
          }
        }
        
        this.setData({
          traversalResult: traversalSequence
        });
      }
      
      // 处理树状态，支持新的API格式
      let treeState = null;
      
      // 如果当前帧包含树状态，优先使用
      if (currentFrame.treeState) {
        treeState = currentFrame.treeState;
      } 
      // 新API格式可能在state.tree中
      else if (currentFrame.state && currentFrame.state.tree) {
        treeState = currentFrame.state.tree;
      }
      
      if (treeState) {
        const depth = this.calculateTreeDepth(treeState);
        const layout = this.generateTreeLayout(treeState, depth);
        const connections = this.calculateNodeConnections(treeState, layout, depth);
        
        this.setData({
          treeData: treeState,
          treeDepth: depth,
          treeLayout: layout,
          nodeConnections: connections
        });
      }
      
      this.setData({
        userSequence: traversalSequence || this.data.userSequence,
        highlightedNodes: currentFrame.highlightedNodes || []
      });
      
      // 延迟播放下一帧
      const frameDelay = 1000 / (this.data.animationSpeed || 5);
      setTimeout(() => {
        this.setData({ currentFrameIndex: currentFrameIndex + 1 });
        this.playTraversalAnimation();
      }, frameDelay);
    },
    
    // 动画展示遍历过程 (本地模式)
    animateTraversal: function() {
      if (!this.data.hasNodes) return;
      
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
    
    // 展示正确的遍历动画 (本地模式)
    showCorrectTraversal: function() {
      if (!this.data.hasNodes) return;
      
      const { traversalMode } = this.data;
      
      // 重置状态
      this.setData({
        userSequence: [],
        selectedNodes: [],
        highlightedNodes: [],
        animationStep: 0
      });
      
      // 根据当前树重新计算正确序列
      let correctSequence = [];
      
      switch (traversalMode) {
        case 'preorder':
          correctSequence = this.calculatePreOrder(this.data.treeData);
          break;
        case 'inorder':
          correctSequence = this.calculateInOrder(this.data.treeData);
          break;
        case 'postorder':
          correctSequence = this.calculatePostOrder(this.data.treeData);
          break;
        case 'levelorder':
          correctSequence = this.calculateLevelOrder(this.data.treeData);
          break;
      }
      
      // 保存遍历结果
      this.setData({ 
        correctSequence,
        traversalResult: correctSequence
      });
      
      // 开始动画
      this.animateTraversal();
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
    
    // 计算前序遍历序列 (本地模式)
    calculatePreOrder: function(node, result = []) {
      if (!node) return result;
      
      result.push(node.value);
      this.calculatePreOrder(node.left, result);
      this.calculatePreOrder(node.right, result);
      
      return result;
    },
    
    // 计算中序遍历序列 (本地模式)
    calculateInOrder: function(node, result = []) {
      if (!node) return result;
      
      this.calculateInOrder(node.left, result);
      result.push(node.value);
      this.calculateInOrder(node.right, result);
      
      return result;
    },
    
    // 计算后序遍历序列 (本地模式)
    calculatePostOrder: function(node, result = []) {
      if (!node) return result;
      
      this.calculatePostOrder(node.left, result);
      this.calculatePostOrder(node.right, result);
      result.push(node.value);
      
      return result;
    },
    
    // 计算层序遍历序列 (本地模式)
    calculateLevelOrder: function(root) {
      if (!root) return [];
      
      const result = [];
      const queue = [root];
      
      while (queue.length > 0) {
        const node = queue.shift();
        result.push(node.value);
        
        if (node.left) queue.push(node.left);
        if (node.right) queue.push(node.right);
      }
      
      return result;
    },
    
    // 检查指定层级和位置上是否有节点
    hasNodeAtPosition: function(levelIndex, posIndex) {
      if (!this.data.treeLayout || !this.data.treeLayout[levelIndex]) {
        return false;
      }
      
      // 在当前层查找是否有节点位于指定位置
      for (let i = 0; i < this.data.treeLayout[levelIndex].length; i++) {
        if (this.data.treeLayout[levelIndex][i].positionIndex === posIndex) {
          return true;
        }
      }
      
      return false;
    },
    
    // 根据位置获取节点
    getNodeAtPosition: function(levelIndex, posIndex) {
      if (!this.data.treeLayout || !this.data.treeLayout[levelIndex]) {
        return null;
      }
      
      // 在当前层查找指定位置的节点
      for (let i = 0; i < this.data.treeLayout[levelIndex].length; i++) {
        if (this.data.treeLayout[levelIndex][i].positionIndex === posIndex) {
          return this.data.treeLayout[levelIndex][i];
        }
      }
      
      return null;
    },

    // 处理节点值输入
    onNodeValueInput(e) {
      this.setData({
        nodeValueInput: e.detail.value
      });
    }
  }
}); 