#include <crow.h>
#include <crow/middlewares/cors.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <mutex>
#include <iostream>
#include "animation_manager.h"
#include "game_interaction_handler.h"
#include "../response_builder.h"

using json = nlohmann::json;

// 游戏元素交互处理函数
crow::response handleGameInteract(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        // 必要参数验证
        if (!bodyData.contains("sessionId") || !bodyData.contains("actionType") || !bodyData.contains("elementId")) {
            return ResponseBuilder::buildErrorResponse("缺少必要参数: sessionId, actionType, elementId");
        }
        
        std::string sessionId = bodyData["sessionId"];
        std::string actionType = bodyData["actionType"];
        std::string elementId = bodyData["elementId"];
        
        // 验证会话是否存在
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::buildErrorResponse("无效的会话ID");
        }
        
        // 获取当前会话
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        // 根据交互类型执行相应的操作
        json result;
        if (actionType == "click") {
            // 处理点击事件
            result = handleElementClick(session, currentState, elementId, bodyData);
        } else if (actionType == "drag") {
            // 处理拖拽事件
            result = handleElementDrag(session, currentState, elementId, bodyData);
        } else if (actionType == "drop") {
            // 处理放置事件
            result = handleElementDrop(session, currentState, elementId, bodyData);
        } else if (actionType == "hover") {
            // 处理悬停事件
            result = handleElementHover(session, currentState, elementId, bodyData);
        } else {
            return ResponseBuilder::buildErrorResponse("不支持的交互类型");
        }
        
        // 更新游戏状态
        session.updateGameState(result);
        
        // 构建响应
        json response = {
            {"success", true},
            {"data", {
                {"state", result},
                {"interactionType", actionType},
                {"elementId", elementId}
            }}
        };
        
        return ResponseBuilder::buildJsonResponse(response);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("处理游戏交互发生错误: ") + e.what());
    }
}

// 处理点击事件
json handleElementClick(AnimationSession& session, const json& currentState, const std::string& elementId, const json& data) {
    json newState = currentState;
    std::string gameType = session.getGameType();
    
    // 检查是否包含点击位置信息
    int x = -1, y = -1;
    if (data.contains("position") && data["position"].is_object()) {
        if (data["position"].contains("x") && data["position"]["x"].is_number()) {
            x = data["position"]["x"];
        }
        if (data["position"].contains("y") && data["position"]["y"].is_number()) {
            y = data["position"]["y"];
        }
    }
    
    // 添加一个新的动画帧，展示点击效果
    json frameData = newState;
    
    // 根据游戏类型执行不同的点击处理逻辑
    if (gameType == "linkedlist") {
        // 链表元素点击逻辑
        updateLinkedListElementState(newState, elementId, "clicked");
        frameData["elements"][elementId]["state"] = "clicked";
        session.addFrame(frameData, "点击了链表节点 " + elementId);
    } 
    else if (gameType == "stack") {
        // 栈元素点击逻辑
        updateStackElementState(newState, elementId, "clicked");
        frameData["elements"][elementId]["state"] = "clicked";
        session.addFrame(frameData, "点击了栈元素 " + elementId);
    } 
    else if (gameType == "queue") {
        // 队列元素点击逻辑
        updateQueueElementState(newState, elementId, "clicked");
        frameData["elements"][elementId]["state"] = "clicked";
        session.addFrame(frameData, "点击了队列元素 " + elementId);
    } 
    else if (gameType == "binarytree") {
        // 二叉树元素点击逻辑
        updateBinaryTreeElementState(newState, elementId, "clicked");
        frameData["nodes"][elementId]["state"] = "clicked";
        session.addFrame(frameData, "点击了二叉树节点 " + elementId);
    } 
    else if (gameType == "graph") {
        // 图元素点击逻辑
        updateGraphElementState(newState, elementId, "clicked");
        frameData["nodes"][elementId]["state"] = "clicked";
        session.addFrame(frameData, "点击了图节点 " + elementId);
    }
    else if (gameType == "sorting") {
        // 排序元素点击逻辑
        updateSortingElementState(newState, elementId, "clicked");
        frameData["elements"][elementId]["state"] = "clicked";
        session.addFrame(frameData, "点击了排序元素 " + elementId);
    }
    else if (gameType == "search") {
        // 搜索元素点击逻辑
        updateSearchElementState(newState, elementId, "clicked");
        frameData["elements"][elementId]["state"] = "clicked";
        session.addFrame(frameData, "点击了搜索元素 " + elementId);
    }
    else if (gameType == "maze") {
        // 迷宫元素点击逻辑
        updateMazeElementState(newState, elementId, "clicked", x, y);
        frameData["cells"][y][x]["state"] = "clicked";
        session.addFrame(frameData, "点击了迷宫单元格 (" + std::to_string(x) + "," + std::to_string(y) + ")");
    }
    
    return newState;
}

