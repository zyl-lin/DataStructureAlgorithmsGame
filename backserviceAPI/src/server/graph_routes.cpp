#include "game_animation_routes.h"
#include "response_builder.h"
#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <set>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <numeric>

// 图遍历DFS实现
crow::response handleGraphTraverseDFS(const crow::request& req, Database& db) {
    // 提取动画参数
    AnimationParams params;
    if (req.url_params.get("animate") != nullptr) {
        params.animate = std::string(req.url_params.get("animate")) == "true";
    } else {
        params.animate = false;
    }
    
    if (req.url_params.get("speed") != nullptr) {
        params.speed = std::stoi(req.url_params.get("speed"));
    } else {
        params.speed = 5; // 默认速度
    }
    
    if (req.url_params.get("step") != nullptr) {
        params.step = std::string(req.url_params.get("step")) == "true";
    } else {
        params.step = false;
    }
    
    try {
        // 获取图的当前状态
        json graphState = db.getGameState("graph");
        
        // 如果图为空
        if (graphState.empty() || !graphState.contains("nodes") || !graphState.contains("edges")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "图为空，无法进行DFS遍历", "graph").dump());
        }
        
        // 提取起始节点
        int startNodeId = 0;
        if (req.body != "") {
            auto bodyJson = json::parse(req.body);
            if (bodyJson.contains("startNode")) {
                startNodeId = bodyJson["startNode"].get<int>();
            }
        }
        
        // 确保起始节点存在
        bool startNodeExists = false;
        for (const auto& node : graphState["nodes"]) {
            if (node.is_object() && node.contains("id") && node["id"] == startNodeId) {
                startNodeExists = true;
                break;
            }
        }
        
        if (!startNodeExists) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "指定的起始节点不存在", "graph").dump());
        }
        
        // 如果不需要动画，直接返回DFS遍历结果
        if (!params.animate) {
            // 构建邻接表
            std::map<int, std::vector<int>> adjList;
            for (const auto& edge : graphState["edges"]) {
                if (edge.is_object() && edge.contains("source") && edge.contains("target")) {
                    int source = edge["source"].get<int>();
                    int target = edge["target"].get<int>();
                    adjList[source].push_back(target);
                }
            }
            
            // 执行DFS遍历
            std::vector<int> result;
            std::set<int> visited;
            std::function<void(int)> dfs = [&](int nodeId) {
                visited.insert(nodeId);
                result.push_back(nodeId);
                
                for (int neighbor : adjList[nodeId]) {
                    if (visited.find(neighbor) == visited.end()) {
                        dfs(neighbor);
                    }
                }
            };
            
            dfs(startNodeId);
            
            // 更新图状态，添加遍历结果
            json updatedState = graphState;
            updatedState["traversal"] = {
                {"type", "dfs"},
                {"result", result}
            };
            
            // 更新数据库中的状态
            db.updateGameState("graph", updatedState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(updatedState, "graph").dump());
        }
        
        // 如果需要动画，准备动画帧
        json frames = json::array();
        std::vector<int> traversalResult;
        
        // 构建邻接表
        std::map<int, std::vector<int>> adjList;
        for (const auto& edge : graphState["edges"]) {
            if (edge.is_object() && edge.contains("source") && edge.contains("target")) {
                int source = edge["source"].get<int>();
                int target = edge["target"].get<int>();
                adjList[source].push_back(target);
                // 如果是无向图，添加反向边
                if (!graphState.contains("directed") || !graphState["directed"].get<bool>()) {
                    adjList[target].push_back(source);
                }
            }
        }
        
        // 初始帧：当前状态
        json initialFrame = graphState;
        initialFrame["highlight"] = nullptr;
        initialFrame["visited"] = json::array();
        initialFrame["traversalResult"] = json::array();
        initialFrame["stack"] = json::array();
        initialFrame["description"] = "开始DFS遍历: 从节点 " + std::to_string(startNodeId) + " 开始";
        frames.push_back(initialFrame);
        
        // 生成DFS动画帧
        std::set<int> visited;
        std::stack<int> nodeStack;
        nodeStack.push(startNodeId);
        
        // 帧：起始节点入栈
        json stackFrame = initialFrame;
        stackFrame["stack"] = json::array({startNodeId});
        stackFrame["highlight"] = startNodeId;
        stackFrame["description"] = "将起始节点 " + std::to_string(startNodeId) + " 加入栈";
        frames.push_back(stackFrame);
        
        while (!nodeStack.empty()) {
            int currentNode = nodeStack.top();
            nodeStack.pop();
            
            if (visited.find(currentNode) != visited.end()) {
                continue; // 已访问过，跳过
            }
            
            // 帧：访问当前节点
            json visitFrame = frames.back();
            
            // 更新栈状态
            json newStack = json::array();
            std::stack<int> tempStack = nodeStack;
            std::vector<int> stackVec;
            while (!tempStack.empty()) {
                stackVec.push_back(tempStack.top());
                tempStack.pop();
            }
            std::reverse(stackVec.begin(), stackVec.end());
            for (int node : stackVec) {
                newStack.push_back(node);
            }
            visitFrame["stack"] = newStack;
            
            visitFrame["highlight"] = currentNode;
            visitFrame["description"] = "访问节点 " + std::to_string(currentNode);
            frames.push_back(visitFrame);
            
            // 添加到遍历结果
            visited.insert(currentNode);
            traversalResult.push_back(currentNode);
            
            // 帧：标记为已访问
            json resultFrame = visitFrame;
            json visitedArray = json::array();
            for (const auto& node : visited) {
                visitedArray.push_back(node);
            }
            resultFrame["visited"] = visitedArray;
            resultFrame["traversalResult"] = traversalResult;
            resultFrame["description"] = "将节点 " + std::to_string(currentNode) + " 标记为已访问，并添加到结果";
            frames.push_back(resultFrame);
            
            // 逆序遍历邻居节点，以保持与传统DFS相同的遍历顺序
            std::vector<int> neighbors = adjList[currentNode];
            std::reverse(neighbors.begin(), neighbors.end());
            
            for (int neighbor : neighbors) {
                if (visited.find(neighbor) == visited.end()) {
                    // 帧：将未访问的邻居节点加入栈
                    json pushFrame = frames.back();
                    nodeStack.push(neighbor);
                    
                    // 更新栈状态
                    json updatedStack = json::array();
                    std::stack<int> tempStackUpdate = nodeStack;
                    std::vector<int> stackVecUpdate;
                    while (!tempStackUpdate.empty()) {
                        stackVecUpdate.push_back(tempStackUpdate.top());
                        tempStackUpdate.pop();
                    }
                    std::reverse(stackVecUpdate.begin(), stackVecUpdate.end());
                    for (int node : stackVecUpdate) {
                        updatedStack.push_back(node);
                    }
                    pushFrame["stack"] = updatedStack;
                    
                    pushFrame["highlight"] = neighbor;
                    pushFrame["description"] = "将未访问的邻居节点 " + std::to_string(neighbor) + " 加入栈";
                    frames.push_back(pushFrame);
                }
            }
        }
        
        // 最终帧：遍历完成
        json finalFrame = frames.back();
        finalFrame["highlight"] = nullptr;
        finalFrame["description"] = "DFS遍历完成: " + 
                                   std::accumulate(
                                       traversalResult.begin(), 
                                       traversalResult.end(), 
                                       std::string(), 
                                       [](const std::string& a, int b) {
                                           return a + (a.empty() ? "" : ", ") + std::to_string(b);
                                       });
        frames.push_back(finalFrame);
        
        // 更新图状态，添加遍历结果
        json updatedState = graphState;
        updatedState["traversal"] = {
            {"type", "dfs"},
            {"result", traversalResult}
        };
        
        // 更新数据库中的状态
        db.updateGameState("graph", updatedState);
        
        // 计算总步数和速度
        int totalSteps = frames.size();
        int speed = 1000 / params.speed; // 将1-10的速度转换为毫秒
        
        return crow::response(200, ResponseBuilder::createAnimationResponse(updatedState, frames, totalSteps, speed, "graph").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "graph").dump());
    }
}

