// 成就页面逻辑
Page({
  /**
   * 页面的初始数据
   */
  data: {
    totalCompleted: 0,
    totalAchievements: 0,
    completionRate: 0,
    achievements: [
      {
        id: 1,
        title: '算法新手',
        description: '完成第一个算法挑战',
        iconPath: '/assets/icons/achievement/beginner.png',
        completed: true,
        completedSteps: 1,
        totalSteps: 1
      },
      {
        id: 2,
        title: '迷宫探险家',
        description: '完成所有迷宫寻路挑战',
        iconPath: '/assets/icons/achievement/maze.png',
        completed: false,
        completedSteps: 3,
        totalSteps: 5
      },
      {
        id: 3,
        title: '链表大师',
        description: '完成所有链表操作挑战',
        iconPath: '/assets/icons/achievement/linkedlist.png',
        completed: false,
        completedSteps: 2,
        totalSteps: 5
      },
      {
        id: 4,
        title: '栈操作专家',
        description: '完成所有栈操作挑战',
        iconPath: '/assets/icons/achievement/stack.png',
        completed: false,
        completedSteps: 1,
        totalSteps: 5
      },
      {
        id: 5,
        title: '队列操作专家',
        description: '完成所有队列操作挑战',
        iconPath: '/assets/icons/achievement/queue.png',
        completed: false,
        completedSteps: 0,
        totalSteps: 5
      },
      {
        id: 6,
        title: '二叉树专家',
        description: '完成所有二叉树遍历挑战',
        iconPath: '/assets/icons/achievement/binarytree.png',
        completed: false,
        completedSteps: 0,
        totalSteps: 4
      },
      {
        id: 7,
        title: '图遍历专家',
        description: '完成所有图遍历挑战',
        iconPath: '/assets/icons/achievement/graph.png',
        completed: false,
        completedSteps: 0,
        totalSteps: 3
      },
      {
        id: 8,
        title: '查找算法专家',
        description: '完成所有查找算法挑战',
        iconPath: '/assets/icons/achievement/search.png',
        completed: false,
        completedSteps: 0,
        totalSteps: 3
      },
      {
        id: 9,
        title: '排序算法专家',
        description: '完成所有排序算法挑战',
        iconPath: '/assets/icons/achievement/sort.png',
        completed: false,
        completedSteps: 0,
        totalSteps: 4
      },
      {
        id: 10,
        title: '动态规划达人',
        description: '完成所有动态规划算法挑战',
        iconPath: '/assets/icons/achievement/dp.png',
        completed: false,
        completedSteps: 0,
        totalSteps: 3
      },
      {
        id: 11,
        title: '贪心算法达人',
        description: '完成所有贪心算法挑战',
        iconPath: '/assets/icons/achievement/greedy.png',
        completed: false,
        completedSteps: 0,
        totalSteps: 3
      }
    ]
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function () {
    this.calculateAchievementStats();
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function () {
    // 每次显示页面时重新获取最新的成就数据
    this.loadAchievementData();
  },

  /**
   * 计算成就统计数据
   */
  calculateAchievementStats: function () {
    const achievements = this.data.achievements;
    const totalAchievements = achievements.length;
    let totalCompleted = 0;

    achievements.forEach(item => {
      if (item.completed) {
        totalCompleted++;
      }
    });

    const completionRate = totalAchievements > 0 
      ? Math.round((totalCompleted / totalAchievements) * 100) 
      : 0;

    this.setData({
      totalCompleted,
      totalAchievements,
      completionRate
    });
  },

  /**
   * 从本地存储加载成就数据
   */
  loadAchievementData: function () {
    // 从本地存储获取成就数据
    // 实际应用中，这些数据可能需要从服务器获取
    const achievementData = wx.getStorageSync('achievementData');
    if (achievementData) {
      // 更新页面数据
      this.setData({
        achievements: achievementData
      });
      this.calculateAchievementStats();
    }
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    return {
      title: '我在数据结构与算法游戏中获得了' + this.data.totalCompleted + '项成就！',
      path: '/src/pages/achievement/achievement'
    }
  }
}); 