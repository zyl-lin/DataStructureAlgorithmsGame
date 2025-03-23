#include "sorting_animations.h"
#include "../../common/response_builder.h"
#include <algorithm>
#include <ctime>

// 创建初始排序状态
json createInitialSortingState(const std::string& sortingType) {
    json state;
    state["sortingType"] = sortingType;
    state["array"] = {64, 34, 25, 12, 22, 11, 90};  // 默认数组
    state["currentStep"] = 0;
    state["currentIndex"] = 0;
    state["comparingIndex"] = -1;
    state["swappingIndices"] = json::array();
    state["completed"] = false;
    state["description"] = "开始" + sortingType + "排序";
    
    // 添加算法特定状态
    if (sortingType == "quickSort") {
        state["pivotIndex"] = -1;
        state["partitionStart"] = -1;
        state["partitionEnd"] = -1;
    }
    
    return state;
}

// 冒泡排序算法处理函数
crow::response handleBubbleSortAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "sorting").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "sorting").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "sorting") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是排序类型", "sorting").dump());
        }
        
        currentState["sortingType"] = "bubbleSort";
        createBubbleSortAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "bubbleSort"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建冒泡排序动画发生错误: ") + e.what(), "sorting").dump());
    }
}

// 冒泡排序动画帧生成函数
json createBubbleSortAnimationFrames(AnimationSession& session, json& currentState) {
    std::vector<int> arr = currentState["array"].get<std::vector<int>>();
    int n = arr.size();
    
    // 生成初始帧
    json frame = currentState;
    frame["array"] = arr;
    frame["currentIndex"] = 0;
    frame["comparingIndex"] = -1;
    frame["swappingIndices"] = json::array();
    session.addFrame(frame, "开始冒泡排序");
    
    // 冒泡排序过程
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        
        for (int j = 0; j < n - i - 1; j++) {
            // 比较帧
            frame = currentState;
            frame["array"] = arr;
            frame["currentIndex"] = j;
            frame["comparingIndex"] = j + 1;
            frame["swappingIndices"] = json::array();
            std::string compareDesc = "比较元素 " + std::to_string(arr[j]) + 
                                    " 和 " + std::to_string(arr[j + 1]);
            session.addFrame(frame, compareDesc);
            
            if (arr[j] > arr[j + 1]) {
                // 交换帧
                std::swap(arr[j], arr[j + 1]);
                swapped = true;
                
                frame = currentState;
                frame["array"] = arr;
                frame["currentIndex"] = j;
                frame["comparingIndex"] = j + 1;
                frame["swappingIndices"] = json::array({j, j + 1});
                std::string swapDesc = "交换元素 " + std::to_string(arr[j + 1]) + 
                                     " 和 " + std::to_string(arr[j]);
                session.addFrame(frame, swapDesc);
            }
        }
        
        if (!swapped) {
            break;
        }
    }
    
    // 生成最终帧
    frame = currentState;
    frame["array"] = arr;
    frame["completed"] = true;
    frame["currentIndex"] = -1;
    frame["comparingIndex"] = -1;
    frame["swappingIndices"] = json::array();
    session.addFrame(frame, "冒泡排序完成");
    
    return frame;
}

// 选择排序算法处理函数
crow::response handleSelectionSortAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "sorting").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "sorting").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "sorting") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是排序类型", "sorting").dump());
        }
        
        currentState["sortingType"] = "selectionSort";
        createSelectionSortAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "selectionSort"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建选择排序动画发生错误: ") + e.what(), "sorting").dump());
    }
}

