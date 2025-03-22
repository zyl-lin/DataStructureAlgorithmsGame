#include "greedy_routes.h"
#include "game_animation_routes.h"
#include "response_builder.h"
#include <queue>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <numeric>
#include <functional>
#include <crow.h>

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

// 贪心算法游戏操作 - 零钱兑换
crow::response handleGreedyCoinChange(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 解析请求体
        json requestBody = json::parse(req.body);
        
        if (!requestBody.contains("amount") || !requestBody.contains("coins")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数：amount, coins", "greedy").dump());
        }
        
        int amount = requestBody["amount"].get<int>();
        std::vector<int> coins = requestBody["coins"].get<std::vector<int>>();
        
        // 对硬币面值进行降序排序
        std::sort(coins.begin(), coins.end(), std::greater<int>());
        
        // 记录每种硬币使用的数量
        std::vector<int> coinCounts(coins.size(), 0);
        std::vector<json> steps;
        int remainingAmount = amount;
        
        // 贪心选择
        for (size_t i = 0; i < coins.size(); i++) {
            if (remainingAmount >= coins[i]) {
                coinCounts[i] = remainingAmount / coins[i];
                remainingAmount %= coins[i];
                
                // 记录这一步的状态
                json coinStep = json::object();
                coinStep["coin"] = coins[i];
                coinStep["count"] = coinCounts[i];
                coinStep["remainingAmount"] = remainingAmount;
                steps.push_back(coinStep);
            }
        }
        
        // 检查是否找到解
        bool found = (remainingAmount == 0);
        
        // 计算使用的总硬币数
        int totalCoins = std::accumulate(coinCounts.begin(), coinCounts.end(), 0);
        
        // 构建结果
        json result = json::object();
        result["algorithm"] = "coinChange";
        json input = json::object();
        input["amount"] = amount;
        input["coins"] = coins;
        result["input"] = input;
        result["found"] = found;
        result["solution"] = json::object();
        result["solution"]["coins"] = coins;
        result["solution"]["counts"] = json(coinCounts);
        result["solution"]["totalCoins"] = totalCoins;
        result["steps"] = steps;
        
        // 更新数据库中的状态
        db.updateGameState("greedy", result);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(result, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("零钱兑换失败: ") + e.what(), "greedy").dump());
    }
}

// 活动安排
crow::response handleActivitySelection(const crow::request& req) {
    try {
        auto reqJson = json::parse(req.body);
        
        // 检查必要参数
        if (!reqJson.contains("start") || !reqJson.contains("finish")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少start或finish参数", "greedy").dump());
        }
        
        std::vector<int> start = reqJson["start"];
        std::vector<int> finish = reqJson["finish"];
        
        // 验证输入
        if (start.size() != finish.size()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "start和finish数组长度必须相同", "greedy").dump());
        }
        
        if (start.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "输入数组不能为空", "greedy").dump());
        }

        int n = start.size();
        std::vector<std::pair<int, int>> activities(n);
        for (int i = 0; i < n; i++) {
            activities[i] = {finish[i], start[i]};  // 按结束时间排序
        }
        
        std::sort(activities.begin(), activities.end());
        
        std::vector<int> selected;
        std::vector<json> frames;
        
        // 记录初始状态
        frames.push_back({
            {"activities", activities},
            {"selected", selected},
            {"currentIndex", -1},
            {"lastSelected", -1}
        });
        
        // 选择第一个活动
        selected.push_back(0);
        int lastFinish = activities[0].first;
        
        frames.push_back({
            {"activities", activities},
            {"selected", selected},
            {"currentIndex", 0},
            {"lastSelected", 0},
            {"lastFinish", lastFinish}
        });
        
        // 贪心选择过程
        for (int i = 1; i < n; i++) {
            frames.push_back({
                {"activities", activities},
                {"selected", selected},
                {"currentIndex", i},
                {"lastSelected", selected.back()},
                {"lastFinish", lastFinish},
                {"checking", true}
            });
            
            if (activities[i].second >= lastFinish) {
                selected.push_back(i);
                lastFinish = activities[i].first;
                
                frames.push_back({
                    {"activities", activities},
                    {"selected", selected},
                    {"currentIndex", i},
                    {"lastSelected", i},
                    {"lastFinish", lastFinish},
                    {"selected", true}
                });
            }
        }
        
        // 最终状态
        frames.push_back({
            {"activities", activities},
            {"selected", selected},
            {"currentIndex", -1},
            {"completed", true}
        });

        json response = {
            {"state", {
                {"activities", activities},
                {"selected", selected},
                {"completed", true}
            }},
            {"animation", {
                {"frames", frames},
                {"totalSteps", frames.size()},
                {"speed", 5}
            }}
        };

        return crow::response(200, ResponseBuilder::createSuccessResponse(response, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("活动安排计算错误: ") + e.what(), "greedy").dump());
    }
}

