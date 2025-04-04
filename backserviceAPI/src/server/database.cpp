#include "database.h"
#include <iostream>

Database::Database(const std::string& path) : dbPath(path) {
    db = nullptr;
    
    // 初始化游戏状态为空
    linkedListState = json::object();
    queueState = json::object();
    stackState = json::object();
    binaryTreeState = json::object();
    graphState = json::object();
    sortingState = json::object();
    searchState = json::object();
    mazeState = json::object();
    
    // 初始化数据库连接
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

bool Database::initialize() {
    // 先删除现有表，强制重建
    executeQuery("DROP TABLE IF EXISTS user_achievements");
    executeQuery("DROP TABLE IF EXISTS achievements");
    executeQuery("DROP TABLE IF EXISTS user_progress");
    executeQuery("DROP TABLE IF EXISTS levels");
    executeQuery("DROP TABLE IF EXISTS games");
    executeQuery("DROP TABLE IF EXISTS users");
    
    // 创建用户表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            email TEXT,
            nickname TEXT,
            avatar TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    )");
    
    // 创建游戏表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS games (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            description TEXT,
            category TEXT,
            difficulty INTEGER,
            image_url TEXT
        )
    )");
    
    // 创建关卡表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS levels (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            game_id INTEGER,
            name TEXT NOT NULL,
            description TEXT,
            difficulty INTEGER,
            content TEXT,
            hint TEXT,
            solution TEXT,
            FOREIGN KEY (game_id) REFERENCES games (id)
        )
    )");
    
    // 创建用户进度表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS user_progress (
            user_id INTEGER,
            game_id INTEGER,
            level_id INTEGER,
            status TEXT,
            solution TEXT,
            completed_at TIMESTAMP,
            PRIMARY KEY (user_id, game_id, level_id),
            FOREIGN KEY (user_id) REFERENCES users (id),
            FOREIGN KEY (game_id) REFERENCES games (id),
            FOREIGN KEY (level_id) REFERENCES levels (id)
        )
    )");
    
    // 创建成就表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS achievements (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            description TEXT,
            criteria TEXT,
            image_url TEXT
        )
    )");
    
    // 创建用户成就表
    executeQuery(R"(
        CREATE TABLE IF NOT EXISTS user_achievements (
            user_id INTEGER,
            achievement_id INTEGER,
            achieved_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (user_id, achievement_id),
            FOREIGN KEY (user_id) REFERENCES users (id),
            FOREIGN KEY (achievement_id) REFERENCES achievements (id)
        )
    )");

    // 创建示例数据
    populateInitialData();
    
    return true;
}

bool Database::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

json Database::executeSelect(const std::string& query) {
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    
    json result = json::array();
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return result;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        json row = json::object();
        
        for (int i = 0; i < sqlite3_column_count(stmt); i++) {
            std::string columnName = sqlite3_column_name(stmt, i);
            
            switch (sqlite3_column_type(stmt, i)) {
                case SQLITE_INTEGER:
                    row[columnName] = sqlite3_column_int(stmt, i);
                    break;
                case SQLITE_FLOAT:
                    row[columnName] = sqlite3_column_double(stmt, i);
                    break;
                case SQLITE_TEXT:
                    row[columnName] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
                    break;
                case SQLITE_NULL:
                    row[columnName] = nullptr;
                    break;
                default:
                    row[columnName] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, i)));
            }
        }
        
        result.push_back(row);
    }
    
    sqlite3_finalize(stmt);
    return result;
}

