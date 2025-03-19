const app = getApp()

Page({
  data: {
    articleId: null,
    article: {},
    relatedArticles: [],
    feedback: '' // 可以是 'helpful' 或 'not-helpful'
  },

  onLoad: function(options) {
    const { id, title } = options
    this.setData({
      articleId: parseInt(id)
    })
    
    if (title) {
      this.setData({
        'article.title': title
      })
    }
    
    this.fetchArticleData()
  },

  fetchArticleData: function() {
    // 这里应该从服务器获取文章数据
    // 现在使用模拟数据
    const allArticles = this.getAllArticles()
    const article = allArticles.find(a => a.id === this.data.articleId)
    
    if (article) {
      this.setData({
        article: article
      })
      
      // 获取相关文章（同类型的其他文章）
      const related = allArticles
        .filter(a => a.id !== this.data.articleId && a.category === article.category)
        .slice(0, 3)
      
      this.setData({
        relatedArticles: related
      })
    }
  },
  
  getAllArticles: function() {
    // 模拟数据库中的所有文章
    return [
      {
        id: 101,
        title: '链表基础知识',
        content: '<div><h3>什么是链表？</h3><p>链表是一种常见的数据结构，由节点组成，每个节点包含数据和指向下一个节点的指针。链表不要求逻辑上相邻的元素在物理存储位置上也相邻，因此链表在插入和删除操作上比数组更加高效。</p><h3>链表的类型</h3><p>链表主要有以下几种类型：</p><ul><li>单向链表：每个节点包含数据和指向下一个节点的指针</li><li>双向链表：每个节点包含数据、指向下一个节点的指针和指向上一个节点的指针</li><li>循环链表：最后一个节点指向第一个节点，形成一个环</li></ul><h3>基本操作</h3><p>链表的基本操作包括：</p><ul><li>插入节点</li><li>删除节点</li><li>查找节点</li><li>遍历链表</li></ul></div>',
        summary: '了解链表的基本概念、类型和基本操作',
        date: '2024-03-10',
        views: 328,
        category: 'datastructure',
        tags: ['数据结构', '链表', '指针']
      },
      {
        id: 102,
        title: '栈和队列入门',
        content: '<div><h3>栈(Stack)</h3><p>栈是一种遵循后进先出(LIFO)原则的数据结构。也就是说，最后添加的元素会被最先删除。栈的基本操作包括：</p><ul><li>push: 将元素添加到栈顶</li><li>pop: 移除栈顶元素</li><li>peek: 查看栈顶元素但不移除</li></ul><h3>队列(Queue)</h3><p>队列是一种遵循先进先出(FIFO)原则的数据结构。也就是说，最先添加的元素会被最先删除。队列的基本操作包括：</p><ul><li>enqueue: 将元素添加到队列末尾</li><li>dequeue: 移除队列头部元素</li><li>peek: 查看队列头部元素但不移除</li></ul></div>',
        summary: '学习栈和队列的基本概念和应用场景',
        date: '2024-03-12',
        views: 245,
        category: 'datastructure',
        tags: ['数据结构', '栈', '队列', 'LIFO', 'FIFO']
      },
      {
        id: 201,
        title: '排序算法详解',
        content: '<div><h3>常见排序算法</h3><p>排序算法是计算机科学中最基本的算法之一，用于将一组数据按照特定顺序重新排列。常见的排序算法包括：</p><ul><li>冒泡排序</li><li>选择排序</li><li>插入排序</li><li>快速排序</li><li>归并排序</li><li>堆排序</li></ul><h3>算法比较</h3><p>不同的排序算法有不同的时间和空间复杂度：</p><table><tr><th>算法</th><th>平均时间复杂度</th><th>最坏时间复杂度</th><th>空间复杂度</th><th>稳定性</th></tr><tr><td>冒泡排序</td><td>O(n²)</td><td>O(n²)</td><td>O(1)</td><td>稳定</td></tr><tr><td>选择排序</td><td>O(n²)</td><td>O(n²)</td><td>O(1)</td><td>不稳定</td></tr><tr><td>插入排序</td><td>O(n²)</td><td>O(n²)</td><td>O(1)</td><td>稳定</td></tr><tr><td>快速排序</td><td>O(n log n)</td><td>O(n²)</td><td>O(log n)</td><td>不稳定</td></tr><tr><td>归并排序</td><td>O(n log n)</td><td>O(n log n)</td><td>O(n)</td><td>稳定</td></tr><tr><td>堆排序</td><td>O(n log n)</td><td>O(n log n)</td><td>O(1)</td><td>不稳定</td></tr></table></div>',
        summary: '了解常见排序算法的原理和实现',
        date: '2024-03-15',
        views: 412,
        category: 'algorithm',
        tags: ['算法', '排序', '复杂度分析']
      }
    ]
  },

  onRelatedTap: function(e) {
    const article = e.currentTarget.dataset.article
    wx.redirectTo({
      url: `/pages/learning/article?id=${article.id}&title=${article.title}`
    })
  },

  onFeedbackTap: function(e) {
    const type = e.currentTarget.dataset.type
    this.setData({
      feedback: type
    })
    
    // 这里可以向服务器发送反馈数据
    wx.showToast({
      title: type === 'helpful' ? '感谢您的反馈！' : '感谢您的反馈，我们会改进内容',
      icon: 'none',
      duration: 2000
    })
  },

  goBack: function() {
    wx.navigateBack()
  }
}) 