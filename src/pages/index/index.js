const config = require('../../../config.js');

Page({
  data: {
    message: '', // 用于存储后端返回的消息
    status: ''   // 用于存储后端返回的状态
  },

  onLoad: function() {
    // 调用GET API
    this.fetchHelloMessage();

    // 调用POST API
    this.sendGreeting();
  },

  // 获取Hello消息
  fetchHelloMessage: function() {
    wx.request({
      url: `http://${config.serverIp}:${config.serverPort}/api/hello`, // 使用配置的IP和端口
      method: 'GET',
      success: (res) => {
        if (res.statusCode === 200) {
          this.setData({
            message: res.data.message,
            status: res.data.status
          });
        } else {
          console.error('Failed to fetch hello message:', res.data);
        }
      },
      fail: (err) => {
        console.error('Request failed:', err);
      }
    });
  },

  // 发送问候请求
  sendGreeting: function() {
    wx.request({
      url: `http://${config.serverIp}:${config.serverPort}/api/greet`, // 使用配置的IP和端口
      method: 'POST',
      data: {
        name: 'WeChat Mini Program'
      },
      success: (res) => {
        if (res.statusCode === 200) {
          console.log('Greeting response:', res.data);
        } else {
          console.error('Failed to send greeting:', res.data);
        }
      },
      fail: (err) => {
        console.error('Request failed:', err);
      }
    });
  }
}); 