#ifndef ANIMATION_ROUTES_H
#define ANIMATION_ROUTES_H

#include <crow.h>
#include <crow/middlewares/cors.h>
#include "../common/animation_manager.h"
#include "../common/game_interaction_handler.h"
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Helper function to get session ID from request
std::string getSessionIdFromRequest(const crow::request& req);

// 注册所有动画相关的路由
void registerAnimationRoutes(crow::App<crow::CORSHandler>& app);

// 注册游戏动画相关的路由
void registerGameAnimationRoutes(crow::App<crow::CORSHandler>& app);

// 注册动态规划相关的路由
void registerDPRoutes(crow::App<crow::CORSHandler>& app);

// 注册贪心算法相关的路由
void registerGreedyRoutes(crow::App<crow::CORSHandler>& app);

// 注册排序算法相关的路由
void registerSortingRoutes(crow::App<crow::CORSHandler>& app);

// 注册搜索算法相关的路由
void registerSearchRoutes(crow::App<crow::CORSHandler>& app);

// 注册迷宫算法相关的路由
void registerMazeRoutes(crow::App<crow::CORSHandler>& app);

// 注册图算法相关的路由
void registerGraphRoutes(crow::App<crow::CORSHandler>& app);

// 会话管理相关的处理函数
crow::response handleCreateSession(const crow::request& req);

// 动画控制相关的处理函数
crow::response handlePauseAnimation(const crow::request& req);
crow::response handleResumeAnimation(const crow::request& req);
crow::response handleStopAnimation(const crow::request& req);
crow::response handleGetAnimationStatus(const crow::request& req);

// 动画帧控制相关的处理函数
crow::response handleNextFrame(const crow::request& req);
crow::response handlePrevFrame(const crow::request& req);
crow::response handleGetFrames(const crow::request& req);
crow::response handleSetSpeed(const crow::request& req);

// 动画速度控制相关的处理函数
crow::response handleSpeedControl(const crow::request& req);

// 步骤回退相关的处理函数
crow::response handleStepBack(const crow::request& req); 

#endif // ANIMATION_ROUTES_H 