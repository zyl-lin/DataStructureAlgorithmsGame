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
      },
      {
        id: 3,
        name: '高级算法',
        games: [
          {
            id: 7,
            name: '搜索算法',
            icon: '/assets/images/games/search.png',
            description: '学习线性和二分搜索算法'
          },
          {
            id: 8,
            name: '动态规划',
            icon: '/assets/images/games/dp.png',
            description: '通过经典问题学习动态规划'
          },
          {
            id: 9,
            name: '贪心算法',
            icon: '/assets/images/games/greedy.png',
            description: '掌握贪心策略解决问题'
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
    let url = '';
    
    // 根据游戏id跳转到对应页面
    switch(game.id) {
      case 1: // 链表
        url = `/src/pages/linkedlist/linkedlist`;
        break;
      case 2: // 栈与队列 (假设默认展示队列)
        url = `/src/pages/queue/queue`;
        break;
      case 3: // 迷宫
        url = `/src/pages/maze/maze`;
        break;
      case 4: // 排序
        url = `/src/pages/sorting/sorting`;
        break;
      case 5: // 二叉树
        url = `/src/pages/binarytree/binarytree`;
        break;
      case 6: // 图
        url = `/src/pages/graph/graph`;
        break;
      case 7: // 搜索
        url = `/src/pages/search/search`;
        break;
      case 8: // 动态规划
        url = `/src/pages/dp/dp`;
        break;
      case 9: // 贪心
        url = `/src/pages/greedy/greedy`;
        break;
      default:
        url = `/src/pages/game/game?id=${game.id}&name=${game.name}`;
    }
    
    wx.navigateTo({ url });
  }
}); 