// 选择排序动画帧生成函数
json createSelectionSortAnimationFrames(AnimationSession& session, json& currentState) {
    std::vector<int> arr = currentState["array"].get<std::vector<int>>();
    int n = arr.size();
    
    // 生成初始帧
    json frame = currentState;
    frame["array"] = arr;
    frame["currentIndex"] = 0;
    frame["comparingIndex"] = -1;
    frame["swappingIndices"] = json::array();
    session.addFrame(frame, "开始选择排序");
    
    // 选择排序过程
    for (int i = 0; i < n - 1; i++) {
        int minIdx = i;
        
        // 查找最小值
        for (int j = i + 1; j < n; j++) {
            frame = currentState;
            frame["array"] = arr;
            frame["currentIndex"] = i;
            frame["comparingIndex"] = j;
            frame["minIndex"] = minIdx;
            std::string compareDesc = "比较元素 " + std::to_string(arr[j]) + 
                                    " 和当前最小值 " + std::to_string(arr[minIdx]);
            session.addFrame(frame, compareDesc);
            
            if (arr[j] < arr[minIdx]) {
                minIdx = j;
                frame = currentState;
                frame["array"] = arr;
                frame["currentIndex"] = i;
                frame["comparingIndex"] = j;
                frame["minIndex"] = minIdx;
                std::string newMinDesc = "找到新的最小值 " + std::to_string(arr[minIdx]);
                session.addFrame(frame, newMinDesc);
            }
        }
        
        // 交换元素
        if (minIdx != i) {
            std::swap(arr[i], arr[minIdx]);
            frame = currentState;
            frame["array"] = arr;
            frame["currentIndex"] = i;
            frame["swappingIndices"] = json::array({i, minIdx});
            std::string swapDesc = "交换元素 " + std::to_string(arr[i]) + 
                                 " 和 " + std::to_string(arr[minIdx]);
            session.addFrame(frame, swapDesc);
        }
    }
    
    // 生成最终帧
    frame = currentState;
    frame["array"] = arr;
    frame["completed"] = true;
    frame["currentIndex"] = -1;
    frame["comparingIndex"] = -1;
    frame["swappingIndices"] = json::array();
    session.addFrame(frame, "选择排序完成");
    
    return frame;
}

// 插入排序算法处理函数
crow::response handleInsertionSortAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "sorting").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "sorting").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "sorting") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是排序类型", "sorting").dump());
        }
        
        currentState["sortingType"] = "insertionSort";
        createInsertionSortAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "insertionSort"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建插入排序动画发生错误: ") + e.what(), "sorting").dump());
    }
}

// 插入排序动画帧生成函数
json createInsertionSortAnimationFrames(AnimationSession& session, json& currentState) {
    std::vector<int> arr = currentState["array"].get<std::vector<int>>();
    int n = arr.size();
    
    // 生成初始帧
    json frame = currentState;
    frame["array"] = arr;
    frame["currentIndex"] = 1;
    frame["comparingIndex"] = -1;
    frame["swappingIndices"] = json::array();
    session.addFrame(frame, "开始插入排序");
    
    // 插入排序过程
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        
        // 显示当前要插入的元素
        frame = currentState;
        frame["array"] = arr;
        frame["currentIndex"] = i;
        frame["key"] = key;
        std::string keyDesc = "当前要插入的元素: " + std::to_string(key);
        session.addFrame(frame, keyDesc);
        
        // 移动元素
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            frame = currentState;
            frame["array"] = arr;
            frame["currentIndex"] = i;
            frame["comparingIndex"] = j;
            frame["swappingIndices"] = json::array({j, j + 1});
            std::string moveDesc = "移动元素 " + std::to_string(arr[j]) + " 到位置 " + std::to_string(j + 1);
            session.addFrame(frame, moveDesc);
            j--;
        }
        
        arr[j + 1] = key;
        frame = currentState;
        frame["array"] = arr;
        frame["currentIndex"] = j + 1;
        std::string insertDesc = "将元素 " + std::to_string(key) + " 插入到位置 " + std::to_string(j + 1);
        session.addFrame(frame, insertDesc);
    }
    
    // 生成最终帧
    frame = currentState;
    frame["array"] = arr;
    frame["completed"] = true;
    frame["currentIndex"] = -1;
    frame["comparingIndex"] = -1;
    frame["swappingIndices"] = json::array();
    session.addFrame(frame, "插入排序完成");
    
    return frame;
}

// 快速排序算法处理函数
crow::response handleQuickSortAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "sorting").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "sorting").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (session.getGameType() != "sorting") {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "当前会话不是排序类型", "sorting").dump());
        }
        
        currentState["sortingType"] = "quickSort";
        createQuickSortAnimationFrames(session, currentState);
        
        json responseData = {
            {"sessionId", sessionId},
            {"animationType", "quickSort"},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("创建快速排序动画发生错误: ") + e.what(), "sorting").dump());
    }
}

// 快速排序动画帧生成函数
json createQuickSortAnimationFrames(AnimationSession& session, json& currentState) {
    std::vector<int> arr = currentState["array"].get<std::vector<int>>();
    
    // 生成初始帧
    json frame = currentState;
    frame["array"] = arr;
    frame["pivotIndex"] = -1;
    frame["partitionStart"] = -1;
    frame["partitionEnd"] = -1;
    session.addFrame(frame, "开始快速排序");
    
    // 开始快速排序
    generateQuickSortFrames(session, currentState, arr, 0, arr.size() - 1);
    
    // 生成最终帧
    frame = currentState;
    frame["array"] = arr;
    frame["completed"] = true;
    frame["pivotIndex"] = -1;
    frame["partitionStart"] = -1;
    frame["partitionEnd"] = -1;
    session.addFrame(frame, "快速排序完成");
    
    return frame;
}

