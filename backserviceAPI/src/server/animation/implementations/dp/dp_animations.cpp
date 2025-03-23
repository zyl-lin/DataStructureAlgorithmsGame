#include "dp_animations.h"
#include "../../common/response_builder.h"
#include <queue>
#include <algorithm>
#include <cmath>

namespace dp_animations {

// LCS执行记录结构
struct LCSExecutionRecord {
    int i;                      // 当前行索引
    int j;                      // 当前列索引
    int value;                  // 计算得到的值
    std::string operation;      // 操作类型（"init", "match", "mismatch"）
    std::string explanation;    // 详细解释
    json dependencies;          // 依赖的单元格
};

// LCS状态生成函数
json createInitialLCSState() {
    json state;
    state["dpType"] = "lcs";
    state["string1"] = "ABCDE";  // 默认字符串1
    state["string2"] = "ACDBE";  // 默认字符串2
    state["currentStep"] = 0;
    state["currentI"] = 0;
    state["currentJ"] = 0;
    state["cells"] = json::object();
    
    return state;
}

// LCS动画处理函数
crow::response handleDPLCS(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "dp" || !currentState.contains("dpType") || 
            currentState["dpType"] != "lcs") {
            return ResponseBuilder::createErrorResponse(400, "当前会话不是LCS动态规划类型", "dp");
        }
        
        createDPLCSAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "lcs"},
            {"frameCount", session.getFrameCount()}
        };
        
        return ResponseBuilder::createSuccessResponse(responseData, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("创建LCS动态规划动画发生错误: ") + e.what(), "dp");
    }
}

// LCS状态保存处理函数
crow::response handleDPStateSave(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (!currentState.contains("dpType") || currentState["dpType"] != "lcs") {
            return ResponseBuilder::createErrorResponse(400, "非LCS类型会话", "dp");
        }
        
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            now.time_since_epoch()
        ).count();
        
        // Save state with a fixed key for latest state
        session.saveState("lcs_state_latest", currentState);
        
        json responseData = {
            {"timestamp", timestamp},
            {"currentStep", currentState.value("currentStep", 0)}
        };
        
        return ResponseBuilder::createSuccessResponse(responseData, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("保存LCS状态时发生错误: ") + e.what(), "dp");
    }
}

// LCS状态恢复处理函数
crow::response handleDPStateRestore(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        
        // Try to load the latest state directly
        std::string latestStateKey = "lcs_state_latest";
        json savedState = session.loadState(latestStateKey);
        
        if (savedState.empty()) {
            return ResponseBuilder::createErrorResponse(400, "未找到保存的状态", "dp");
        }
        
        session.updateGameState(savedState);
        
        json responseData = {
            {"timestamp", std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()},
            {"currentStep", savedState.value("currentStep", 0)},
            {"state", savedState}
        };
        
        return ResponseBuilder::createSuccessResponse(responseData, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("恢复LCS状态时发生错误: ") + e.what(), "dp");
    }
}

// LCS执行回放处理函数
crow::response handleDPExecutionPlayback(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("step")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId, step", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        int step = bodyData["step"];
        
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "dp" || !currentState.contains("dpType") || 
            currentState["dpType"] != "lcs" || !currentState.contains("executionRecords")) {
            return ResponseBuilder::createErrorResponse(400, 
                "当前会话不是LCS动态规划类型或未包含执行记录", "dp");
        }
        
        std::vector<LCSExecutionRecord> records;
        for (const auto& record : currentState["executionRecords"]) {
            LCSExecutionRecord lcsRecord;
            lcsRecord.i = record["i"].get<int>();
            lcsRecord.j = record["j"].get<int>();
            lcsRecord.value = record["value"].get<int>();
            lcsRecord.operation = record["operation"].get<std::string>();
            lcsRecord.explanation = record["explanation"].get<std::string>();
            lcsRecord.dependencies = record["dependencies"].get<std::vector<std::pair<int, int>>>();
            records.push_back(lcsRecord);
        }
        
        if (step < 0 || step >= records.size()) {
            return ResponseBuilder::createErrorResponse(400, "步骤超出范围", "dp");
        }
        
        const auto& record = records[step];
        
        // 构建响应数据
        json responseData = {
            {"success", true},
            {"currentStep", step},
            {"totalSteps", records.size()},
            {"record", {
                {"i", record.i},
                {"j", record.j},
                {"value", record.value},
                {"operation", record.operation},
                {"explanation", record.explanation},
                {"dependencies", record.dependencies}
            }}
        };
        
        return ResponseBuilder::createSuccessResponse(responseData, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("LCS执行回放发生错误: ") + e.what(), "dp");
    }
}

