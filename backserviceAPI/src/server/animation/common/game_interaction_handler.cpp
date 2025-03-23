#include "game_interaction_handler.h"
#include "response_builder.h"
#include "../implementations/dp/dp_animations.h"
#include <sstream>
#include <algorithm>

// 分割字符串辅助函数
std::vector<std::string> GameInteractionHandler::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// 验证会话ID
bool GameInteractionHandler::validateSession(const std::string& sessionId, AnimationSession& session) {
    if (sessionId.empty()) {
        return false;
    }
    auto animationManager = AnimationManager::getInstance();
    if (!animationManager->hasSession(sessionId)) {
        return false;
    }
    session = animationManager->getSession(sessionId);
    return true;
}

// 处理LCS单元格点击
json GameInteractionHandler::handleLCSCellClick(const std::string& cellId, AnimationSession& session) {
    auto tokens = split(cellId, '-');
    if (tokens.size() != 3) {
        return json{{"error", "Invalid cell ID format"}};
    }
    
    int i = std::stoi(tokens[1]);
    int j = std::stoi(tokens[2]);
    
    json state = session.getGameState();
    if (state["cells"].is_null()) {
        return json{{"error", "Invalid game state"}};
    }
    
    // 更新单元格状态
    state["cells"][i][j]["state"] = "clicked";
    
    // 获取依赖单元格
    json dependencies = state["cells"][i][j]["dependencies"];
    if (!dependencies.is_null()) {
        for (const auto& dep : dependencies) {
            auto depTokens = split(dep["id"].get<std::string>(), '-');
            int depI = std::stoi(depTokens[1]);
            int depJ = std::stoi(depTokens[2]);
            state["cells"][depI][depJ]["state"] = "dependency";
        }
    }
    
    session.updateGameState(state);
    return state;
}

// 处理二叉树节点点击
json GameInteractionHandler::handleBinaryTreeNodeClick(const std::string& nodeId, AnimationSession& session) {
    json state = session.getGameState();
    if (state["nodes"].is_null()) {
        return json{{"error", "Invalid game state"}};
    }
    
    // 更新节点状态
    for (auto& node : state["nodes"]) {
        if (node["id"] == nodeId) {
            node["state"] = "clicked";
        } else {
            node["state"] = "normal";
        }
    }
    
    session.updateGameState(state);
    return state;
}

// 处理图节点点击
json GameInteractionHandler::handleGraphNodeClick(const std::string& nodeId, AnimationSession& session) {
    json state = session.getGameState();
    if (state["nodes"].is_null()) {
        return json{{"error", "Invalid game state"}};
    }
    
    // 更新节点状态
    for (auto& node : state["nodes"]) {
        if (node["id"] == nodeId) {
            node["state"] = "clicked";
        } else {
            node["state"] = "normal";
        }
    }
    
    session.updateGameState(state);
    return state;
}

// 处理点击事件
crow::response GameInteractionHandler::handleClick(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("elementId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing required parameters", "interaction").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        std::string elementId = bodyData["elementId"];
        
        AnimationSession session;
        if (!validateSession(sessionId, session)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "interaction").dump());
        }
        
        json result;
        if (elementId.find("cell") != std::string::npos) {
            result = handleLCSCellClick(elementId, session);
        } else if (elementId.find("node") != std::string::npos) {
            if (session.getGameType() == "binarytree") {
                result = handleBinaryTreeNodeClick(elementId, session);
            } else if (session.getGameType() == "graph") {
                result = handleGraphNodeClick(elementId, session);
            }
        }
        
        if (result.contains("error")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, result["error"], "interaction").dump());
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(result, "interaction").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Click event handling error: ") + e.what(), "interaction").dump());
    }
}

// 处理拖拽事件
crow::response GameInteractionHandler::handleDrag(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("elementId") || 
            !bodyData.contains("position")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing required parameters", "interaction").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        std::string elementId = bodyData["elementId"];
        json position = bodyData["position"];
        
        AnimationSession session;
        if (!validateSession(sessionId, session)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "interaction").dump());
        }
        
        json state = session.getGameState();
        
        // 更新元素位置
        if (state["nodes"].is_null()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid game state", "interaction").dump());
        }
        
        for (auto& node : state["nodes"]) {
            if (node["id"] == elementId) {
                node["x"] = position["x"];
                node["y"] = position["y"];
                node["state"] = "dragging";
                break;
            }
        }
        
        session.updateGameState(state);
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "interaction").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Drag event handling error: ") + e.what(), "interaction").dump());
    }
}

// 处理放置事件
crow::response GameInteractionHandler::handleDrop(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("elementId") || 
            !bodyData.contains("position")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing required parameters", "interaction").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        std::string elementId = bodyData["elementId"];
        json position = bodyData["position"];
        
        AnimationSession session;
        if (!validateSession(sessionId, session)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "interaction").dump());
        }
        
        json state = session.getGameState();
        
        // 更新元素位置和状态
        if (state["nodes"].is_null()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid game state", "interaction").dump());
        }
        
        for (auto& node : state["nodes"]) {
            if (node["id"] == elementId) {
                node["x"] = position["x"];
                node["y"] = position["y"];
                node["state"] = "normal";
                break;
            }
        }
        
        session.updateGameState(state);
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "interaction").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Drop event handling error: ") + e.what(), "interaction").dump());
    }
}

// 处理悬停事件
crow::response GameInteractionHandler::handleHover(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("elementId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing required parameters", "interaction").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        std::string elementId = bodyData["elementId"];
        
        AnimationSession session;
        if (!validateSession(sessionId, session)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "interaction").dump());
        }
        
        json state = session.getGameState();
        
        if (elementId.find("cell") != std::string::npos) {
            auto tokens = split(elementId, '-');
            if (tokens.size() == 3) {
                int i = std::stoi(tokens[1]);
                int j = std::stoi(tokens[2]);
                
                // 更新单元格状态
                state["cells"][i][j]["state"] = "hovered";
                
                // 高亮依赖单元格
                json dependencies = state["cells"][i][j]["dependencies"];
                if (!dependencies.is_null()) {
                    for (const auto& dep : dependencies) {
                        auto depTokens = split(dep["id"].get<std::string>(), '-');
                        int depI = std::stoi(depTokens[1]);
                        int depJ = std::stoi(depTokens[2]);
                        state["cells"][depI][depJ]["state"] = "dependency";
                    }
                }
            }
        } else {
            // 处理其他类型元素的悬停
            if (!state["nodes"].is_null()) {
                for (auto& node : state["nodes"]) {
                    if (node["id"] == elementId) {
                        node["state"] = "hovered";
                    } else if (node["state"] == "hovered") {
                        node["state"] = "normal";
                    }
                }
            }
        }
        
        session.updateGameState(state);
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "interaction").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Hover event handling error: ") + e.what(), "interaction").dump());
    }
}

// 处理通用交互事件
crow::response GameInteractionHandler::handleInteract(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("actionType") || 
            !bodyData.contains("elementId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing required parameters", "interaction").dump());
        }
        
        std::string actionType = bodyData["actionType"];
        
        if (actionType == "click") {
            return handleClick(req);
        } else if (actionType == "drag") {
            return handleDrag(req);
        } else if (actionType == "drop") {
            return handleDrop(req);
        } else if (actionType == "hover") {
            return handleHover(req);
        }
        
        return crow::response(400, ResponseBuilder::createErrorResponse(
            400, "Invalid action type", "interaction").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Interaction handling error: ") + e.what(), "interaction").dump());
    }
} 