// 快速排序辅助函数
void generateQuickSortFrames(AnimationSession& session, json& currentState,
                           std::vector<int>& arr, int low, int high) {
    if (low < high) {
        // 选择基准元素
        json frame = currentState;
        frame["array"] = arr;
        frame["pivotIndex"] = high;
        frame["partitionStart"] = low;
        frame["partitionEnd"] = high;
        std::string pivotDesc = "选择基准元素: " + std::to_string(arr[high]);
        session.addFrame(frame, pivotDesc);
        
        // 分区过程
        int pi = partition(arr, low, high, session, currentState);
        
        // 递归排序左半部分
        generateQuickSortFrames(session, currentState, arr, low, pi - 1);
        
        // 递归排序右半部分
        generateQuickSortFrames(session, currentState, arr, pi + 1, high);
    }
}

// 分区函数
int partition(std::vector<int>& arr, int low, int high,
             AnimationSession& session, json& currentState) {
    int pivot = arr[high];
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        // 比较元素
        json frame = currentState;
        frame["array"] = arr;
        frame["pivotIndex"] = high;
        frame["comparingIndex"] = j;
        frame["currentIndex"] = i + 1;
        std::string compareDesc = "比较元素 " + std::to_string(arr[j]) + 
                                " 和基准元素 " + std::to_string(pivot);
        session.addFrame(frame, compareDesc);
        
        if (arr[j] < pivot) {
            i++;
            std::swap(arr[i], arr[j]);
            
            if (i != j) {
                frame = currentState;
                frame["array"] = arr;
                frame["pivotIndex"] = high;
                frame["swappingIndices"] = json::array({i, j});
                std::string swapDesc = "交换元素 " + std::to_string(arr[i]) + 
                                     " 和 " + std::to_string(arr[j]);
                session.addFrame(frame, swapDesc);
            }
        }
    }
    
    std::swap(arr[i + 1], arr[high]);
    
    // 显示基准元素放置到最终位置
    json frame = currentState;
    frame["array"] = arr;
    frame["pivotIndex"] = i + 1;
    frame["swappingIndices"] = json::array({i + 1, high});
    std::string finalDesc = "将基准元素 " + std::to_string(pivot) + 
                           " 放置到最终位置 " + std::to_string(i + 1);
    session.addFrame(frame, finalDesc);
    
    return i + 1;
}

// 排序算法单步执行处理函数
crow::response handleSortingStepAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "sorting").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "sorting").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        if (!currentState.contains("sortingType")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的排序算法类型", "sorting").dump());
        }
        
        std::string sortingType = currentState["sortingType"];
        json frame;
        
        if (sortingType == "bubbleSort") {
            frame = createBubbleSortAnimationFrames(session, currentState);
        } else if (sortingType == "selectionSort") {
            frame = createSelectionSortAnimationFrames(session, currentState);
        } else if (sortingType == "insertionSort") {
            frame = createInsertionSortAnimationFrames(session, currentState);
        } else if (sortingType == "quickSort") {
            frame = createQuickSortAnimationFrames(session, currentState);
        } else {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "不支持的排序算法类型", "sorting").dump());
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(frame, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("执行排序算法步骤时发生错误: ") + e.what(), "sorting").dump());
    }
}

// 获取排序状态处理函数
crow::response handleGetSortingState(const crow::request& req) {
    try {
        std::string sessionId = req.url_params.get("sessionId") ? req.url_params.get("sessionId") : "";
        
        if (sessionId.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "sorting").dump());
        }
        
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "sorting").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        json currentState = session.getGameState();
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(currentState, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("获取排序状态时发生错误: ") + e.what(), "sorting").dump());
    }
}

// 重置排序状态处理函数
crow::response handleResetSortingState(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数: sessionId", "sorting").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "无效的会话ID", "sorting").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        std::string sortingType = session.getGameState()["sortingType"];
        
        // 创建新的初始状态
        json newState = createInitialSortingState(sortingType);
        session.updateGameState(newState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置排序状态时发生错误: ") + e.what(), "sorting").dump());
    }
} 