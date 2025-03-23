#ifndef DP_ANIMATIONS_H
#define DP_ANIMATIONS_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "../../common/animation_manager.h"

using json = nlohmann::json;

// 动态规划动画相关的处理函数
namespace dp_animations {
    // 斐波那契数列相关函数
    crow::response handleDPFibonacci(const crow::request& req);
    json createDPFibonacciAnimationFrames(AnimationSession& session, json& currentState);
    
    // 背包问题相关函数
    crow::response handleDPKnapsack(const crow::request& req);
    json createDPKnapsackAnimationFrames(AnimationSession& session, json& currentState);
    
    // 最长公共子序列相关函数
    crow::response handleDPLCS(const crow::request& req);
    json createDPLCSAnimationFrames(AnimationSession& session, json& currentState);
    
    // 状态管理相关函数
    crow::response handleDPState(const crow::request& req);
    crow::response handleDPReset(const crow::request& req);
    
    // 单步执行相关函数
    crow::response handleDPStep(const crow::request& req);
    
    // 状态保存和恢复相关函数
    crow::response handleDPStateSave(const crow::request& req);
    crow::response handleDPStateRestore(const crow::request& req);
    
    // 执行回放相关函数
    crow::response handleDPExecutionPlayback(const crow::request& req);
    
    // 辅助函数
    json createInitialDPState(const std::string& dpType);
    json executeDPStep(const std::string& dpType, json& currentState, size_t step);
    void buildDependencyGraph(json& state);
    void calculateOptimalSolution(json& state);
} // namespace dp_animations

#endif // DP_ANIMATIONS_H 