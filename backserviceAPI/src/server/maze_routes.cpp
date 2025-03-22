#include "maze_routes.h"
#include "game_animation_routes.h"
#include "response_builder.h"
#include <queue>
#include <nlohmann/json.hpp>
#include <random>
#include <algorithm>
#include <functional>
#include <climits>
#include <cmath>

using json = nlohmann::json;

// 迷宫游戏操作 - 获取当前状态
crow::response handleMazeState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("maze");
        if (state.empty()) {
            // 如果状态为空，初始化一个默认状态
            state = {
                {"maze", json::array()},  // 空的迷宫网格
                {"start", nullptr},       // 起点坐标
                {"end", nullptr},         // 终点坐标
                {"path", nullptr},        // 当前路径
                {"visited", nullptr}      // 已访问的单元格
            };
            db.updateGameState("maze", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "maze").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取迷宫状态失败: ") + e.what(), "maze").dump());
    }
}

// 迷宫游戏操作 - 重置状态
crow::response handleMazeReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的迷宫状态
        json initialState = json::object();
        initialState["maze"] = json::array();
        initialState["start"] = nullptr;
        initialState["end"] = nullptr;
        initialState["path"] = json::array();
        initialState["visited"] = json::array();
        
        // 更新数据库中的状态
        db.updateGameState("maze", initialState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(initialState, "maze").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置迷宫状态失败: ") + e.what(), "maze").dump());
    }
}

// 迷宫游戏操作 - 生成迷宫
crow::response handleMazeGenerate(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取迷宫大小参数
        int width = 10;  // 默认宽度
        int height = 10; // 默认高度
        
        auto widthParam = req.url_params.get("width");
        auto heightParam = req.url_params.get("height");
        
        if (widthParam) width = std::stoi(widthParam);
        if (heightParam) height = std::stoi(heightParam);
        
        // 限制迷宫大小
        width = std::max(5, std::min(20, width));
        height = std::max(5, std::min(20, height));
        
        // 初始化迷宫网格
        std::vector<std::vector<int>> maze(height, std::vector<int>(width, 1)); // 1表示墙
        
        // 使用深度优先搜索生成迷宫
        std::function<void(int, int)> generateMaze = [&](int row, int col) {
            maze[row][col] = 0; // 0表示通道
            
            // 定义四个方向：上、右、下、左
            std::vector<std::pair<int, int>> directions = {
                {-2, 0}, {0, 2}, {2, 0}, {0, -2}
            };
            
            // 随机打乱方向
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(directions.begin(), directions.end(), gen);
            
            // 向四个方向尝试扩展
            for (const auto& [dy, dx] : directions) {
                int newRow = row + dy;
                int newCol = col + dx;
                
                if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width && maze[newRow][newCol] == 1) {
                    // 打通中间的墙
                    maze[row + dy/2][col + dx/2] = 0;
                    generateMaze(newRow, newCol);
                }
            }
        };
        
        // 从左上角开始生成迷宫
        generateMaze(1, 1);
        
        // 设置起点和终点
        std::pair<int, int> start = {1, 1};
        std::pair<int, int> end = {height-2, width-2};
        maze[start.first][start.second] = 0;
        maze[end.first][end.second] = 0;
        
        // 转换为JSON格式
        json mazeJson = json::array();
        for (const auto& row : maze) {
            mazeJson.push_back(row);
        }
        
        // 初始化状态
        json initialState = json::object();
        initialState["maze"] = mazeJson;
        initialState["start"] = {start.first, start.second};
        initialState["end"] = {end.first, end.second};
        initialState["visited"] = json::array();
        initialState["path"] = json::array();
        
        // 创建新的迷宫状态
        json newState = json::object();
        newState["maze"] = mazeJson;
        newState["start"] = {start.first, start.second};
        newState["end"] = {end.first, end.second};
        newState["visited"] = json::array();
        newState["path"] = json::array();
        
        // 更新数据库中的状态
        db.updateGameState("maze", newState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "maze").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("生成迷宫失败: ") + e.what(), "maze").dump());
    }
}

