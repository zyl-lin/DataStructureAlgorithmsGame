#include "greedy_routes.h"
#include "game_animation_routes.h"
#include "response_builder.h"
#include <queue>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <numeric>
#include <functional>

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

// 贪心算法游戏操作 - 活动安排
crow::response handleGreedyActivitySelection(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 解析请求体
        json requestBody = json::parse(req.body);
        
        if (!requestBody.contains("start") || !requestBody.contains("end")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数：start, end", "greedy").dump());
        }
        
        std::vector<int> start = requestBody["start"].get<std::vector<int>>();
        std::vector<int> end = requestBody["end"].get<std::vector<int>>();
        
        if (start.size() != end.size()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "开始时间和结束时间数组长度必须相同", "greedy").dump());
        }
        
        int n = start.size();
        
        // 创建活动索引数组并按结束时间排序
        std::vector<int> activities(n);
        std::iota(activities.begin(), activities.end(), 0);
        std::sort(activities.begin(), activities.end(),
                 [&end](int i, int j) { return end[i] < end[j]; });
        
        // 记录选择的活动和步骤
        std::vector<int> selected;
        std::vector<json> steps;
        
        // 选择第一个活动
        if (!activities.empty()) {
            int lastEnd = end[activities[0]];
            selected.push_back(activities[0]);
            
            json activitySelected = json::object();
            activitySelected["activity"] = activities[0];
            activitySelected["start"] = start[activities[0]];
            activitySelected["end"] = end[activities[0]];
            activitySelected["step"] = steps.size() + 1;
            steps.push_back(activitySelected);
            
            // 贪心选择后续活动
            for (int i = 1; i < n; i++) {
                int current = activities[i];
                
                if (start[current] >= lastEnd) {
                    selected.push_back(current);
                    lastEnd = end[current];
                    
                    json activitySelected = json::object();
                    activitySelected["activity"] = current;
                    activitySelected["start"] = start[current];
                    activitySelected["end"] = end[current];
                    activitySelected["step"] = steps.size() + 1;
                    steps.push_back(activitySelected);
                } else {
                    json activitySelected = json::object();
                    activitySelected["activity"] = current;
                    activitySelected["start"] = start[current];
                    activitySelected["end"] = end[current];
                    activitySelected["step"] = steps.size() + 1;
                    steps.push_back(activitySelected);
                }
            }
        }
        
        // 构建结果
        json result = json::object();
        result["algorithm"] = "activitySelection";
        json input = json::object();
        input["start"] = start;
        input["end"] = end;
        result["input"] = input;
        result["selected"] = selected;
        result["steps"] = steps;
        
        // 更新数据库中的状态
        db.updateGameState("greedy", result);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(result, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("活动安排失败: ") + e.what(), "greedy").dump());
    }
}

// 贪心算法游戏操作 - 哈夫曼编码
crow::response handleGreedyHuffman(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 解析请求体
        json requestBody = json::parse(req.body);
        
        if (!requestBody.contains("chars") || !requestBody.contains("frequencies")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数：chars, frequencies", "greedy").dump());
        }
        
        std::vector<std::string> chars = requestBody["chars"].get<std::vector<std::string>>();
        std::vector<int> frequencies = requestBody["frequencies"].get<std::vector<int>>();
        
        if (chars.size() != frequencies.size()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "字符和频率数组长度必须相同", "greedy").dump());
        }
        
        // 定义哈夫曼树节点
        struct Node {
            std::string chars;
            int frequency;
            Node* left;
            Node* right;
            
            Node(std::string c, int f) : chars(c), frequency(f), left(nullptr), right(nullptr) {}
        };
        
        // 定义优先队列的比较函数
        auto compare = [](Node* a, Node* b) { return a->frequency > b->frequency; };
        std::priority_queue<Node*, std::vector<Node*>, decltype(compare)> pq(compare);
        
        // 记录构建步骤
        std::vector<json> steps;
        
        // 创建叶子节点并加入优先队列
        for (size_t i = 0; i < chars.size(); i++) {
            pq.push(new Node(chars[i], frequencies[i]));
            json stepObj = json::object();
            stepObj["type"] = "leaf";
            stepObj["char"] = chars[i];
            stepObj["frequency"] = frequencies[i];
            steps.push_back(stepObj);
        }
        
        // 构建哈夫曼树
        while (pq.size() > 1) {
            Node* left = pq.top(); pq.pop();
            Node* right = pq.top(); pq.pop();
            
            Node* parent = new Node(left->chars + right->chars, 
                                  left->frequency + right->frequency);
            parent->left = left;
            parent->right = right;
            
            pq.push(parent);
            
            json mergeStep = json::object();
            mergeStep["type"] = "merge";
            mergeStep["left"] = left->chars;
            mergeStep["right"] = right->chars;
            mergeStep["merged"] = parent->chars;
            mergeStep["frequency"] = parent->frequency;
            steps.push_back(mergeStep);
        }
        
        // 生成编码
        std::map<std::string, std::string> codes;
        std::function<void(Node*, std::string)> generateCodes = [&](Node* node, std::string code) {
            if (!node) return;
            
            if (!node->left && !node->right) {
                codes[node->chars] = code;
                json codeStep = json::object();
                codeStep["type"] = "code";
                codeStep["char"] = node->chars;
                codeStep["code"] = code;
                steps.push_back(codeStep);
            }
            
            generateCodes(node->left, code + "0");
            generateCodes(node->right, code + "1");
        };
        
        if (!pq.empty()) {
            generateCodes(pq.top(), "");
        }
        
        // 构建结果
        json result = json::object();
        result["algorithm"] = "huffman";
        json input = json::object();
        input["chars"] = chars;
        input["frequencies"] = frequencies;
        result["input"] = input;
        result["codes"] = codes;
        result["steps"] = steps;
        
        // 更新数据库中的状态
        db.updateGameState("greedy", result);
        
        // 清理内存
        std::function<void(Node*)> cleanup = [&](Node* node) {
            if (!node) return;
            cleanup(node->left);
            cleanup(node->right);
            delete node;
        };
        
        if (!pq.empty()) {
            cleanup(pq.top());
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(result, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("构建哈夫曼编码失败: ") + e.what(), "greedy").dump());
    }
} 