#ifndef GREEDY_ANIMATIONS_H
#define GREEDY_ANIMATIONS_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "../../common/animation_manager.h"

using json = nlohmann::json;

// 贪心算法动画相关的处理函数
namespace greedy_animations {
    // 零钱兑换相关函数
    crow::response handleGreedyCoinChange(const crow::request& req);
    json createGreedyCoinChangeAnimationFrames(AnimationSession& session, json& currentState);
    
    // 活动选择相关函数
    crow::response handleGreedyActivitySelection(const crow::request& req);
    json createGreedyActivitySelectionAnimationFrames(AnimationSession& session, json& currentState);
    
    // 哈夫曼编码相关函数
    crow::response handleGreedyHuffman(const crow::request& req);
    json createGreedyHuffmanAnimationFrames(AnimationSession& session, json& currentState);
    
    // 状态管理相关函数
    crow::response handleGreedyState(const crow::request& req);
    crow::response handleGreedyReset(const crow::request& req);
    
    // 单步执行相关函数
    crow::response handleGreedyStep(const crow::request& req);
    
    // 辅助函数
    json createInitialGreedyState(const std::string& greedyType);
    json executeGreedyStep(const std::string& greedyType, json& currentState, size_t step);
    void buildHuffmanTree(json& state);
    void calculateOptimalSolution(json& state);
} // namespace greedy_animations

#endif // GREEDY_ANIMATIONS_H 