void Database::populateInitialData() {
    // 添加一些示例游戏
    if (executeSelect("SELECT COUNT(*) as count FROM games").at(0)["count"] == 0) {
        executeQuery(R"(
            INSERT INTO games (name, description, category, difficulty, image_url) VALUES
            ('迷宫寻路', '学习和实践多种寻路算法，包括DFS、BFS和A*算法', '图算法', 2, '/images/maze.png'),
            ('链表操作', '学习单链表、双链表和循环链表的基本操作', '线性数据结构', 1, '/images/linked_list.png'),
            ('栈与队列', '掌握栈和队列的基本操作和应用场景', '线性数据结构', 1, '/images/stack_queue.png'),
            ('二叉树遍历', '学习前序、中序、后序和层序遍历算法', '树结构', 3, '/images/binary_tree.png'),
            ('图遍历算法', '掌握图的DFS和BFS遍历以及最短路径算法', '图算法', 4, '/images/graph.png'),
            ('查找算法', '学习二分查找、哈希查找等高效查找算法', '查找排序', 2, '/images/search.png'),
            ('排序算法', '掌握冒泡、插入、快速和归并等排序算法', '查找排序', 3, '/images/sort.png')
        )");
    }
    
    // 添加一些示例关卡
    if (executeSelect("SELECT COUNT(*) as count FROM levels").at(0)["count"] == 0) {
        // 迷宫寻路关卡
        executeQuery(R"(
            INSERT INTO levels (game_id, name, description, difficulty, content, hint, solution) VALUES
            (1, '简单迷宫-DFS', '使用深度优先搜索解决简单迷宫问题', 1, 
             '{"maze":[[0,1,0,0,0],[0,1,0,1,0],[0,0,0,1,0],[1,1,0,1,0],[0,0,0,0,0]],"start":[0,0],"end":[4,4]}', 
             '思考如何使用递归方式实现DFS', 
             'function dfs(maze, current, end, visited) {\n  if (current[0] === end[0] && current[1] === end[1]) return true;\n  visited[current[0]][current[1]] = true;\n  // 四个方向：上、右、下、左\n  const directions = [[-1,0], [0,1], [1,0], [0,-1]];\n  for (const [dx, dy] of directions) {\n    const nx = current[0] + dx;\n    const ny = current[1] + dy;\n    if (isValid(maze, nx, ny) && !visited[nx][ny]) {\n      if (dfs(maze, [nx, ny], end, visited)) return true;\n    }\n  }\n  return false;\n}'),
            
            (1, '中等迷宫-BFS', '使用广度优先搜索解决中等迷宫问题', 2, 
             '{"maze":[[0,1,0,0,0],[0,1,0,1,0],[0,0,0,1,0],[1,1,0,1,0],[0,1,0,0,0]],"start":[0,0],"end":[4,4]}', 
             '考虑使用队列实现BFS', 
             'function bfs(maze, start, end) {\n  const queue = [start];\n  const visited = Array(maze.length).fill().map(() => Array(maze[0].length).fill(false));\n  visited[start[0]][start[1]] = true;\n  const directions = [[-1,0], [0,1], [1,0], [0,-1]];\n  while (queue.length > 0) {\n    const current = queue.shift();\n    if (current[0] === end[0] && current[1] === end[1]) return true;\n    for (const [dx, dy] of directions) {\n      const nx = current[0] + dx;\n      const ny = current[1] + dy;\n      if (isValid(maze, nx, ny) && !visited[nx][ny]) {\n        queue.push([nx, ny]);\n        visited[nx][ny] = true;\n      }\n    }\n  }\n  return false;\n}'),
            
            (1, '复杂迷宫-A*', '使用A*算法解决复杂迷宫问题', 3, 
             '{"maze":[[0,1,0,0,0],[0,1,0,1,0],[0,0,1,1,0],[1,1,0,1,0],[0,1,0,0,0]],"start":[0,0],"end":[4,4]}', 
             'A*算法结合了BFS的完备性和贪心的效率，需要设计好启发函数', 
             'function aStar(maze, start, end) {\n  const openSet = [start];\n  const closedSet = [];\n  const gScore = {}; // 从起点到当前点的实际代价\n  const fScore = {}; // 从起点经由当前点到终点的估计总代价\n  const cameFrom = {};\n  gScore[start] = 0;\n  fScore[start] = heuristic(start, end);\n  // 实现省略...\n}')
        )");
        
        // 链表操作关卡
        executeQuery(R"(
            INSERT INTO levels (game_id, name, description, difficulty, content, hint, solution) VALUES
            (2, '单链表基本操作', '实现单链表的创建、插入、删除和查找', 1, 
             '{"operations":["create","insert","delete","find"]}', 
             '链表节点需要包含数据和指向下一个节点的指针', 
             'class ListNode {\n  constructor(val) {\n    this.val = val;\n    this.next = null;\n  }\n}\n\nclass LinkedList {\n  constructor() {\n    this.head = null;\n  }\n  \n  insert(val, position) {\n    const newNode = new ListNode(val);\n    if (position === 0) {\n      newNode.next = this.head;\n      this.head = newNode;\n      return;\n    }\n    // 实现省略...\n  }\n}'),
            
            (2, '双链表实现', '实现双向链表的基本操作', 2, 
             '{"operations":["create","insertBefore","insertAfter","delete"]}', 
             '双链表的节点需要同时包含前向和后向指针', 
             'class DoublyListNode {\n  constructor(val) {\n    this.val = val;\n    this.prev = null;\n    this.next = null;\n  }\n}\n\nclass DoublyLinkedList {\n  constructor() {\n    this.head = null;\n    this.tail = null;\n  }\n  \n  // 实现省略...\n}'),
            
            (2, '循环链表实现', '实现循环链表并解决约瑟夫问题', 3, 
             '{"problem":"josephus","n":7,"k":3}', 
             '循环链表的尾节点指向头节点，形成一个环', 
             'function solveJosephus(n, k) {\n  // 创建循环链表\n  let head = new ListNode(1);\n  let current = head;\n  for (let i = 2; i <= n; i++) {\n    current.next = new ListNode(i);\n    current = current.next;\n  }\n  current.next = head; // 形成环\n  \n  // 约瑟夫问题求解\n  let prev = current;\n  current = head;\n  while (current.next !== current) {\n    for (let i = 1; i < k; i++) {\n      prev = current;\n      current = current.next;\n    }\n    prev.next = current.next;\n    current = prev.next;\n  }\n  return current.val;\n}')
        )");
        
        // 继续添加其他类型的关卡...
    }
    
    // 添加一些示例成就
    if (executeSelect("SELECT COUNT(*) as count FROM achievements").at(0)["count"] == 0) {
        executeQuery(R"(
            INSERT INTO achievements (name, description, criteria, image_url) VALUES
            ('算法新手', '完成你的第一个算法挑战', '完成任意一个关卡', '/images/achievement/novice.png'),
            ('迷宫大师', '完成所有迷宫寻路关卡', '完成所有迷宫寻路游戏的关卡', '/images/achievement/maze_master.png'),
            ('链表专家', '掌握链表的所有操作', '完成所有链表操作关卡', '/images/achievement/linked_list_expert.png'),
            ('排序王者', '熟练掌握多种排序算法', '完成所有排序算法关卡', '/images/achievement/sort_king.png'),
            ('全能算法师', '完成所有算法挑战', '完成所有游戏的所有关卡', '/images/achievement/algorithm_master.png')
        )");
    }
}