// 迷宫游戏操作 - DFS求解
crow::response handleMazeSolveDFS(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前迷宫状态
        json currentState = db.getGameState("maze");
        if (!currentState.contains("maze") || !currentState.contains("start") || !currentState.contains("end")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "迷宫状态无效，请先生成迷宫", "maze").dump());
        }
        
        // 获取迷宫数据
        std::vector<std::vector<int>> maze = currentState["maze"].get<std::vector<std::vector<int>>>();
        std::vector<int> start = currentState["start"].get<std::vector<int>>();
        std::vector<int> end = currentState["end"].get<std::vector<int>>();
        
        int height = maze.size();
        int width = maze[0].size();
        
        // 初始化访问数组和前驱数组
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::vector<std::vector<std::pair<int, int>>> parent(height, std::vector<std::pair<int, int>>(width, std::make_pair(-1, -1)));
        std::vector<std::pair<int, int>> path;
        bool found = false;
        
        // DFS函数
        std::function<bool(int, int)> dfs = [&](int row, int col) {
            if (row == end[0] && col == end[1]) {
                path.push_back(std::make_pair(row, col));
                found = true;
                return true;
            }
            
            visited[row][col] = true;
            path.push_back(std::make_pair(row, col));
            
            // 四个方向：上、右、下、左
            std::vector<std::pair<int, int>> directions = {
                {-1, 0}, {0, 1}, {1, 0}, {0, -1}
            };
            
            for (const auto& [dy, dx] : directions) {
                int newRow = row + dy;
                int newCol = col + dx;
                
                if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width && 
                    maze[newRow][newCol] == 0 && !visited[newRow][newCol]) {
                    // 记录前驱节点
                    parent[newRow][newCol] = std::make_pair(row, col);
                    if (dfs(newRow, newCol)) {
                        return true;
                    }
                }
            }
            
            path.pop_back();
            return false;
        };
        
        // 从起点开始DFS
        dfs(start[0], start[1]);
        
        // 构建路径
        std::vector<std::pair<int, int>> finalPath;
        if (found) {
            // 回溯路径
            int row = end[0], col = end[1];
            finalPath.push_back(std::make_pair(row, col));
            
            while (parent[row][col].first != -1) {
                int newRow = parent[row][col].first;
                int newCol = parent[row][col].second;
                finalPath.push_back(std::make_pair(newRow, newCol));
                row = newRow;
                col = newCol;
            }
            std::reverse(finalPath.begin(), finalPath.end());
        }
        
        // 转换路径为JSON
        json pathJson = json::array();
        for (const auto& coord : finalPath) {
            json coordPair = json::array();
            coordPair.push_back(coord.first);
            coordPair.push_back(coord.second);
            pathJson.push_back(coordPair);
        }
        
        // 转换访问数组为JSON格式
        json visitedJson = json::array();
        for (int i = 0; i < height; i++) {
            json row = json::array();
            for (int j = 0; j < width; j++) {
                row.push_back(static_cast<bool>(visited[i][j]));
            }
            visitedJson.push_back(row);
        }
        
        // 生成新的状态
        json newState = json::object();
        newState["maze"] = maze;
        newState["start"] = start;
        newState["end"] = end;
        newState["visited"] = visitedJson;
        newState["path"] = pathJson;
        newState["algorithm"] = "dfs";
        newState["found"] = found;
        
        // 更新数据库中的状态
        db.updateGameState("maze", newState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "maze").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("DFS求解迷宫失败: ") + e.what(), "maze").dump());
    }
}

