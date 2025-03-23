#include "greedy_animations.h"
#include "../../common/response_builder.h"
#include <queue>
#include <algorithm>
#include <ctime>
#include <cmath>

namespace greedy_animations {

// 零钱兑换初始状态生成
json createInitialCoinChangeState() {
    json state;
    state["greedyType"] = "coinChange";
    state["coins"] = {25, 10, 5, 1};  // 硬币面值
    state["amount"] = 67;  // 目标金额
    state["currentStep"] = 0;
    state["currentCoin"] = 0;
    state["remainingAmount"] = 67;
    state["selectedCoins"] = json::array();
    
    return state;
}

// 活动选择初始状态生成
json createInitialActivitySelectionState() {
    json state;
    state["greedyType"] = "activitySelection";
    state["activities"] = json::array();
    
    // 添加示例活动
    state["activities"].push_back({
        {"id", 1},
        {"start", 1},
        {"end", 4}
    });
    state["activities"].push_back({
        {"id", 2},
        {"start", 3},
        {"end", 5}
    });
    state["activities"].push_back({
        {"id", 3},
        {"start", 0},
        {"end", 6}
    });
    
    state["currentStep"] = 0;
    state["selectedActivities"] = json::array();
    
    return state;
}

// 哈夫曼编码初始状态生成
json createInitialHuffmanState() {
    json state;
    state["greedyType"] = "huffman";
    state["characters"] = {
        {"A", 5},
        {"B", 9},
        {"C", 12},
        {"D", 13},
        {"E", 16},
        {"F", 45}
    };
    state["currentStep"] = 0;
    state["nodes"] = json::array();
    state["edges"] = json::array();
    state["codes"] = json::object();
    
    return state;
}

// 通用初始状态生成函数
json createInitialGreedyState(const std::string& greedyType) {
    json state;
    
    if (greedyType == "coinChange") {
        state["type"] = "greedy";
        state["greedyType"] = "coinChange";
        state["amount"] = 63;  // 需要兑换的金额
        state["coins"] = json::array({25, 10, 5, 1});  // 可用的硬币面值
        state["selected"] = json::array();  // 已选择的硬币
        state["remainingAmount"] = 63;  // 剩余需要兑换的金额
        state["currentStep"] = 0;
        state["completed"] = false;
        state["description"] = "初始化零钱兑换问题";
    }
    else if (greedyType == "activitySelection") {
        state["type"] = "greedy";
        state["greedyType"] = "activitySelection";
        state["activities"] = json::array();
        
        // 添加示例活动
        json activities = {
            {{"start", 1}, {"end", 4}},
            {{"start", 3}, {"end", 5}},
            {{"start", 0}, {"end", 6}},
            {{"start", 5}, {"end", 7}},
            {{"start", 3}, {"end", 8}},
            {{"start", 5}, {"end", 9}},
            {{"start", 6}, {"end", 10}},
            {{"start", 8}, {"end", 11}},
            {{"start", 8}, {"end", 12}},
            {{"start", 2}, {"end", 13}},
            {{"start", 12}, {"end", 14}}
        };
        
        state["activities"] = activities;
        state["selected"] = json::array();
        state["currentStep"] = 0;
        state["completed"] = false;
        state["description"] = "初始化活动选择问题";
    }
    else if (greedyType == "huffman") {
        state["type"] = "greedy";
        state["greedyType"] = "huffman";
        state["characters"] = json::array();
        state["frequencies"] = json::array();
        
        // 添加示例字符和频率
        json chars = {"a", "b", "c", "d", "e", "f"};
        json freqs = {45, 13, 12, 16, 9, 5};
        
        state["characters"] = chars;
        state["frequencies"] = freqs;
        state["nodes"] = json::array();
        state["edges"] = json::array();
        state["codes"] = json::object();
        state["currentStep"] = 0;
        state["completed"] = false;
        state["description"] = "初始化哈夫曼编码问题";
    }
    
    return state;
}

// 零钱兑换算法处理函数
crow::response handleGreedyCoinChangeAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "greedy").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "greedy").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "greedy" || !currentState.contains("greedyType") || 
            currentState["greedyType"] != "coinChange") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是零钱兑换类型", "greedy").dump());
        }
        
        createGreedyCoinChangeAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "coinChange"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建零钱兑换动画发生错误: ") + e.what(), "greedy").dump());
    }
}

