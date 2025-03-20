Page({
  /**
   * 页面的初始数据
   */
  data: {
    currentLevel: 1,
    levelNames: ['斐波那契数列', '背包问题', '最长公共子序列']
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    // 如果有传入关卡参数，则设置当前关卡
    if (options.level) {
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
    this.setData({
      currentLevel: level
    });
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    return {
      title: '动态规划算法可视化',
      path: '/src/pages/dp/dp?level=' + this.data.currentLevel
    };
  }
}) 