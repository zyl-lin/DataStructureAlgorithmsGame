#include "maze_animations.h"
#include "../../common/response_builder.h"
#include "../../common/animation_session.h"
#include <queue>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <crow.h>
#include <nlohmann/json.hpp>
#include "database.h"

using json = nlohmann::json;

// 创建初始迷宫状态
json createInitialMazeState() {
    json state;
    state["mazeSize"] = 10;  // 10x10的迷宫
    state["grid"] = json::array();
    state["startX"] = 0;
    state["startY"] = 0;
    state["endX"] = 9;
    state["endY"] = 9;
    state["currentX"] = -1;
    state["currentY"] = -1;
    state["visited"] = json::array();
    state["path"] = json::array();
    state["completed"] = false;
    state["description"] = "初始迷宫状态";
    
    // 初始化网格
    for (int i = 0; i < 10; i++) {
        json row = json::array();
        for (int j = 0; j < 10; j++) {
            row.push_back(1);  // 1表示墙壁
        }
        state["grid"].push_back(row);
    }
    
    // 初始化访问数组
    for (int i = 0; i < 10; i++) {
        json row = json::array();
        for (int j = 0; j < 10; j++) {
            row.push_back(false);
        }
        state["visited"].push_back(row);
    }
    
    return state;
}

// 迷宫生成处理函数
crow::response handleMazeGenerate(const crow::request& req) {
    try {
        json body = json::parse(req.body);
        // TODO: 实现迷宫生成逻辑
        return crow::response(200, "Maze generated successfully");
    } catch (const std::exception& e) {
        return crow::response(400, std::string("Error: ") + e.what());
    }
}

// 迷宫DFS求解处理函数
crow::response handleMazeSolveDFS(const crow::request& req) {
    try {
        json body = json::parse(req.body);
        // TODO: 实现迷宫DFS求解逻辑
        return crow::response(200, "Maze solved using DFS");
    } catch (const std::exception& e) {
        return crow::response(400, std::string("Error: ") + e.what());
    }
}

// 迷宫BFS求解处理函数
crow::response handleMazeSolveBFS(const crow::request& req) {
    try {
        json body = json::parse(req.body);
        // TODO: 实现迷宫BFS求解逻辑
        return crow::response(200, "Maze solved using BFS");
    } catch (const std::exception& e) {
        return crow::response(400, std::string("Error: ") + e.what());
    }
}

// 迷宫A*求解处理函数
crow::response handleMazeSolveAStar(const crow::request& req) {
    try {
        json body = json::parse(req.body);
        // TODO: 实现迷宫A*求解逻辑
        return crow::response(200, "Maze solved using A*");
    } catch (const std::exception& e) {
        return crow::response(400, std::string("Error: ") + e.what());
    }
}

// 获取迷宫状态处理函数
crow::response handleGetMazeState(const crow::request& req) {
    try {
        // TODO: 实现获取迷宫状态逻辑
        json response = {
            {"status", "success"},
            {"state", "current_maze_state"}
        };
        return crow::response(200, response.dump());
    } catch (const std::exception& e) {
        return crow::response(400, std::string("Error: ") + e.what());
    }
}

// 重置迷宫状态处理函数
crow::response handleResetMazeState(const crow::request& req) {
    try {
        // TODO: 实现重置迷宫状态逻辑
        return crow::response(200, "Maze state reset successfully");
    } catch (const std::exception& e) {
        return crow::response(400, std::string("Error: ") + e.what());
    }
}

// 带数据库参数的处理函数重载
crow::response handleMazeGenerate(const crow::request& req, Database& db) {
    return handleMazeGenerate(req);
}

crow::response handleMazeSolveDFS(const crow::request& req, Database& db) {
    return handleMazeSolveDFS(req);
}

crow::response handleMazeSolveBFS(const crow::request& req, Database& db) {
    return handleMazeSolveBFS(req);
}

crow::response handleMazeSolveAStar(const crow::request& req, Database& db) {
    return handleMazeSolveAStar(req);
}

crow::response handleMazeState(const crow::request& req, Database& db) {
    return handleGetMazeState(req);
}

crow::response handleMazeReset(const crow::request& req, Database& db) {
    return handleResetMazeState(req);
}

// 迷宫生成动画帧生成函数
json createMazeGenerationFrames(AnimationSession& session, json& currentState) {
    // 重置迷宫状态
    currentState = createInitialMazeState();
    
    // 生成迷宫
    generateMaze(currentState);
    
    // 添加初始帧
    session.addFrame(currentState, "迷宫生成完成");
    
    return currentState;
}

