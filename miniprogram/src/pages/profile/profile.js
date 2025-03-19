const app = getApp()

Page({
  data: {
    userInfo: {
      nickName: '',
      avatarUrl: '',
      level: 1
    },
    stats: {
      completedGames: 6,
      achievements: 3,
      streak: 5
    }
  },

  onLoad: function() {
    this.getUserInfo()
  },

  onShow: function() {
    // 每次页面显示时更新数据
    this.getUserInfo()
    this.getStats()
  },

  getUserInfo: function() {
    // 从本地存储获取用户信息
    const userInfo = wx.getStorageSync('userInfo')
    if (userInfo) {
      this.setData({
        userInfo: userInfo
      })
    }
  },

  getStats: function() {
    // 获取用户统计数据
    // 这里使用模拟数据，实际应从服务器获取
    // wx.request({
    //   url: 'http://yourserver/api/stats',
    //   success: (res) => {
    //     this.setData({
    //       stats: res.data
    //     })
    //   }
    // })
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
    wx.showToast({
      title: '功能开发中',
      icon: 'none'
    })
  },

  navigateToFavorites: function() {
    wx.showToast({
      title: '功能开发中',
      icon: 'none'
    })
  },

  navigateToAchievements: function() {
    wx.showToast({
      title: '功能开发中',
      icon: 'none'
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

  navigateToAbout: function() {
    wx.showToast({
      title: '功能开发中',
      icon: 'none'
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