// 迷宫游戏操作 - BFS求解
crow::response handleMazeSolveBFS(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前迷宫状态
        json currentState = db.getGameState("maze");
        if (!currentState.contains("maze") || !currentState.contains("start") || !currentState.contains("end")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "迷宫状态无效，请先生成迷宫", "maze").dump());
        }
        
        // 获取迷宫数据
        std::vector<std::vector<int>> maze = currentState["maze"].get<std::vector<std::vector<int>>>();
        std::vector<int> start = currentState["start"].get<std::vector<int>>();
        std::vector<int> end = currentState["end"].get<std::vector<int>>();
        
        int height = maze.size();
        int width = maze[0].size();
        
        // 初始化访问数组和前驱数组
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::vector<std::vector<std::pair<int, int>>> parent(height, std::vector<std::pair<int, int>>(width, std::make_pair(-1, -1)));
        std::vector<std::pair<int, int>> path;
        
        // BFS队列
        std::queue<std::pair<int, int>> q;
        q.push({start[0], start[1]});
        visited[start[0]][start[1]] = true;
        bool found = false;
        
        // 四个方向：上、右、下、左
        std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {0, 1}, {1, 0}, {0, -1}
        };
        
        // BFS搜索
        while (!q.empty() && !found) {
            auto [row, col] = q.front();
            q.pop();
            
            if (row == end[0] && col == end[1]) {
                found = true;
                break;
            }
            
            for (const auto& [dy, dx] : directions) {
                int newRow = row + dy;
                int newCol = col + dx;
                
                if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width && 
                    maze[newRow][newCol] == 0 && !visited[newRow][newCol]) {
                    visited[newRow][newCol] = true;
                    parent[newRow][newCol] = std::make_pair(row, col);
                    q.push({newRow, newCol});
                }
            }
        }
        
        // 构建路径
        std::vector<std::pair<int, int>> finalPath;
        if (found) {
            // 回溯路径
            int row = end[0], col = end[1];
            finalPath.push_back(std::make_pair(row, col));
            
            while (parent[row][col].first != -1) {
                int newRow = parent[row][col].first;
                int newCol = parent[row][col].second;
                finalPath.push_back(std::make_pair(newRow, newCol));
                row = newRow;
                col = newCol;
            }
            std::reverse(finalPath.begin(), finalPath.end());
        }
        
        // 转换路径为JSON
        json pathJson = json::array();
        for (const auto& coord : finalPath) {
            json coordPair = json::array();
            coordPair.push_back(coord.first);
            coordPair.push_back(coord.second);
            pathJson.push_back(coordPair);
        }
        
        // 转换访问数组为JSON格式
        json visitedJson = json::array();
        for (int i = 0; i < height; i++) {
            json row = json::array();
            for (int j = 0; j < width; j++) {
                row.push_back(static_cast<bool>(visited[i][j]));
            }
            visitedJson.push_back(row);
        }
        
        // 生成新的状态
        json newState = json::object();
        newState["maze"] = maze;
        newState["start"] = start;
        newState["end"] = end;
        newState["visited"] = visitedJson;
        newState["path"] = pathJson;
        newState["algorithm"] = "bfs";
        newState["found"] = found;
        
        // 更新数据库中的状态
        db.updateGameState("maze", newState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "maze").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("BFS求解迷宫失败: ") + e.what(), "maze").dump());
    }
}

