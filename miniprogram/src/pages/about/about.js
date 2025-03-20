// 关于我们页面逻辑
const app = getApp();

Page({
  data: {
    // 页面数据
  },

  onLoad: function() {
    // 页面加载时执行
  },

  // 显示反馈对话框
  showFeedback: function() {
    wx.showModal({
      title: '意见反馈',
      content: '感谢您的使用！如有问题或建议，请发送邮件至：support@example.com',
      showCancel: false,
      confirmText: '知道了'
    });
  }
}); 