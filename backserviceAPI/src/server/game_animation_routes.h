#ifndef GAME_ANIMATION_ROUTES_H
#define GAME_ANIMATION_ROUTES_H

#include <crow.h>
#include <crow/middlewares/cors.h>
#include <nlohmann/json.hpp>
#include "database.h"
#include "animation/animation_manager.h"

using json = nlohmann::json;

// 注册数据结构与算法游戏API路由
void registerGameAnimationRoutes(crow::App<crow::CORSHandler>& app, Database& db);

// 提取公共查询参数
struct AnimationParams {
    bool animate;
    int speed;
    bool step;
};

AnimationParams extractAnimationParams(const crow::request& req);

// 二叉树布局优化函数
void recalculateTreeLayout(std::vector<json>& nodes, const std::vector<json>& edges, int rootId, int baseX = 500, int baseY = 50, int levelHeight = 100);

// 链表游戏操作API处理函数
crow::response handleLinkedListInsert(const crow::request& req, Database& db);
crow::response handleLinkedListDelete(const crow::request& req, Database& db);
crow::response handleLinkedListFind(const crow::request& req, Database& db);
crow::response handleLinkedListReverse(const crow::request& req, Database& db);
crow::response handleLinkedListState(const crow::request& req, Database& db);

// 队列游戏操作API处理函数
crow::response handleQueueEnqueue(const crow::request& req, Database& db);
crow::response handleQueueDequeue(const crow::request& req, Database& db);
crow::response handleQueuePeek(const crow::request& req, Database& db);
crow::response handleQueueState(const crow::request& req, Database& db);

// 栈游戏操作API处理函数
crow::response handleStackPush(const crow::request& req, Database& db);
crow::response handleStackPop(const crow::request& req, Database& db);
crow::response handleStackPeek(const crow::request& req, Database& db);
crow::response handleStackState(const crow::request& req, Database& db);

// 二叉树游戏操作API处理函数
crow::response handleBinaryTreeInsert(const crow::request& req, Database& db);
crow::response handleBinaryTreeTraversePreOrder(const crow::request& req, Database& db);
crow::response handleBinaryTreeTraverseInOrder(const crow::request& req, Database& db);
crow::response handleBinaryTreeTraversePostOrder(const crow::request& req, Database& db);
crow::response handleBinaryTreeTraverseLevelOrder(const crow::request& req, Database& db);
crow::response handleBinaryTreeState(const crow::request& req, Database& db);

// 图游戏操作API处理函数
crow::response handleGraphTraverseDFS(const crow::request& req, Database& db);
crow::response handleGraphTraverseBFS(const crow::request& req, Database& db);
crow::response handleGraphFindPath(const crow::request& req, Database& db);
crow::response handleGraphState(const crow::request& req, Database& db);

// 排序游戏操作API处理函数
crow::response handleSortingBubbleSort(const crow::request& req, Database& db);
crow::response handleSortingSelectionSort(const crow::request& req, Database& db);
crow::response handleSortingInsertionSort(const crow::request& req, Database& db);
crow::response handleSortingQuickSort(const crow::request& req, Database& db);
crow::response handleSortingState(const crow::request& req, Database& db);
crow::response handleSortingReset(const crow::request& req, Database& db);

// 搜索游戏操作API处理函数
crow::response handleSearchLinear(const crow::request& req, Database& db);
crow::response handleSearchBinary(const crow::request& req, Database& db);
crow::response handleSearchState(const crow::request& req, Database& db);

// 迷宫游戏操作API处理函数
crow::response handleMazeGenerate(const crow::request& req, Database& db);
crow::response handleMazeSolveDFS(const crow::request& req, Database& db);
crow::response handleMazeSolveBFS(const crow::request& req, Database& db);
crow::response handleMazeSolveAStar(const crow::request& req, Database& db);
crow::response handleMazeState(const crow::request& req, Database& db);

// 动态规划游戏操作API处理函数
crow::response handleDPFibonacci(const crow::request& req, Database& db);
crow::response handleDPKnapsack(const crow::request& req, Database& db);
crow::response handleDPLCS(const crow::request& req, Database& db);
crow::response handleDPState(const crow::request& req, Database& db);

// 贪心算法游戏操作API处理函数
crow::response handleGreedyCoinChange(const crow::request& req, Database& db);
crow::response handleGreedyActivitySelection(const crow::request& req, Database& db);
crow::response handleGreedyHuffman(const crow::request& req, Database& db);
crow::response handleGreedyState(const crow::request& req, Database& db);

#endif // GAME_ANIMATION_ROUTES_H 