// 处理拖拽事件
json handleElementDrag(AnimationSession& session, const json& currentState, const std::string& elementId, const json& data) {
    json newState = currentState;
    std::string gameType = session.getGameType();
    
    // 检查拖拽位置信息
    if (!data.contains("position") || !data["position"].is_object() ||
        !data["position"].contains("x") || !data["position"].contains("y")) {
        return currentState; // 位置信息不完整，保持状态不变
    }
    
    int x = data["position"]["x"];
    int y = data["position"]["y"];
    
    // 添加拖拽动画帧
    json frameData = newState;
    
    // 根据游戏类型更新元素位置
    if (gameType == "linkedlist") {
        // 链表元素拖拽逻辑
        updateLinkedListElementPosition(newState, elementId, x, y);
        frameData["elements"][elementId]["position"] = {{"x", x}, {"y", y}};
        frameData["elements"][elementId]["state"] = "dragging";
        session.addFrame(frameData, "拖拽链表节点 " + elementId);
    } 
    else if (gameType == "binarytree") {
        // 二叉树元素拖拽逻辑
        updateBinaryTreeElementPosition(newState, elementId, x, y);
        frameData["nodes"][elementId]["position"] = {{"x", x}, {"y", y}};
        frameData["nodes"][elementId]["state"] = "dragging";
        session.addFrame(frameData, "拖拽二叉树节点 " + elementId);
    }
    else if (gameType == "graph") {
        // 图元素拖拽逻辑
        updateGraphElementPosition(newState, elementId, x, y);
        frameData["nodes"][elementId]["position"] = {{"x", x}, {"y", y}};
        frameData["nodes"][elementId]["state"] = "dragging";
        session.addFrame(frameData, "拖拽图节点 " + elementId);
    }
    
    return newState;
}

// 处理放置事件
json handleElementDrop(AnimationSession& session, const json& currentState, const std::string& elementId, const json& data) {
    json newState = currentState;
    std::string gameType = session.getGameType();
    
    // 检查放置位置信息
    if (!data.contains("position") || !data["position"].is_object() ||
        !data["position"].contains("x") || !data["position"].contains("y")) {
        return currentState; // 位置信息不完整，保持状态不变
    }
    
    int x = data["position"]["x"];
    int y = data["position"]["y"];
    
    // 检查目标元素信息
    std::string targetId = "";
    if (data.contains("targetId") && data["targetId"].is_string()) {
        targetId = data["targetId"];
    }
    
    // 添加放置动画帧
    json frameData = newState;
    
    // 根据游戏类型更新元素状态
    if (gameType == "linkedlist") {
        // 链表元素放置逻辑
        updateLinkedListElementPosition(newState, elementId, x, y);
        updateLinkedListElementState(newState, elementId, "normal");
        frameData["elements"][elementId]["position"] = {{"x", x}, {"y", y}};
        frameData["elements"][elementId]["state"] = "normal";
        
        std::string message = "放置链表节点 " + elementId;
        if (!targetId.empty()) {
            message += " 到目标位置 " + targetId;
        }
        session.addFrame(frameData, message);
    } 
    else if (gameType == "binarytree") {
        // 二叉树元素放置逻辑
        updateBinaryTreeElementPosition(newState, elementId, x, y);
        updateBinaryTreeElementState(newState, elementId, "normal");
        frameData["nodes"][elementId]["position"] = {{"x", x}, {"y", y}};
        frameData["nodes"][elementId]["state"] = "normal";
        
        std::string message = "放置二叉树节点 " + elementId;
        if (!targetId.empty()) {
            message += " 成为节点 " + targetId + " 的子节点";
        }
        session.addFrame(frameData, message);
    }
    else if (gameType == "graph") {
        // 图元素放置逻辑
        updateGraphElementPosition(newState, elementId, x, y);
        updateGraphElementState(newState, elementId, "normal");
        frameData["nodes"][elementId]["position"] = {{"x", x}, {"y", y}};
        frameData["nodes"][elementId]["state"] = "normal";
        
        std::string message = "放置图节点 " + elementId;
        if (!targetId.empty()) {
            message += " 连接到节点 " + targetId;
        }
        session.addFrame(frameData, message);
    }
    
    return newState;
}

