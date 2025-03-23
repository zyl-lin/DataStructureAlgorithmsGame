#ifndef SEARCH_ANIMATIONS_H
#define SEARCH_ANIMATIONS_H
#include <crow.h>
#include <nlohmann/json.hpp>
#include "../../common/animation_session.h"

using json = nlohmann::json;

// 线性搜索算法处理函数
crow::response handleLinearSearchAnimation(const crow::request& req);
json createLinearSearchAnimationFrames(AnimationSession& session, json& currentState);

// 二分搜索算法处理函数
crow::response handleBinarySearchAnimation(const crow::request& req);
json createBinarySearchAnimationFrames(AnimationSession& session, json& currentState);

// 搜索算法单步执行处理函数
crow::response handleSearchStepAnimation(const crow::request& req);

// 获取搜索状态处理函数
crow::response handleGetSearchState(const crow::request& req);

// 重置搜索状态处理函数
crow::response handleResetSearchState(const crow::request& req);

// 状态管理函数
json createInitialSearchState(const std::string& searchType);

// 辅助函数
void generateBinarySearchFrames(AnimationSession& session, json& currentState, 
                              std::vector<int>& arr, int target, int left, int right); 

#endif // SEARCH_ANIMATIONS_H