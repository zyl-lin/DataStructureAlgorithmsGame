// src/pages/greedy/greedy.js
Page({
  /**
   * 页面的初始数据
   */
  data: {
    currentLevel: 1, // 默认关卡为零钱兑换
    levelCompleted: {
      1: false,
      2: false,
      3: false
    }
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    // 如果有参数，检查是否指定了关卡
    if (options && options.level) {
      const level = parseInt(options.level);
      if (level >= 1 && level <= 3) {
        this.setData({
          currentLevel: level
        });
      }
    }
  },

  /**
   * 选择关卡
   */
  selectLevel: function (e) {
    const level = parseInt(e.currentTarget.dataset.level);
    if (level >= 1 && level <= 3) {
      this.setData({
        currentLevel: level
      });
    }
  },

  /**
   * 标记关卡完成
   */
  markLevelCompleted: function (level) {
    const levelCompleted = this.data.levelCompleted;
    levelCompleted[level] = true;
    this.setData({ levelCompleted });
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    return {
      title: '贪心算法可视化 - 数据结构与算法游戏',
      path: '/src/pages/greedy/greedy',
      imageUrl: '/assets/images/share-greedy.png' // 确保图片存在
    }
  }
}) 