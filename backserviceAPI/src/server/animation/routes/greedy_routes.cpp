#include "greedy_routes.h"
#include "../implementations/greedy/greedy_animations.h"
#include "../common/response_builder.h"
#include <nlohmann/json.hpp>
#include <crow/middlewares/cors.h>

using json = nlohmann::json;

// 处理零钱兑换请求
crow::response handleGreedyCoinChange(const crow::request& req, Database& db) {
    try {
        return greedy_animations::handleGreedyCoinChange(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("零钱兑换计算失败: ") + e.what(), "greedy").dump());
    }
}

// 处理活动选择请求
crow::response handleGreedyActivitySelection(const crow::request& req, Database& db) {
    try {
        return greedy_animations::handleGreedyActivitySelection(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("活动选择计算失败: ") + e.what(), "greedy").dump());
    }
}

// 处理哈夫曼编码请求
crow::response handleGreedyHuffman(const crow::request& req, Database& db) {
    try {
        return greedy_animations::handleGreedyHuffman(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("哈夫曼编码计算失败: ") + e.what(), "greedy").dump());
    }
}

// 获取贪心算法状态
crow::response handleGreedyState(const crow::request& req, Database& db) {
    try {
        return greedy_animations::handleGreedyState(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取贪心算法状态失败: ") + e.what(), "greedy").dump());
    }
}

// 重置贪心算法状态
crow::response handleGreedyReset(const crow::request& req, Database& db) {
    try {
        return greedy_animations::handleGreedyReset(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置贪心算法状态失败: ") + e.what(), "greedy").dump());
    }
}

void registerGreedyRoutes(crow::App<crow::CORSHandler>& app) {
    // 注册零钱兑换贪心算法路由
    CROW_ROUTE(app, "/api/greedy/coin-change")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return greedy_animations::handleGreedyCoinChange(req);
        });
    
    // 注册活动选择贪心算法路由
    CROW_ROUTE(app, "/api/greedy/activity-selection")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return greedy_animations::handleGreedyActivitySelection(req);
        });
    
    // 注册哈夫曼编码贪心算法路由
    CROW_ROUTE(app, "/api/greedy/huffman")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return greedy_animations::handleGreedyHuffman(req);
        });
    
    // 注册贪心算法单步执行路由
    CROW_ROUTE(app, "/api/greedy/step")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return greedy_animations::handleGreedyStep(req);
        });
} 