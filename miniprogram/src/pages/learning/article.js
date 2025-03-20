const app = getApp()

Page({
  /**
   * 页面的初始数据
   */
  data: {
    articleId: null,
    article: null,
    formattedContent: '',
    relatedArticles: [],
    isFavorite: false,
    loading: true,
    comments: [],
    commentContent: '',
    replyTo: '',
    replyToCommentId: null
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    if (options.id) {
      this.getArticleDetail(options.id);
      this.loadComments(); // 加载评论
      this.checkFavoriteStatus(options.id);
    } else {
      wx.showToast({
        title: '文章不存在',
        icon: 'error'
      });
      setTimeout(() => {
        wx.navigateBack();
      }, 1500);
    }
  },
  
  /**
   * 获取文章数据
   */
  getArticleDetail: function (id) {
    // 模拟API请求获取文章详情
    wx.showLoading({
      title: '加载中',
    });

    // 这里应替换为实际的API请求
    setTimeout(() => {
      const articleData = {
        id: id,
        title: '算法讲解：动态规划入门指南',
        coverImage: '/assets/icons/learning.png',
        date: '2023-11-15',
        views: 1342,
        tags: ['动态规划', '算法', '入门'],
        content: '<div class="h1">什么是动态规划？</div><p>动态规划（Dynamic Programming，简称DP）是一种将复杂问题分解成更小子问题求解的算法思想。它的核心是找到问题的最优子结构和重叠子问题，通过存储中间结果来避免重复计算。</p><div class="h2">动态规划的基本步骤</div><p>1. 定义状态（确定dp数组的含义）</p><p>2. 找出状态转移方程</p><p>3. 确定初始条件和边界情况</p><p>4. 按顺序计算出所有状态</p><div class="h2">经典例题：斐波那契数列</div><p>斐波那契数列是理解动态规划的绝佳例子。数列定义为：F(0) = 0, F(1) = 1, F(n) = F(n-1) + F(n-2)（n ≥ 2）。</p><p>递归解法（非DP）：</p><div class="code">function fib(n) {\n  if (n <= 1) return n;\n  return fib(n-1) + fib(n-2);\n}</div><p>动态规划解法：</p><div class="code">function fib(n) {\n  if (n <= 1) return n;\n  let dp = [0, 1];\n  for (let i = 2; i <= n; i++) {\n    dp[i] = dp[i-1] + dp[i-2];\n  }\n  return dp[n];\n}</div><div class="h2">空间优化</div><p>很多动态规划问题可以进行空间优化，例如斐波那契数列可以进一步优化为：</p><div class="code">function fib(n) {\n  if (n <= 1) return n;\n  let a = 0, b = 1;\n  for (let i = 2; i <= n; i++) {\n    let c = a + b;\n    a = b;\n    b = c;\n  }\n  return b;\n}</div>'
      };

      // 格式化文章内容
      const formattedContent = this.formatContent(articleData.content);

      // 获取相关文章
      const relatedArticles = this.getRelatedArticles(id);

      // 检查是否已收藏
      const isFavorite = this.checkIsFavorite(id);

      this.setData({
        article: articleData,
        formattedContent: formattedContent,
        relatedArticles: relatedArticles,
        isFavorite: isFavorite,
        loading: false
      });

      wx.hideLoading();
    }, 1000);
  },
  
  /**
   * 格式化文章内容为富文本
   */
  formatContent: function (content) {
    // 简单替换一些HTML标签为微信小程序支持的格式
    // 实际项目中可能需要更复杂的处理
    let formatted = content;
    
    // 替换代码块
    formatted = formatted.replace(/<div class="code">([\s\S]*?)<\/div>/g, '<pre style="background-color: #f5f7f9; padding: 20rpx; border-radius: 8rpx; margin: 20rpx 0; font-family: monospace; font-size: 26rpx; line-height: 1.6; overflow-x: auto; white-space: pre-wrap; word-break: break-all;">$1</pre>');
    
    return formatted;
  },
  
  /**
   * 获取相关文章
   */
  getRelatedArticles: function (currentId) {
    // 模拟获取相关文章
    // 实际应该通过API获取
    return [
      {
        id: '2',
        title: '贪心算法与动态规划的区别',
        coverImage: '/assets/icons/learning.png',
        views: 986,
        tags: ['算法']
      },
      {
        id: '3',
        title: '如何解决背包问题 - 动态规划经典应用',
        coverImage: '/assets/icons/learning.png',
        views: 1208,
        tags: ['动态规划']
      },
      {
        id: '4',
        title: '从零开始学习编程算法',
        coverImage: '/assets/icons/learning.png',
        views: 2453,
        tags: ['编程基础']
      }
    ];
  },
  
  /**
   * 检查是否已收藏
   */
  checkIsFavorite: function (id) {
    // 检查是否已收藏
    // 实际应该从本地存储或用户数据中获取
    const favorites = wx.getStorageSync('favorites') || [];
    return favorites.includes(id);
  },
  
  /**
   * 检查收藏状态
   */
  checkFavoriteStatus: function(articleId) {
    const favoriteArticles = wx.getStorageSync('favoriteArticles') || [];
    const isFavorite = favoriteArticles.includes(articleId);
    this.setData({
      isFavorite
    });
  },

  /**
   * 收藏或取消收藏文章
   */
  toggleFavorite: function() {
    const articleId = this.data.article.id;
    let favoriteArticles = wx.getStorageSync('favoriteArticles') || [];
    const isFavorite = this.data.isFavorite;
    
    if (isFavorite) {
      // 取消收藏
      favoriteArticles = favoriteArticles.filter(id => id !== articleId);
      wx.showToast({
        title: '已取消收藏',
        icon: 'none'
      });
    } else {
      // 添加收藏
      favoriteArticles.push(articleId);
      wx.showToast({
        title: '收藏成功',
        icon: 'success'
      });
    }
    
    this.setData({
      isFavorite: !isFavorite
    });
    
    // 保存到本地存储
    wx.setStorageSync('favoriteArticles', favoriteArticles);
  },
  
  /**
   * 导航到其他文章
   */
  navigateToArticle: function (e) {
    const id = e.currentTarget.dataset.id;
    wx.navigateTo({
      url: '../learning/article?id=' + id
    });
  },
  
  /**
   * 返回上一页
   */
  navigateBack: function () {
    wx.navigateBack();
  },

  /**
   * 分享文章
   */
  shareArticle: function() {
    wx.showShareMenu({
      withShareTicket: true,
      menus: ['shareAppMessage', 'shareTimeline']
    });
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    const article = this.data.article;
    return {
      title: article.title,
      path: `/src/pages/learning/article?id=${article.id}`,
      imageUrl: article.coverImage
    };
  },

  /**
   * 加载评论列表
   */
  loadComments: function() {
    const that = this;
    wx.showLoading({
      title: '加载评论中',
    });
    
    // 模拟评论数据，实际项目中应通过API获取
    setTimeout(() => {
      const mockComments = [
        {
          id: '1',
          userName: '数据结构爱好者',
          avatar: '/assets/icons/default-avatar.png',
          content: '这篇文章讲解得很清晰，对理解二叉树很有帮助！',
          createTime: '2023-05-15 14:30',
          likeCount: 12,
          isLiked: false,
          replies: [
            {
              id: '1-1',
              userName: '学习达人',
              content: '是的，我也觉得很实用，特别是平衡树的部分',
              createTime: '2023-05-15 15:20'
            }
          ]
        },
        {
          id: '2',
          userName: '算法学习者',
          avatar: '/assets/icons/default-avatar.png',
          content: '希望能多出一些算法分析的文章，期待更新！',
          createTime: '2023-05-14 09:45',
          likeCount: 8,
          isLiked: true,
          replies: []
        }
      ];
      
      that.setData({
        comments: mockComments
      });
      
      wx.hideLoading();
    }, 1000);
  },
  
  /**
   * 点赞评论
   */
  likeComment: function(e) {
    const commentId = e.currentTarget.dataset.id;
    const comments = this.data.comments.map(comment => {
      if (comment.id === commentId) {
        return {
          ...comment,
          isLiked: !comment.isLiked,
          likeCount: comment.isLiked ? (comment.likeCount - 1) : (comment.likeCount + 1)
        };
      }
      return comment;
    });
    
    this.setData({
      comments
    });
    
    // 实际项目中应发送请求至服务器更新点赞状态
    wx.showToast({
      title: comments.find(c => c.id === commentId).isLiked ? '点赞成功' : '已取消点赞',
      icon: 'none'
    });
  },
  
  /**
   * 回复评论
   */
  replyComment: function(e) {
    const commentId = e.currentTarget.dataset.id;
    const userName = e.currentTarget.dataset.name;
    
    this.setData({
      replyTo: userName,
      replyToCommentId: commentId
    });
  },
  
  /**
   * 监听评论输入
   */
  onCommentInput: function(e) {
    this.setData({
      commentContent: e.detail.value
    });
  },
  
  /**
   * 提交评论
   */
  submitComment: function() {
    if (!this.data.commentContent.trim()) {
      wx.showToast({
        title: '评论内容不能为空',
        icon: 'none'
      });
      return;
    }
    
    wx.showLoading({
      title: '发布中'
    });
    
    const that = this;
    // 模拟评论提交，实际项目中应通过API提交
    setTimeout(() => {
      const now = new Date();
      const formatDate = `${now.getFullYear()}-${String(now.getMonth() + 1).padStart(2, '0')}-${String(now.getDate()).padStart(2, '0')} ${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}`;
      
      if (that.data.replyToCommentId) {
        // 回复评论
        const comments = that.data.comments.map(comment => {
          if (comment.id === that.data.replyToCommentId) {
            const replies = comment.replies || [];
            replies.push({
              id: `${comment.id}-${replies.length + 1}`,
              userName: '我',
              content: that.data.commentContent,
              createTime: formatDate
            });
            return {
              ...comment,
              replies
            };
          }
          return comment;
        });
        
        that.setData({
          comments,
          commentContent: '',
          replyTo: '',
          replyToCommentId: null
        });
      } else {
        // 新评论
        const newComment = {
          id: String(that.data.comments.length + 1),
          userName: '我',
          avatar: '/assets/icons/default-avatar.png',
          content: that.data.commentContent,
          createTime: formatDate,
          likeCount: 0,
          isLiked: false,
          replies: []
        };
        
        that.setData({
          comments: [newComment, ...that.data.comments],
          commentContent: ''
        });
      }
      
      wx.hideLoading();
      wx.showToast({
        title: '发布成功',
        icon: 'success'
      });
    }, 1000);
  },
  
  /**
   * 取消回复
   */
  cancelReply: function() {
    this.setData({
      replyTo: '',
      replyToCommentId: null
    });
  },

  /**
   * 返回页面顶部
   */
  backToTop: function() {
    wx.pageScrollTo({
      scrollTop: 0,
      duration: 300
    });
    
    wx.showToast({
      title: '已返回顶部',
      icon: 'none',
      duration: 1500
    });
  }
}) 