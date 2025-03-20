const config = require('../../../config.js');
const app = getApp();

Page({
  data: {
    message: '', // 用于存储后端返回的消息
    status: '',   // 用于存储后端返回的状态
    userInfo: {},
    gameCategories: [
      {
        id: 1,
        name: '基础数据结构',
        games: [
          {
            id: 1,
            name: '链表操作',
            icon: '/assets/images/games/linked-list.png',
            description: '学习链表的基本操作'
          },
          {
            id: 2,
            name: '栈与队列',
            icon: '/assets/images/games/stack-queue.png',
            description: '掌握栈和队列的应用'
          },
          {
            id: 5,
            name: '二叉树遍历',
            icon: '/assets/images/games/binary-tree.png',
            description: '学习二叉树的四种遍历方式'
          }
        ]
      },
      {
        id: 2,
        name: '算法游戏',
        games: [
          {
            id: 3,
            name: '迷宫寻路',
            icon: '/assets/images/games/maze.png',
            description: '使用不同算法解决迷宫问题'
          },
          {
            id: 4,
            name: '排序可视化',
            icon: '/assets/images/games/sorting.png',
            description: '可视化各种排序算法'
          },
          {
            id: 6,
            name: '图遍历算法',
            icon: '/assets/images/games/graph.png',
            description: '学习图的DFS和BFS遍历'
          }
        ]
      }
    ]
  },

  onLoad: function() {
    // 调用GET API
    this.fetchHelloMessage();

    // 调用POST API
    this.sendGreeting();

    this.getUserInfo();
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
  },

  getUserInfo() {
    const userInfo = wx.getStorageSync('userInfo');
    if (userInfo) {
      this.setData({ userInfo });
    }
  },

  onGameTap(e) {
    const { game } = e.currentTarget.dataset;
    wx.navigateTo({
      url: `/src/pages/game/game?id=${game.id}&name=${game.name}`
    });
  }
}); 