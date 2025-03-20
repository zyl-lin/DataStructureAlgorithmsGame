const app = getApp()

Page({
  /**
   * 页面的初始数据
   */
  data: {
    totalCompleted: 0,
    totalInProgress: 0,
    totalLocked: 0,
    completionRate: 0,
    achievements: [
      {
        id: '1',
        icon: '算',
        iconPath: '/assets/icons/achievement/beginner.png',
        title: '算法新手',
        description: '完成第一个算法挑战',
        status: '已完成',
        progress: 1,
        maxProgress: 1
      },
      {
        id: '2',
        icon: '迷',
        iconPath: '/assets/icons/achievement/maze.png',
        title: '迷宫探险家',
        description: '完成所有迷宫类型的挑战',
        status: '进度 3/5',
        progress: 3,
        maxProgress: 5
      },
      {
        id: '3',
        icon: '链',
        iconPath: '/assets/icons/achievement/linkedlist.png',
        title: '链表操作专家',
        description: '完成所有链表操作挑战',
        status: '已完成',
        progress: 5,
        maxProgress: 5
      },
      {
        id: '4',
        icon: '排',
        iconPath: '/assets/icons/achievement/sort.png',
        title: '排序大师',
        description: '完成所有排序算法挑战',
        status: '进度 2/5',
        progress: 2,
        maxProgress: 5
      },
      {
        id: '5',
        icon: '树',
        iconPath: '/assets/icons/achievement/binarytree.png',
        title: '二叉树专家',
        description: '完成所有二叉树操作挑战',
        status: '进度 1/5',
        progress: 1,
        maxProgress: 5
      },
      {
        id: '6',
        icon: '图',
        iconPath: '/assets/icons/achievement/graph.png',
        title: '图论操作大师',
        description: '完成所有图论操作挑战',
        status: '未开始',
        progress: 0,
        maxProgress: 5
      },
      {
        id: '7',
        icon: '队',
        iconPath: '/assets/icons/achievement/queue.png',
        title: '队列操作专家',
        description: '完成所有队列操作挑战',
        status: '未开始',
        progress: 0,
        maxProgress: 3
      },
      {
        id: '8',
        icon: '栈',
        iconPath: '/assets/icons/achievement/stack.png',
        title: '栈操作专家',
        description: '完成所有栈操作挑战',
        status: '未开始',
        progress: 0,
        maxProgress: 3
      },
      {
        id: '9',
        icon: '搜',
        iconPath: '/assets/icons/achievement/search.png',
        title: '查找算法专家',
        description: '完成所有查找算法挑战',
        status: '未开始',
        progress: 0,
        maxProgress: 3
      },
      {
        id: '10',
        icon: '动',
        iconPath: '/assets/icons/achievement/dp.png',
        title: '动态规划达人',
        description: '完成所有动态规划算法挑战',
        status: '未开始',
        progress: 0,
        maxProgress: 3
      },
      {
        id: '11',
        icon: '贪',
        iconPath: '/assets/icons/achievement/greedy.png',
        title: '贪心算法达人',
        description: '完成所有贪心算法挑战',
        status: '未开始',
        progress: 0,
        maxProgress: 3
      }
    ]
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    this.loadAchievements();
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function() {
    // 每次显示页面时重新获取最新的成就数据
    this.loadAchievements();
  },

  /**
   * 加载成就数据并计算统计信息
   */
  loadAchievements: function() {
    // 在实际应用中，这里应该从服务器或本地存储中获取成就数据
    const storedAchievements = wx.getStorageSync('achievementData');
    let achievementsData = this.data.achievements;
    
    if (storedAchievements) {
      // 合并存储的数据和默认数据
      achievementsData = this.mergeAchievementData(storedAchievements);
    }
    
    // 更新状态文本
    achievementsData = achievementsData.map(achievement => {
      if (achievement.progress < achievement.maxProgress && achievement.progress > 0) {
        achievement.status = `进度 ${achievement.progress}/${achievement.maxProgress}`;
      } else if (achievement.progress >= achievement.maxProgress) {
        achievement.status = '已完成';
      } else {
        achievement.status = '未开始';
      }
      return achievement;
    });
    
    // 计算统计数据
    const totalCompleted = achievementsData.filter(a => a.progress >= a.maxProgress).length;
    const totalInProgress = achievementsData.filter(a => a.progress > 0 && a.progress < a.maxProgress).length;
    const totalLocked = achievementsData.filter(a => a.progress === 0).length;
    const totalAchievements = achievementsData.length;
    const completionRate = totalAchievements > 0 
      ? Math.round((totalCompleted / totalAchievements) * 100) 
      : 0;
    
    this.setData({
      achievements: achievementsData,
      totalCompleted,
      totalInProgress,
      totalLocked,
      completionRate
    });
  },

  /**
   * 合并存储的成就数据与默认数据
   */
  mergeAchievementData: function(storedData) {
    // 如果存储数据格式与当前不一致，进行合并
    return this.data.achievements.map(achievement => {
      const stored = storedData.find(item => 
        item.id.toString() === achievement.id.toString() || 
        item.title === achievement.title
      );
      
      if (stored) {
        return {
          ...achievement,
          progress: stored.completedSteps || stored.progress || 0,
          maxProgress: stored.totalSteps || stored.maxProgress || 1
        };
      }
      return achievement;
    });
  },

  /**
   * 查看成就详情
   */
  viewAchievementDetail: function(e) {
    const id = e.currentTarget.dataset.id;
    const achievement = this.data.achievements.find(item => item.id === id);
    
    if (!achievement) return;
    
    wx.showModal({
      title: achievement.title,
      content: `描述：${achievement.description}\n进度：${achievement.progress}/${achievement.maxProgress}\n状态：${achievement.status}`,
      showCancel: false
    });
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    return {
      title: '我在数据结构与算法游戏中获得了' + this.data.totalCompleted + '项成就！',
      path: '/src/pages/achievements/achievements'
    }
  }
}) 