// 迷宫游戏操作 - A*求解
crow::response handleMazeSolveAStar(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前迷宫状态
        json currentState = db.getGameState("maze");
        if (!currentState.contains("maze") || !currentState.contains("start") || !currentState.contains("end")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "迷宫状态无效，请先生成迷宫", "maze").dump());
        }
        
        // 获取迷宫数据
        std::vector<std::vector<int>> maze = currentState["maze"].get<std::vector<std::vector<int>>>();
        std::vector<int> start = currentState["start"].get<std::vector<int>>();
        std::vector<int> end = currentState["end"].get<std::vector<int>>();
        
        int height = maze.size();
        int width = maze[0].size();
        
        // 启发式函数：曼哈顿距离
        auto heuristic = [](int row1, int col1, int row2, int col2) {
            return std::abs(row1 - row2) + std::abs(col1 - col2);
        };
        
        // 定义节点结构
        struct Node {
            int row, col;
            int g, h;
            std::pair<int, int> parent;
            
            Node(int r, int c, int g_, int h_, std::pair<int, int> p)
                : row(r), col(c), g(g_), h(h_), parent(std::move(p)) {}
            
            Node(int r, int c, int g_, int h_, int pr, int pc)
                : row(r), col(c), g(g_), h(h_), parent(std::make_pair(pr, pc)) {}
            
            int f() const { return g + h; }
            
            bool operator>(const Node& other) const {
                return f() > other.f();
            }
        };
        
        // 优先队列，存储待访问的节点
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
        
        // 访问数组和代价数组
        std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
        std::vector<std::vector<int>> gScore(height, std::vector<int>(width, INT_MAX));
        std::vector<std::vector<std::pair<int, int>>> parent(height, std::vector<std::pair<int, int>>(width, std::make_pair(-1, -1)));
        std::vector<std::pair<int, int>> path;
        
        // 初始化起点
        openSet.push(Node(start[0], start[1], 0, 
                         heuristic(start[0], start[1], end[0], end[1]),
                         {-1, -1}));
        gScore[start[0]][start[1]] = 0;
        
        bool found = false;
        
        // 四个方向：上、右、下、左
        std::vector<std::pair<int, int>> directions = {
            {-1, 0}, {0, 1}, {1, 0}, {0, -1}
        };
        
        // A*搜索
        while (!openSet.empty() && !found) {
            Node current = openSet.top();
            openSet.pop();
            
            int row = current.row;
            int col = current.col;
            
            if (visited[row][col]) continue;
            visited[row][col] = true;
            
            if (row == end[0] && col == end[1]) {
                found = true;
                parent[row][col] = current.parent;
                break;
            }
            
            for (const auto& [dy, dx] : directions) {
                int newRow = row + dy;
                int newCol = col + dx;
                
                if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width && 
                    maze[newRow][newCol] == 0 && !visited[newRow][newCol]) {
                    
                    int newG = gScore[row][col] + 1;
                    
                    if (newG < gScore[newRow][newCol]) {
                        gScore[newRow][newCol] = newG;
                        parent[newRow][newCol] = std::make_pair(current.row, current.col);
                        
                        openSet.push(Node(newRow, newCol, newG,
                                       heuristic(newRow, newCol, end[0], end[1]),
                                       std::make_pair(current.row, current.col)));
                    }
                }
            }
        }
        
        // 构建路径
        std::vector<std::pair<int, int>> finalPath;
        if (found) {
            // 回溯路径
            int row = end[0], col = end[1];
            finalPath.push_back(std::make_pair(row, col));
            
            while (parent[row][col].first != -1) {
                int newRow = parent[row][col].first;
                int newCol = parent[row][col].second;
                finalPath.push_back(std::make_pair(newRow, newCol));
                row = newRow;
                col = newCol;
            }
            std::reverse(finalPath.begin(), finalPath.end());
        }
        
        // 转换路径为JSON
        json pathJson = json::array();
        for (const auto& coord : finalPath) {
            json coordPair = json::array();
            coordPair.push_back(coord.first);
            coordPair.push_back(coord.second);
            pathJson.push_back(coordPair);
        }
        
        // 转换访问数组为JSON格式
        json visitedJson = json::array();
        for (int i = 0; i < height; i++) {
            json row = json::array();
            for (int j = 0; j < width; j++) {
                row.push_back(static_cast<bool>(visited[i][j]));
            }
            visitedJson.push_back(row);
        }
        
        // 生成新的状态
        json newState = json::object();
        newState["maze"] = maze;
        newState["start"] = start;
        newState["end"] = end;
        newState["visited"] = visitedJson;
        newState["path"] = pathJson;
        newState["algorithm"] = "astar";
        newState["found"] = found;
        
        // 更新数据库中的状态
        db.updateGameState("maze", newState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "maze").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("A*求解迷宫失败: ") + e.what(), "maze").dump());
    }
} 