#ifndef SORTING_ANIMATIONS_H
#define SORTING_ANIMATIONS_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "../../common/animation_session.h"

using json = nlohmann::json;

// 冒泡排序算法处理函数
crow::response handleBubbleSortAnimation(const crow::request& req);
json createBubbleSortAnimationFrames(AnimationSession& session, json& currentState);

// 选择排序算法处理函数
crow::response handleSelectionSortAnimation(const crow::request& req);
json createSelectionSortAnimationFrames(AnimationSession& session, json& currentState);

// 插入排序算法处理函数
crow::response handleInsertionSortAnimation(const crow::request& req);
json createInsertionSortAnimationFrames(AnimationSession& session, json& currentState);

// 快速排序算法处理函数
crow::response handleQuickSortAnimation(const crow::request& req);
json createQuickSortAnimationFrames(AnimationSession& session, json& currentState);

// 排序算法单步执行处理函数
crow::response handleSortingStepAnimation(const crow::request& req);

// 获取排序状态处理函数
crow::response handleGetSortingState(const crow::request& req);

// 重置排序状态处理函数
crow::response handleResetSortingState(const crow::request& req);

// 状态管理函数
json createInitialSortingState(const std::string& sortingType);

// 辅助函数
void generateQuickSortFrames(AnimationSession& session, json& currentState, 
                           std::vector<int>& arr, int low, int high);
int partition(std::vector<int>& arr, int low, int high, 
             AnimationSession& session, json& currentState);

#endif // SORTING_ANIMATIONS_H