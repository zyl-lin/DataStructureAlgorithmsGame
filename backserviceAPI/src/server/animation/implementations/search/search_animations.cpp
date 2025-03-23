#include "search_animations.h"
#include "../../common/response_builder.h"
#include <algorithm>
#include <ctime>

// 创建初始搜索状态
json createInitialSearchState(const std::string& searchType) {
    json state;
    state["searchType"] = searchType;
    state["array"] = {11, 22, 25, 34, 64, 90};  // 默认有序数组
    state["target"] = 34;  // 默认目标值
    state["currentIndex"] = 0;
    state["comparingIndex"] = -1;
    state["left"] = -1;
    state["right"] = -1;
    state["middle"] = -1;
    state["found"] = false;
    state["completed"] = false;
    state["description"] = "开始" + searchType + "搜索";
    
    return state;
}

// 线性搜索算法处理函数
crow::response handleLinearSearchAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "search").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "search").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "search") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是搜索类型", "search").dump());
        }
        
        currentState["searchType"] = "linearSearch";
        createLinearSearchAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "linearSearch"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "search").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建线性搜索动画发生错误: ") + e.what(), "search").dump());
    }
}

// 线性搜索动画帧生成函数
json createLinearSearchAnimationFrames(AnimationSession& session, json& currentState) {
    std::vector<int> arr = currentState["array"].get<std::vector<int>>();
    int target = currentState["target"].get<int>();
    int n = arr.size();
    
    // 生成初始帧
    json frame = currentState;
    frame["currentIndex"] = 0;
    frame["comparingIndex"] = -1;
    session.addFrame(frame, "开始线性搜索，目标值: " + std::to_string(target));
    
    // 线性搜索过程
    for (int i = 0; i < n; i++) {
        // 比较帧
        frame = currentState;
        frame["currentIndex"] = i;
        frame["comparingIndex"] = i;
        std::string compareDesc = "比较元素 " + std::to_string(arr[i]) + 
                                " 和目标值 " + std::to_string(target);
        session.addFrame(frame, compareDesc);
        
        if (arr[i] == target) {
            frame = currentState;
            frame["currentIndex"] = i;
            frame["found"] = true;
            frame["completed"] = true;
            std::string foundDesc = "在位置 " + std::to_string(i) + 
                                  " 找到目标值 " + std::to_string(target);
            session.addFrame(frame, foundDesc);
            return frame;
        }
    }
    
    // 未找到目标值
    frame = currentState;
    frame["currentIndex"] = n;
    frame["found"] = false;
    frame["completed"] = true;
    std::string notFoundDesc = "未找到目标值 " + std::to_string(target);
    session.addFrame(frame, notFoundDesc);
    
    return frame;
}

// 二分搜索算法处理函数
crow::response handleBinarySearchAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "search").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "search").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "search") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是搜索类型", "search").dump());
        }
        
        currentState["searchType"] = "binarySearch";
        createBinarySearchAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "binarySearch"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "search").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建二分搜索动画发生错误: ") + e.what(), "search").dump());
    }
}

// 二分搜索动画帧生成函数
json createBinarySearchAnimationFrames(AnimationSession& session, json& currentState) {
    std::vector<int> arr = currentState["array"].get<std::vector<int>>();
    int target = currentState["target"].get<int>();
    
    // 生成初始帧
    json frame = currentState;
    frame["left"] = 0;
    frame["right"] = arr.size() - 1;
    frame["middle"] = -1;
    session.addFrame(frame, "开始二分搜索，目标值: " + std::to_string(target));
    
    // 开始二分搜索
    generateBinarySearchFrames(session, currentState, arr, target, 0, arr.size() - 1);
    
    return currentState;
}

// 二分搜索辅助函数
void generateBinarySearchFrames(AnimationSession& session, json& currentState,
                              std::vector<int>& arr, int target, int left, int right) {
    if (left <= right) {
        int mid = left + (right - left) / 2;
        
        // 更新当前状态
        json frame = currentState;
        frame["left"] = left;
        frame["right"] = right;
        frame["middle"] = mid;
        frame["currentIndex"] = mid;
        frame["comparingIndex"] = mid;
        
        std::string compareDesc = "比较中间元素 " + std::to_string(arr[mid]) + 
                                " 和目标值 " + std::to_string(target);
        session.addFrame(frame, compareDesc);
        
        if (arr[mid] == target) {
            frame["found"] = true;
            frame["completed"] = true;
            std::string foundDesc = "在位置 " + std::to_string(mid) + 
                                  " 找到目标值 " + std::to_string(target);
            session.addFrame(frame, foundDesc);
            currentState = frame;
            return;
        }
        
        if (arr[mid] > target) {
            std::string searchLeftDesc = "目标值小于中间元素，在左半部分继续搜索";
            session.addFrame(frame, searchLeftDesc);
            generateBinarySearchFrames(session, currentState, arr, target, left, mid - 1);
        } else {
            std::string searchRightDesc = "目标值大于中间元素，在右半部分继续搜索";
            session.addFrame(frame, searchRightDesc);
            generateBinarySearchFrames(session, currentState, arr, target, mid + 1, right);
        }
    } else {
        // 未找到目标值
        json frame = currentState;
        frame["found"] = false;
        frame["completed"] = true;
        frame["left"] = -1;
        frame["right"] = -1;
        frame["middle"] = -1;
        std::string notFoundDesc = "未找到目标值 " + std::to_string(target);
        session.addFrame(frame, notFoundDesc);
        currentState = frame;
    }
}

// 搜索算法单步执行处理函数
crow::response handleSearchStepAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "search").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "search").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (!currentState.contains("searchType")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的搜索算法类型", "search").dump());
        }
        
        std::string searchType = currentState["searchType"];
        json frame;
        
        if (searchType == "linearSearch") {
            frame = createLinearSearchAnimationFrames(session, currentState);
        } else if (searchType == "binarySearch") {
            frame = createBinarySearchAnimationFrames(session, currentState);
        } else {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "不支持的搜索算法类型", "search").dump());
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(frame, "search").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("执行搜索算法步骤时发生错误: ") + e.what(), "search").dump());
    }
}

// 获取搜索状态处理函数
crow::response handleGetSearchState(const crow::request& req) {
    try {
        std::string sessionId = req.url_params.get("sessionId") ? req.url_params.get("sessionId") : "";
        
        if (sessionId.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "search").dump());
        }
        
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "search").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(currentState, "search").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取搜索状态时发生错误: ") + e.what(), "search").dump());
    }
}

// 重置搜索状态处理函数
crow::response handleResetSearchState(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "search").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "search").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        std::string searchType = session.getGameState()["searchType"];
        
        // 创建新的初始状态
        json newState = createInitialSearchState(searchType);
        session.updateGameState(newState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "search").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置搜索状态时发生错误: ") + e.what(), "search").dump());
    }
} 