// 用户相关操作实现
bool Database::registerUser(const std::string& username, const std::string& password, const std::string& email) {
    // 检查用户名是否已存在
    json result = executeSelect("SELECT COUNT(*) as count FROM users WHERE username = '" + username + "'");
    if (result.at(0)["count"] > 0) {
        return false;
    }
    
    // TODO: 在实际应用中应该对密码进行哈希处理
    std::string query = "INSERT INTO users (username, password, email) VALUES ('" + 
                         username + "', '" + password + "', '" + email + "')";
    return executeQuery(query);
}

bool Database::authenticateUser(const std::string& username, const std::string& password) {
    std::string query = "SELECT COUNT(*) as count FROM users WHERE username = '" + 
                       username + "' AND password = '" + password + "'";
    json result = executeSelect(query);
    return result.at(0)["count"] > 0;
}

json Database::getUserProfile(int userId) {
    std::string query = "SELECT id, username, email, created_at FROM users WHERE id = " + std::to_string(userId);
    json result = executeSelect(query);
    return result.size() > 0 ? result.at(0) : json::object();
}

bool Database::updateUserProfile(int userId, const json& profileData) {
    // 构建更新查询
    std::string setClause;
    if (profileData.contains("email")) {
        setClause += "email = '" + profileData["email"].get<std::string>() + "'";
    }
    
    if (profileData.contains("password")) {
        if (!setClause.empty()) setClause += ", ";
        // TODO: 实际应用中需要对密码进行哈希处理
        setClause += "password = '" + profileData["password"].get<std::string>() + "'";
    }
    
    // 添加对nickname字段的支持
    if (profileData.contains("nickname")) {
        if (!setClause.empty()) setClause += ", ";
        setClause += "nickname = '" + profileData["nickname"].get<std::string>() + "'";
    }
    
    // 添加对avatar字段的支持
    if (profileData.contains("avatar")) {
        if (!setClause.empty()) setClause += ", ";
        setClause += "avatar = '" + profileData["avatar"].get<std::string>() + "'";
    }
    
    // 添加对name字段的支持
    if (profileData.contains("name")) {
        if (!setClause.empty()) setClause += ", ";
        setClause += "name = '" + profileData["name"].get<std::string>() + "'";
    }
    
    if (setClause.empty()) return false;
    
    std::string query = "UPDATE users SET " + setClause + " WHERE id = " + std::to_string(userId);
    return executeQuery(query);
}

