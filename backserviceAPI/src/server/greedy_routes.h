#ifndef GREEDY_ROUTES_H
#define GREEDY_ROUTES_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "database.h"

// 贪心算法游戏操作API处理函数
crow::response handleGreedyCoinChange(const crow::request& req, Database& db);
crow::response handleGreedyActivitySelection(const crow::request& req, Database& db);
crow::response handleGreedyHuffman(const crow::request& req, Database& db);
crow::response handleGreedyState(const crow::request& req, Database& db);
crow::response handleGreedyReset(const crow::request& req, Database& db);

#endif // GREEDY_ROUTES_H 