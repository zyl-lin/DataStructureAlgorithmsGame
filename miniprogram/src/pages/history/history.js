// 学习记录页面逻辑
const app = getApp();

Page({
  data: {
    currentTab: 0, // 当前选中的标签页，0: 最近学习，1: 学习统计
    historyList: [], // 学习历史记录
    statistics: {
      totalGames: 0,
      completedGames: 0,
      totalDays: 0,
      totalHours: 0,
      avgDailyMinutes: 0,
      weeklyData: [],
      categories: []
    }
  },

  onLoad: function() {
    // 加载页面时读取历史记录和统计数据
    this.loadHistoryData();
    this.loadStatisticsData();
  },

  onShow: function() {
    // 每次显示页面时刷新数据
    this.loadHistoryData();
    this.loadStatisticsData();
  },

  // 切换标签页
  switchTab: function(e) {
    const tabIndex = parseInt(e.currentTarget.dataset.tab);
    this.setData({
      currentTab: tabIndex
    });
  },

  // 加载历史记录数据
  loadHistoryData: function() {
    // 从本地存储或服务器获取历史记录
    // 这里使用模拟数据
    const historyList = wx.getStorageSync('historyList') || this.getMockHistoryData();
    this.setData({
      historyList: historyList
    });
  },

  // 加载统计数据
  loadStatisticsData: function() {
    // 从本地存储或服务器获取统计数据
    // 这里使用模拟数据
    const statistics = wx.getStorageSync('studyStatistics') || this.getMockStatisticsData();
    this.setData({
      statistics: statistics
    });
  },

  // 继续游戏
  continueGame: function(e) {
    const gameId = e.currentTarget.dataset.id;
    const game = this.data.historyList.find(item => item.id === gameId);
    
    if (game) {
      // 根据游戏类型跳转到对应页面
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

  // 获取模拟历史记录数据
  getMockHistoryData: function() {
    return [
      {
        id: '1',
        title: '链表操作',
        description: '学习单链表的基本操作：插入、删除和查找',
        icon: '/assets/icons/linkedlist.png',
        time: '今天 14:30',
        duration: 15,
        completed: true,
        url: '/pages/linkedlist/linkedlist'
      },
      {
        id: '2',
        title: '二叉树遍历',
        description: '学习二叉树的四种遍历方式',
        icon: '/assets/icons/binarytree.png',
        time: '今天 10:05',
        duration: 25,
        completed: false,
        url: '/pages/binarytree/binarytree'
      },
      {
        id: '3',
        title: '排序算法',
        description: '学习并比较不同排序算法的执行效率',
        icon: '/assets/icons/sorting.png',
        time: '昨天 16:45',
        duration: 20,
        completed: true,
        url: '/pages/sorting/sorting'
      },
      {
        id: '4',
        title: '图遍历算法',
        description: '学习深度优先搜索和广度优先搜索',
        icon: '/assets/icons/graph.png',
        time: '昨天 09:20',
        duration: 30,
        completed: true,
        url: '/pages/graph/graph'
      },
      {
        id: '5',
        title: '动态规划',
        description: '通过斐波那契数列学习动态规划',
        icon: '/assets/icons/dp.png',
        time: '3天前 11:30',
        duration: 35,
        completed: false,
        url: '/pages/dp/dp'
      }
    ];
  },

  // 获取模拟统计数据
  getMockStatisticsData: function() {
    // 获取过去7天的日期标签
    const weekdays = ['周日', '周一', '周二', '周三', '周四', '周五', '周六'];
    const today = new Date();
    const weeklyData = [];
    
    for (let i = 6; i >= 0; i--) {
      const date = new Date(today);
      date.setDate(today.getDate() - i);
      const dayOfWeek = weekdays[date.getDay()];
      
      // 模拟每天的学习时间(分钟)
      const minutes = Math.floor(Math.random() * 60) + 15;
      const percent = Math.min(100, Math.round(minutes / 90 * 100));
      
      weeklyData.push({
        day: dayOfWeek,
        value: minutes,
        percent: percent
      });
    }

    return {
      totalGames: 8,
      completedGames: 5,
      totalDays: 12,
      totalHours: 8.5,
      avgDailyMinutes: 42,
      weeklyData: weeklyData,
      categories: [
        { name: '数据结构', percent: 65 },
        { name: '基本算法', percent: 45 },
        { name: '高级算法', percent: 25 },
        { name: '动态规划', percent: 30 }
      ]
    };
  }
}); 