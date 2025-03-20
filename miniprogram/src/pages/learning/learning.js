const app = getApp()

Page({
  /**
   * 页面的初始数据
   */
  data: {
    currentCategory: 'all', // 当前选中的分类
    searchKeyword: '', // 搜索关键词
    articles: [
      {
        id: 1,
        title: '链表基础与应用',
        description: '介绍链表的基本概念、数据结构和常见操作，包括单链表、双链表和循环链表的特点与实现。',
        coverImage: '/assets/images/articles/linkedlist.png',
        category: '数据结构',
        date: '2024-03-20',
        content: '链表是一种常见的基础数据结构，由节点组成，每个节点包含数据和指向下一个节点的指针...'
      },
      {
        id: 2,
        title: '栈与队列详解',
        description: '深入讲解栈和队列的工作原理、特性及其在算法中的应用，包含实际编程案例和性能分析。',
        coverImage: '/assets/images/articles/stack-queue.png',
        category: '数据结构',
        date: '2024-03-18',
        content: '栈是一种遵循后进先出(LIFO)原则的有序集合，队列则遵循先进先出(FIFO)原则...'
      },
      {
        id: 3,
        title: '图算法及其应用',
        description: '全面解析图数据结构及常用算法，包括深度优先搜索、广度优先搜索、最短路径等在实际问题中的应用。',
        coverImage: '/assets/images/articles/graph.png',
        category: '数据结构',
        date: '2024-03-15',
        content: '图是一种由顶点和连接顶点的边组成的数据结构，可用于表示各种关系和网络...'
      },
      {
        id: 4,
        title: '二叉树遍历技巧',
        description: '详细介绍二叉树的前序、中序、后序及层序遍历算法，以及递归与非递归实现方式的对比。',
        coverImage: '/assets/images/articles/binary-tree.png',
        category: '数据结构',
        date: '2024-03-12',
        content: '二叉树是一种树形数据结构，其中每个节点最多有两个子节点，通常被称为左子节点和右子节点...'
      },
      {
        id: 5,
        title: '排序算法大比拼',
        description: '对比分析主流排序算法的工作原理、时间复杂度、空间复杂度及适用场景，助你在实际开发中选择最佳方案。',
        coverImage: '/assets/images/articles/sorting.png',
        category: '算法',
        date: '2024-03-08',
        content: '排序算法是计算机科学中最基本也是最重要的算法之一，用于将一组特定的数据按照既定规则进行排序...'
      },
      {
        id: 6,
        title: '搜索算法实战指南',
        description: '从简单到复杂，全面讲解各类搜索算法，包括线性搜索、二分搜索、哈希搜索等技术及优化策略。',
        coverImage: '/assets/images/articles/search.png',
        category: '算法',
        date: '2024-03-05',
        content: '搜索算法是在数据集合中查找特定元素的方法，不同的搜索算法适用于不同的数据组织方式...'
      },
      {
        id: 7,
        title: '动态规划解题思路',
        description: '系统讲解动态规划的核心思想和解题步骤，通过经典问题剖析如何构建状态转移方程和优化解法。',
        coverImage: '/assets/images/articles/dp.png',
        category: '算法',
        date: '2024-03-01',
        content: '动态规划是解决具有重叠子问题和最优子结构性质问题的算法策略，通过将问题分解为子问题并存储子问题的解...'
      },
      {
        id: 8,
        title: '贪心算法设计技巧',
        description: '从原理到实践，详解贪心算法的思想、应用场景及局限性，帮助你掌握解决实际问题的能力。',
        coverImage: '/assets/images/articles/greedy.png',
        category: '算法',
        date: '2024-02-28',
        content: '贪心算法是一种在每一步选择中都采取当前状态下最好或最优的选择，从而希望导致结果是最好或最优的算法...'
      }
    ],
    filteredArticles: [] // 过滤后的文章列表
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function () {
    // 初始化显示所有文章
    this.setData({
      filteredArticles: this.data.articles
    });
  },

  /**
   * 搜索输入变化处理
   */
  onSearchInput: function (e) {
    const keyword = e.detail.value;
    this.setData({
      searchKeyword: keyword
    });
    this.filterArticles();
  },

  /**
   * 清除搜索内容
   */
  clearSearch: function () {
    this.setData({
      searchKeyword: ''
    });
    this.filterArticles();
  },

  /**
   * 选择文章分类
   */
  selectCategory: function (e) {
    const category = e.currentTarget.dataset.category;
    this.setData({
      currentCategory: category
    });
    this.filterArticles();
  },

  /**
   * 根据当前条件过滤文章
   */
  filterArticles: function () {
    const { articles, currentCategory, searchKeyword } = this.data;
    
    let filtered = articles;
    
    // 按分类过滤
    if (currentCategory !== 'all') {
      const categoryMap = {
        'dataStructure': '数据结构',
        'algorithm': '算法'
      };
      filtered = filtered.filter(article => article.category === categoryMap[currentCategory]);
    }
    
    // 按关键词过滤
    if (searchKeyword) {
      const keyword = searchKeyword.toLowerCase();
      filtered = filtered.filter(article => 
        article.title.toLowerCase().includes(keyword) || 
        article.description.toLowerCase().includes(keyword)
      );
    }
    
    this.setData({
      filteredArticles: filtered
    });
  },

  /**
   * 跳转到文章详情页
   */
  navigateToArticle: function (e) {
    const articleId = e.currentTarget.dataset.id;
    wx.navigateTo({
      url: `./article?id=${articleId}`
    });
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    return {
      title: '数据结构与算法学习资源',
      path: '/src/pages/learning/learning'
    }
  }
}) 