// 哈夫曼编码的节点结构
struct HuffmanNode {
    std::string symbol;
    int frequency;
    std::string code;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(std::string s, int f) : 
        symbol(s), frequency(f), code(""), left(nullptr), right(nullptr) {}
};

// 用于优先队列的比较函数
struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// 递归生成编码
void generateCodes(HuffmanNode* node, std::string code, std::vector<json>& frames) {
    if (node == nullptr) return;
    
    node->code = code;
    
    if (node->left == nullptr && node->right == nullptr) {
        frames.push_back({
            {"symbol", node->symbol},
            {"frequency", node->frequency},
            {"code", node->code},
            {"type", "leaf"}
        });
    } else {
        frames.push_back({
            {"symbol", node->symbol},
            {"frequency", node->frequency},
            {"code", node->code},
            {"type", "internal"}
        });
        generateCodes(node->left, code + "0", frames);
        generateCodes(node->right, code + "1", frames);
    }
}

// 清理哈夫曼树
void cleanupHuffmanTree(HuffmanNode* node) {
    if (node == nullptr) return;
    cleanupHuffmanTree(node->left);
    cleanupHuffmanTree(node->right);
    delete node;
}

// 哈夫曼编码
crow::response handleHuffman(const crow::request& req) {
    try {
        auto reqJson = json::parse(req.body);
        
        // 检查必要参数
        if (!reqJson.contains("chars") || !reqJson.contains("freqs")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少chars或freqs参数", "greedy").dump());
        }
        
        std::vector<std::string> chars = reqJson["chars"];
        std::vector<int> freqs = reqJson["freqs"];
        
        // 验证输入
        if (chars.size() != freqs.size()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "chars和freqs数组长度必须相同", "greedy").dump());
        }
        
        if (chars.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "输入数组不能为空", "greedy").dump());
        }

        // 创建优先队列
        std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;
        std::vector<json> frames;
        
        // 初始化叶子节点
        for (size_t i = 0; i < chars.size(); i++) {
            pq.push(new HuffmanNode(chars[i], freqs[i]));
            frames.push_back({
                {"action", "create_leaf"},
                {"symbol", chars[i]},
                {"frequency", freqs[i]}
            });
        }
        
        // 构建哈夫曼树
        while (pq.size() > 1) {
            HuffmanNode* left = pq.top(); pq.pop();
            HuffmanNode* right = pq.top(); pq.pop();
            
            frames.push_back({
                {"action", "merge"},
                {"left", {{"symbol", left->symbol}, {"frequency", left->frequency}}},
                {"right", {{"symbol", right->symbol}, {"frequency", right->frequency}}}
            });
            
            HuffmanNode* parent = new HuffmanNode(left->symbol + right->symbol, 
                                                left->frequency + right->frequency);
            parent->left = left;
            parent->right = right;
            pq.push(parent);
        }
        
        // 生成编码
        HuffmanNode* root = pq.top();
        std::vector<json> codeFrames;
        generateCodes(root, "", codeFrames);
        
        // 收集结果
        std::vector<json> results;
        for (const auto& frame : codeFrames) {
            if (frame["type"] == "leaf") {
                results.push_back({
                    {"symbol", frame["symbol"]},
                    {"frequency", frame["frequency"]},
                    {"code", frame["code"]}
                });
            }
        }
        
        // 构建最终响应
        json response = {
            {"state", {
                {"codes", results},
                {"completed", true}
            }},
            {"animation", {
                {"frames", frames},
                {"codeFrames", codeFrames},
                {"totalSteps", frames.size() + codeFrames.size()},
                {"speed", 5}
            }}
        };
        
        // 清理内存
        cleanupHuffmanTree(root);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(response, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("哈夫曼编码计算错误: ") + e.what(), "greedy").dump());
    }
}

// 活动安排（包含Database参数的版本）
crow::response handleGreedyActivitySelection(const crow::request& req, Database& db) {
    try {
        // 调用原来的活动安排实现
        crow::response result = handleActivitySelection(req);
        
        // 如果计算成功，更新数据库中的状态
        if (result.code == 200) {
            json resultData = json::parse(result.body);
            if (resultData.contains("state")) {
                db.updateGameState("greedy", resultData["state"]);
            }
        }
        
        return result;
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("活动安排计算失败: ") + e.what(), "greedy").dump());
    }
}

// 哈夫曼编码（包含Database参数的版本）
crow::response handleGreedyHuffman(const crow::request& req, Database& db) {
    try {
        // 调用原来的哈夫曼编码实现
        crow::response result = handleHuffman(req);
        
        // 如果计算成功，更新数据库中的状态
        if (result.code == 200) {
            json resultData = json::parse(result.body);
            if (resultData.contains("state")) {
                db.updateGameState("greedy", resultData["state"]);
            }
        }
        
        return result;
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("哈夫曼编码计算失败: ") + e.what(), "greedy").dump());
    }
} 