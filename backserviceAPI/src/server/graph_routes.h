#ifndef GRAPH_ROUTES_H
#define GRAPH_ROUTES_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "database.h"

// 图游戏操作API处理函数
crow::response handleGraphTraverseDFS(const crow::request& req, Database& db);
crow::response handleGraphTraverseBFS(const crow::request& req, Database& db);
crow::response handleGraphState(const crow::request& req, Database& db);
crow::response handleGraphReset(const crow::request& req, Database& db);

#endif // GRAPH_ROUTES_H 