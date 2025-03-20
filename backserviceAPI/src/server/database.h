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

private:
    sqlite3* db;
    
    
}; 