// 图遍历BFS实现
crow::response handleGraphTraverseBFS(const crow::request& req, Database& db) {
    // 提取动画参数
    AnimationParams params;
    if (req.url_params.get("animate") != nullptr) {
        params.animate = std::string(req.url_params.get("animate")) == "true";
    } else {
        params.animate = false;
    }
    
    if (req.url_params.get("speed") != nullptr) {
        params.speed = std::stoi(req.url_params.get("speed"));
    } else {
        params.speed = 5; // 默认速度
    }
    
    if (req.url_params.get("step") != nullptr) {
        params.step = std::string(req.url_params.get("step")) == "true";
    } else {
        params.step = false;
    }
    
    try {
        // 获取图的当前状态
        json graphState = db.getGameState("graph");
        
        // 如果图为空
        if (graphState.empty() || !graphState.contains("nodes") || !graphState.contains("edges")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "图为空，无法进行BFS遍历", "graph").dump());
        }
        
        // 提取起始节点
        int startNodeId = 0;
        if (req.body != "") {
            auto bodyJson = json::parse(req.body);
            if (bodyJson.contains("startNode")) {
                startNodeId = bodyJson["startNode"].get<int>();
            }
        }
        
        // 确保起始节点存在
        bool startNodeExists = false;
        for (const auto& node : graphState["nodes"]) {
            if (node.is_object() && node.contains("id") && node["id"] == startNodeId) {
                startNodeExists = true;
                break;
            }
        }
        
        if (!startNodeExists) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "指定的起始节点不存在", "graph").dump());
        }
        
        // 如果不需要动画，直接返回BFS遍历结果
        if (!params.animate) {
            // 构建邻接表
            std::map<int, std::vector<int>> adjList;
            for (const auto& edge : graphState["edges"]) {
                if (edge.is_object() && edge.contains("source") && edge.contains("target")) {
                    int source = edge["source"].get<int>();
                    int target = edge["target"].get<int>();
                    adjList[source].push_back(target);
                    // 如果是无向图，添加反向边
                    if (!graphState.contains("directed") || !graphState["directed"].get<bool>()) {
                        adjList[target].push_back(source);
                    }
                }
            }
            
            // 执行BFS遍历
            std::vector<int> result;
            std::set<int> visited;
            std::queue<int> q;
            
            q.push(startNodeId);
            visited.insert(startNodeId);
            
            while (!q.empty()) {
                int node = q.front();
                q.pop();
                result.push_back(node);
                
                for (int neighbor : adjList[node]) {
                    if (visited.find(neighbor) == visited.end()) {
                        visited.insert(neighbor);
                        q.push(neighbor);
                    }
                }
            }
            
            // 更新图状态，添加遍历结果
            json updatedState = graphState;
            updatedState["traversal"] = {
                {"type", "bfs"},
                {"result", result}
            };
            
            // 更新数据库中的状态
            db.updateGameState("graph", updatedState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(updatedState, "graph").dump());
        }
        
        // 如果需要动画，准备动画帧
        json frames = json::array();
        std::vector<int> traversalResult;
        
        // 构建邻接表
        std::map<int, std::vector<int>> adjList;
        for (const auto& edge : graphState["edges"]) {
            if (edge.is_object() && edge.contains("source") && edge.contains("target")) {
                int source = edge["source"].get<int>();
                int target = edge["target"].get<int>();
                adjList[source].push_back(target);
                // 如果是无向图，添加反向边
                if (!graphState.contains("directed") || !graphState["directed"].get<bool>()) {
                    adjList[target].push_back(source);
                }
            }
        }
        
        // 初始帧：当前状态
        json initialFrame = graphState;
        initialFrame["highlight"] = nullptr;
        initialFrame["visited"] = json::array();
        initialFrame["traversalResult"] = json::array();
        initialFrame["queue"] = json::array();
        initialFrame["description"] = "开始BFS遍历: 从节点 " + std::to_string(startNodeId) + " 开始";
        frames.push_back(initialFrame);
        
        // 生成BFS动画帧
        std::set<int> visited;
        std::queue<int> nodeQueue;
        
        // 初始节点入队
        nodeQueue.push(startNodeId);
        visited.insert(startNodeId);
        
        // 帧：起始节点入队
        json queueFrame = initialFrame;
        json visitedArray = json::array();
        for (const auto& node : visited) {
            visitedArray.push_back(node);
        }
        queueFrame["queue"] = json::array({startNodeId});
        queueFrame["visited"] = visitedArray;
        queueFrame["highlight"] = startNodeId;
        queueFrame["description"] = "将起始节点 " + std::to_string(startNodeId) + " 加入队列并标记为已访问";
        frames.push_back(queueFrame);
        
        while (!nodeQueue.empty()) {
            int currentNode = nodeQueue.front();
            nodeQueue.pop();
            
            // 帧：当前节点出队
            json dequeueFrame = frames.back();
            
            // 更新队列状态
            std::queue<int> tempQueue = nodeQueue;
            json newQueue = json::array();
            std::vector<int> queueVec;
            while (!tempQueue.empty()) {
                queueVec.push_back(tempQueue.front());
                tempQueue.pop();
            }
            for (int node : queueVec) {
                newQueue.push_back(node);
            }
            dequeueFrame["queue"] = newQueue;
            
            dequeueFrame["highlight"] = currentNode;
            dequeueFrame["description"] = "从队列中取出节点 " + std::to_string(currentNode);
            frames.push_back(dequeueFrame);
            
            // 添加到遍历结果
            traversalResult.push_back(currentNode);
            
            // 帧：添加到结果
            json resultFrame = dequeueFrame;
            resultFrame["traversalResult"] = traversalResult;
            resultFrame["description"] = "将节点 " + std::to_string(currentNode) + " 添加到结果";
            frames.push_back(resultFrame);
            
            // 遍历相邻节点
            for (int neighbor : adjList[currentNode]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    nodeQueue.push(neighbor);
                    
                    // 帧：将未访问的邻居节点入队
                    json enqueueFrame = frames.back();
                    
                    // 更新队列状态
                    tempQueue = nodeQueue;
                    json updatedQueue = json::array();
                    std::vector<int> updatedQueueVec;
                    while (!tempQueue.empty()) {
                        updatedQueueVec.push_back(tempQueue.front());
                        tempQueue.pop();
                    }
                    for (int node : updatedQueueVec) {
                        updatedQueue.push_back(node);
                    }
                    enqueueFrame["queue"] = updatedQueue;
                    
                    json visitedArray = json::array();
                    for (const auto& node : visited) {
                        visitedArray.push_back(node);
                    }
                    enqueueFrame["visited"] = visitedArray;
                    enqueueFrame["highlight"] = neighbor;
                    enqueueFrame["description"] = "将未访问的邻居节点 " + std::to_string(neighbor) + " 加入队列并标记为已访问";
                    frames.push_back(enqueueFrame);
                }
            }
        }
        
        // 最终帧：遍历完成
        json finalFrame = frames.back();
        finalFrame["highlight"] = nullptr;
        finalFrame["description"] = "BFS遍历完成: " + 
                                   std::accumulate(
                                       traversalResult.begin(), 
                                       traversalResult.end(), 
                                       std::string(), 
                                       [](const std::string& a, int b) {
                                           return a + (a.empty() ? "" : ", ") + std::to_string(b);
                                       });
        frames.push_back(finalFrame);
        
        // 更新图状态，添加遍历结果
        json updatedState = graphState;
        updatedState["traversal"] = {
            {"type", "bfs"},
            {"result", traversalResult}
        };
        
        // 更新数据库中的状态
        db.updateGameState("graph", updatedState);
        
        // 计算总步数和速度
        int totalSteps = frames.size();
        int speed = 1000 / params.speed; // 将1-10的速度转换为毫秒
        
        return crow::response(200, ResponseBuilder::createAnimationResponse(updatedState, frames, totalSteps, speed, "graph").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "graph").dump());
    }
}

