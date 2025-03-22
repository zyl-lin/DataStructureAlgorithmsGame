#include "dp_routes.h"
#include "game_animation_routes.h"
#include "response_builder.h"
#include <nlohmann/json.hpp>
#include <numeric>
#include <algorithm>
#include <crow.h>

using json = nlohmann::json;

// 动态规划游戏操作 - 获取当前状态
crow::response handleDPState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("dp");
        if (state.empty()) {
            // 如果状态为空，初始化一个默认状态
            state = {
                {"algorithm", "fibonacci"},  // 默认算法
                {"input", nullptr},         // 输入数据
                {"table", json::array()},   // DP表格
                {"result", nullptr},        // 计算结果
                {"steps", json::array()}    // 计算步骤
            };
            db.updateGameState("dp", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "dp").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取动态规划状态失败: ") + e.what(), "dp").dump());
    }
}

// 动态规划游戏操作 - 重置状态
crow::response handleDPReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的动态规划状态
        json initialState = {
            {"algorithm", "fibonacci"},  // 默认算法
            {"input", nullptr},         // 输入数据
            {"table", json::array()},   // DP表格
            {"result", nullptr},        // 计算结果
            {"steps", json::array()}    // 计算步骤
        };
        
        // 更新数据库中的状态
        db.updateGameState("dp", initialState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(initialState, "dp").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置动态规划状态失败: ") + e.what(), "dp").dump());
    }
}

// 斐波那契数列计算
crow::response handleFibonacci(const crow::request& req) {
    try {
        auto reqJson = json::parse(req.body);
        
        // 检查n参数
        if (!reqJson.contains("n")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少参数n", "dp").dump());
        }
        
        int n = reqJson["n"];
        
        // 验证n的范围
        if (n < 0 || n > 30) {  // 限制n的范围以避免整数溢出
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "参数n必须在0到30之间", "dp").dump());
        }

        // 计算斐波那契数列
        std::vector<int> fib(n + 1);
        std::vector<json> frames;

        if (n >= 0) fib[0] = 0;
        if (n >= 1) fib[1] = 1;

        // 记录初始状态
        frames.push_back({
            {"sequence", fib},
            {"currentIndex", -1},
            {"calculating", false}
        });

        // 计算并记录每一步
        for (int i = 2; i <= n; i++) {
            // 显示正在计算的位置
            frames.push_back({
                {"sequence", fib},
                {"currentIndex", i},
                {"calculating", true},
                {"prev1", fib[i-1]},
                {"prev2", fib[i-2]}
            });

            fib[i] = fib[i-1] + fib[i-2];

            // 显示计算结果
            frames.push_back({
                {"sequence", fib},
                {"currentIndex", i},
                {"calculating", false},
                {"result", fib[i]}
            });
        }

        // 最终状态
        frames.push_back({
            {"sequence", fib},
            {"currentIndex", -1},
            {"calculating", false},
            {"completed", true}
        });

        json response = {
            {"state", {
                {"sequence", fib},
                {"completed", true}
            }},
            {"animation", {
                {"frames", frames},
                {"totalSteps", frames.size()},
                {"speed", 5}
            }}
        };

        return crow::response(200, ResponseBuilder::createSuccessResponse(response, "dp").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("斐波那契数列计算错误: ") + e.what(), "dp").dump());
    }
}

// 斐波那契数列计算（包含Database参数的版本）
crow::response handleDPFibonacci(const crow::request& req, Database& db) {
    try {
        // 调用原来的斐波那契实现
        crow::response result = handleFibonacci(req);
        
        // 如果计算成功，更新数据库中的状态
        if (result.code == 200) {
            json resultData = json::parse(result.body);
            if (resultData.contains("state")) {
                db.updateGameState("dp", resultData["state"]);
            }
        }
        
        return result;
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("斐波那契数列计算失败: ") + e.what(), "dp").dump());
    }
}

