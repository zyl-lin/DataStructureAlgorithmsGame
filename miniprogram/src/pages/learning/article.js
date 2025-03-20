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
    replyToCommentId: null,
    showEmoji: false,
    emojis: ['😊', '👍', '🎉', '❤️', '😄', '🤔', '👏', '🙏', '😎', '🔥']
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
      let article;
      
      // 为二叉树特殊文章添加丰富内容
      if (id === '9') {
        article = {
          id: '9',
          title: '二叉树基础知识与遍历方法',
          coverImage: '/assets/icons/learning.png',
          author: '数据结构讲师',
          date: '2023-12-05',
          views: 2354,
          content: `
            <h2 style="font-size:18px;font-weight:bold;margin:15px 0;">二叉树基础知识</h2>
            <p style="line-height:1.6;margin-bottom:10px;">二叉树是一种树形数据结构，其中每个节点最多有两个子节点，通常称为"左子节点"和"右子节点"。二叉树是许多高级数据结构的基础，如二叉搜索树、堆和红黑树等。</p>
            
            <h3 style="font-size:16px;font-weight:bold;margin:15px 0 10px;">二叉树的类型</h3>
            <ul style="padding-left:20px;margin-bottom:15px;">
              <li style="margin-bottom:5px;">完全二叉树：除了最后一层，其他层的节点都是满的，最后一层的节点都靠左排列。</li>
              <li style="margin-bottom:5px;">满二叉树：每一层的节点都是满的，即有2^h-1个节点（h为树高）。</li>
              <li style="margin-bottom:5px;">平衡二叉树：任意节点的左右子树高度差不超过1。</li>
              <li style="margin-bottom:5px;">二叉搜索树：左子树上所有节点的值都小于根节点，右子树上所有节点的值都大于根节点。</li>
            </ul>
            
            <div style="background-color:#f5f5f5;padding:15px;border-radius:8px;margin:15px 0;">
              <p style="font-weight:bold;color:#333;margin-bottom:10px;">二叉树的基本性质</p>
              <p style="margin-bottom:5px;">• 第i层最多有2^(i-1)个节点</p>
              <p style="margin-bottom:5px;">• 高度为h的二叉树最多有2^h-1个节点</p>
              <p style="margin-bottom:5px;">• 对于任何一棵二叉树，叶子节点的数量比度为2的节点多一个</p>
            </div>
            
            <h3 style="font-size:16px;font-weight:bold;margin:15px 0 10px;">二叉树的表示</h3>
            <p style="line-height:1.6;margin-bottom:10px;">在程序中，二叉树通常通过节点对象和引用（指针）来表示：</p>
            
            <pre style="background-color:#f8f8f8;padding:10px;border-radius:5px;overflow-x:auto;margin:10px 0;font-family:monospace;">
class TreeNode {
    int val;
    TreeNode left;
    TreeNode right;
    
    TreeNode(int x) { 
        val = x; 
        left = null;
        right = null;
    }
}</pre>
            
            <div style="text-align:center;margin:20px 0;">
              <svg width="220" height="160" viewBox="0 0 220 160" style="max-width:100%;">
                <!-- 节点 -->
                <circle cx="110" cy="30" r="20" fill="#4CAF50" stroke="#fff" stroke-width="2"></circle>
                <circle cx="60" cy="80" r="20" fill="#4CAF50" stroke="#fff" stroke-width="2"></circle>
                <circle cx="160" cy="80" r="20" fill="#4CAF50" stroke="#fff" stroke-width="2"></circle>
                <circle cx="35" cy="130" r="20" fill="#4CAF50" stroke="#fff" stroke-width="2"></circle>
                <circle cx="85" cy="130" r="20" fill="#4CAF50" stroke="#fff" stroke-width="2"></circle>
                <circle cx="135" cy="130" r="20" fill="#4CAF50" stroke="#fff" stroke-width="2"></circle>
                <circle cx="185" cy="130" r="20" fill="#4CAF50" stroke="#fff" stroke-width="2"></circle>
                
                <!-- 连线 -->
                <line x1="100" y1="40" x2="70" y2="70" stroke="#333" stroke-width="2"></line>
                <line x1="120" y1="40" x2="150" y2="70" stroke="#333" stroke-width="2"></line>
                <line x1="50" y1="90" x2="40" y2="120" stroke="#333" stroke-width="2"></line>
                <line x1="70" y1="90" x2="80" y2="120" stroke="#333" stroke-width="2"></line>
                <line x1="150" y1="90" x2="140" y2="120" stroke="#333" stroke-width="2"></line>
                <line x1="170" y1="90" x2="180" y2="120" stroke="#333" stroke-width="2"></line>
                
                <!-- 节点文本 -->
                <text x="110" y="35" fill="#fff" text-anchor="middle" dominant-baseline="middle" font-weight="bold">10</text>
                <text x="60" y="85" fill="#fff" text-anchor="middle" dominant-baseline="middle" font-weight="bold">5</text>
                <text x="160" y="85" fill="#fff" text-anchor="middle" dominant-baseline="middle" font-weight="bold">15</text>
                <text x="35" y="135" fill="#fff" text-anchor="middle" dominant-baseline="middle" font-weight="bold">3</text>
                <text x="85" y="135" fill="#fff" text-anchor="middle" dominant-baseline="middle" font-weight="bold">7</text>
                <text x="135" y="135" fill="#fff" text-anchor="middle" dominant-baseline="middle" font-weight="bold">12</text>
                <text x="185" y="135" fill="#fff" text-anchor="middle" dominant-baseline="middle" font-weight="bold">18</text>
              </svg>
              <p style="font-size:12px;color:#666;margin-top:5px;">一个简单的二叉搜索树示例</p>
            </div>
            
            <h2 style="font-size:18px;font-weight:bold;margin:20px 0 15px;">二叉树的遍历方式</h2>
            <p style="line-height:1.6;margin-bottom:10px;">二叉树遍历是指按某种次序访问树中的所有节点。主要有四种遍历方式：</p>
            
            <h3 style="font-size:16px;font-weight:bold;margin:15px 0 10px;">1. 前序遍历 (Preorder)</h3>
            <p style="line-height:1.6;margin-bottom:5px;">访问顺序：<strong>根节点 → 左子树 → 右子树</strong></p>
            <p style="line-height:1.6;margin-bottom:10px;">上图树的前序遍历结果：<span style="color:#4CAF50;font-weight:bold;">10, 5, 3, 7, 15, 12, 18</span></p>
            
            <h3 style="font-size:16px;font-weight:bold;margin:15px 0 10px;">2. 中序遍历 (Inorder)</h3>
            <p style="line-height:1.6;margin-bottom:5px;">访问顺序：<strong>左子树 → 根节点 → 右子树</strong></p>
            <p style="line-height:1.6;margin-bottom:10px;">上图树的中序遍历结果：<span style="color:#4CAF50;font-weight:bold;">3, 5, 7, 10, 12, 15, 18</span></p>
            <p style="line-height:1.6;margin-bottom:10px;"><em>注意：二叉搜索树的中序遍历结果是有序的。</em></p>
            
            <h3 style="font-size:16px;font-weight:bold;margin:15px 0 10px;">3. 后序遍历 (Postorder)</h3>
            <p style="line-height:1.6;margin-bottom:5px;">访问顺序：<strong>左子树 → 右子树 → 根节点</strong></p>
            <p style="line-height:1.6;margin-bottom:10px;">上图树的后序遍历结果：<span style="color:#4CAF50;font-weight:bold;">3, 7, 5, 12, 18, 15, 10</span></p>
            
            <h3 style="font-size:16px;font-weight:bold;margin:15px 0 10px;">4. 层序遍历 (Level Order)</h3>
            <p style="line-height:1.6;margin-bottom:5px;">访问顺序：<strong>按层从上到下，每层从左到右</strong></p>
            <p style="line-height:1.6;margin-bottom:10px;">上图树的层序遍历结果：<span style="color:#4CAF50;font-weight:bold;">10, 5, 15, 3, 7, 12, 18</span></p>
            
            <div style="background-color:#f5f5f5;padding:15px;border-radius:8px;margin:20px 0;">
              <p style="font-weight:bold;color:#333;margin-bottom:10px;">应用场景</p>
              <ul style="padding-left:20px;margin-bottom:0;">
                <li style="margin-bottom:5px;">前序遍历：用于创建树的拷贝，序列化树结构</li>
                <li style="margin-bottom:5px;">中序遍历：在二叉搜索树中获取有序序列</li>
                <li style="margin-bottom:5px;">后序遍历：用于删除树，计算目录大小</li>
                <li style="margin-bottom:0px;">层序遍历：按层次处理树节点，计算树的宽度</li>
              </ul>
            </div>
            
            <h2 style="font-size:18px;font-weight:bold;margin:20px 0 15px;">二叉树的实现与应用</h2>
            <p style="line-height:1.6;margin-bottom:10px;">二叉树在许多领域都有广泛应用：</p>
            <ul style="padding-left:20px;margin-bottom:15px;">
              <li style="margin-bottom:5px;">编译器中的表达式树</li>
              <li style="margin-bottom:5px;">优先队列（堆实现）</li>
              <li style="margin-bottom:5px;">数据库索引（B树、B+树）</li>
              <li style="margin-bottom:5px;">文件系统的目录结构</li>
              <li style="margin-bottom:5px;">网络路由算法</li>
            </ul>
            
            <div style="margin:25px 0;text-align:center;">
              <a href="/src/pages/binarytree/binarytree" style="display:inline-block;background-color:#4CAF50;color:white;padding:10px 20px;border-radius:5px;text-decoration:none;font-weight:bold;">
                前往练习二叉树遍历
              </a>
            </div>
          `,
          tags: ['数据结构', '二叉树', '树', '算法']
        };
        
        // 添加相关推荐文章
        this.setRelatedArticles([
          {
            id: '11',
            title: '栈与队列在算法中的应用',
            coverImage: '/assets/icons/learning.png',
            views: 1532
          },
          {
            id: '15',
            title: '图论算法：从基础到实战',
            coverImage: '/assets/icons/learning.png',
            views: 1854
          }
        ]);
      } else {
        // 其他文章处理逻辑保持不变
        // ... existing code ...
      }

      // 格式化文章内容
      const formattedContent = this.formatContent(article.content);

      // 获取相关文章
      const relatedArticles = this.getRelatedArticles(id);

      // 检查是否已收藏
      const isFavorite = this.checkIsFavorite(id);

      this.setData({
        article: article,
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
      path: `../learning/article?id=${article.id}`,
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
        const newIsLiked = !comment.isLiked;
        const newLikeCount = comment.isLiked ? (comment.likeCount - 1) : (comment.likeCount + 1);
        
        // 显示点赞动画效果
        if (newIsLiked) {
          wx.vibrateShort({
            type: 'light'
          });
        }
        
        return {
          ...comment,
          isLiked: newIsLiked,
          likeCount: newLikeCount
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
      icon: 'none',
      duration: 1000
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
    
    wx.showToast({
      title: '已取消回复',
      icon: 'none',
      duration: 800
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
  },

  /**
   * 打开/关闭表情面板
   */
  toggleEmojiPanel: function() {
    this.setData({
      showEmoji: !this.data.showEmoji
    });
  },
  
  /**
   * 选择表情
   */
  selectEmoji: function(e) {
    const emoji = e.currentTarget.dataset.emoji;
    this.setData({
      commentContent: this.data.commentContent + emoji,
      showEmoji: false
    });
  }
}) 