// 从原有的dp_animations.cpp移动相关函数实现
// ... 这里需要移动所有动态规划相关的函数实现 ...

// 示例：移动createInitialDPState函数实现
json createInitialDPState(const std::string& dpType) {
    json state;
    
    if (dpType == "fibonacci") {
        state["type"] = "dp";
        state["dpType"] = "fibonacci";
        state["n"] = 10;  // 计算前10个斐波那契数
        state["cells"] = json::array();
        state["currentStep"] = 0;
        state["completed"] = false;
        state["description"] = "初始化斐波那契数列计算";
        
        // 初始化单元格
        for (int i = 0; i < 10; i++) {
            json cell;
            cell["value"] = 0;
            cell["state"] = "normal";
            cell["dependencies"] = json::array();
            state["cells"].push_back(cell);
        }
    }
    else if (dpType == "knapsack") {
        state["type"] = "dp";
        state["dpType"] = "knapsack";
        state["capacity"] = 10;  // 背包容量
        state["items"] = json::array();
        state["dp"] = json::array();
        state["currentStep"] = 0;
        state["completed"] = false;
        state["description"] = "初始化背包问题";
        
        // 添加示例物品
        json items = {
            {{"weight", 2}, {"value", 3}},
            {{"weight", 3}, {"value", 4}},
            {{"weight", 4}, {"value", 5}},
            {{"weight", 5}, {"value", 6}}
        };
        state["items"] = items;
        
        // 初始化DP表格
        for (size_t i = 0; i <= items.size(); i++) {
            json row = json::array();
            for (int j = 0; j <= state["capacity"]; j++) {
                json cell;
                cell["value"] = 0;
                cell["state"] = "normal";
                cell["dependencies"] = json::array();
                row.push_back(cell);
            }
            state["dp"].push_back(row);
        }
    }
    else if (dpType == "lcs") {
        state["type"] = "dp";
        state["dpType"] = "lcs";
        state["string1"] = "ABCBDAB";
        state["string2"] = "BDCABA";
        state["cells"] = json::array();
        state["currentStep"] = 0;
        state["completed"] = false;
        state["description"] = "初始化最长公共子序列计算";
        
        // 初始化DP表格
        for (size_t i = 0; i <= state["string1"].get<std::string>().length(); i++) {
            json row = json::array();
            for (size_t j = 0; j <= state["string2"].get<std::string>().length(); j++) {
                json cell;
                cell["value"] = 0;
                cell["state"] = "normal";
                cell["dependencies"] = json::array();
                row.push_back(cell);
            }
            state["cells"].push_back(row);
        }
    }
    
    json table = json::array();
    table.push_back(json::array({0}));
    table.push_back(json::array({1}));
    
    state["table"] = table;
    state["computed"] = 1;
    
    state["cells"] = {
        {"cell-0", {
            {"value", 0},
            {"position", {{"x", 100}, {"y", 150}}},
            {"state", "computed"}
        }},
        {"cell-1", {
            {"value", 1},
            {"position", {{"x", 200}, {"y", 150}}},
            {"state", "computed"}
        }}
        // ... 其他单元格 ...
    };
    
    return state;
}

// 继续移动其他函数实现... 

