Component({
  properties: {
    title: {
      type: String,
      value: '单链表游戏'
    },
    description: {
      type: String,
      value: '练习单链表的基本操作'
    },
    level: {
      type: Number,
      value: 1
    }
  },

  data: {
    nodes: [],
    isCompleted: false
  },

  lifetimes: {
    attached: function() {
      // 组件加载时初始化
      this.initLevel();
    }
  },

  methods: {
    // 初始化关卡
    initLevel: function() {
      const level = this.properties.level;
      let nodes = [];
      
      // 根据关卡生成不同的链表
      switch(level) {
        case 1:
          nodes = [
            { value: 5 },
            { value: 8 },
            { value: 12 },
            { value: 15 },
            { value: 20 }
          ];
          break;
        case 2:
          nodes = [
            { value: 3 },
            { value: 7 },
            { value: 10 },
            { value: 15 },
            { value: 22 },
            { value: 30 }
          ];
          break;
        case 3:
          nodes = [
            { value: 6 },
            { value: 12 },
            { value: 18 },
            { value: 24 }
          ];
          break;
        default:
          nodes = [
            { value: 5 },
            { value: 10 },
            { value: 15 }
          ];
      }
      
      this.setData({ nodes, isCompleted: false });
    },

    // 开始游戏
    onStart: function() {
      // 触发开始游戏事件
      this.triggerEvent('gamestart', { level: this.properties.level });
      
      // 这里可以添加游戏开始的动画或其他效果
      wx.showToast({
        title: `第${this.properties.level}关开始`,
        icon: 'none',
        duration: 1500
      });
    },

    // 下一关
    onNextLevel: function() {
      // 触发下一关事件
      this.triggerEvent('nextlevel', { 
        currentLevel: this.properties.level,
        nextLevel: this.properties.level + 1
      });
    },

    // 标记当前关卡完成
    markCompleted: function() {
      this.setData({ isCompleted: true });
      
      wx.showToast({
        title: '关卡完成！',
        icon: 'success',
        duration: 2000
      });
      
      // 触发关卡完成事件
      this.triggerEvent('levelcompleted', { level: this.properties.level });
    }
  }
}); 