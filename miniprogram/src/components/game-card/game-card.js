Component({
  properties: {
    game: {
      type: Object,
      value: {}
    }
  },
  data: {
    progress: 0,
    iconPath: "/assets/default-game-icon.png"
  },
  lifetimes: {
    attached() {
      // 获取进度数据
      this.getGameProgress();
      
      // 检查图标是否存在，不存在则使用默认图标
      this.checkIconExists();
    }
  },
  methods: {
    onTap() {
      // 将游戏数据传递给父组件
      this.triggerEvent('tap', { game: this.data.game });
    },
    getGameProgress() {
      // 从本地存储获取游戏进度
      const progressData = wx.getStorageSync('gameProgress') || {};
      const gameProgress = progressData[this.data.game.id] || { currentLevel: 1, completedLevels: [] };
      
      // 计算完成进度百分比，假设每个游戏有10个关卡
      const totalLevels = 10;
      const completedCount = gameProgress.completedLevels.length;
      const progress = Math.round((completedCount / totalLevels) * 100);
      
      this.setData({
        progress: progress
      });
    },
    checkIconExists() {
      // 检查图标是否存在，如果不存在则使用默认图标
      if (this.data.game.icon) {
        wx.getFileSystemManager().access({
          path: this.data.game.icon,
          fail: () => {
            // 图标文件不存在，使用默认图标
            this.setData({
              'game.icon': this.data.iconPath
            });
          }
        });
      } else {
        // 未提供图标，使用默认图标
        this.setData({
          'game.icon': this.data.iconPath
        });
      }
    }
  }
}) 