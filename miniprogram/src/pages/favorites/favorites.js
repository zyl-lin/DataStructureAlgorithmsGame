// 收藏页面逻辑
const app = getApp();

Page({
  data: {
    favoriteList: [] // 收藏列表
  },

  onLoad: function() {
    this.loadFavorites();
  },

  onShow: function() {
    // 每次显示页面时刷新收藏列表
    this.loadFavorites();
  },

  // 加载收藏列表
  loadFavorites: function() {
    // 从本地存储或服务器获取收藏列表
    // 这里使用模拟数据
    const favoriteList = wx.getStorageSync('favoriteList') || this.getMockFavoriteData();
    this.setData({
      favoriteList: favoriteList
    });
  },

  // 移除收藏
  removeFavorite: function(e) {
    const gameId = e.currentTarget.dataset.id;
    
    wx.showModal({
      title: '取消收藏',
      content: '确定要取消收藏该游戏吗？',
      success: (res) => {
        if (res.confirm) {
          // 从收藏列表中移除
          const newList = this.data.favoriteList.filter(item => item.id !== gameId);
          
          this.setData({
            favoriteList: newList
          });
          
          // 保存到本地存储
          wx.setStorageSync('favoriteList', newList);
          
          wx.showToast({
            title: '已取消收藏',
            icon: 'success'
          });
        }
      }
    });
  },

  // 开始游戏
  startGame: function(e) {
    const gameId = e.currentTarget.dataset.id;
    const game = this.data.favoriteList.find(item => item.id === gameId);
    
    if (game) {
      wx.navigateTo({
        url: game.url
      });
    }
  },

  // 导航到首页
  navigateToIndex: function() {
    wx.switchTab({
      url: '/pages/index/index'
    });
  },

  // 获取模拟收藏数据
  getMockFavoriteData: function() {
    return [
      {
        id: '1',
        title: '动态规划',
        description: '动态规划是一种解决复杂问题的方法，它将问题分解为子问题，并存储子问题的解以避免重复计算。本游戏将通过斐波那契数列、背包问题和最长公共子序列三个经典案例，帮助你理解动态规划的核心思想。',
        icon: '/assets/icons/dp.png',
        tags: ['动态规划', '算法', '高级'],
        url: '/pages/dp/dp'
      },
      {
        id: '2',
        title: '二叉树遍历',
        description: '二叉树是一种非常重要的数据结构，其遍历方式包括前序、中序、后序和层序遍历。本游戏将可视化展示不同遍历方式的执行过程，帮助你深入理解二叉树遍历算法。',
        icon: '/assets/icons/binarytree.png',
        tags: ['数据结构', '二叉树', '遍历'],
        url: '/pages/binarytree/binarytree'
      },
      {
        id: '3',
        title: '图遍历算法',
        description: '图是一种复杂的非线性数据结构，由节点和边组成。本游戏将通过可视化方式展示深度优先搜索(DFS)和广度优先搜索(BFS)两种图遍历算法的执行过程，帮助你理解这两种算法的区别和应用场景。',
        icon: '/assets/icons/graph.png',
        tags: ['数据结构', '图', 'DFS', 'BFS'],
        url: '/pages/graph/graph'
      }
    ];
  }
}); 