const app = getApp()

Page({
  /**
   * 页面的初始数据
   */
  data: {
    articleId: null,
    article: {},
    formattedContent: '',
    relatedArticles: [],
    isFavorite: false
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    const { id } = options;
    if (id) {
      this.setData({
        articleId: parseInt(id)
      });
      this.fetchArticleData();
    }
  },
  
  /**
   * 获取文章数据
   */
  fetchArticleData: function () {
    // 从数据源获取文章数据
    const allArticles = this.getAllArticles();
    const article = allArticles.find(a => a.id === this.data.articleId);
    
    if (article) {
      // 检查是否收藏
      const favorites = wx.getStorageSync('favoriteArticles') || [];
      const isFavorite = favorites.includes(article.id);
      
      // 格式化文章内容为富文本
      const formattedContent = this.formatContent(article.content);
      
      // 获取相关文章（同类别的其他文章，最多3篇）
      const related = allArticles
        .filter(a => a.id !== article.id && a.category === article.category)
        .slice(0, 3);
      
      this.setData({
        article,
        formattedContent,
        relatedArticles: related,
        isFavorite
      });
    } else {
      wx.showToast({
        title: '文章不存在',
        icon: 'none',
        duration: 2000
      });
      setTimeout(() => {
        wx.navigateBack();
      }, 2000);
    }
  },
  
  /**
   * 格式化文章内容为富文本
   */
  formatContent: function (content) {
    // 在实际应用中，这里可能需要将Markdown或HTML格式的内容转换为小程序支持的富文本格式
    // 这里使用简单的示例进行格式化
    
    // 添加标题和段落样式
    let formatted = content;
    
    // 将内容分成段落
    formatted = formatted.replace(/\.\.\./g, '。<br><br>');
    
    // 添加一些富文本标签样式
    const html = `
      <div style="font-size: 30rpx; line-height: 1.8; color: #333;">
        <h2 style="font-size: 34rpx; font-weight: bold; margin: 30rpx 0 20rpx; color: #333;">文章内容</h2>
        <p style="margin-bottom: 20rpx; text-align: justify;">${formatted}</p>
        
        <h2 style="font-size: 34rpx; font-weight: bold; margin: 30rpx 0 20rpx; color: #333;">核心要点</h2>
        <ul style="padding-left: 30rpx; margin: 20rpx 0;">
          <li style="margin-bottom: 10rpx;">理解${this.data.article.category}的基本概念</li>
          <li style="margin-bottom: 10rpx;">掌握常见实现方法</li>
          <li style="margin-bottom: 10rpx;">了解实际应用场景</li>
        </ul>
        
        <h2 style="font-size: 34rpx; font-weight: bold; margin: 30rpx 0 20rpx; color: #333;">复杂度分析</h2>
        <p style="margin-bottom: 20rpx; text-align: justify;">
          <strong style="font-weight: bold;">时间复杂度：</strong> O(n)<br>
          <strong style="font-weight: bold;">空间复杂度：</strong> O(n)
        </p>
        
        <blockquote style="border-left: 8rpx solid #e0e0e0; padding: 15rpx 20rpx; color: #666; margin: 20rpx 0; background-color: #f9f9f9;">
          学习${this.data.article.category}的关键是理解其核心原理和应用场景。通过实践练习加深理解。
        </blockquote>
      </div>
    `;
    
    return html;
  },
  
  /**
   * 获取所有文章数据
   */
  getAllArticles: function () {
    // 这应该是从服务器或本地存储获取的数据
    return [
      {
        id: 1,
        title: '链表基础与应用',
        description: '介绍链表的基本概念、数据结构和常见操作，包括单链表、双链表和循环链表的特点与实现。',
        coverImage: '/assets/images/articles/linkedlist.png',
        category: '数据结构',
        date: '2024-03-20',
        content: '链表是一种常见的基础数据结构，由节点组成，每个节点包含数据和指向下一个节点的指针。链表不要求逻辑上相邻的元素在物理存储位置上也相邻，因此链表在插入和删除操作上比数组更加高效。链表主要有单链表、双链表和循环链表几种类型。单链表中每个节点只包含一个指向下一节点的指针，而双链表则包含两个指针，分别指向前一个节点和后一个节点。循环链表的最后一个节点指向第一个节点，形成一个环...'
      },
      {
        id: 2,
        title: '栈与队列详解',
        description: '深入讲解栈和队列的工作原理、特性及其在算法中的应用，包含实际编程案例和性能分析。',
        coverImage: '/assets/images/articles/stack-queue.png',
        category: '数据结构',
        date: '2024-03-18',
        content: '栈是一种遵循后进先出(LIFO)原则的有序集合，队列则遵循先进先出(FIFO)原则。栈的基本操作包括入栈(push)、出栈(pop)和查看栈顶元素(peek)。队列的基本操作包括入队(enqueue)、出队(dequeue)和查看队首元素(peek)。这两种数据结构在算法实现中非常常见，如递归算法的实现、浏览器的前进后退功能、操作系统的进程调度等。在实际编程中，栈可以用于检查括号匹配、实现函数调用和表达式求值，而队列则适用于广度优先搜索算法、打印机任务调度等场景...'
      },
      {
        id: 3,
        title: '图算法及其应用',
        description: '全面解析图数据结构及常用算法，包括深度优先搜索、广度优先搜索、最短路径等在实际问题中的应用。',
        coverImage: '/assets/images/articles/graph.png',
        category: '数据结构',
        date: '2024-03-15',
        content: '图是一种由顶点和连接顶点的边组成的数据结构，可用于表示各种关系和网络。图有有向图和无向图之分，边可以带权重也可以不带权重。图的表示方法主要有邻接矩阵和邻接表两种。图的遍历算法主要有深度优先搜索(DFS)和广度优先搜索(BFS)。深度优先搜索通常使用栈或递归实现，而广度优先搜索则使用队列实现。在实际应用中，图算法可以解决最短路径问题(如Dijkstra算法、Floyd算法)、最小生成树问题(如Prim算法、Kruskal算法)、网络流问题等...'
      },
      {
        id: 4,
        title: '二叉树遍历技巧',
        description: '详细介绍二叉树的前序、中序、后序及层序遍历算法，以及递归与非递归实现方式的对比。',
        coverImage: '/assets/images/articles/binary-tree.png',
        category: '数据结构',
        date: '2024-03-12',
        content: '二叉树是一种树形数据结构，其中每个节点最多有两个子节点，通常被称为左子节点和右子节点。二叉树的遍历方式主要有四种：前序遍历(根-左-右)、中序遍历(左-根-右)、后序遍历(左-右-根)和层序遍历(逐层从左到右)。前三种遍历方式可以使用递归或栈实现，而层序遍历通常使用队列实现。在递归实现中，代码简洁易懂，但可能导致栈溢出；非递归实现则避免了栈溢出问题，但代码相对复杂。二叉树遍历在表达式求值、语法分析、编译器设计等领域有广泛应用...'
      },
      {
        id: 5,
        title: '排序算法大比拼',
        description: '对比分析主流排序算法的工作原理、时间复杂度、空间复杂度及适用场景，助你在实际开发中选择最佳方案。',
        coverImage: '/assets/images/articles/sorting.png',
        category: '算法',
        date: '2024-03-08',
        content: '排序算法是计算机科学中最基本也是最重要的算法之一，用于将一组特定的数据按照既定规则进行排序。常见的排序算法包括冒泡排序、选择排序、插入排序、快速排序、归并排序和堆排序等。这些算法在时间复杂度、空间复杂度和稳定性方面各有特点。例如，冒泡排序、选择排序和插入排序的时间复杂度为O(n²)，适用于小规模数据；而快速排序、归并排序和堆排序的时间复杂度为O(n log n)，适用于大规模数据。在选择排序算法时，需要考虑数据规模、数据特点(如是否接近有序)、稳定性要求和内存限制等因素...'
      },
      {
        id: 6,
        title: '搜索算法实战指南',
        description: '从简单到复杂，全面讲解各类搜索算法，包括线性搜索、二分搜索、哈希搜索等技术及优化策略。',
        coverImage: '/assets/images/articles/search.png',
        category: '算法',
        date: '2024-03-05',
        content: '搜索算法是在数据集合中查找特定元素的方法，不同的搜索算法适用于不同的数据组织方式。线性搜索是最简单的搜索算法，时间复杂度为O(n)，适用于无序数据。二分搜索要求数据有序，时间复杂度为O(log n)，效率较高。哈希搜索利用哈希表，在理想情况下时间复杂度为O(1)，但需要额外的空间来存储哈希表。在实际应用中，需要根据数据特性选择合适的搜索算法。例如，对于频繁查询但较少修改的数据，可以预先排序并使用二分搜索；对于查询频率极高的场景，可以使用哈希表提高效率...'
      },
      {
        id: 7,
        title: '动态规划解题思路',
        description: '系统讲解动态规划的核心思想和解题步骤，通过经典问题剖析如何构建状态转移方程和优化解法。',
        coverImage: '/assets/images/articles/dp.png',
        category: '算法',
        date: '2024-03-01',
        content: '动态规划是解决具有重叠子问题和最优子结构性质问题的算法策略，通过将问题分解为子问题并存储子问题的解来避免重复计算。动态规划的核心步骤包括：定义状态、确定状态转移方程、初始化边界条件和计算最终结果。常见的动态规划问题包括斐波那契数列、背包问题、最长公共子序列等。例如，在斐波那契数列问题中，可以定义状态dp[i]为第i个斐波那契数，状态转移方程为dp[i] = dp[i-1] + dp[i-2]。动态规划通常有自顶向下(记忆化搜索)和自底向上两种实现方法...'
      },
      {
        id: 8,
        title: '贪心算法设计技巧',
        description: '从原理到实践，详解贪心算法的思想、应用场景及局限性，帮助你掌握解决实际问题的能力。',
        coverImage: '/assets/images/articles/greedy.png',
        category: '算法',
        date: '2024-02-28',
        content: '贪心算法是一种在每一步选择中都采取当前状态下最好或最优的选择，从而希望导致结果是最好或最优的算法。贪心算法的核心是贪心选择性质和最优子结构。贪心算法适用的问题包括活动选择问题、哈夫曼编码、最小生成树算法(如Prim算法和Kruskal算法)等。在实际应用中，贪心算法通常比动态规划更高效，但不是所有问题都能用贪心算法解决。例如，在零钱兑换问题中，如果可用的硬币面值具有特殊性质，贪心算法可能会得到次优解。因此，在使用贪心算法前，需要证明贪心选择能够导致全局最优解...'
      }
    ];
  },
  
  /**
   * 切换收藏状态
   */
  toggleFavorite: function () {
    const articleId = this.data.article.id;
    let favorites = wx.getStorageSync('favoriteArticles') || [];
    
    if (this.data.isFavorite) {
      // 取消收藏
      favorites = favorites.filter(id => id !== articleId);
      wx.showToast({
        title: '已取消收藏',
        icon: 'none',
        duration: 1500
      });
    } else {
      // 添加收藏
      favorites.push(articleId);
      wx.showToast({
        title: '已收藏',
        icon: 'success',
        duration: 1500
      });
    }
    
    wx.setStorageSync('favoriteArticles', favorites);
    this.setData({
      isFavorite: !this.data.isFavorite
    });
  },
  
  /**
   * 导航到其他文章
   */
  navigateToArticle: function (e) {
    const articleId = e.currentTarget.dataset.id;
    wx.redirectTo({
      url: `./article?id=${articleId}`
    });
  },
  
  /**
   * 返回上一页
   */
  navigateBack: function () {
    wx.navigateBack();
  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {
    const { article } = this.data;
    return {
      title: article.title,
      path: `/src/pages/learning/article?id=${article.id}`,
      imageUrl: article.coverImage
    };
  }
}) 