// 处理悬停事件
json handleElementHover(AnimationSession& session, const json& currentState, const std::string& elementId, const json& data) {
    json newState = currentState;
    std::string gameType = session.getGameType();
    
    // 添加悬停动画帧
    json frameData = newState;
    
    // 根据游戏类型更新元素状态
    if (gameType == "linkedlist") {
        // 链表元素悬停逻辑
        updateLinkedListElementState(newState, elementId, "hover");
        frameData["elements"][elementId]["state"] = "hover";
        session.addFrame(frameData, "悬停在链表节点 " + elementId + " 上");
    } 
    else if (gameType == "binarytree") {
        // 二叉树元素悬停逻辑
        updateBinaryTreeElementState(newState, elementId, "hover");
        frameData["nodes"][elementId]["state"] = "hover";
        session.addFrame(frameData, "悬停在二叉树节点 " + elementId + " 上");
    }
    else if (gameType == "graph") {
        // 图元素悬停逻辑
        updateGraphElementState(newState, elementId, "hover");
        frameData["nodes"][elementId]["state"] = "hover";
        session.addFrame(frameData, "悬停在图节点 " + elementId + " 上");
    }
    else if (gameType == "sorting") {
        // 排序元素悬停逻辑
        updateSortingElementState(newState, elementId, "hover");
        frameData["elements"][elementId]["state"] = "hover";
        session.addFrame(frameData, "悬停在排序元素 " + elementId + " 上");
    }
    else if (gameType == "maze") {
        // 获取位置信息
        int x = -1, y = -1;
        if (data.contains("position") && data["position"].is_object()) {
            if (data["position"].contains("x") && data["position"]["x"].is_number()) {
                x = data["position"]["x"];
            }
            if (data["position"].contains("y") && data["position"]["y"].is_number()) {
                y = data["position"]["y"];
            }
        }
        
        // 迷宫元素悬停逻辑
        if (x >= 0 && y >= 0) {
            updateMazeElementState(newState, elementId, "hover", x, y);
            frameData["cells"][y][x]["state"] = "hover";
            session.addFrame(frameData, "悬停在迷宫单元格 (" + std::to_string(x) + "," + std::to_string(y) + ")");
        }
    }
    
    return newState;
}

// 以下是辅助函数，用于更新不同游戏类型元素的状态

// 更新链表元素状态
void updateLinkedListElementState(json& state, const std::string& elementId, const std::string& newState) {
    if (state.contains("elements") && state["elements"].contains(elementId)) {
        state["elements"][elementId]["state"] = newState;
    }
}

// 更新链表元素位置
void updateLinkedListElementPosition(json& state, const std::string& elementId, int x, int y) {
    if (state.contains("elements") && state["elements"].contains(elementId)) {
        state["elements"][elementId]["position"]["x"] = x;
        state["elements"][elementId]["position"]["y"] = y;
    }
}

// 更新栈元素状态
void updateStackElementState(json& state, const std::string& elementId, const std::string& newState) {
    if (state.contains("elements") && state["elements"].contains(elementId)) {
        state["elements"][elementId]["state"] = newState;
    }
}

// 更新队列元素状态
void updateQueueElementState(json& state, const std::string& elementId, const std::string& newState) {
    if (state.contains("elements") && state["elements"].contains(elementId)) {
        state["elements"][elementId]["state"] = newState;
    }
}