// DFS求解动画帧生成函数
json createMazeDFSFrames(AnimationSession& session, json& currentState) {
    int startX = currentState["startX"];
    int startY = currentState["startY"];
    int endX = currentState["endX"];
    int endY = currentState["endY"];
    
    std::vector<std::vector<bool>> visited(10, std::vector<bool>(10, false));
    std::vector<std::pair<int, int>> path;
    
    // 重置访问状态
    currentState["visited"] = json::array();
    for (int i = 0; i < 10; i++) {
        json row = json::array();
        for (int j = 0; j < 10; j++) {
            row.push_back(false);
        }
        currentState["visited"].push_back(row);
    }
    
    // 开始DFS
    dfsMaze(startX, startY, endX, endY, visited, path, currentState, session);
    
    return currentState;
}

// BFS迷宫求解辅助函数
void bfsMaze(int startX, int startY, int endX, int endY, json& currentState, AnimationSession& session) {
    std::queue<std::pair<int, int>> q;
    std::vector<std::vector<std::pair<int, int>>> parent(10, std::vector<std::pair<int, int>>(10, {-1, -1}));
    std::vector<std::vector<bool>> visited(10, std::vector<bool>(10, false));
    
    q.push({startX, startY});
    visited[startX][startY] = true;
    
    // 方向数组：上、右、下、左
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};
    
    bool found = false;
    
    while (!q.empty() && !found) {
        auto [x, y] = q.front();
        q.pop();
        
        // 更新当前状态
        currentState["currentX"] = x;
        currentState["currentY"] = y;
        currentState["visited"][x][y] = true;
        
        // 添加动画帧
        std::string description = "访问位置 (" + std::to_string(x) + ", " + std::to_string(y) + ")";
        session.addFrame(currentState, description);
        
        if (x == endX && y == endY) {
            found = true;
            break;
        }
        
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10 && !visited[nx][ny] && currentState["grid"][nx][ny] == 0) {
                q.push({nx, ny});
                visited[nx][ny] = true;
                parent[nx][ny] = {x, y};
            }
        }
    }
    
    if (found) {
        // 重建路径
        std::vector<std::pair<int, int>> path;
        int x = endX, y = endY;
        
        while (x != -1 && y != -1) {
            path.push_back({x, y});
            auto [px, py] = parent[x][y];
            x = px;
            y = py;
        }
        
        std::reverse(path.begin(), path.end());
        
        // 更新状态
        currentState["path"] = json::array();
        for (const auto& p : path) {
            currentState["path"].push_back({p.first, p.second});
        }
        currentState["completed"] = true;
        session.addFrame(currentState, "找到路径！");
    }
}

// A*迷宫求解辅助函数
void aStarMaze(int startX, int startY, int endX, int endY, json& currentState, AnimationSession& session) {
    // 优先队列中的元素为：{f值, {x, y}}
    // f = g + h，其中g是从起点到当前点的实际距离，h是当前点到终点的曼哈顿距离
    auto cmp = [](const std::pair<int, std::pair<int, int>>& a, 
                  const std::pair<int, std::pair<int, int>>& b) {
        return a.first > b.first;
    };
    std::priority_queue<std::pair<int, std::pair<int, int>>, 
                       std::vector<std::pair<int, std::pair<int, int>>>,
                       decltype(cmp)> pq(cmp);
    
    std::vector<std::vector<int>> g(10, std::vector<int>(10, INT_MAX));
    std::vector<std::vector<std::pair<int, int>>> parent(10, std::vector<std::pair<int, int>>(10, {-1, -1}));
    std::vector<std::vector<bool>> visited(10, std::vector<bool>(10, false));
    
    // 计算曼哈顿距离
    auto h = [](int x, int y, int ex, int ey) {
        return std::abs(x - ex) + std::abs(y - ey);
    };
    
    g[startX][startY] = 0;
    pq.push({h(startX, startY, endX, endY), {startX, startY}});
    
    // 方向数组：上、右、下、左
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};
    
    bool found = false;
    
    while (!pq.empty() && !found) {
        auto [f, pos] = pq.top();
        auto [x, y] = pos;
        pq.pop();
        
        if (visited[x][y]) continue;
        visited[x][y] = true;
        
        // 更新当前状态
        currentState["currentX"] = x;
        currentState["currentY"] = y;
        currentState["visited"][x][y] = true;
        
        // 添加动画帧
        std::string description = "访问位置 (" + std::to_string(x) + ", " + std::to_string(y) + 
                                "), f = " + std::to_string(f);
        session.addFrame(currentState, description);
        
        if (x == endX && y == endY) {
            found = true;
            break;
        }
        
        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            
            if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10 && !visited[nx][ny] && 
                currentState["grid"][nx][ny] == 0) {
                int ng = g[x][y] + 1;
                if (ng < g[nx][ny]) {
                    g[nx][ny] = ng;
                    parent[nx][ny] = {x, y};
                    pq.push({ng + h(nx, ny, endX, endY), {nx, ny}});
                }
            }
        }
    }
    
    if (found) {
        // 重建路径
        std::vector<std::pair<int, int>> path;
        int x = endX, y = endY;
        
        while (x != -1 && y != -1) {
            path.push_back({x, y});
            auto [px, py] = parent[x][y];
            x = px;
            y = py;
        }
        
        std::reverse(path.begin(), path.end());
        
        // 更新状态
        currentState["path"] = json::array();
        for (const auto& p : path) {
            currentState["path"].push_back({p.first, p.second});
        }
        currentState["completed"] = true;
        session.addFrame(currentState, "找到最短路径！");
    }
}

