#pragma once

#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Database {
public:
    Database(const std::string& dbPath);
    ~Database();
    
    // 初始化数据库（创建表结构）
    bool initialize();
    
    // 用户相关操作
    bool registerUser(const std::string& username, const std::string& password, const std::string& email);
    bool authenticateUser(const std::string& username, const std::string& password);
    json getUserProfile(int userId);
    bool updateUserProfile(int userId, const json& profileData);
    
    // 游戏相关操作
    json getAllGames();
    json getGameDetails(int gameId);
    json getGameLevels(int gameId);
    json getLevelDetails(int gameId, int levelId);
    
    // 游戏进度相关操作
    bool startLevel(int userId, int gameId, int levelId);
    bool submitSolution(int userId, int gameId, int levelId, const std::string& solution);
    json getLevelHint(int gameId, int levelId);
    json getLevelSolution(int gameId, int levelId);
    
    // 成就相关操作
    json getAllAchievements();
    json getUserAchievements(int userId);
    json getUserProgress(int userId);
    bool syncUserProgress(int userId, const json& progressData);

    // 辅助方法
    bool executeQuery(const std::string& query);
    json executeSelect(const std::string& query);
    
    // 初始化和成就检查方法
    void populateInitialData();
    void checkAchievements(int userId);
    bool unlockAchievement(int userId, int achievementId);

    // 通用的游戏状态管理方法
    json getGameState(const std::string& gameType);
    bool updateGameState(const std::string& gameType, const json& state);
    
    // 数据结构游戏状态管理
    // 链表游戏状态
    json getLinkedListState();
    bool updateLinkedListState(const json& state);
    
    // 队列游戏状态
    json getQueueState();
    bool updateQueueState(const json& state);
    
    // 栈游戏状态
    json getStackState();
    bool updateStackState(const json& state);
    
    // 二叉树游戏状态
    json getBinaryTreeState();
    bool updateBinaryTreeState(const json& state);
    
    // 图游戏状态
    json getGraphState();
    bool updateGraphState(const json& state);
    
    // 排序游戏状态
    json getSortingState();
    bool updateSortingState(const json& state);
    
    // 搜索游戏状态
    json getSearchState();
    bool updateSearchState(const json& state);
    
    // 迷宫游戏状态
    json getMazeState();
    bool updateMazeState(const json& state);

private:
    // 数据库连接相关
    sqlite3* db;
    std::string dbPath;
    
    // 游戏状态
    json linkedListState;  // 链表游戏状态
    json queueState;      // 队列游戏状态
    json stackState;      // 栈游戏状态
    json binaryTreeState; // 二叉树游戏状态
    json graphState;      // 图游戏状态
    json sortingState;    // 排序游戏状态
    json searchState;     // 搜索游戏状态
    json mazeState;      // 迷宫游戏状态
}; 