// LCS动态规划核心算法实现
json createDPLCSAnimationFrames(AnimationSession& session, json& currentState) {
    // 获取输入字符串
    std::string str1 = currentState["string1"];
    std::string str2 = currentState["string2"];
    int m = str1.length();
    int n = str2.length();
    
    // 创建DP表格
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    
    // 生成执行记录
    std::vector<LCSExecutionRecord> records;
    
    // 记录初始化过程
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            if (i == 0 || j == 0) {
                records.push_back({
                    i, j, 0, "init",
                    "初始化边界条件：空字符串与任何字符串的LCS长度为0",
                    json::array()
                });
                
                // 创建初始化帧
                json frame = currentState;
                std::string cellId = "cell-" + std::to_string(i) + "-" + std::to_string(j);
                frame["cells"][cellId] = {
                    {"value", 0},
                    {"state", "computed"},
                    {"explanation", "边界初始化"}
                };
                session.addFrame(frame, "初始化单元格 [" + std::to_string(i) + "][" + 
                               std::to_string(j) + "] = 0");
            }
        }
    }
    
    // 填充DP表格
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            // 创建当前计算的帧
            json frame = currentState;
            std::string cellId = "cell-" + std::to_string(i) + "-" + std::to_string(j);
            
            // 设置依赖关系
            json dependencies = json::array();
            dependencies.push_back({
                {"i", i-1},
                {"j", j-1},
                {"type", "diagonal"}
            });
            dependencies.push_back({
                {"i", i-1},
                {"j", j},
                {"type", "up"}
            });
            dependencies.push_back({
                {"i", i},
                {"j", j-1},
                {"type", "left"}
            });
            
            if (str1[i-1] == str2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
                
                records.push_back({
                    i, j, dp[i][j], "match",
                    "字符匹配：'" + std::string(1, str1[i-1]) + "' == '" + 
                    std::string(1, str2[j-1]) + "'，取左上角值(" + 
                    std::to_string(dp[i-1][j-1]) + ") + 1 = " + 
                    std::to_string(dp[i][j]),
                    dependencies
                });
                
                frame["cells"][cellId] = {
                    {"value", dp[i][j]},
                    {"state", "match"},
                    {"explanation", records.back().explanation},
                    {"dependencies", dependencies}
                };
            } else {
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
                
                records.push_back({
                    i, j, dp[i][j], "mismatch",
                    "字符不匹配：'" + std::string(1, str1[i-1]) + "' != '" + 
                    std::string(1, str2[j-1]) + "'，取上方值(" + 
                    std::to_string(dp[i-1][j]) + ")和左方值(" + 
                    std::to_string(dp[i][j-1]) + ")的较大值 = " + 
                    std::to_string(dp[i][j]),
                    dependencies
                });
                
                frame["cells"][cellId] = {
                    {"value", dp[i][j]},
                    {"state", "mismatch"},
                    {"explanation", records.back().explanation},
                    {"dependencies", dependencies}
                };
            }
            
            session.addFrame(frame, records.back().explanation);
        }
    }
    
    // 回溯找出LCS
    std::string lcs;
    std::vector<std::pair<int, int>> path;
    int i = m, j = n;
    
    while (i > 0 && j > 0) {
        json frame = currentState;
        std::string cellId = "cell-" + std::to_string(i) + "-" + std::to_string(j);
        
        if (str1[i-1] == str2[j-1]) {
            lcs = str1[i-1] + lcs;
            path.push_back({i, j});
            frame["cells"][cellId]["state"] = "path";
            session.addFrame(frame, "找到LCS中的字符 '" + std::string(1, str1[i-1]) + "'");
            i--; j--;
        } else if (dp[i-1][j] > dp[i][j-1]) {
            frame["cells"][cellId]["state"] = "visited";
            session.addFrame(frame, "向上移动");
            i--;
        } else {
            frame["cells"][cellId]["state"] = "visited";
            session.addFrame(frame, "向左移动");
            j--;
        }
    }
    
    // 创建最终帧
    json finalFrame = currentState;
    finalFrame["lcs"] = lcs;
    finalFrame["lcsLength"] = dp[m][n];
    finalFrame["lcsPath"] = path;
    // 将records转换为json数组
    json recordsJson = json::array();
    for (const auto& record : records) {
        recordsJson.push_back({
            {"i", record.i},
            {"j", record.j},
            {"value", record.value},
            {"operation", record.operation},
            {"explanation", record.explanation},
            {"dependencies", record.dependencies}
        });
    }
    finalFrame["executionRecords"] = recordsJson;
    
    session.addFrame(finalFrame, "最长公共子序列是: \"" + lcs + "\", 长度为 " + 
                    std::to_string(dp[m][n]));
    
    // 更新当前状态
    currentState = finalFrame;
    return currentState;
}

