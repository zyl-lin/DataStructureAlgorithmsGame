#include "animation_routes.h"
#include "../implementations/dp/dp_animations.h"
#include "../common/response_builder.h"
#include <nlohmann/json.hpp>
#include "database.h"

using json = nlohmann::json;

void registerDPRoutes(crow::App<crow::CORSHandler>& app) {
    // 注册斐波那契动态规划路由
    CROW_ROUTE(app, "/api/dp/fibonacci")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPFibonacci(req);
        });
    
    // 注册背包问题动态规划路由
    CROW_ROUTE(app, "/api/dp/knapsack")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPKnapsack(req);
        });
    
    // 注册LCS动态规划路由
    CROW_ROUTE(app, "/api/dp/lcs")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPLCS(req);
        });
    
    // 注册LCS路径显示路由
    CROW_ROUTE(app, "/api/dp/lcs/path")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPState(req);
        });

    // 注册LCS单元格计算路由
    CROW_ROUTE(app, "/api/dp/lcs/cell")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPStep(req);
        });

    // 注册LCS状态保存路由
    CROW_ROUTE(app, "/api/dp/lcs/save")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPStateSave(req);
        });

    // 注册LCS状态恢复路由
    CROW_ROUTE(app, "/api/dp/lcs/restore")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPStateRestore(req);
        });

    // 注册LCS执行回放路由
    CROW_ROUTE(app, "/api/dp/lcs/playback")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPExecutionPlayback(req);
        });
    
    // 注册动态规划单步执行路由
    CROW_ROUTE(app, "/api/dp/step")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return dp_animations::handleDPStep(req);
        });
}

// 处理斐波那契数列请求
crow::response handleDPFibonacci(const crow::request& req, Database& db) {
    try {
        return dp_animations::handleDPFibonacci(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("斐波那契数列计算失败: ") + e.what(), "dp").dump());
    }
}

// 处理背包问题请求
crow::response handleDPKnapsack(const crow::request& req, Database& db) {
    try {
        return dp_animations::handleDPKnapsack(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("背包问题计算失败: ") + e.what(), "dp").dump());
    }
}

// 处理最长公共子序列请求
crow::response handleDPLCS(const crow::request& req, Database& db) {
    try {
        return dp_animations::handleDPLCS(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("最长公共子序列计算失败: ") + e.what(), "dp").dump());
    }
}

// 获取动态规划状态
crow::response handleDPState(const crow::request& req, Database& db) {
    try {
        return dp_animations::handleDPState(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取动态规划状态失败: ") + e.what(), "dp").dump());
    }
}

// 重置动态规划状态
crow::response handleDPReset(const crow::request& req, Database& db) {
    try {
        return dp_animations::handleDPReset(req);
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置动态规划状态失败: ") + e.what(), "dp").dump());
    }
} 