// 零钱兑换动画帧生成函数
json createGreedyCoinChangeAnimationFrames(AnimationSession& session, json& currentState) {
    std::vector<int> coins = currentState["coins"].get<std::vector<int>>();
    int amount = currentState["amount"].get<int>();
    int remainingAmount = amount;
    std::vector<std::pair<int, int>> selectedCoins;  // pair<面值, 数量>
    
    // 按面值从大到小排序硬币
    std::sort(coins.begin(), coins.end(), std::greater<int>());
    
    // 生成初始帧
    json frame = currentState;
    frame["remainingAmount"] = remainingAmount;
    frame["selectedCoins"] = json::array();
    session.addFrame(frame, "开始零钱兑换，目标金额: " + std::to_string(amount));
    
    // 贪心选择过程
    for (int coin : coins) {
        if (remainingAmount >= coin) {
            int count = remainingAmount / coin;
            remainingAmount -= count * coin;
            selectedCoins.push_back({coin, count});
            
            frame = currentState;
            frame["currentCoin"] = coin;
            frame["remainingAmount"] = remainingAmount;
            
            json selectedCoinsJson = json::array();
            for (const auto& [value, cnt] : selectedCoins) {
                selectedCoinsJson.push_back({
                    {"value", value},
                    {"count", cnt}
                });
            }
            frame["selectedCoins"] = selectedCoinsJson;
            
            std::string description = "选择 " + std::to_string(count) + " 个 " + 
                                    std::to_string(coin) + " 元硬币，剩余金额: " + 
                                    std::to_string(remainingAmount);
            session.addFrame(frame, description);
        }
    }
    
    // 生成最终帧
    frame = currentState;
    frame["completed"] = true;
    frame["remainingAmount"] = remainingAmount;
    
    json selectedCoinsJson = json::array();
    for (const auto& [value, count] : selectedCoins) {
        selectedCoinsJson.push_back({
            {"value", value},
            {"count", count}
        });
    }
    frame["selectedCoins"] = selectedCoinsJson;
    
    std::string finalDescription = "零钱兑换完成，共使用 " + 
                                 std::to_string(selectedCoins.size()) + " 种硬币";
    session.addFrame(frame, finalDescription);
    
    return frame;
}

// 活动选择算法处理函数
crow::response handleGreedyActivitySelectionAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "greedy").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "greedy").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "greedy" || !currentState.contains("greedyType") || 
            currentState["greedyType"] != "activitySelection") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是活动选择类型", "greedy").dump());
        }
        
        createGreedyActivitySelectionAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "activitySelection"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建活动选择动画发生错误: ") + e.what(), "greedy").dump());
    }
}

// 活动选择动画帧生成函数
json createGreedyActivitySelectionAnimationFrames(AnimationSession& session, json& currentState) {
    auto activities = currentState["activities"].get<std::vector<json>>();
    
    // 按结束时间排序活动
    std::sort(activities.begin(), activities.end(), 
              [](const json& a, const json& b) {
                  return a["end"] < b["end"];
              });
    
    // 生成初始帧
    json frame = currentState;
    frame["activities"] = activities;
    frame["selectedActivities"] = json::array();
    session.addFrame(frame, "开始活动选择，共有 " + std::to_string(activities.size()) + " 个活动");
    
    // 贪心选择过程
    std::vector<json> selectedActivities;
    int lastEnd = 0;
    
    for (const auto& activity : activities) {
        int start = activity["start"];
        int end = activity["end"];
        
        frame = currentState;
        frame["currentActivity"] = activity;
        
        if (start >= lastEnd) {
            selectedActivities.push_back(activity);
            lastEnd = end;
            
            frame["selectedActivities"] = selectedActivities;
            std::string description = "选择活动 " + std::to_string(activity["id"].get<int>()) + 
                                    "（开始时间: " + std::to_string(start) + 
                                    ", 结束时间: " + std::to_string(end) + "）";
            session.addFrame(frame, description);
        } else {
            std::string description = "跳过活动 " + std::to_string(activity["id"].get<int>()) + 
                                    "，与已选活动时间冲突";
            session.addFrame(frame, description);
        }
    }
    
    // 生成最终帧
    frame = currentState;
    frame["completed"] = true;
    frame["selectedActivities"] = selectedActivities;
    
    std::string finalDescription = "活动选择完成，共选择 " + 
                                 std::to_string(selectedActivities.size()) + " 个活动";
    session.addFrame(frame, finalDescription);
    
    return frame;
}

// 哈夫曼编码算法处理函数
crow::response handleGreedyHuffmanAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "greedy").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "greedy").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "greedy" || !currentState.contains("greedyType") || 
            currentState["greedyType"] != "huffman") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是哈夫曼编码类型", "greedy").dump());
        }
        
        createGreedyHuffmanAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "huffman"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建哈夫曼编码动画发生错误: ") + e.what(), "greedy").dump());
    }
}