// LCS单步执行函数
crow::response executeLCSDPStep(AnimationSession& session, json& currentState, int step) {
    try {
        // 获取输入字符串
        std::string str1 = currentState["string1"];
        std::string str2 = currentState["string2"];
        int m = str1.length();
        int n = str2.length();
        
        // 获取当前计算位置
        int currentI = currentState.value("currentI", 0);
        int currentJ = currentState.value("currentJ", 0);
        
        // 获取或初始化DP表格
        json& dp = currentState["dp"];
        if (dp.empty()) {
            for (int i = 0; i <= m; i++) {
                json row = json::array();
                for (int j = 0; j <= n; j++) {
                    row.push_back(0);
                }
                dp.push_back(row);
            }
        }
        
        // 执行单步计算
        if (currentI <= m && currentJ <= n) {
            std::string cellId = "cell-" + std::to_string(currentI) + "-" + 
                                std::to_string(currentJ);
            
            if (currentI == 0 || currentJ == 0) {
                // 初始化边界
                currentState["cells"][cellId] = {
                    {"value", 0},
                    {"state", "computed"},
                    {"explanation", "边界初始化"}
                };
            } else {
                // 计算当前单元格
                json dependencies = json::array();
                dependencies.push_back({
                    {"i", currentI-1},
                    {"j", currentJ-1},
                    {"type", "diagonal"}
                });
                
                if (str1[currentI-1] == str2[currentJ-1]) {
                    int newValue = dp[currentI-1][currentJ-1].get<int>() + 1;
                    dp[currentI][currentJ] = newValue;
                    
                    currentState["cells"][cellId] = {
                        {"value", newValue},
                        {"state", "match"},
                        {"explanation", "字符匹配：'" + std::string(1, str1[currentI-1]) + 
                                      "' == '" + std::string(1, str2[currentJ-1]) + "'"},
                        {"dependencies", dependencies}
                    };
                } else {
                    dependencies.push_back({
                        {"i", currentI-1},
                        {"j", currentJ},
                        {"type", "up"}
                    });
                    dependencies.push_back({
                        {"i", currentI},
                        {"j", currentJ-1},
                        {"type", "left"}
                    });
                    
                    int newValue = std::max(dp[currentI-1][currentJ].get<int>(), 
                                          dp[currentI][currentJ-1].get<int>());
                    dp[currentI][currentJ] = newValue;
                    
                    currentState["cells"][cellId] = {
                        {"value", newValue},
                        {"state", "mismatch"},
                        {"explanation", "字符不匹配：'" + std::string(1, str1[currentI-1]) + 
                                      "' != '" + std::string(1, str2[currentJ-1]) + "'"},
                        {"dependencies", dependencies}
                    };
                }
            }
            
            // 更新计算位置
            currentJ++;
            if (currentJ > n) {
                currentJ = 0;
                currentI++;
            }
            
            currentState["currentI"] = currentI;
            currentState["currentJ"] = currentJ;
            
            // 检查是否完成
            if (currentI > m) {
                currentState["completed"] = true;
                // 回溯找出LCS
                std::string lcs;
                std::vector<std::pair<int, int>> path;
                int i = m, j = n;
                
                while (i > 0 && j > 0) {
                    if (str1[i-1] == str2[j-1]) {
                        lcs = str1[i-1] + lcs;
                        path.push_back({i, j});
                        i--; j--;
                    } else if (dp[i-1][j].get<int>() > dp[i][j-1].get<int>()) {
                        i--;
                    } else {
                        j--;
                    }
                }
                
                currentState["lcs"] = lcs;
                currentState["lcsLength"] = dp[m][n];
                currentState["lcsPath"] = path;
            }
            
            // 更新会话状态
            session.updateGameState(currentState);
            
            return ResponseBuilder::createSuccessResponse({
                {"currentI", currentI},
                {"currentJ", currentJ},
                {"completed", currentI > m},
                {"state", currentState}
            }, "dp");
        }
        
        return ResponseBuilder::createErrorResponse(400, "计算已完成", "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("执行LCS计算步骤时发生错误: ") + e.what(), "dp");
    }
}

// 斐波那契数列动画处理函数
crow::response handleDPFibonacci(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "dp" || !currentState.contains("dpType") || 
            currentState["dpType"] != "fibonacci") {
            return ResponseBuilder::createErrorResponse(400, "当前会话不是斐波那契动态规划类型", "dp");
        }
        
        // 创建动画帧
        json frames = json::array();
        int n = currentState.value("n", 10);  // 默认计算前10个斐波那契数
        std::vector<int> fib(n + 1);
        
        // 初始化前两个数
        if (n >= 0) fib[0] = 0;
        if (n >= 1) fib[1] = 1;
        
        // 记录初始状态
        json frame = currentState;
        frame["sequence"] = fib;
        frame["currentIndex"] = -1;
        frame["calculating"] = false;
        session.addFrame(frame, "初始化斐波那契数列");
        
        // 计算并记录每一步
        for (int i = 2; i <= n; i++) {
            // 显示正在计算的位置
            frame = currentState;
            frame["sequence"] = fib;
            frame["currentIndex"] = i;
            frame["calculating"] = true;
            frame["prev1"] = fib[i-1];
            frame["prev2"] = fib[i-2];
            session.addFrame(frame, "计算第 " + std::to_string(i) + " 个数");
            
            // 计算当前位置的值
            fib[i] = fib[i-1] + fib[i-2];
            
            // 显示计算结果
            frame = currentState;
            frame["sequence"] = fib;
            frame["currentIndex"] = i;
            frame["calculating"] = false;
            frame["result"] = fib[i];
            session.addFrame(frame, "第 " + std::to_string(i) + " 个数计算完成: " + std::to_string(fib[i]));
        }
        
        // 最终状态
        frame = currentState;
        frame["sequence"] = fib;
        frame["currentIndex"] = -1;
        frame["calculating"] = false;
        frame["completed"] = true;
        session.addFrame(frame, "斐波那契数列计算完成");
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "fibonacci"},
            {"frameCount", session.getFrameCount()}
        };
        
        return ResponseBuilder::createSuccessResponse(responseData, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("创建斐波那契动画发生错误: ") + e.what(), "dp");
    }
}

