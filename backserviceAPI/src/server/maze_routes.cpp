#include "game_animation_routes.h"
#include "response_builder.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 迷宫游戏操作 - 获取当前状态
crow::response handleMazeState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("maze");
        if (state.empty()) {
            // 如果状态为空，初始化一个默认状态
            state = {
                {"maze", json::array()},  // 空的迷宫网格
                {"start", nullptr},       // 起点坐标
                {"end", nullptr},         // 终点坐标
                {"path", nullptr},        // 当前路径
                {"visited", nullptr}      // 已访问的单元格
            };
            db.updateGameState("maze", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "maze").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取迷宫状态失败: ") + e.what(), "maze").dump());
    }
}

// 迷宫游戏操作 - 重置状态
crow::response handleMazeReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的迷宫状态
        json initialState = {
            {"maze", json::array()},  // 空的迷宫网格
            {"start", nullptr},       // 起点坐标
            {"end", nullptr},         // 终点坐标
            {"path", nullptr},        // 当前路径
            {"visited", nullptr}      // 已访问的单元格
        };
        
        // 更新数据库中的状态
        db.updateGameState("maze", initialState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(initialState, "maze").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置迷宫状态失败: ") + e.what(), "maze").dump());
    }
} 