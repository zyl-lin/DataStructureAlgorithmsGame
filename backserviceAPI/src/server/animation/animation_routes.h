#ifndef ANIMATION_ROUTES_H
#define ANIMATION_ROUTES_H

#include <crow.h>
#include <crow/middlewares/cors.h>
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include "game_interaction_handler.h"

using json = nlohmann::json;

// 注册动画相关的路由
void registerAnimationRoutes(crow::App<crow::CORSHandler>& app);

// 会话管理函数
crow::response handleCreateSession(const crow::request& req);

// 动画控制相关处理函数
crow::response handleAnimationStart(const crow::request& req);
crow::response handleAnimationPause(const crow::request& req);
crow::response handleAnimationResume(const crow::request& req);
crow::response handleAnimationStop(const crow::request& req);
crow::response handleAnimationStatus(const crow::request& req);

// 游戏元素交互处理函数
// 已移至game_interaction_handler.h

// 动画帧控制处理函数
crow::response handleNextFrame(const crow::request& req);
crow::response handlePrevFrame(const crow::request& req);
crow::response handleGetFrames(const crow::request& req);
crow::response handleSetSpeed(const crow::request& req);

// 初始化状态创建函数
json createInitialLinkedListState();
json createInitialStackState();
json createInitialQueueState();
json createInitialBinaryTreeState();
json createInitialGraphState();
json createInitialSortingState();
json createInitialSearchState();
json createInitialMazeState();
json createInitialDPState();
json createInitialGreedyState();

// 特定游戏动画处理函数
// 链表游戏动画
crow::response handleLinkedListInsertAnimation(const crow::request& req);
crow::response handleLinkedListDeleteAnimation(const crow::request& req);
crow::response handleLinkedListFindAnimation(const crow::request& req);
crow::response handleLinkedListReverseAnimation(const crow::request& req);

// 队列游戏动画
crow::response handleQueueEnqueueAnimation(const crow::request& req);
crow::response handleQueueDequeueAnimation(const crow::request& req);

// 栈游戏动画
crow::response handleStackPushAnimation(const crow::request& req);
crow::response handleStackPopAnimation(const crow::request& req);

// 二叉树游戏动画
crow::response handleBinaryTreeInsertAnimation(const crow::request& req);
crow::response handleBinaryTreePreOrderAnimation(const crow::request& req);
crow::response handleBinaryTreeInOrderAnimation(const crow::request& req);
crow::response handleBinaryTreePostOrderAnimation(const crow::request& req);
crow::response handleBinaryTreeLevelOrderAnimation(const crow::request& req);

// 图游戏动画
crow::response handleGraphDFSAnimation(const crow::request& req);
crow::response handleGraphBFSAnimation(const crow::request& req);
crow::response handleGraphPathAnimation(const crow::request& req);

// 排序游戏动画
crow::response handleSortingBubbleAnimation(const crow::request& req);
crow::response handleSortingSelectionAnimation(const crow::request& req);
crow::response handleSortingInsertionAnimation(const crow::request& req);
crow::response handleSortingQuickAnimation(const crow::request& req);
crow::response handleSortingStepAnimation(const crow::request& req);

// 搜索游戏动画
crow::response handleSearchLinearAnimation(const crow::request& req);
crow::response handleSearchBinaryAnimation(const crow::request& req);
crow::response handleSearchStepAnimation(const crow::request& req);

// 迷宫游戏动画
crow::response handleMazeGenerateAnimation(const crow::request& req);
crow::response handleMazeSolveDFSAnimation(const crow::request& req);
crow::response handleMazeSolveBFSAnimation(const crow::request& req);
crow::response handleMazeSolveAStarAnimation(const crow::request& req);
crow::response handleMazeStepAnimation(const crow::request& req);

// 动态规划游戏动画
crow::response handleDPFibonacciAnimation(const crow::request& req);
crow::response handleDPKnapsackAnimation(const crow::request& req);
crow::response handleDPLCSAnimation(const crow::request& req);
crow::response handleDPStepAnimation(const crow::request& req);

// 贪心算法游戏动画
crow::response handleGreedyCoinChangeAnimation(const crow::request& req);
crow::response handleGreedyActivitySelectionAnimation(const crow::request& req);
crow::response handleGreedyHuffmanAnimation(const crow::request& req);
crow::response handleGreedyStepAnimation(const crow::request& req);

#endif // ANIMATION_ROUTES_H 