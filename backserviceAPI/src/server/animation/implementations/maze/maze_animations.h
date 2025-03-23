#ifndef MAZE_ANIMATIONS_H
#define MAZE_ANIMATIONS_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "../../common/animation_session.h"
#include "database.h"

using json = nlohmann::json;

// 迷宫生成处理函数
crow::response handleMazeGenerate(const crow::request& req);
crow::response handleMazeGenerate(const crow::request& req, Database& db);
json createMazeGenerationFrames(AnimationSession& session, json& currentState);

// DFS求解处理函数
crow::response handleMazeSolveDFS(const crow::request& req);
crow::response handleMazeSolveDFS(const crow::request& req, Database& db);
json createMazeDFSFrames(AnimationSession& session, json& currentState);

// BFS求解处理函数
crow::response handleMazeSolveBFS(const crow::request& req);
crow::response handleMazeSolveBFS(const crow::request& req, Database& db);
json createMazeBFSFrames(AnimationSession& session, json& currentState);

// A*求解处理函数
crow::response handleMazeSolveAStar(const crow::request& req);
crow::response handleMazeSolveAStar(const crow::request& req, Database& db);
json createMazeAStarFrames(AnimationSession& session, json& currentState);

// 获取迷宫状态处理函数
crow::response handleGetMazeState(const crow::request& req);
crow::response handleMazeState(const crow::request& req, Database& db);

// 重置迷宫状态处理函数
crow::response handleResetMazeState(const crow::request& req);
crow::response handleMazeReset(const crow::request& req, Database& db);

// 状态管理函数
json createInitialMazeState();

// 辅助函数
void generateMaze(json& state);
bool dfsMaze(int x, int y, int endX, int endY, std::vector<std::vector<bool>>& visited,
             std::vector<std::pair<int, int>>& path, json& currentState, AnimationSession& session);
void bfsMaze(int startX, int startY, int endX, int endY, json& currentState, AnimationSession& session);
void aStarMaze(int startX, int startY, int endX, int endY, json& currentState, AnimationSession& session); 

#endif // MAZE_ANIMATIONS_H