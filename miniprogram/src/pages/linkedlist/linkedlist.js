Page({
  data: {
    linkedList: [],
    currentTask: '当前任务: 在值为12的节点后插入值为13的新节点',
    operationTypes: ['插入节点', '删除节点', '查找节点'],
    operationTypeIndex: 0,
    positionTypes: ['头部', '尾部', '指定节点之后'],
    positionIndex: 2,
    nodeValue: 13,
    targetNodeValue: 12,
    showPositionPicker: true,
    showTargetNode: true,
    operationResult: '',
    taskCompleted: false
  },

  onLoad: function() {
    this.initLinkedList();
  },

  // 初始化链表
  initLinkedList: function() {
    const linkedList = [
      { value: 5 },
      { value: 8 },
      { value: 12 },
      { value: 15 },
      { value: 20 }
    ];
    this.setData({ linkedList });
  },

  // 操作类型改变
  onOperationTypeChange: function(e) {
    const operationTypeIndex = parseInt(e.detail.value);
    let showPositionPicker = true;
    let showTargetNode = true;

    // 根据操作类型更新界面状态
    switch (operationTypeIndex) {
      case 0: // 插入节点
        showPositionPicker = true;
        showTargetNode = this.data.positionIndex === 2; // 如果是在指定节点后插入才显示目标节点
        break;
      case 1: // 删除节点
        showPositionPicker = false;
        showTargetNode = true;
        break;
      case 2: // 查找节点
        showPositionPicker = false;
        showTargetNode = false;
        break;
    }

    this.setData({
      operationTypeIndex,
      showPositionPicker,
      showTargetNode
    });
  },

  // 位置改变
  onPositionChange: function(e) {
    const positionIndex = parseInt(e.detail.value);
    const showTargetNode = positionIndex === 2; // 只有在"指定节点之后"时才显示目标节点输入框
    this.setData({
      positionIndex,
      showTargetNode
    });
  },

  // 节点值输入
  onNodeValueInput: function(e) {
    this.setData({
      nodeValue: e.detail.value
    });
  },

  // 目标节点值输入
  onTargetNodeValueInput: function(e) {
    this.setData({
      targetNodeValue: e.detail.value
    });
  },

  // 执行链表操作
  executeOperation: function() {
    const { operationTypeIndex, positionIndex, nodeValue, targetNodeValue, linkedList } = this.data;

    // 转换为数字类型
    const nodeValueNum = parseInt(nodeValue);
    const targetNodeValueNum = parseInt(targetNodeValue);

    // 检查输入
    if (operationTypeIndex === 0 && isNaN(nodeValueNum)) {
      this.showResult('请输入有效的节点值');
      return;
    }

    if (operationTypeIndex !== 2 && (positionIndex === 2 || operationTypeIndex === 1) && isNaN(targetNodeValueNum)) {
      this.showResult('请输入有效的目标节点值');
      return;
    }

    // 根据操作类型执行相应的链表操作
    switch (operationTypeIndex) {
      case 0: // 插入节点
        this.insertNode(nodeValueNum, positionIndex, targetNodeValueNum);
        break;
      case 1: // 删除节点
        this.deleteNode(targetNodeValueNum);
        break;
      case 2: // 查找节点
        this.findNode(nodeValueNum);
        break;
    }

    // 检查当前任务是否完成
    this.checkTaskCompletion();
  },

  // 插入节点
  insertNode: function(value, positionIndex, targetValue) {
    const linkedList = [...this.data.linkedList];
    let newList = [];
    let inserted = false;

    switch (positionIndex) {
      case 0: // 头部插入
        newList = [{ value }, ...linkedList];
        inserted = true;
        this.showResult(`成功在链表头部插入值为 ${value} 的节点`);
        break;
      case 1: // 尾部插入
        newList = [...linkedList, { value }];
        inserted = true;
        this.showResult(`成功在链表尾部插入值为 ${value} 的节点`);
        break;
      case 2: // 指定节点后插入
        // 查找目标节点
        const targetIndex = linkedList.findIndex(node => node.value === targetValue);
        if (targetIndex === -1) {
          this.showResult(`未找到值为 ${targetValue} 的节点`);
          return;
        }
        // 在目标节点后插入
        newList = [
          ...linkedList.slice(0, targetIndex + 1),
          { value },
          ...linkedList.slice(targetIndex + 1)
        ];
        inserted = true;
        this.showResult(`成功在值为 ${targetValue} 的节点后插入值为 ${value} 的节点`);
        break;
    }

    if (inserted) {
      this.setData({ linkedList: newList });
    }
  },

  // 删除节点
  deleteNode: function(value) {
    const linkedList = [...this.data.linkedList];
    const targetIndex = linkedList.findIndex(node => node.value === value);

    if (targetIndex === -1) {
      this.showResult(`未找到值为 ${value} 的节点`);
      return;
    }

    linkedList.splice(targetIndex, 1);
    this.setData({ linkedList });
    this.showResult(`成功删除值为 ${value} 的节点`);
  },

  // 查找节点
  findNode: function(value) {
    const linkedList = [...this.data.linkedList];
    const targetIndex = linkedList.findIndex(node => node.value === value);

    if (targetIndex === -1) {
      this.showResult(`未找到值为 ${value} 的节点`);
      return;
    }

    this.showResult(`找到值为 ${value} 的节点，位置为第 ${targetIndex + 1} 个节点`);
  },

  // 显示操作结果
  showResult: function(message) {
    this.setData({
      operationResult: message
    });

    // 3秒后自动清除结果消息
    setTimeout(() => {
      this.setData({
        operationResult: ''
      });
    }, 3000);
  },

  // 检查任务完成情况
  checkTaskCompletion: function() {
    // 检查当前任务是否已完成
    if (this.data.currentTask.includes('在值为12的节点后插入值为13的新节点')) {
      const linkedList = this.data.linkedList;
      const index12 = linkedList.findIndex(node => node.value === 12);
      
      if (index12 !== -1 && index12 < linkedList.length - 1 && linkedList[index12 + 1].value === 13) {
        // 任务完成
        if (!this.data.taskCompleted) {
          wx.showToast({
            title: '任务完成！',
            icon: 'success',
            duration: 2000
          });
          this.setData({ taskCompleted: true });
        }
      }
    }
  }
}); 