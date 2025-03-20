const app = getApp()

Page({
  /**
   * 页面的初始数据
   */
  data: {
    categories: [
      { id: 'dataStructure', name: '数据结构' },
      { id: 'algorithm', name: '算法' },
      { id: 'programming', name: '编程基础' },
      { id: 'advanced', name: '高级技巧' }
    ],
    currentCategory: 'all',
    categoryName: '',
    featuredArticles: [],
    recommendedArticles: [],
    articles: [],
    pageNum: 1,
    pageSize: 10,
    isLoading: false,
    noMoreData: false,
    isRefreshing: false,
    showBackToTop: false,
    searchValue: ''
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function () {
    this.loadInitialData();
  },

  onShow: function() {
    // 刷新文章列表，以便更新收藏状态
    this.loadInitialData();
  },

  loadInitialData: function() {
    // 加载轮播图数据
    this.loadFeaturedArticles();
    
    // 加载推荐文章
    this.loadRecommendedArticles();
    
    // 加载文章列表
    this.loadArticles();
  },

  // 加载轮播图文章
  loadFeaturedArticles: function() {
    // 模拟API请求获取特色文章
    const featuredArticles = [
      {
        id: '1',
        title: '动态规划：从入门到精通',
        coverImage: '/assets/images/dp-banner.jpg'
      },
      {
        id: '2',
        title: '数据结构基础知识全解析',
        coverImage: '/assets/images/data-structure-banner.jpg'
      },
      {
        id: '3',
        title: '算法解题技巧与常见模式',
        coverImage: '/assets/images/algorithm-banner.jpg'
      }
    ];

    this.setData({
      featuredArticles: featuredArticles
    });
  },

  // 加载推荐文章
  loadRecommendedArticles: function() {
    // 模拟API请求获取推荐文章
    const recommendedArticles = [
      {
        id: '5',
        title: '如何高效学习算法与数据结构',
        description: '本文介绍了一套科学的学习方法，帮助你系统地掌握算法与数据结构知识，提高学习效率。',
        coverImage: '/assets/images/learning-method.jpg',
        views: 3562,
        tags: ['学习方法', '入门指南']
      },
      {
        id: '6',
        title: '10个必须掌握的编程算法',
        description: '这10个算法是每个程序员的必备技能，本文详细介绍了它们的原理、应用场景和实现方式。',
        coverImage: '/assets/images/top-algorithms.jpg',
        views: 4891,
        tags: ['算法', '编程技巧']
      }
    ];

    this.setData({
      recommendedArticles: recommendedArticles
    });
  },

  // 加载文章列表
  loadArticles: function() {
    if (this.data.isLoading || this.data.noMoreData) return;

    this.setData({
      isLoading: true
    });

    // 模拟API请求获取文章列表
    setTimeout(() => {
      // 模拟根据分类筛选
      const category = this.data.currentCategory;
      let newArticles = this.getArticlesByCategory(category, this.data.pageNum, this.data.pageSize);
      
      // 模拟搜索过滤
      if (this.data.searchValue) {
        newArticles = newArticles.filter(article => 
          article.title.toLowerCase().includes(this.data.searchValue.toLowerCase()) ||
          article.description.toLowerCase().includes(this.data.searchValue.toLowerCase())
        );
      }

      // 判断是否还有更多数据
      const noMoreData = newArticles.length < this.data.pageSize;

      if (this.data.pageNum === 1) {
        // 第一页，直接设置
        this.setData({
          articles: newArticles,
          isLoading: false,
          noMoreData: noMoreData,
          isRefreshing: false
        });
      } else {
        // 加载更多，追加数据
        this.setData({
          articles: [...this.data.articles, ...newArticles],
          isLoading: false,
          noMoreData: noMoreData
        });
      }
    }, 1000);
  },

  // 根据分类获取文章
  getArticlesByCategory: function(category, pageNum, pageSize) {
    // 模拟文章数据
    const allArticles = [
      {
        id: '10',
        title: '链表基础与常用操作',
        description: '详细讲解链表的基本概念、类型及其在实际编程中的常用操作，包括增删改查及经典应用。',
        coverImage: '/assets/images/linked-list.jpg',
        date: '2023-11-20',
        views: 1845,
        category: 'dataStructure',
        tags: ['数据结构', '链表']
      },
      {
        id: '11',
        title: '栈与队列在算法中的应用',
        description: '深入探讨栈和队列这两种基础数据结构在算法设计中的重要作用，以及如何选择合适的数据结构。',
        coverImage: '/assets/images/stack-queue.jpg',
        date: '2023-11-18',
        views: 1532,
        category: 'dataStructure',
        tags: ['数据结构', '栈', '队列']
      },
      {
        id: '12',
        title: '二叉树遍历算法全解析',
        description: '全面介绍二叉树的前序、中序、后序和层序遍历算法，包括递归与非递归实现方式及其应用。',
        coverImage: '/assets/images/binary-tree.jpg',
        date: '2023-11-15',
        views: 2134,
        category: 'dataStructure',
        tags: ['数据结构', '二叉树', '遍历算法']
      },
      {
        id: '13',
        title: '图的表示方法与遍历算法',
        description: '详解图这种复杂数据结构的表示方法，以及深度优先搜索和广度优先搜索算法的实现与应用。',
        coverImage: '/assets/images/graph.jpg',
        date: '2023-11-12',
        views: 1876,
        category: 'dataStructure',
        tags: ['数据结构', '图', 'DFS', 'BFS']
      },
      {
        id: '14',
        title: '排序算法的时间复杂度分析',
        description: '对比分析常见排序算法的时间复杂度、空间复杂度及其适用场景，帮助你选择最适合的排序方法。',
        coverImage: '/assets/images/sorting.jpg',
        date: '2023-11-10',
        views: 2567,
        category: 'algorithm',
        tags: ['算法', '排序', '复杂度分析']
      },
      {
        id: '15',
        title: '贪心算法解题技巧',
        description: '通过实际案例讲解贪心算法的核心思想和应用技巧，以及如何判断一个问题是否适合使用贪心策略。',
        coverImage: '/assets/images/greedy.jpg',
        date: '2023-11-08',
        views: 1923,
        category: 'algorithm',
        tags: ['算法', '贪心算法']
      },
      {
        id: '16',
        title: '深入理解递归与回溯',
        description: '探讨递归与回溯算法的本质，通过经典问题的解析，掌握这类算法的设计方法和优化技巧。',
        coverImage: '/assets/images/recursion.jpg',
        date: '2023-11-05',
        views: 2145,
        category: 'algorithm',
        tags: ['算法', '递归', '回溯']
      },
      {
        id: '17',
        title: '动态规划解题框架',
        description: '系统讲解动态规划的思考过程和解题框架，通过多个经典例题，掌握状态设计和状态转移方程的推导。',
        coverImage: '/assets/images/dp.jpg',
        date: '2023-11-03',
        views: 3245,
        category: 'algorithm',
        tags: ['算法', '动态规划']
      },
      {
        id: '18',
        title: 'JavaScript数组方法详解',
        description: '全面介绍JavaScript数组的常用方法及其在实际编程中的应用，提高代码效率和可读性。',
        coverImage: '/assets/images/js-array.jpg',
        date: '2023-11-01',
        views: 2786,
        category: 'programming',
        tags: ['JavaScript', '编程基础']
      },
      {
        id: '19',
        title: '面向对象编程核心概念',
        description: '深入讲解面向对象编程的核心概念：封装、继承和多态，以及如何在实际项目中应用这些原则。',
        coverImage: '/assets/images/oop.jpg',
        date: '2023-10-28',
        views: 2354,
        category: 'programming',
        tags: ['编程基础', '面向对象']
      }
    ];

    // 根据分类筛选
    let filteredArticles = allArticles;
    if (category !== 'all') {
      filteredArticles = allArticles.filter(article => article.category === category);
      
      // 设置分类名称
      const categoryObj = this.data.categories.find(item => item.id === category);
      this.setData({
        categoryName: categoryObj ? categoryObj.name : ''
      });
    }

    // 模拟分页
    const start = (pageNum - 1) * pageSize;
    const end = start + pageSize;
    
    return filteredArticles.slice(start, end);
  },

  // 切换分类
  changeCategory: function(e) {
    const category = e.currentTarget.dataset.category;
    if (category !== this.data.currentCategory) {
      this.setData({
        currentCategory: category,
        pageNum: 1,
        noMoreData: false,
        articles: []
      });
      this.loadArticles();
    }
  },

  // 加载更多文章
  loadMoreArticles: function() {
    if (!this.data.isLoading && !this.data.noMoreData) {
      this.setData({
        pageNum: this.data.pageNum + 1
      });
      this.loadArticles();
    }
  },

  // 刷新数据
  onRefresh: function() {
    this.setData({
      isRefreshing: true,
      pageNum: 1,
      noMoreData: false
    });
    this.loadInitialData();
  },

  // 搜索输入
  onSearchInput: function(e) {
    this.setData({
      searchValue: e.detail.value
    });
  },

  // 执行搜索
  onSearch: function(e) {
    const value = e.detail.value || this.data.searchValue;
    this.setData({
      searchValue: value,
      pageNum: 1,
      noMoreData: false,
      articles: []
    });
    this.loadArticles();
  },

  // 查看更多
  viewMore: function(e) {
    const type = e.currentTarget.dataset.type;
    // 这里可以跳转到更多页面，根据type区分推荐还是最新
    wx.showToast({
      title: '功能开发中',
      icon: 'none'
    });
  },

  // 跳转到文章详情
  navigateToArticle: function(e) {
    const id = e.currentTarget.dataset.id;
    wx.navigateTo({
      url: `/src/pages/learning/article?id=${id}`
    });
  },

  // 滚动到顶部
  scrollToTop: function() {
    wx.pageScrollTo({
      scrollTop: 0,
      duration: 300
    });
  },

  // 监听页面滚动
  onPageScroll: function(e) {
    const scrollTop = e.scrollTop;
    // 当滚动超过一定距离显示返回顶部按钮
    this.setData({
      showBackToTop: scrollTop > 300
    });
  },

  // 页面相关事件处理函数--监听用户下拉动作
  onPullDownRefresh: function() {
    this.onRefresh();
    wx.stopPullDownRefresh();
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    return {
      title: '数据结构与算法学习',
      path: '/pages/learning/learning'
    }
  }
}) 