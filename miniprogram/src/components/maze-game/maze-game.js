Component({
  properties: {
    title: {
      type: String,
      value: '迷宫寻路游戏'
    },
    description: {
      type: String,
      value: '使用不同的算法找到从起点到终点的路径'
    },
    level: {
      type: Number,
      value: 1
    },
    isApiMode: {
      type: Boolean,
      value: false
    }
  },

  data: {
    maze: [],
    algorithm: 'dfs',
    isSolving: false,
    isCompleted: false,
    startPoint: null,
    endPoint: null,
    isApiLoading: false,
    apiError: '',
    animationSpeed: 5,
    showSpeedControl: false
  },

  lifetimes: {
    attached: function() {
      // 组件加载时初始化
      this.initMaze();
    }
  },

  methods: {
    // 初始化迷宫
    initMaze: function() {
      const level = this.properties.level;
      let maze = [];
      
      // 根据关卡生成不同的迷宫
      switch(level) {
        case 1:
          maze = [
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'start', value: 'S' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'end', value: 'E' },
            { type: 'wall', value: '' }
          ];
          break;
        case 2:
          // 生成更复杂的迷宫
          maze = [
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'start', value: 'S' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'end', value: 'E' },
            { type: 'wall', value: '' }
          ];
          break;
        default:
          // 默认迷宫
          maze = [
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'start', value: 'S' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: '', value: '' },
            { type: 'wall', value: '' },
            
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'wall', value: '' },
            { type: 'end', value: 'E' },
            { type: 'wall', value: '' }
          ];
      }
      
      // 找到起点和终点
      const startIndex = maze.findIndex(cell => cell.type === 'start');
      const endIndex = maze.findIndex(cell => cell.type === 'end');
      
      const columns = level === 2 ? 7 : 5; // 第二关是7列的迷宫
      
      this.setData({
        maze,
        isCompleted: false,
        isSolving: false,
        startPoint: {
          x: startIndex % columns,
          y: Math.floor(startIndex / columns)
        },
        endPoint: {
          x: endIndex % columns,
          y: Math.floor(endIndex / columns)
        }
      });
    },
    
    // 选择算法
    selectAlgorithm: function(e) {
      const algorithm = e.currentTarget.dataset.algorithm;
      this.setData({ algorithm });
    },
    
    // 开始寻路
    onStart: function() {
      if (this.data.isSolving) return;
      
      this.setData({ isSolving: true });
      
      // 如果在API模式下，调用API进行寻路
      if (this.data.isApiMode) {
        this.solveWithApi();
        return;
      }
      
      // 重置迷宫路径
      this.resetPath();
      
      // 根据选择的算法执行寻路
      switch (this.data.algorithm) {
        case 'dfs':
          this.dfs();
          break;
        case 'bfs':
          this.bfs();
          break;
        case 'astar':
          this.astar();
          break;
      }
    },
    
    // 使用API解决迷宫
    solveWithApi: function() {
      const api = require('../../services/api').maze;
      const { algorithm, animationSpeed } = this.data;
      
      this.setData({ isApiLoading: true, apiError: '' });
      
      let apiMethod;
      switch (algorithm) {
        case 'dfs':
          apiMethod = api.solveDFS;
          break;
        case 'bfs':
          apiMethod = api.solveBFS;
          break;
        case 'astar':
          apiMethod = api.solveAStar;
          break;
      }
      
      if (!apiMethod) {
        this.setData({
          apiError: '不支持的算法类型',
          isApiLoading: false
        });
        return;
      }
      
      const requestData = {
        maze: this.data.maze,
        start: this.data.startPoint,
        end: this.data.endPoint
      };
      
      apiMethod(requestData, true, animationSpeed)
        .then(response => {
          if (response.success) {
            this.handleApiResponse(response.data);
          } else {
            this.setData({
              apiError: '寻路失败',
              isApiLoading: false
            });
          }
        })
        .catch(error => {
          console.error('API请求失败:', error);
          this.setData({
            apiError: error.message || '网络请求失败',
            isApiLoading: false
          });
        });
    },
    
    // 处理API返回的数据
    handleApiResponse: function(data) {
      if (data.animation && data.animation.frames) {
        this.playAnimationFrames(data.animation.frames);
      } else {
        // 如果没有动画帧，直接显示结果
        this.updateMazeState(data.state);
      }
    },
    
    // 播放动画帧
    playAnimationFrames: function(frames) {
      let currentFrame = 0;
      
      const playFrame = () => {
        if (currentFrame >= frames.length) {
          this.setData({
            isApiLoading: false,
            isSolving: false,
            isCompleted: true
          });
          return;
        }
        
        this.updateMazeState(frames[currentFrame]);
        currentFrame++;
        
        setTimeout(playFrame, 1000 / this.data.animationSpeed);
      };
      
      playFrame();
    },
    
    // 更新迷宫状态
    updateMazeState: function(state) {
      if (!state || !state.maze) return;
      
      this.setData({
        maze: state.maze,
        isCompleted: state.completed || false
      });
    },
    
    // 重置迷宫
    onReset: function() {
      // 如果是API模式，调用API重置
      if (this.data.isApiMode) {
        this.resetMazeApi();
      }
      
      this.resetMaze();
      
      // 触发迷宫重置事件
      this.triggerEvent('resetmaze');
    },
    
    // 重置迷宫状态
    resetMaze: function() {
      // 如果在API模式下，调用API重置
      if (this.data.isApiMode) {
        this.resetMazeApi();
        return;
      }
      
      // 本地重置
      this.initMaze();
    },
    
    // 重置API状态
    resetMazeApi: function() {
      if (!this.data.isApiMode) return;
      
      const api = require('../../services/api').maze;
      this.setData({ isApiLoading: true });
      
      api.reset()
        .then(() => {
          console.log('迷宫状态重置成功');
          this.initMaze();
        })
        .catch(error => {
          console.error('重置迷宫状态失败:', error);
          this.setData({
            apiError: '重置状态失败，请重试'
          });
        })
        .finally(() => {
          this.setData({ isApiLoading: false });
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
    
    // 重置路径
    resetPath: function() {
      const maze = this.data.maze.map(cell => {
        return cell.type === 'path' ? { ...cell, type: '' } : cell;
      });
      
      this.setData({ maze });
    },
    
    // 深度优先搜索
    dfs: function() {
      const { startPoint, endPoint, maze } = this.data;
      const columns = this.properties.level === 2 ? 7 : 5;
      const rows = maze.length / columns;
      
      // 创建访问标记
      const visited = new Array(rows).fill().map(() => new Array(columns).fill(false));
      
      // 寻找路径
      const path = [];
      const found = this.dfsSearch(startPoint.y, startPoint.x, endPoint, maze, visited, path, columns);
      
      if (found) {
        this.animatePath(path);
      } else {
        wx.showToast({
          title: '未找到路径',
          icon: 'none',
          duration: 2000
        });
        this.setData({ isSolving: false });
      }
    },
    
    // DFS搜索算法
    dfsSearch: function(y, x, end, maze, visited, path, columns) {
      // 检查是否超出边界或者是墙
      if (y < 0 || x < 0 || y >= maze.length / columns || x >= columns || 
          maze[y * columns + x].type === 'wall' || visited[y][x]) {
        return false;
      }
      
      // 标记为已访问
      visited[y][x] = true;
      
      // 添加到路径
      path.push({ y, x });
      
      // 检查是否到达终点
      if (x === end.x && y === end.y) {
        return true;
      }
      
      // 向四个方向搜索
      const directions = [
        { dy: -1, dx: 0 }, // 上
        { dy: 0, dx: 1 },  // 右
        { dy: 1, dx: 0 },  // 下
        { dy: 0, dx: -1 }  // 左
      ];
      
      for (const dir of directions) {
        const newY = y + dir.dy;
        const newX = x + dir.dx;
        
        if (this.dfsSearch(newY, newX, end, maze, visited, path, columns)) {
          return true;
        }
      }
      
      // 回溯
      path.pop();
      return false;
    },
    
    // 广度优先搜索
    bfs: function() {
      const { startPoint, endPoint, maze } = this.data;
      const columns = this.properties.level === 2 ? 7 : 5;
      const rows = maze.length / columns;
      
      // 创建访问标记
      const visited = new Array(rows).fill().map(() => new Array(columns).fill(false));
      
      // 创建队列
      const queue = [{ y: startPoint.y, x: startPoint.x, path: [] }];
      visited[startPoint.y][startPoint.x] = true;
      
      // 方向数组
      const directions = [
        { dy: -1, dx: 0 }, // 上
        { dy: 0, dx: 1 },  // 右
        { dy: 1, dx: 0 },  // 下
        { dy: 0, dx: -1 }  // 左
      ];
      
      while (queue.length > 0) {
        const { y, x, path } = queue.shift();
        
        // 当前路径
        const currentPath = [...path, { y, x }];
        
        // 检查是否到达终点
        if (x === endPoint.x && y === endPoint.y) {
          this.animatePath(currentPath);
          return;
        }
        
        // 向四个方向搜索
        for (const dir of directions) {
          const newY = y + dir.dy;
          const newX = x + dir.dx;
          
          // 检查是否有效
          if (newY >= 0 && newX >= 0 && newY < rows && newX < columns && 
              maze[newY * columns + newX].type !== 'wall' && !visited[newY][newX]) {
            visited[newY][newX] = true;
            queue.push({ y: newY, x: newX, path: currentPath });
          }
        }
      }
      
      // 未找到路径
      wx.showToast({
        title: '未找到路径',
        icon: 'none',
        duration: 2000
      });
      this.setData({ isSolving: false });
    },
    
    // A*算法
    astar: function() {
      const { startPoint, endPoint, maze } = this.data;
      const columns = this.properties.level === 2 ? 7 : 5;
      const rows = maze.length / columns;
      
      // 创建开放列表和关闭列表
      const openList = [];
      const closedList = [];
      const gScore = {};
      const fScore = {};
      const cameFrom = {};
      
      // 初始化起点
      const startKey = `${startPoint.y},${startPoint.x}`;
      gScore[startKey] = 0;
      fScore[startKey] = this.heuristic(startPoint, endPoint);
      openList.push(startKey);
      
      // 方向数组
      const directions = [
        { dy: -1, dx: 0 }, // 上
        { dy: 0, dx: 1 },  // 右
        { dy: 1, dx: 0 },  // 下
        { dy: 0, dx: -1 }  // 左
      ];
      
      while (openList.length > 0) {
        // 找到f值最小的节点
        let currentKey = openList[0];
        let currentIndex = 0;
        
        for (let i = 1; i < openList.length; i++) {
          if (fScore[openList[i]] < fScore[currentKey]) {
            currentKey = openList[i];
            currentIndex = i;
          }
        }
        
        // 移除当前节点从开放列表
        openList.splice(currentIndex, 1);
        
        // 解析当前节点坐标
        const [y, x] = currentKey.split(',').map(Number);
        
        // 检查是否到达终点
        if (x === endPoint.x && y === endPoint.y) {
          // 重建路径
          const path = this.reconstructPath(cameFrom, currentKey);
          this.animatePath(path);
          return;
        }
        
        // 添加到关闭列表
        closedList.push(currentKey);
        
        // 检查相邻节点
        for (const dir of directions) {
          const newY = y + dir.dy;
          const newX = x + dir.dx;
          
          // 检查是否有效
          if (newY < 0 || newX < 0 || newY >= rows || newX >= columns || 
              maze[newY * columns + newX].type === 'wall') {
            continue;
          }
          
          const neighborKey = `${newY},${newX}`;
          
          // 如果在关闭列表中，跳过
          if (closedList.includes(neighborKey)) {
            continue;
          }
          
          // 计算g值
          const tentativeG = gScore[currentKey] + 1;
          
          // 如果不在开放列表中，添加
          if (!openList.includes(neighborKey)) {
            openList.push(neighborKey);
          } else if (tentativeG >= gScore[neighborKey]) {
            // 如果已经在开放列表中且路径不更好，跳过
            continue;
          }
          
          // 记录这条路径
          cameFrom[neighborKey] = currentKey;
          gScore[neighborKey] = tentativeG;
          fScore[neighborKey] = gScore[neighborKey] + this.heuristic({ y: newY, x: newX }, endPoint);
        }
      }
      
      // 未找到路径
      wx.showToast({
        title: '未找到路径',
        icon: 'none',
        duration: 2000
      });
      this.setData({ isSolving: false });
    },
    
    // 启发式函数(曼哈顿距离)
    heuristic: function(a, b) {
      return Math.abs(a.x - b.x) + Math.abs(a.y - b.y);
    },
    
    // 重建路径
    reconstructPath: function(cameFrom, currentKey) {
      const path = [];
      
      while (currentKey in cameFrom) {
        const [y, x] = currentKey.split(',').map(Number);
        path.unshift({ y, x });
        currentKey = cameFrom[currentKey];
      }
      
      const [y, x] = currentKey.split(',').map(Number);
      path.unshift({ y, x });
      
      return path;
    },
    
    // 动画展示路径
    animatePath: function(path) {
      // 移除起点和终点
      if (path.length > 0) {
        path.shift(); // 移除起点
        path.pop();   // 移除终点
      }
      
      if (path.length === 0) {
        this.setData({ isSolving: false, isCompleted: true });
        wx.showToast({
          title: '路径找到！',
          icon: 'success',
          duration: 2000
        });
        return;
      }
      
      let index = 0;
      const columns = this.properties.level === 2 ? 7 : 5;
      
      const interval = setInterval(() => {
        if (index >= path.length) {
          clearInterval(interval);
          this.setData({ isSolving: false, isCompleted: true });
          
          // 触发路径完成事件
          this.triggerEvent('pathcompleted', { 
            algorithm: this.data.algorithm,
            pathLength: path.length
          });
          
          wx.showToast({
            title: '路径找到！',
            icon: 'success',
            duration: 2000
          });
          return;
        }
        
        const point = path[index];
        const mazeIndex = point.y * columns + point.x;
        
        // 更新迷宫显示路径
        const maze = [...this.data.maze];
        maze[mazeIndex] = { ...maze[mazeIndex], type: 'path' };
        
        this.setData({ maze });
        index++;
      }, 200);
    },

    // 切换API模式
    toggleApiMode: function(e) {
      if (this.data.isSolving || this.data.isApiLoading) return;
      
      const mode = e.currentTarget.dataset.mode;
      const isApiMode = mode === 'api';
      
      this.setData({ 
        isApiMode,
        apiError: '',
        showSpeedControl: isApiMode
      });
      
      // 切换到API模式时重置状态
      if (isApiMode) {
        this.resetMazeApi();
      }
      
      wx.showToast({
        title: `已切换至${isApiMode ? 'API' : '本地'}模式`,
        icon: 'none'
      });
    },

    // 设置动画速度
    setAnimationSpeed: function(e) {
      this.setData({
        animationSpeed: e.detail.value
      });
    },

    // 切换速度控制显示状态
    toggleSpeedControl: function() {
      this.setData({
        showSpeedControl: !this.data.showSpeedControl
      });
    }
  }
}); 