// 动态规划游戏操作 - 背包问题
crow::response handleDPKnapsack(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 解析请求体
        json requestBody = json::parse(req.body);
        
        if (!requestBody.contains("capacity") || !requestBody.contains("weights") || !requestBody.contains("values")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数：capacity, weights, values", "dp").dump());
        }
        
        int capacity = requestBody["capacity"].get<int>();
        std::vector<int> weights = requestBody["weights"].get<std::vector<int>>();
        std::vector<int> values = requestBody["values"].get<std::vector<int>>();
        
        if (weights.size() != values.size()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "weights和values的长度必须相同", "dp").dump());
        }
        
        int n = weights.size();
        
        // 创建动态规划表
        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));
        std::vector<std::vector<bool>> selected(n + 1, std::vector<bool>(capacity + 1, false));
        
        // 记录计算步骤
        std::vector<json> steps;
        
        // 动态规划求解
        for (int i = 1; i <= n; i++) {
            for (int w = 0; w <= capacity; w++) {
                if (weights[i-1] <= w) {
                    int include = values[i-1] + dp[i-1][w - weights[i-1]];
                    int exclude = dp[i-1][w];
                    
                    if (include > exclude) {
                        dp[i][w] = include;
                        selected[i][w] = true;
                    } else {
                        dp[i][w] = exclude;
                        selected[i][w] = false;
                    }
                } else {
                    dp[i][w] = dp[i-1][w];
                    selected[i][w] = false;
                }
                
                // 修复初始化列表问题
                json stepObj = json::object();
                stepObj["item"] = i-1;
                stepObj["weight"] = w;
                stepObj["value"] = dp[i][w];
                stepObj["selected"] = static_cast<bool>(selected[i][w]);
                steps.push_back(stepObj);
            }
        }
        
        // 回溯找出选择的物品
        std::vector<int> selectedItems;
        int w = capacity;
        for (int i = n; i > 0; i--) {
            if (selected[i][w]) {
                selectedItems.push_back(i-1);
                w -= weights[i-1];
            }
        }
        std::reverse(selectedItems.begin(), selectedItems.end());
        
        // 初始化游戏状态
        json initialState = json::object();
        initialState["dp"] = dp;
        initialState["selectedItems"] = json::array();
        initialState["maxValue"] = dp[n][capacity];
        initialState["weights"] = weights;
        initialState["values"] = values;
        initialState["capacity"] = capacity;
        
        // 构建结果
        json result = json::object();
        result["algorithm"] = "knapsack";
        json input = json::object();
        input["capacity"] = capacity;
        input["weights"] = weights;
        input["values"] = values;
        result["input"] = input;
        result["result"] = dp[n][capacity];
        result["table"] = dp;
        result["steps"] = steps;
        result["selectedItems"] = selectedItems;
        
        // 更新数据库中的状态
        db.updateGameState("dp", result);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(result, "dp").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("求解背包问题失败: ") + e.what(), "dp").dump());
    }
}

// 动态规划游戏操作 - 最长公共子序列
crow::response handleDPLCS(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 解析请求体
        json requestBody = json::parse(req.body);
        
        if (!requestBody.contains("str1") || !requestBody.contains("str2")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数：str1, str2", "dp").dump());
        }
        
        std::string str1 = requestBody["str1"].get<std::string>();
        std::string str2 = requestBody["str2"].get<std::string>();
        
        int m = str1.length();
        int n = str2.length();
        
        // 创建动态规划表
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
        std::vector<std::vector<std::string>> direction(m + 1, std::vector<std::string>(n + 1, ""));
        
        // 记录计算步骤
        std::vector<json> steps;
        
        // 动态规划求解
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                if (str1[i-1] == str2[j-1]) {
                    dp[i][j] = dp[i-1][j-1] + 1;
                    direction[i][j] = "diagonal";
                } else {
                    if (dp[i-1][j] >= dp[i][j-1]) {
                        dp[i][j] = dp[i-1][j];
                        direction[i][j] = "up";
                    } else {
                        dp[i][j] = dp[i][j-1];
                        direction[i][j] = "left";
                    }
                }
                
                // 修复初始化列表问题
                json stepObj = json::object();
                stepObj["i"] = i-1;
                stepObj["j"] = j-1;
                stepObj["lcsLength"] = dp[i][j];
                stepObj["match"] = (i > 0 && j > 0 && str1[i-1] == str2[j-1]);
                steps.push_back(stepObj);
            }
        }
        
        // 回溯找出最长公共子序列
        std::string lcs;
        int i = m, j = n;
        while (i > 0 && j > 0) {
            if (direction[i][j] == "diagonal") {
                lcs = str1[i-1] + lcs;
                i--; j--;
            } else if (direction[i][j] == "up") {
                i--;
            } else {
                j--;
            }
        }
        
        // 构建结果
        json result = json::object();
        result["algorithm"] = "lcs";
        json input = json::object();
        input["str1"] = str1;
        input["str2"] = str2;
        result["input"] = input;
        result["result"] = lcs;
        result["length"] = dp[m][n];
        result["table"] = dp;
        result["steps"] = steps;
        
        // 更新数据库中的状态
        db.updateGameState("dp", result);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(result, "dp").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("求解最长公共子序列失败: ") + e.what(), "dp").dump());
    }
} 