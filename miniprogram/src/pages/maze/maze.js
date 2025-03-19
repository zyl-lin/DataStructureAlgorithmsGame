Page({
  data: {
    currentLevel: 1,
    lastUsedAlgorithm: '',
    lastPathLength: 0
  },

  onLoad: function(options) {
    // 如果有传入关卡参数，则使用该关卡
    if (options && options.level) {
      this.setData({
        currentLevel: parseInt(options.level) || 1
      });
    }
  },

  onReady: function() {
    // 获取迷宫游戏组件实例
    this.mazeGame = this.selectComponent('#mazeGame');
  },

  // 处理寻路完成事件
  onPathCompleted: function(e) {
    const { algorithm, pathLength } = e.detail;
    
    let algorithmName = '';
    switch (algorithm) {
      case 'dfs':
        algorithmName = '深度优先搜索';
        break;
      case 'bfs':
        algorithmName = '广度优先搜索';
        break;
      case 'astar':
        algorithmName = 'A*算法';
        break;
      default:
        algorithmName = algorithm;
    }
    
    this.setData({
      lastUsedAlgorithm: algorithmName,
      lastPathLength: pathLength
    });
    
    // 可以在这里记录用户完成情况，例如上传到云服务
    console.log(`用户使用${algorithmName}完成了第${this.data.currentLevel}关，路径长度：${pathLength}`);
  },

  // 处理重置迷宫事件
  onMazeReset: function() {
    console.log('迷宫已重置');
    // 重置统计数据
    this.setData({
      lastUsedAlgorithm: '',
      lastPathLength: 0
    });
  },

  // 处理下一关事件
  onNextLevel: function(e) {
    const { currentLevel, nextLevel } = e.detail;
    
    // 更新当前关卡
    this.setData({
      currentLevel: nextLevel,
      lastUsedAlgorithm: '',
      lastPathLength: 0
    });
    
    console.log(`从第${currentLevel}关进入第${nextLevel}关`);
    
    // 可以在这里记录用户进度
    // 例如：保存到本地存储或上传到云服务
    try {
      wx.setStorageSync('maze_current_level', nextLevel);
    } catch (e) {
      console.error('保存关卡进度失败:', e);
    }
  },

  // 分享功能
  onShareAppMessage: function() {
    return {
      title: `我正在挑战数据结构与算法迷宫游戏的第${this.data.currentLevel}关！`,
      path: `/src/pages/maze/maze?level=${this.data.currentLevel}`,
      imageUrl: '/assets/images/maze_share.png' // 分享图片（如果有）
    };
  }
}); 