// 获取图状态
crow::response handleGraphState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("graph");
        
        // 如果图状态为空，初始化一个示例图
        if (state.empty()) {
            state = {
                {"nodes", json::array({
                    {{"id", 0}, {"label", "A"}, {"x", 100}, {"y", 100}},
                    {{"id", 1}, {"label", "B"}, {"x", 200}, {"y", 100}},
                    {{"id", 2}, {"label", "C"}, {"x", 150}, {"y", 200}},
                    {{"id", 3}, {"label", "D"}, {"x", 250}, {"y", 200}},
                    {{"id", 4}, {"label", "E"}, {"x", 300}, {"y", 100}}
                })},
                {"edges", json::array({
                    {{"source", 0}, {"target", 1}},
                    {{"source", 0}, {"target", 2}},
                    {{"source", 1}, {"target", 3}},
                    {{"source", 1}, {"target", 4}},
                    {{"source", 2}, {"target", 3}}
                })},
                {"directed", false}
            };
            
            db.updateGameState("graph", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "graph").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "graph").dump());
    }
}

// 图游戏操作 - 重置状态
crow::response handleGraphReset(const crow::request& req, Database& db) {
    try {
        // 创建一个默认的图状态（小型示例图）
        json initialState = {
            {"nodes", json::array({
                {{"id", 0}, {"label", "A"}, {"x", 100}, {"y", 100}},
                {{"id", 1}, {"label", "B"}, {"x", 200}, {"y", 100}},
                {{"id", 2}, {"label", "C"}, {"x", 150}, {"y", 200}},
                {{"id", 3}, {"label", "D"}, {"x", 250}, {"y", 200}},
                {{"id", 4}, {"label", "E"}, {"x", 300}, {"y", 100}}
            })},
            {"edges", json::array({
                {{"source", 0}, {"target", 1}},
                {{"source", 0}, {"target", 2}},
                {{"source", 1}, {"target", 3}},
                {{"source", 1}, {"target", 4}},
                {{"source", 2}, {"target", 3}}
            })},
            {"directed", false}
        };
        
        // 更新数据库中的状态
        db.updateGameState("graph", initialState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(initialState, "graph").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置图状态失败: ") + e.what(), "graph").dump());
    }
} 