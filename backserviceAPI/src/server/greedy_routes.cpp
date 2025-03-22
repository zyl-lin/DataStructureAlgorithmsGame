#include "game_animation_routes.h"
#include "response_builder.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 贪心算法游戏操作 - 获取当前状态
crow::response handleGreedyState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("greedy");
        if (state.empty()) {
            // 如果状态为空，初始化一个默认状态
            state = {
                {"algorithm", "coinChange"},  // 默认算法
                {"input", nullptr},          // 输入数据
                {"solution", nullptr},        // 解决方案
                {"steps", json::array()},     // 计算步骤
                {"currentStep", 0}           // 当前步骤
            };
            db.updateGameState("greedy", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取贪心算法状态失败: ") + e.what(), "greedy").dump());
    }
}

// 贪心算法游戏操作 - 重置状态
crow::response handleGreedyReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的贪心算法状态
        json initialState = {
            {"algorithm", "coinChange"},  // 默认算法
            {"input", nullptr},          // 输入数据
            {"solution", nullptr},        // 解决方案
            {"steps", json::array()},     // 计算步骤
            {"currentStep", 0}           // 当前步骤
        };
        
        // 更新数据库中的状态
        db.updateGameState("greedy", initialState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(initialState, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置贪心算法状态失败: ") + e.what(), "greedy").dump());
    }
} 