// 游戏相关操作实现
json Database::getAllGames() {
    return executeSelect("SELECT * FROM games");
}

json Database::getGameDetails(int gameId) {
    std::string query = "SELECT * FROM games WHERE id = " + std::to_string(gameId);
    json result = executeSelect(query);
    return result.size() > 0 ? result.at(0) : json::object();
}

json Database::getGameLevels(int gameId) {
    std::string query = "SELECT id, name, description, difficulty FROM levels WHERE game_id = " + std::to_string(gameId);
    return executeSelect(query);
}

json Database::getLevelDetails(int gameId, int levelId) {
    std::string query = "SELECT * FROM levels WHERE game_id = " + std::to_string(gameId) + 
                         " AND id = " + std::to_string(levelId);
    json result = executeSelect(query);
    if (result.size() > 0) {
        // 不返回解决方案，只返回关卡内容和提示
        json levelDetails = result.at(0);
        levelDetails.erase("solution");
        return levelDetails;
    }
    return json::object();
}

// 游戏进度相关操作
bool Database::startLevel(int userId, int gameId, int levelId) {
    // 检查关卡是否存在
    json level = executeSelect("SELECT id FROM levels WHERE game_id = " + std::to_string(gameId) + 
                              " AND id = " + std::to_string(levelId));
    if (level.size() == 0) return false;
    
    // 开始新的尝试或更新现有尝试
    std::string query = "INSERT OR REPLACE INTO user_progress (user_id, game_id, level_id, status) VALUES (" +
                         std::to_string(userId) + ", " + std::to_string(gameId) + ", " + 
                         std::to_string(levelId) + ", 'in_progress')";
    return executeQuery(query);
}

bool Database::submitSolution(int userId, int gameId, int levelId, const std::string& solution) {
    // 获取关卡的正确解答
    std::string query = "SELECT solution FROM levels WHERE game_id = " + std::to_string(gameId) + 
                         " AND id = " + std::to_string(levelId);
    json result = executeSelect(query);
    if (result.size() == 0) return false;
    
    // 在实际应用中，这里应该进行解答的实际验证，而不仅仅是字符串比较
    // 这里简化处理，假设解答正确
    bool isCorrect = true; // TODO: 实现实际的解答验证逻辑
    
    // 更新用户进度
    std::string status = isCorrect ? "completed" : "attempted";
    query = "UPDATE user_progress SET status = '" + status + "', solution = '" + solution + "', " +
            "completed_at = CURRENT_TIMESTAMP WHERE user_id = " + std::to_string(userId) + 
            " AND game_id = " + std::to_string(gameId) + " AND level_id = " + std::to_string(levelId);
    
    if (isCorrect) {
        // 检查是否满足任何成就条件
        checkAchievements(userId);
    }
    
    return executeQuery(query);
}

json Database::getLevelHint(int gameId, int levelId) {
    std::string query = "SELECT hint FROM levels WHERE game_id = " + std::to_string(gameId) + 
                         " AND id = " + std::to_string(levelId);
    json result = executeSelect(query);
    if (result.size() > 0) {
        return json({{"hint", result.at(0)["hint"]}});
    }
    return json({{"hint", ""}});
}

json Database::getLevelSolution(int gameId, int levelId) {
    std::string query = "SELECT solution FROM levels WHERE game_id = " + std::to_string(gameId) + 
                         " AND id = " + std::to_string(levelId);
    json result = executeSelect(query);
    if (result.size() > 0) {
        return json({{"solution", result.at(0)["solution"]}});
    }
    return json({{"solution", ""}});
}

// 成就相关操作
json Database::getAllAchievements() {
    return executeSelect("SELECT * FROM achievements");
}

json Database::getUserAchievements(int userId) {
    std::string query = "SELECT a.* FROM achievements a JOIN user_achievements ua "
                        "ON a.id = ua.achievement_id WHERE ua.user_id = " + std::to_string(userId);
    return executeSelect(query);
}