// 更新二叉树元素状态
void updateBinaryTreeElementState(json& state, const std::string& elementId, const std::string& newState) {
    if (state.contains("nodes") && state["nodes"].contains(elementId)) {
        state["nodes"][elementId]["state"] = newState;
    }
}

// 更新二叉树元素位置
void updateBinaryTreeElementPosition(json& state, const std::string& elementId, int x, int y) {
    if (state.contains("nodes") && state["nodes"].contains(elementId)) {
        state["nodes"][elementId]["position"]["x"] = x;
        state["nodes"][elementId]["position"]["y"] = y;
    }
}

// 更新图元素状态
void updateGraphElementState(json& state, const std::string& elementId, const std::string& newState) {
    if (state.contains("nodes") && state["nodes"].contains(elementId)) {
        state["nodes"][elementId]["state"] = newState;
    }
}

// 更新图元素位置
void updateGraphElementPosition(json& state, const std::string& elementId, int x, int y) {
    if (state.contains("nodes") && state["nodes"].contains(elementId)) {
        state["nodes"][elementId]["position"]["x"] = x;
        state["nodes"][elementId]["position"]["y"] = y;
    }
}

// 更新排序元素状态
void updateSortingElementState(json& state, const std::string& elementId, const std::string& newState) {
    if (state.contains("elements") && state["elements"].contains(elementId)) {
        state["elements"][elementId]["state"] = newState;
    }
}

// 更新搜索元素状态
void updateSearchElementState(json& state, const std::string& elementId, const std::string& newState) {
    if (state.contains("elements") && state["elements"].contains(elementId)) {
        state["elements"][elementId]["state"] = newState;
    }
}

// 更新迷宫元素状态
void updateMazeElementState(json& state, const std::string& elementId, const std::string& newState, int x, int y) {
    if (state.contains("cells") && state["cells"].is_array() && 
        y >= 0 && y < state["cells"].size() && 
        x >= 0 && x < state["cells"][y].size()) {
        state["cells"][y][x]["state"] = newState;
    }
}

// 游戏交互API处理
crow::response handleGameElementClick(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        // 确保包含必要参数
        if (!bodyData.contains("sessionId") || !bodyData.contains("elementId")) {
            return ResponseBuilder::buildErrorResponse("缺少必要参数: sessionId, elementId");
        }
        
        // 添加点击类型
        bodyData["actionType"] = "click";
        
        // 调用通用交互处理函数
        return handleGameInteract(req);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("处理点击事件发生错误: ") + e.what());
    }
}

crow::response handleGameElementDrag(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        // 确保包含必要参数
        if (!bodyData.contains("sessionId") || !bodyData.contains("elementId") || 
            !bodyData.contains("position")) {
            return ResponseBuilder::buildErrorResponse("缺少必要参数: sessionId, elementId, position");
        }
        
        // 添加拖拽类型
        bodyData["actionType"] = "drag";
        
        // 调用通用交互处理函数
        return handleGameInteract(req);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("处理拖拽事件发生错误: ") + e.what());
    }
}

crow::response handleGameElementDrop(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        // 确保包含必要参数
        if (!bodyData.contains("sessionId") || !bodyData.contains("elementId") || 
            !bodyData.contains("position")) {
            return ResponseBuilder::buildErrorResponse("缺少必要参数: sessionId, elementId, position");
        }
        
        // 添加放置类型
        bodyData["actionType"] = "drop";
        
        // 调用通用交互处理函数
        return handleGameInteract(req);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("处理放置事件发生错误: ") + e.what());
    }
}

crow::response handleGameElementHover(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        // 确保包含必要参数
        if (!bodyData.contains("sessionId") || !bodyData.contains("elementId")) {
            return ResponseBuilder::buildErrorResponse("缺少必要参数: sessionId, elementId");
        }
        
        // 添加悬停类型
        bodyData["actionType"] = "hover";
        
        // 调用通用交互处理函数
        return handleGameInteract(req);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("处理悬停事件发生错误: ") + e.what());
    }
} 