const app = getApp()

Page({
  data: {
    userInfo: {},
    userLevel: 1,
    userTitle: '算法新手',
    isLoggedIn: false,
    completedGames: 0,
    studyDays: 0,
    achievements: 0,
    studyProgress: [
      { id: 1, name: '数据结构', percent: 60 },
      { id: 2, name: '基本算法', percent: 45 },
      { id: 3, name: '高级算法', percent: 20 },
      { id: 4, name: '动态规划', percent: 30 }
    ]
  },

  onLoad: function() {
    this.getUserInfo()
    this.getStudyStatistics()
  },

  onShow: function() {
    // 每次显示页面时刷新数据
    this.getStudyStatistics()
  },

  getUserInfo: function() {
    const userInfo = wx.getStorageSync('userInfo')
    if (userInfo) {
      this.setData({
        userInfo: userInfo,
        isLoggedIn: true
      })
      this.calculateLevel(userInfo)
    }
  },

  getUserProfile: function() {
    wx.getUserProfile({
      desc: '用于完善用户资料',
      success: (res) => {
        const userInfo = res.userInfo
        wx.setStorageSync('userInfo', userInfo)
        this.setData({
          userInfo: userInfo,
          isLoggedIn: true
        })
        this.calculateLevel(userInfo)
        // 可以在这里调用API保存用户信息到服务器
      },
      fail: (err) => {
        console.log('获取用户信息失败', err)
        wx.showToast({
          title: '获取用户信息失败',
          icon: 'none'
        })
      }
    })
  },

  calculateLevel: function(userInfo) {
    // 这里可以根据用户的学习情况计算等级
    // 简单示例：根据完成游戏数量计算
    const completedGames = this.data.completedGames
    let level = 1
    let title = '算法新手'

    if (completedGames >= 50) {
      level = 5
      title = '算法大师'
    } else if (completedGames >= 30) {
      level = 4
      title = '算法专家'
    } else if (completedGames >= 20) {
      level = 3
      title = '算法高手'
    } else if (completedGames >= 10) {
      level = 2
      title = '算法学徒'
    }

    this.setData({
      userLevel: level,
      userTitle: title
    })
  },

  getStudyStatistics: function() {
    // 实际应用中应该从服务器或本地存储获取
    // 这里使用模拟数据
    const studyStats = wx.getStorageSync('studyStats') || {
      completedGames: 8,
      studyDays: 12,
      achievements: 5
    }

    this.setData({
      completedGames: studyStats.completedGames,
      studyDays: studyStats.studyDays,
      achievements: studyStats.achievements
    })

    // 刷新用户等级
    this.calculateLevel(this.data.userInfo)
  },

  editProfile: function() {
    wx.showActionSheet({
      itemList: ['修改昵称', '修改头像'],
      success: (res) => {
        if (res.tapIndex === 0) {
          this.changeNickname()
        } else if (res.tapIndex === 1) {
          this.changeAvatar()
        }
      }
    })
  },

  changeNickname: function() {
    wx.showModal({
      title: '修改昵称',
      editable: true,
      placeholderText: '请输入新的昵称',
      success: (res) => {
        if (res.confirm && res.content) {
          const userInfo = this.data.userInfo
          userInfo.nickName = res.content
          this.setData({
            userInfo: userInfo
          })
          wx.setStorageSync('userInfo', userInfo)
          
          wx.showToast({
            title: '昵称修改成功',
            icon: 'success'
          })
        }
      }
    })
  },

  changeAvatar: function() {
    wx.chooseImage({
      count: 1,
      sizeType: ['compressed'],
      sourceType: ['album', 'camera'],
      success: (res) => {
        const tempFilePath = res.tempFilePaths[0]
        const userInfo = this.data.userInfo
        userInfo.avatarUrl = tempFilePath
        this.setData({
          userInfo: userInfo
        })
        wx.setStorageSync('userInfo', userInfo)
        
        wx.showToast({
          title: '头像修改成功',
          icon: 'success'
        })
      }
    })
  },

  navigateToHistory: function() {
    wx.navigateTo({
      url: '/pages/history/history'
    })
  },

  navigateToFavorites: function() {
    wx.navigateTo({
      url: '/pages/favorites/favorites'
    })
  },

  navigateToAchievement: function() {
    wx.navigateTo({
      url: '/pages/achievement/achievement'
    })
  },

  navigateToSettings: function() {
    wx.showToast({
      title: '功能开发中',
      icon: 'none'
    })
  },

  navigateToHelp: function() {
    wx.showToast({
      title: '功能开发中',
      icon: 'none'
    })
  },

  showAbout: function() {
    wx.navigateTo({
      url: '/pages/about/about'
    })
  },

  showFeedback: function() {
    wx.showModal({
      title: '意见反馈',
      content: '感谢您的使用！如有问题或建议，请发送邮件至：feedback@example.com',
      showCancel: false,
      confirmText: '知道了'
    })
  },

  clearStorage: function() {
    wx.showModal({
      title: '清除缓存',
      content: '确定要清除本地缓存数据吗？这不会删除您的账号信息。',
      success: (res) => {
        if (res.confirm) {
          // 保留用户信息，清除其他数据
          const userInfo = wx.getStorageSync('userInfo')
          wx.clearStorageSync()
          if (userInfo) {
            wx.setStorageSync('userInfo', userInfo)
          }
          
          wx.showToast({
            title: '缓存已清除',
            icon: 'success'
          })
          
          // 重新加载页面数据
          this.onLoad()
        }
      }
    })
  },

  navigateToIndex: function() {
    wx.switchTab({
      url: '/pages/index/index'
    })
  },

  navigateToLearning: function() {
    wx.switchTab({
      url: '/pages/learning/learning'
    })
  }
}) 