json Database::getUserProgress(int userId) {
    std::string query = "SELECT g.id as game_id, g.name as game_name, "
                        "COUNT(l.id) as total_levels, "
                        "COUNT(up.level_id) as completed_levels "
                        "FROM games g "
                        "JOIN levels l ON g.id = l.game_id "
                        "LEFT JOIN user_progress up ON l.id = up.level_id AND up.user_id = " + std::to_string(userId) + 
                        " AND up.status = 'completed' "
                        "GROUP BY g.id";
    return executeSelect(query);
}

bool Database::syncUserProgress(int userId, const json& progressData) {
    // 检查是否是统一的单个进度对象格式
    if ((progressData.contains("gameId") || progressData.contains("game_id")) && 
        (progressData.contains("levelId") || progressData.contains("level_id"))) {
        
        // 获取游戏ID，支持不同的键名
        int gameId;
        if (progressData.contains("gameId")) {
            gameId = progressData["gameId"].is_number() ? 
                     progressData["gameId"].get<int>() : 
                     std::stoi(progressData["gameId"].get<std::string>());
        } else {
            gameId = progressData["game_id"].is_number() ? 
                     progressData["game_id"].get<int>() : 
                     std::stoi(progressData["game_id"].get<std::string>());
        }
        
        // 获取关卡ID，支持不同的键名
        int levelId;
        if (progressData.contains("levelId")) {
            levelId = progressData["levelId"].is_number() ? 
                      progressData["levelId"].get<int>() : 
                      std::stoi(progressData["levelId"].get<std::string>());
        } else {
            levelId = progressData["level_id"].is_number() ? 
                      progressData["level_id"].get<int>() : 
                      std::stoi(progressData["level_id"].get<std::string>());
        }
        
        // 处理completed或status字段
        std::string status = "in_progress";
        if (progressData.contains("completed")) {
            if (progressData["completed"].is_boolean() && progressData["completed"].get<bool>()) {
                status = "completed";
            } else if (progressData["completed"].is_string() && 
                      (progressData["completed"].get<std::string>() == "true" || 
                       progressData["completed"].get<std::string>() == "completed")) {
                status = "completed";
            }
        } else if (progressData.contains("status")) {
            status = progressData["status"].get<std::string>();
        }
        
        // 构建基本查询
        std::string query = "INSERT OR REPLACE INTO user_progress (user_id, game_id, level_id, status";
        std::string values = std::to_string(userId) + ", " + std::to_string(gameId) + ", " + 
                            std::to_string(levelId) + ", '" + status + "'";
        
        // 如果有分数，也保存
        if (progressData.contains("score")) {
            query += ", score";
            int score = progressData["score"].is_number() ? 
                       progressData["score"].get<int>() : 
                       std::stoi(progressData["score"].get<std::string>());
            values += ", " + std::to_string(score);
        }
        
        // 完成查询
        query += ") VALUES (" + values + ")";
        return executeQuery(query);
    }
    
    // 如果不是单个进度对象，尝试数组格式
    if (progressData.is_array()) {
        // 直接处理数组格式
        bool success = true;
        for (const auto& progress : progressData) {
            if ((progress.contains("gameId") || progress.contains("game_id")) && 
                (progress.contains("levelId") || progress.contains("level_id"))) {
                
                json singleProgress = progress;
                success = success && syncUserProgress(userId, singleProgress);
            }
        }
        return success;
    }
    
    // 处理包含progress键的对象格式
    if (progressData.is_object() && progressData.contains("progress")) {
        // 如果progress是数组，递归处理每个元素
        if (progressData["progress"].is_array()) {
            bool success = true;
            for (const auto& progress : progressData["progress"]) {
                json singleProgress = progress;
                success = success && syncUserProgress(userId, singleProgress);
            }
            return success;
        }
        // 如果progress是单个对象，递归处理该对象
        else if (progressData["progress"].is_object()) {
            return syncUserProgress(userId, progressData["progress"]);
        }
    }
    
    // 没有找到可处理的格式
    return false;
}

