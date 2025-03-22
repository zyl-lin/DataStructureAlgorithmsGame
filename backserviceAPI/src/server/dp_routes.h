#ifndef DP_ROUTES_H
#define DP_ROUTES_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "database.h"

// 动态规划游戏操作API处理函数
crow::response handleDPFibonacci(const crow::request& req, Database& db);
crow::response handleDPKnapsack(const crow::request& req, Database& db);
crow::response handleDPLCS(const crow::request& req, Database& db);
crow::response handleDPState(const crow::request& req, Database& db);
crow::response handleDPReset(const crow::request& req, Database& db);

#endif // DP_ROUTES_H 