// 背包问题动画处理函数
crow::response handleDPKnapsack(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "dp" || !currentState.contains("dpType") || 
            currentState["dpType"] != "knapsack") {
            return ResponseBuilder::createErrorResponse(400, "当前会话不是背包问题类型", "dp");
        }
        
        // 获取背包容量和物品列表
        int capacity = currentState.value("capacity", 10);
        auto items = currentState.value("items", json::array());
        
        // 创建动态规划表格
        std::vector<std::vector<int>> dp(items.size() + 1, 
                                       std::vector<int>(capacity + 1, 0));
        
        // 记录初始状态
        json frame = currentState;
        frame["dp"] = dp;
        frame["currentItem"] = -1;
        frame["currentWeight"] = -1;
        session.addFrame(frame, "初始化背包问题");
        
        // 动态规划求解过程
        for (size_t i = 1; i <= items.size(); i++) {
            int weight = items[i-1]["weight"];
            int value = items[i-1]["value"];
            
            for (int w = 0; w <= capacity; w++) {
                frame = currentState;
                frame["dp"] = dp;
                frame["currentItem"] = i;
                frame["currentWeight"] = w;
                
                if (weight <= w) {
                    dp[i][w] = std::max(value + dp[i-1][w-weight], dp[i-1][w]);
                    frame["considering"] = {
                        {"include", value + dp[i-1][w-weight]},
                        {"exclude", dp[i-1][w]}
                    };
                } else {
                    dp[i][w] = dp[i-1][w];
                    frame["considering"] = {
                        {"exclude", dp[i-1][w]}
                    };
                }
                
                session.addFrame(frame, "考虑第 " + std::to_string(i) + " 个物品，当前容量: " + 
                               std::to_string(w));
            }
        }
        
        // 最终状态
        frame = currentState;
        frame["dp"] = dp;
        frame["currentItem"] = -1;
        frame["currentWeight"] = -1;
        frame["completed"] = true;
        frame["maxValue"] = dp[items.size()][capacity];
        session.addFrame(frame, "背包问题求解完成，最大价值: " + 
                        std::to_string(dp[items.size()][capacity]));
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "knapsack"},
            {"frameCount", session.getFrameCount()}
        };
        
        return ResponseBuilder::createSuccessResponse(responseData, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("创建背包问题动画发生错误: ") + e.what(), "dp");
    }
}