// 哈夫曼编码动画帧生成函数
json createGreedyHuffmanAnimationFrames(AnimationSession& session, json& currentState) {
    auto& chars = currentState["characters"];
    size_t totalSteps = chars.size() * 2 - 1;  // 构建哈夫曼树需要的步骤数
    
    for (size_t step = 0; step < totalSteps; step++) {
        currentState = executeGreedyStep("huffman", currentState, step);
        std::string description = "构建哈夫曼树步骤 " + std::to_string(step + 1);
        session.addFrame(currentState, description);
    }
    
    if (currentState["completed"]) {
        calculateOptimalSolution(currentState);
        session.addFrame(currentState, "哈夫曼编码完成");
    }
    
    return currentState;
}

// 执行单步计算
json executeGreedyStep(const std::string& greedyType, json& currentState, size_t step) {
    if (greedyType == "coinChange") {
        int remainingAmount = currentState["remainingAmount"];
        auto& coins = currentState["coins"];
        auto& selected = currentState["selected"];
        
        if (remainingAmount > 0 && step < coins.size()) {
            int coin = coins[step];
            int count = remainingAmount / coin;
            
            if (count > 0) {
                for (int i = 0; i < count; i++) {
                    selected.push_back(coin);
                }
                remainingAmount -= count * coin;
                currentState["remainingAmount"] = remainingAmount;
            }
            
            if (remainingAmount == 0) {
                currentState["completed"] = true;
            }
        }
    }
    else if (greedyType == "activitySelection") {
        auto& activities = currentState["activities"];
        auto& selected = currentState["selected"];
        
        if (step == 0) {
            // 按结束时间排序活动
            std::sort(activities.begin(), activities.end(),
                     [](const json& a, const json& b) {
                         return a["end"] < b["end"];
                     });
        }
        
        if (step < activities.size()) {
            // 检查当前活动是否与最后选择的活动冲突
            if (!selected.empty()) {
                int lastSelectedIndex = selected.back();
                if (lastSelectedIndex < activities.size() - 1 &&
                    activities[lastSelectedIndex].at("end").get<int>() > activities[step].at("start").get<int>()) {
                    return currentState;
                }
            }
            selected.push_back(step);
            
            if (step == activities.size() - 1) {
                currentState["completed"] = true;
            }
        }
    }
    else if (greedyType == "huffman") {
        auto& chars = currentState["characters"];
        auto& freqs = currentState["frequencies"];
        auto& nodes = currentState["nodes"];
        auto& edges = currentState["edges"];
        
        if (step == 0) {
            // 初始化叶子节点
            for (size_t i = 0; i < chars.size(); i++) {
                json node;
                node["id"] = "node_" + std::to_string(nodes.size());
                node["value"] = chars[i];
                node["frequency"] = freqs[i];
                node["isLeaf"] = true;
                node["x"] = i * 100;  // 简单的布局
                node["y"] = 0;
                nodes.push_back(node);
            }
        }
        else if (nodes.size() >= 2) {
            // 找到两个最小频率的节点
            std::sort(nodes.begin(), nodes.end(),
                     [](const json& a, const json& b) {
                         return a["frequency"] < b["frequency"];
                     });
            
            json node1 = nodes[0];
            json node2 = nodes[1];
            nodes.erase(nodes.begin(), nodes.begin() + 2);
            
            // 创建新的内部节点
            json newNode;
            newNode["id"] = "node_" + std::to_string(nodes.size());
            newNode["value"] = "";
            newNode["frequency"] = node1["frequency"].get<int>() + 
                                 node2["frequency"].get<int>();
            newNode["isLeaf"] = false;
            newNode["x"] = (node1["x"].get<int>() + node2["x"].get<int>()) / 2;
            newNode["y"] = step * 100;  // 简单的层次布局
            
            // 添加边
            json edge1;
            edge1["source"] = newNode["id"];
            edge1["target"] = node1["id"];
            edge1["position"] = "left";
            edges.push_back(edge1);
            
            json edge2;
            edge2["source"] = newNode["id"];
            edge2["target"] = node2["id"];
            edge2["position"] = "right";
            edges.push_back(edge2);
            
            nodes.push_back(newNode);
            
            if (nodes.size() == 1) {
                currentState["completed"] = true;
                buildHuffmanTree(currentState);
            }
        }
    }
    
    return currentState;
}

