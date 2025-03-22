#ifndef MAZE_ROUTES_H
#define MAZE_ROUTES_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "database.h"

// 迷宫游戏操作API处理函数
crow::response handleMazeGenerate(const crow::request& req, Database& db);
crow::response handleMazeSolveDFS(const crow::request& req, Database& db);
crow::response handleMazeSolveBFS(const crow::request& req, Database& db);
crow::response handleMazeSolveAStar(const crow::request& req, Database& db);
crow::response handleMazeState(const crow::request& req, Database& db);
crow::response handleMazeReset(const crow::request& req, Database& db);

#endif // MAZE_ROUTES_H 