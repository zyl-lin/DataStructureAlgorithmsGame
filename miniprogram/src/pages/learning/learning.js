const app = getApp()

Page({
  data: {
    categories: [
      {
        id: 1,
        name: '数据结构基础',
        icon: '/assets/icons/datastructure.png',
        articles: [
          {
            id: 101,
            title: '链表基础知识',
            content: '链表是一种常见的数据结构，由节点组成，每个节点包含数据和指向下一个节点的指针...',
            summary: '了解链表的基本概念、类型和基本操作',
            date: '2024-03-10',
            views: 328
          },
          {
            id: 102,
            title: '栈和队列入门',
            content: '栈是一种遵循后进先出(LIFO)原则的数据结构，而队列遵循先进先出(FIFO)原则...',
            summary: '学习栈和队列的基本概念和应用场景',
            date: '2024-03-12',
            views: 245
          }
        ]
      },
      {
        id: 2,
        name: '算法入门',
        icon: '/assets/icons/algorithm.png',
        articles: [
          {
            id: 201,
            title: '排序算法详解',
            content: '排序算法是计算机科学中最基本的算法之一，用于将一组数据按照特定顺序重新排列...',
            summary: '了解常见排序算法的原理和实现',
            date: '2024-03-15',
            views: 412
          },
          {
            id: 202,
            title: '搜索算法基础',
            content: '搜索算法用于在数据集合中查找特定元素，常见的搜索算法包括线性搜索和二分搜索...',
            summary: '掌握基本搜索算法的使用方法',
            date: '2024-03-18',
            views: 156
          }
        ]
      },
      {
        id: 3,
        name: '图论基础',
        icon: '/assets/icons/graph.png',
        articles: []
      },
      {
        id: 4,
        name: '树结构',
        icon: '/assets/icons/tree.png',
        articles: []
      }
    ],
    recommendedArticles: [
      {
        id: 301,
        title: '数据结构与算法学习路线图',
        summary: '从零开始学习数据结构与算法的完整指南',
        date: '2024-03-20',
        views: 589
      },
      {
        id: 302,
        title: '如何高效刷算法题',
        summary: '提升算法学习效率的实用技巧和方法',
        date: '2024-03-19',
        views: 421
      }
    ]
  },

  onLoad: function() {
    // 页面加载时执行的逻辑
  },

  onCategoryTap: function(e) {
    const category = e.currentTarget.dataset.category
    wx.navigateTo({
      url: `/pages/learning/category?id=${category.id}&name=${category.name}`
    })
  },

  onArticleTap: function(e) {
    const article = e.currentTarget.dataset.article
    wx.navigateTo({
      url: `/pages/learning/article?id=${article.id}&title=${article.title}`
    })
  },

  navigateToIndex: function() {
    wx.switchTab({
      url: '/pages/index/index'
    })
  },

  navigateToProfile: function() {
    wx.switchTab({
      url: '/pages/profile/profile'
    })
  }
}) 