// 构建哈夫曼树
void buildHuffmanTree(json& state) {
    std::map<std::string, std::string> codes;
    auto& nodes = state["nodes"];
    auto& edges = state["edges"];
    
    // 使用DFS遍历树并生成编码
    std::function<void(const std::string&, const std::string&)> dfs = 
        [&](const std::string& nodeId, const std::string& code) {
            auto it = std::find_if(nodes.begin(), nodes.end(),
                                 [&](const json& node) {
                                     return node["id"] == nodeId;
                                 });
            
            if (it != nodes.end()) {
                if ((*it)["isLeaf"]) {
                    codes[(*it)["value"]] = code;
                } else {
                    // 查找左右子节点
                    for (const auto& edge : edges) {
                        if (edge["source"] == nodeId) {
                            dfs(edge["target"], code + (edge["position"] == "left" ? "0" : "1"));
                        }
                    }
                }
            }
        };
    
    // 从根节点开始遍历
    dfs(nodes.back()["id"], "");
    state["codes"] = codes;
}

// 计算最优解
void calculateOptimalSolution(json& state) {
    if (state["greedyType"] == "coinChange") {
        // 计算选择的硬币总数
        int totalCoins = state["selected"].size();
        
        // 计算总金额 - 使用显式循环替代std::accumulate
        int totalAmount = 0;
        for (const auto& coin : state["selected"]) {
            totalAmount += coin.get<int>();
        }
        
        state["solution"] = {
            {"totalCoins", totalCoins},
            {"totalAmount", totalAmount}
        };
    }
    else if (state["greedyType"] == "activitySelection") {
        int totalActivities = state["selected"].size();
        state["solution"] = {
            {"totalActivities", totalActivities}
        };
    }
    else if (state["greedyType"] == "huffman") {
        // 计算总频率
        int totalFrequency = 0;
        auto& freqs = state["frequencies"];
        for (const auto& freq : freqs) {
            totalFrequency += freq.get<int>();
        }

        // 计算平均编码长度
        double avgLength = 0.0;
        auto& codes = state["codes"];
        for (size_t i = 0; i < freqs.size(); i++) {
            avgLength += codes[i].get<std::string>().length() * freqs[i].get<int>();
        }

        if (totalFrequency > 0) {
            avgLength /= totalFrequency;
        }
        
        state["solution"] = {
            {"averageLength", avgLength}
        };
    }
}

// 处理零钱兑换动画
crow::response handleGreedyCoinChange(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "greedy").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "greedy").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = createInitialGreedyState("coinChange");
        
        createGreedyCoinChangeAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "greedyCoinChange"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Coin change error: ") + e.what(), "greedy").dump());
    }
}

// 处理活动选择动画
crow::response handleGreedyActivitySelection(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "greedy").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "greedy").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = createInitialGreedyState("activitySelection");
        
        createGreedyActivitySelectionAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "greedyActivitySelection"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Activity selection error: ") + e.what(), "greedy").dump());
    }
}

// 处理哈夫曼编码动画
crow::response handleGreedyHuffman(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "greedy").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "greedy").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = createInitialGreedyState("huffman");
        
        createGreedyHuffmanAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "greedyHuffman"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Huffman coding error: ") + e.what(), "greedy").dump());
    }
}

// 贪心算法状态获取函数
crow::response handleGreedyState(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "greedy");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "greedy");
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        return ResponseBuilder::createSuccessResponse(currentState, "greedy");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("获取贪心算法状态发生错误: ") + e.what(), "greedy");
    }
}

// 贪心算法重置函数
crow::response handleGreedyReset(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::createErrorResponse(400, "缺少必要参数: sessionId", "greedy");
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::createErrorResponse(400, "无效的会话ID", "greedy");
        }
        
        auto& session = animationManager->getSession(sessionId);
        std::string greedyType = session.getGameState().value("greedyType", "coinChange");
        
        // 创建初始状态
        json initialState = createInitialGreedyState(greedyType);
        session.updateGameState(initialState);
        
        return ResponseBuilder::createSuccessResponse(initialState, "greedy");
    } catch (const std::exception& e) {
        return ResponseBuilder::createErrorResponse(500, 
            std::string("重置贪心算法状态发生错误: ") + e.what(), "greedy");
    }
}

// 处理贪心算法单步执行
crow::response handleGreedyStep(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "greedy").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "greedy").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        size_t step = bodyData.value("step", 0);
        std::string greedyType = currentState.value("greedyType", "coinChange");
        
        currentState = executeGreedyStep(greedyType, currentState, step);
        session.updateGameState(currentState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(currentState, "greedy").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Greedy step error: ") + e.what(), "greedy").dump());
    }
}

} // namespace greedy_animations 