void Database::checkAchievements(int userId) {
    // 检查"算法新手"成就
    json completed = executeSelect("SELECT COUNT(*) as count FROM user_progress WHERE user_id = " + 
                                  std::to_string(userId) + " AND status = 'completed'");
    if (completed.at(0)["count"] > 0) {
        unlockAchievement(userId, 1); // 算法新手
    }
    
    // 检查"迷宫大师"成就
    json mazeLevels = executeSelect("SELECT COUNT(*) as total FROM levels WHERE game_id = 1");
    json completedMazeLevels = executeSelect("SELECT COUNT(*) as completed FROM user_progress WHERE user_id = " + 
                                            std::to_string(userId) + " AND game_id = 1 AND status = 'completed'");
    if (mazeLevels.at(0)["total"] > 0 && mazeLevels.at(0)["total"] == completedMazeLevels.at(0)["completed"]) {
        unlockAchievement(userId, 2); // 迷宫大师
    }
    
    // 类似地检查其他成就...
    
    // 检查"全能算法师"成就
    json allLevels = executeSelect("SELECT COUNT(*) as total FROM levels");
    json allCompleted = executeSelect("SELECT COUNT(*) as completed FROM user_progress WHERE user_id = " + 
                                     std::to_string(userId) + " AND status = 'completed'");
    if (allLevels.at(0)["total"] > 0 && allLevels.at(0)["total"] == allCompleted.at(0)["completed"]) {
        unlockAchievement(userId, 5); // 全能算法师
    }
}

bool Database::unlockAchievement(int userId, int achievementId) {
    // 检查是否已经获得该成就
    json existing = executeSelect("SELECT COUNT(*) as count FROM user_achievements WHERE user_id = " + 
                                 std::to_string(userId) + " AND achievement_id = " + std::to_string(achievementId));
    if (existing.at(0)["count"] > 0) {
        return true; // 已经获得过该成就
    }
    
    // 解锁新成就
    std::string query = "INSERT INTO user_achievements (user_id, achievement_id) VALUES (" +
                        std::to_string(userId) + ", " + std::to_string(achievementId) + ")";
    return executeQuery(query);
}

// 通用的游戏状态管理方法
json Database::getGameState(const std::string& gameType) {
    if (gameType == "linkedlist") {
        return getLinkedListState();
    } else if (gameType == "queue") {
        return getQueueState();
    } else if (gameType == "stack") {
        return getStackState();
    } else if (gameType == "binarytree") {
        return getBinaryTreeState();
    } else if (gameType == "graph") {
        return getGraphState();
    } else if (gameType == "sorting") {
        return getSortingState();
    } else if (gameType == "search") {
        return getSearchState();
    } else if (gameType == "maze") {
        return getMazeState();
    } else {
        // 对于未知的游戏类型，返回空对象
        return json::object();
    }
}

bool Database::updateGameState(const std::string& gameType, const json& state) {
    if (gameType == "linkedlist") {
        return updateLinkedListState(state);
    } else if (gameType == "queue") {
        return updateQueueState(state);
    } else if (gameType == "stack") {
        return updateStackState(state);
    } else if (gameType == "binarytree") {
        return updateBinaryTreeState(state);
    } else if (gameType == "graph") {
        return updateGraphState(state);
    } else if (gameType == "sorting") {
        return updateSortingState(state);
    } else if (gameType == "search") {
        return updateSearchState(state);
    } else if (gameType == "maze") {
        return updateMazeState(state);
    } else {
        // 对于未知的游戏类型，返回失败
        return false;
    }
}

// 链表游戏状态
json Database::getLinkedListState() {
    return linkedListState;
}

bool Database::updateLinkedListState(const json& state) {
    linkedListState = state;
    return true;
}

// 队列游戏状态
json Database::getQueueState() {
    return queueState;
}

bool Database::updateQueueState(const json& state) {
    queueState = state;
    return true;
}

// 栈游戏状态
json Database::getStackState() {
    return stackState;
}

bool Database::updateStackState(const json& state) {
    stackState = state;
    return true;
}

// 二叉树游戏状态
json Database::getBinaryTreeState() {
    return binaryTreeState;
}

bool Database::updateBinaryTreeState(const json& state) {
    binaryTreeState = state;
    return true;
}

// 图游戏状态
json Database::getGraphState() {
    return graphState;
}

bool Database::updateGraphState(const json& state) {
    graphState = state;
    return true;
}

// 排序游戏状态
json Database::getSortingState() {
    return sortingState;
}

bool Database::updateSortingState(const json& state) {
    sortingState = state;
    return true;
}

// 搜索游戏状态
json Database::getSearchState() {
    return searchState;
}

bool Database::updateSearchState(const json& state) {
    searchState = state;
    return true;
}

// 迷宫游戏状态
json Database::getMazeState() {
    return mazeState;
}

bool Database::updateMazeState(const json& state) {
    mazeState = state;
    return true;
} 