// 迷宫生成辅助函数
void generateMaze(json& state) {
    std::srand(std::time(nullptr));
    int size = state["mazeSize"];
    
    // 使用随机DFS生成迷宫
    std::vector<std::vector<bool>> visited(size, std::vector<bool>(size, false));
    std::vector<std::pair<int, int>> stack;
    
    // 从起点开始
    int x = state["startX"].get<int>();
    int y = state["startY"].get<int>();
    state["grid"][x][y] = 0;  // 0表示通道
    visited[x][y] = true;
    stack.push_back({x, y});
    
    // 方向数组：上、右、下、左
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};
    
    while (!stack.empty()) {
        auto [cx, cy] = stack.back();
        
        // 查找未访问的相邻单元格
        std::vector<int> directions;
        for (int i = 0; i < 4; i++) {
            int nx = cx + dx[i] * 2;
            int ny = cy + dy[i] * 2;
            
            if (nx >= 0 && nx < size && ny >= 0 && ny < size && !visited[nx][ny]) {
                directions.push_back(i);
            }
        }
        
        if (directions.empty()) {
            stack.pop_back();
        } else {
            // 随机选择一个方向
            int dir = directions[std::rand() % directions.size()];
            int nx = cx + dx[dir] * 2;
            int ny = cy + dy[dir] * 2;
            
            // 打通墙壁
            state["grid"][cx + dx[dir]][cy + dy[dir]] = 0;
            state["grid"][nx][ny] = 0;
            
            visited[nx][ny] = true;
            stack.push_back({nx, ny});
        }
    }
    
    // 确保终点可达
    int endX = state["endX"].get<int>();
    int endY = state["endY"].get<int>();
    state["grid"][endX][endY] = 0;
}

// DFS迷宫求解辅助函数
bool dfsMaze(int x, int y, int endX, int endY, std::vector<std::vector<bool>>& visited,
             std::vector<std::pair<int, int>>& path, json& currentState, AnimationSession& session) {
    if (x < 0 || x >= 10 || y < 0 || y >= 10 || visited[x][y] || currentState["grid"][x][y] == 1) {
        return false;
    }
    
    visited[x][y] = true;
    path.push_back({x, y});
    
    // 更新当前状态
    currentState["currentX"] = x;
    currentState["currentY"] = y;
    currentState["visited"][x][y] = true;
    
    // 添加动画帧
    std::string description = "访问位置 (" + std::to_string(x) + ", " + std::to_string(y) + ")";
    session.addFrame(currentState, description);
    
    if (x == endX && y == endY) {
        currentState["path"] = json::array();
        for (const auto& p : path) {
            currentState["path"].push_back({p.first, p.second});
        }
        currentState["completed"] = true;
        session.addFrame(currentState, "找到路径！");
        return true;
    }
    
    // 方向数组：上、右、下、左
    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};
    
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];
        
        if (dfsMaze(nx, ny, endX, endY, visited, path, currentState, session)) {
            return true;
        }
    }
    
    path.pop_back();
    return false;
} 