// 动态规划状态获取函数
crow::response handleDPState(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        return ResponseBuilder::createSuccessResponse(currentState, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("获取动态规划状态发生错误: ") + e.what(), "dp");
    }
}

// 动态规划重置函数
crow::response handleDPReset(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        std::string dpType = session.getGameState().value("dpType", "fibonacci");
        
        // 创建初始状态
        json initialState = createInitialDPState(dpType);
        session.updateGameState(initialState);
        
        return ResponseBuilder::createSuccessResponse(initialState, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("重置动态规划状态发生错误: ") + e.what(), "dp");
    }
}

// 动态规划单步执行函数
crow::response handleDPStep(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "dp");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "dp");
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (!currentState.contains("dpType")) {
            return ResponseBuilder::createErrorResponse(400, "无效的动态规划类型", "dp");
        }
        
        std::string dpType = currentState["dpType"];
        size_t step = bodyData.value("step", 0);
        
        // 执行单步计算
        currentState = executeDPStep(dpType, currentState, step);
        session.updateGameState(currentState);
        
        return ResponseBuilder::createSuccessResponse(currentState, "dp");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("执行动态规划步骤发生错误: ") + e.what(), "dp");
    }
}

// 动态规划单步执行函数
json executeDPStep(const std::string& dpType, json& currentState, size_t step) {
    try {
        if (dpType == "fibonacci") {
            // 斐波那契数列单步执行
            auto& sequence = currentState["sequence"];
            if (step >= 2 && step < sequence.size()) {
                sequence[step] = sequence[step-1].get<int>() + sequence[step-2].get<int>();
                currentState["currentStep"] = step;
            }
        }
        else if (dpType == "knapsack") {
            // 背包问题单步执行
            auto& dp = currentState["dp"];
            auto& items = currentState["items"];
            int capacity = currentState["capacity"].get<int>();
            
            size_t i = step / (capacity + 1) + 1;
            size_t w = step % (capacity + 1);
            
            if (i <= items.size() && w <= capacity) {
                int weight = items[i-1]["weight"].get<int>();
                int value = items[i-1]["value"].get<int>();
                
                if (weight <= w) {
                    int include = value + dp[i-1][w - weight].get<int>();
                    int exclude = dp[i-1][w].get<int>();
                    dp[i][w] = std::max(include, exclude);
                } else {
                    dp[i][w] = dp[i-1][w].get<int>();
                }
                
                currentState["currentStep"] = step;
                currentState["currentI"] = i;
                currentState["currentW"] = w;
            }
        }
        else if (dpType == "lcs") {
            // 最长公共子序列单步执行
            std::string string1 = currentState["string1"].get<std::string>();
            std::string string2 = currentState["string2"].get<std::string>();
            auto& cells = currentState["cells"];
            
            size_t m = string1.length();
            size_t n = string2.length();
            size_t i = step / (n + 1);
            size_t j = step % (n + 1);
            
            if (i <= m && j <= n) {
                if (i == 0 || j == 0) {
                    cells[i][j] = 0;
                }
                else if (string1[i-1] == string2[j-1]) {
                    cells[i][j] = cells[i-1][j-1].get<int>() + 1;
                }
                else {
                    cells[i][j] = std::max(cells[i-1][j].get<int>(), cells[i][j-1].get<int>());
                }
                
                currentState["currentStep"] = step;
                currentState["currentI"] = i;
                currentState["currentJ"] = j;
            }
        }
        
        return currentState;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("执行动态规划步骤时发生错误: " + std::string(e.what()));
    }
}

// ... 其他函数实现 ... 
} 