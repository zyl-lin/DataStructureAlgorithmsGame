#include "game_animation_routes.h"
#include "response_builder.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 动态规划游戏操作 - 获取当前状态
crow::response handleDPState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("dp");
        if (state.empty()) {
            // 如果状态为空，初始化一个默认状态
            state = {
                {"algorithm", "fibonacci"},  // 默认算法
                {"input", nullptr},         // 输入数据
                {"table", json::array()},   // DP表格
                {"result", nullptr},        // 计算结果
                {"steps", json::array()}    // 计算步骤
            };
            db.updateGameState("dp", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "dp").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取动态规划状态失败: ") + e.what(), "dp").dump());
    }
}

// 动态规划游戏操作 - 重置状态
crow::response handleDPReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的动态规划状态
        json initialState = {
            {"algorithm", "fibonacci"},  // 默认算法
            {"input", nullptr},         // 输入数据
            {"table", json::array()},   // DP表格
            {"result", nullptr},        // 计算结果
            {"steps", json::array()}    // 计算步骤
        };
        
        // 更新数据库中的状态
        db.updateGameState("dp", initialState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(initialState, "dp").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置动态规划状态失败: ") + e.what(), "dp").dump());
    }
} 