#include "animation_routes.h"
#include "animation_manager.h"
#include "game_interaction_handler.h"
#include "../response_builder.h"
#include <crow.h>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

// 注册动画相关的路由
void registerAnimationRoutes(crow::App<crow::CORSHandler>& app) {
    // 创建会话路由
    CROW_ROUTE(app, "/api/animation/create-session").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleCreateSession(req);
    });
    
    CROW_ROUTE(app, "/api/animation/pause").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleAnimationPause(req);
    });

    CROW_ROUTE(app, "/api/animation/resume").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleAnimationResume(req);
    });

    CROW_ROUTE(app, "/api/animation/stop").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleAnimationStop(req);
    });

    CROW_ROUTE(app, "/api/animation/status").methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {
        return handleAnimationStatus(req);
    });

    // 动画帧控制路由
    CROW_ROUTE(app, "/api/animation/nextFrame").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleNextFrame(req);
    });

    CROW_ROUTE(app, "/api/animation/prevFrame").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handlePrevFrame(req);
    });

    CROW_ROUTE(app, "/api/animation/frames").methods(crow::HTTPMethod::GET)
    ([](const crow::request& req) {
        return handleGetFrames(req);
    });

    CROW_ROUTE(app, "/api/animation/setSpeed").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleSetSpeed(req);
    });

    // 游戏元素交互路由
    CROW_ROUTE(app, "/api/games/interact").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleGameInteract(req);
    });

    CROW_ROUTE(app, "/api/games/click").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleGameElementClick(req);
    });

    CROW_ROUTE(app, "/api/games/drag").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleGameElementDrag(req);
    });

    CROW_ROUTE(app, "/api/games/drop").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleGameElementDrop(req);
    });
    
    // 新增API路由 - 游戏元素悬停
    CROW_ROUTE(app, "/api/games/hover").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        return handleGameElementHover(req);
    });
}

// 创建会话接口
crow::response handleCreateSession(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        // 必要参数验证
        if (!bodyData.contains("gameType") || !bodyData.contains("animationType")) {
            return ResponseBuilder::buildErrorResponse("缺少必要参数: gameType, animationType");
        }
        
        std::string gameType = bodyData["gameType"];
        std::string animationType = bodyData["animationType"];
        
        // 创建会话
        std::string sessionId = animationManager->createSession(gameType, animationType);
        
        // 创建初始化状态
        json initialState;
        if (gameType == "linkedlist") {
            // 链表初始状态
            initialState = createInitialLinkedListState();
        } 
        else if (gameType == "stack") {
            // 栈初始状态
            initialState = createInitialStackState();
        } 
        else if (gameType == "queue") {
            // 队列初始状态
            initialState = createInitialQueueState();
        } 
        else if (gameType == "binarytree") {
            // 二叉树初始状态
            initialState = createInitialBinaryTreeState();
        } 
        else if (gameType == "graph") {
            // 图初始状态
            initialState = createInitialGraphState();
        }
        else if (gameType == "sorting") {
            // 排序初始状态
            initialState = createInitialSortingState();
        }
        else if (gameType == "search") {
            // 搜索初始状态
            initialState = createInitialSearchState();
        }
        else if (gameType == "maze") {
            // 迷宫初始状态
            initialState = createInitialMazeState();
        }
        else if (gameType == "dp") {
            // 动态规划初始状态
            initialState = createInitialDPState();
        }
        else if (gameType == "greedy") {
            // 贪心算法初始状态
            initialState = createInitialGreedyState();
        }
        else {
            // 默认空状态
            initialState = json::object();
        }
        
        // 更新会话状态
        animationManager->getSession(sessionId).updateGameState(initialState);
        
        // 构建响应
        json responseData = {
            {"sessionId", sessionId},
            {"gameType", gameType},
            {"animationType", animationType},
            {"initialState", initialState}
        };
        
        return ResponseBuilder::buildSuccessResponse("会话创建成功", responseData);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("创建会话发生错误: ") + e.what());
    }
}

// 动画控制函数实现
crow::response handleAnimationPause(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::buildErrorResponse("缺少会话ID参数");
        }
        
        std::string sessionId = bodyData["sessionId"];
        bool success = animationManager->pauseAnimation(sessionId);
        
        if (success) {
            return ResponseBuilder::buildSuccessResponse("动画已暂停", nullptr);
        } else {
            return ResponseBuilder::buildErrorResponse("无法暂停动画，会话不存在或状态无效");
        }
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("暂停动画发生错误: ") + e.what());
    }
}

crow::response handleAnimationResume(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::buildErrorResponse("缺少会话ID参数");
        }
        
        std::string sessionId = bodyData["sessionId"];
        bool success = animationManager->resumeAnimation(sessionId);
        
        if (success) {
            return ResponseBuilder::buildSuccessResponse("动画已恢复", nullptr);
        } else {
            return ResponseBuilder::buildErrorResponse("无法恢复动画，会话不存在或状态无效");
        }
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("恢复动画发生错误: ") + e.what());
    }
}

crow::response handleAnimationStop(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::buildErrorResponse("缺少会话ID参数");
        }
        
        std::string sessionId = bodyData["sessionId"];
        bool success = animationManager->stopAnimation(sessionId);
        
        if (success) {
            return ResponseBuilder::buildSuccessResponse("动画已停止", nullptr);
        } else {
            return ResponseBuilder::buildErrorResponse("无法停止动画，会话不存在或状态无效");
        }
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("停止动画发生错误: ") + e.what());
    }
}

crow::response handleAnimationStatus(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        
        // 从查询字符串中获取会话ID
        std::string sessionId = req.url_params.get("sessionId") ? req.url_params.get("sessionId") : "";
        
        if (sessionId.empty()) {
            return ResponseBuilder::buildErrorResponse("缺少会话ID参数");
        }
        
        if (!animationManager->hasSession(sessionId)) {
            return ResponseBuilder::buildErrorResponse("会话不存在");
        }
        
        AnimationStatus status = animationManager->getAnimationStatus(sessionId);
        std::string statusStr;
        
        switch (status) {
            case AnimationStatus::IDLE:
                statusStr = "idle";
                break;
            case AnimationStatus::RUNNING:
                statusStr = "running";
                break;
            case AnimationStatus::PAUSED:
                statusStr = "paused";
                break;
            case AnimationStatus::COMPLETED:
                statusStr = "completed";
                break;
            default:
                statusStr = "unknown";
        }
        
        json statusData = {
            {"status", statusStr},
            {"sessionId", sessionId},
            {"currentFrame", animationManager->getSession(sessionId).getCurrentFrameIndex()},
            {"totalFrames", animationManager->getSession(sessionId).getFrameCount()},
            {"speed", animationManager->getSession(sessionId).getSpeed()}
        };
        
        return ResponseBuilder::buildSuccessResponse("获取动画状态成功", statusData);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("获取动画状态发生错误: ") + e.what());
    }
}

// 动画帧控制函数实现
crow::response handleNextFrame(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::buildErrorResponse("缺少会话ID参数");
        }
        
        std::string sessionId = bodyData["sessionId"];
        json frameData = animationManager->getNextFrame(sessionId);
        
        if (frameData.is_null()) {
            return ResponseBuilder::buildErrorResponse("无法获取下一帧，会话不存在或已到最后一帧");
        }
        
        return ResponseBuilder::buildSuccessResponse("获取下一帧成功", frameData);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("获取下一帧发生错误: ") + e.what());
    }
}

crow::response handlePrevFrame(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return ResponseBuilder::buildErrorResponse("缺少会话ID参数");
        }
        
        std::string sessionId = bodyData["sessionId"];
        json frameData = animationManager->getPrevFrame(sessionId);
        
        if (frameData.is_null()) {
            return ResponseBuilder::buildErrorResponse("无法获取上一帧，会话不存在或已到第一帧");
        }
        
        return ResponseBuilder::buildSuccessResponse("获取上一帧成功", frameData);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("获取上一帧发生错误: ") + e.what());
    }
}

crow::response handleGetFrames(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        
        // 从查询字符串中获取会话ID
        std::string sessionId = req.url_params.get("sessionId") ? req.url_params.get("sessionId") : "";
        
        if (sessionId.empty()) {
            return ResponseBuilder::buildErrorResponse("缺少会话ID参数");
        }
        
        json framesData = animationManager->getAllFrames(sessionId);
        
        if (framesData.is_null()) {
            return ResponseBuilder::buildErrorResponse("无法获取帧信息，会话不存在");
        }
        
        return ResponseBuilder::buildSuccessResponse("获取所有帧成功", framesData);
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("获取帧信息发生错误: ") + e.what());
    }
}

crow::response handleSetSpeed(const crow::request& req) {
    try {
        auto animationManager = AnimationManager::getInstance();
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("speed")) {
            return ResponseBuilder::buildErrorResponse("缺少必要参数: sessionId, speed");
        }
        
        std::string sessionId = bodyData["sessionId"];
        int speed = bodyData["speed"];
        
        bool success = animationManager->setAnimationSpeed(sessionId, speed);
        
        if (success) {
            return ResponseBuilder::buildSuccessResponse("设置动画速度成功", json{{"speed", speed}});
        } else {
            return ResponseBuilder::buildErrorResponse("无法设置动画速度，会话不存在");
        }
    } catch (const std::exception& e) {
        return ResponseBuilder::buildErrorResponse(std::string("设置动画速度发生错误: ") + e.what());
    }
}

// 创建各种游戏类型的初始状态
json createInitialLinkedListState() {
    // 创建一个简单的链表状态作为示例
    json state;
    
    // 添加节点
    state["elements"] = {
        {"node-1", {
            {"value", 10},
            {"position", {{"x", 100}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"node-2", {
            {"value", 20},
            {"position", {{"x", 200}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"node-3", {
            {"value", 30},
            {"position", {{"x", 300}, {"y", 150}}},
            {"state", "normal"}
        }}
    };
    
    // 添加边（链接）
    state["edges"] = {
        {"edge-1", {
            {"source", "node-1"},
            {"target", "node-2"}
        }},
        {"edge-2", {
            {"source", "node-2"},
            {"target", "node-3"}
        }}
    };
    
    return state;
}

json createInitialStackState() {
    // 创建一个简单的栈状态作为示例
    json state;
    
    // 添加栈元素
    state["elements"] = {
        {"stack-1", {
            {"value", 10},
            {"position", {{"x", 150}, {"y", 300}}},
            {"state", "normal"}
        }},
        {"stack-2", {
            {"value", 20},
            {"position", {{"x", 150}, {"y", 250}}},
            {"state", "normal"}
        }},
        {"stack-3", {
            {"value", 30},
            {"position", {{"x", 150}, {"y", 200}}},
            {"state", "normal"}
        }}
    };
    
    // 栈顶指针
    state["topIndex"] = 2;
    
    return state;
}

json createInitialQueueState() {
    // 创建一个简单的队列状态作为示例
    json state;
    
    // 添加队列元素
    state["elements"] = {
        {"queue-1", {
            {"value", 10},
            {"position", {{"x", 100}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"queue-2", {
            {"value", 20},
            {"position", {{"x", 200}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"queue-3", {
            {"value", 30},
            {"position", {{"x", 300}, {"y", 150}}},
            {"state", "normal"}
        }}
    };
    
    // 队列指针
    state["frontIndex"] = 0;
    state["rearIndex"] = 2;
    
    return state;
}

json createInitialBinaryTreeState() {
    // 创建一个简单的二叉树状态作为示例
    json state;
    
    // 添加节点
    state["nodes"] = {
        {"node-1", {
            {"value", 50},
            {"position", {{"x", 300}, {"y", 100}}},
            {"state", "normal"}
        }},
        {"node-2", {
            {"value", 30},
            {"position", {{"x", 200}, {"y", 200}}},
            {"state", "normal"}
        }},
        {"node-3", {
            {"value", 70},
            {"position", {{"x", 400}, {"y", 200}}},
            {"state", "normal"}
        }},
        {"node-4", {
            {"value", 20},
            {"position", {{"x", 150}, {"y", 300}}},
            {"state", "normal"}
        }},
        {"node-5", {
            {"value", 40},
            {"position", {{"x", 250}, {"y", 300}}},
            {"state", "normal"}
        }}
    };
    
    // 添加边（树连接）
    state["edges"] = {
        {"edge-1", {
            {"source", "node-1"},
            {"target", "node-2"},
            {"type", "left"}
        }},
        {"edge-2", {
            {"source", "node-1"},
            {"target", "node-3"},
            {"type", "right"}
        }},
        {"edge-3", {
            {"source", "node-2"},
            {"target", "node-4"},
            {"type", "left"}
        }},
        {"edge-4", {
            {"source", "node-2"},
            {"target", "node-5"},
            {"type", "right"}
        }}
    };
    
    // 根节点
    state["rootId"] = "node-1";
    
    return state;
}

json createInitialGraphState() {
    // 创建一个简单的图状态作为示例
    json state;
    
    // 添加节点
    state["nodes"] = {
        {"node-1", {
            {"value", "A"},
            {"position", {{"x", 200}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"node-2", {
            {"value", "B"},
            {"position", {{"x", 350}, {"y", 100}}},
            {"state", "normal"}
        }},
        {"node-3", {
            {"value", "C"},
            {"position", {{"x", 400}, {"y", 250}}},
            {"state", "normal"}
        }},
        {"node-4", {
            {"value", "D"},
            {"position", {{"x", 300}, {"y", 350}}},
            {"state", "normal"}
        }},
        {"node-5", {
            {"value", "E"},
            {"position", {{"x", 150}, {"y", 300}}},
            {"state", "normal"}
        }}
    };
    
    // 添加边（图连接）
    state["edges"] = {
        {"edge-1", {
            {"source", "node-1"},
            {"target", "node-2"},
            {"weight", 4}
        }},
        {"edge-2", {
            {"source", "node-1"},
            {"target", "node-5"},
            {"weight", 2}
        }},
        {"edge-3", {
            {"source", "node-2"},
            {"target", "node-3"},
            {"weight", 3}
        }},
        {"edge-4", {
            {"source", "node-3"},
            {"target", "node-4"},
            {"weight", 5}
        }},
        {"edge-5", {
            {"source", "node-4"},
            {"target", "node-5"},
            {"weight", 1}
        }},
        {"edge-6", {
            {"source", "node-5"},
            {"target", "node-3"},
            {"weight", 8}
        }}
    };
    
    return state;
}

json createInitialSortingState() {
    // 创建一个简单的排序状态作为示例
    json state;
    
    // 添加排序元素
    state["elements"] = {
        {"sort-1", {
            {"value", 64},
            {"position", {{"x", 100}, {"y", 150}}},
            {"state", "normal"},
            {"height", 64}
        }},
        {"sort-2", {
            {"value", 34},
            {"position", {{"x", 150}, {"y", 150}}},
            {"state", "normal"},
            {"height", 34}
        }},
        {"sort-3", {
            {"value", 25},
            {"position", {{"x", 200}, {"y", 150}}},
            {"state", "normal"},
            {"height", 25}
        }},
        {"sort-4", {
            {"value", 12},
            {"position", {{"x", 250}, {"y", 150}}},
            {"state", "normal"},
            {"height", 12}
        }},
        {"sort-5", {
            {"value", 22},
            {"position", {{"x", 300}, {"y", 150}}},
            {"state", "normal"},
            {"height", 22}
        }},
        {"sort-6", {
            {"value", 11},
            {"position", {{"x", 350}, {"y", 150}}},
            {"state", "normal"},
            {"height", 11}
        }},
        {"sort-7", {
            {"value", 90},
            {"position", {{"x", 400}, {"y", 150}}},
            {"state", "normal"},
            {"height", 90}
        }}
    };
    
    // 添加原始数组
    state["array"] = {64, 34, 25, 12, 22, 11, 90};
    
    // 添加排序类型
    state["sortType"] = "bubble";
    
    return state;
}

json createInitialSearchState() {
    // 创建一个简单的搜索状态作为示例
    json state;
    
    // 添加搜索元素
    state["elements"] = {
        {"search-1", {
            {"value", 11},
            {"position", {{"x", 100}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"search-2", {
            {"value", 22},
            {"position", {{"x", 150}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"search-3", {
            {"value", 33},
            {"position", {{"x", 200}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"search-4", {
            {"value", 44},
            {"position", {{"x", 250}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"search-5", {
            {"value", 55},
            {"position", {{"x", 300}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"search-6", {
            {"value", 66},
            {"position", {{"x", 350}, {"y", 150}}},
            {"state", "normal"}
        }},
        {"search-7", {
            {"value", 77},
            {"position", {{"x", 400}, {"y", 150}}},
            {"state", "normal"}
        }}
    };
    
    // 添加搜索目标
    state["target"] = 44;
    
    // 添加搜索类型
    state["searchType"] = "binary";
    
    return state;
}

json createInitialMazeState() {
    // 创建一个简单的迷宫状态作为示例
    json state;
    
    // 迷宫参数
    int width = 8;
    int height = 8;
    
    // 创建迷宫单元格
    json cells = json::array();
    for (int y = 0; y < height; y++) {
        json row = json::array();
        for (int x = 0; x < width; x++) {
            // 随机生成墙壁和通道
            bool isWall = (rand() % 4 == 0);  // 25%的几率是墙
            
            // 确保起点和终点不是墙
            if ((x == 0 && y == 0) || (x == width-1 && y == height-1)) {
                isWall = false;
            }
            
            json cell = {
                {"x", x},
                {"y", y},
                {"type", isWall ? "wall" : "path"},
                {"state", "normal"}
            };
            row.push_back(cell);
        }
        cells.push_back(row);
    }
    
    // 设置起点和终点
    cells[0][0]["type"] = "start";
    cells[height-1][width-1]["type"] = "end";
    
    state["cells"] = cells;
    state["width"] = width;
    state["height"] = height;
    state["startPoint"] = {{"x", 0}, {"y", 0}};
    state["endPoint"] = {{"x", width-1}, {"y", height-1}};
    
    return state;
}

// 创建动态规划初始状态
json createInitialDPState() {
    // 创建一个斐波那契数列DP问题的初始状态
    json state;
    
    // DP问题类型
    state["dpType"] = "fibonacci";
    
    // 设置输入值
    state["input"] = 6;  // 计算fib(6)
    
    // 初始化DP表格
    json table = json::array();
    table.push_back(json::array({0}));  // fib(0) = 0
    table.push_back(json::array({1}));  // fib(1) = 1
    
    state["table"] = table;
    state["computed"] = 1;  // 已经计算到fib(1)
    
    // 设置单元格状态
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
        }},
        {"cell-2", {
            {"value", "?"},
            {"position", {{"x", 300}, {"y", 150}}},
            {"state", "pending"}
        }},
        {"cell-3", {
            {"value", "?"},
            {"position", {{"x", 400}, {"y", 150}}},
            {"state", "pending"}
        }},
        {"cell-4", {
            {"value", "?"},
            {"position", {{"x", 500}, {"y", 150}}},
            {"state", "pending"}
        }},
        {"cell-5", {
            {"value", "?"},
            {"position", {{"x", 600}, {"y", 150}}},
            {"state", "pending"}
        }},
        {"cell-6", {
            {"value", "?"},
            {"position", {{"x", 700}, {"y", 150}}},
            {"state", "pending"}
        }}
    };
    
    // 设置依赖关系
    state["dependencies"] = {
        {"dep-2", {
            {"target", "cell-2"},
            {"sources", json::array({"cell-0", "cell-1"})}
        }},
        {"dep-3", {
            {"target", "cell-3"},
            {"sources", json::array({"cell-1", "cell-2"})}
        }},
        {"dep-4", {
            {"target", "cell-4"},
            {"sources", json::array({"cell-2", "cell-3"})}
        }},
        {"dep-5", {
            {"target", "cell-5"},
            {"sources", json::array({"cell-3", "cell-4"})}
        }},
        {"dep-6", {
            {"target", "cell-6"},
            {"sources", json::array({"cell-4", "cell-5"})}
        }}
    };
    
    return state;
}

// 创建贪心算法初始状态
json createInitialGreedyState() {
    // 创建一个零钱兑换贪心问题的初始状态
    json state;
    
    // 贪心问题类型
    state["greedyType"] = "coinChange";
    
    // 目标金额
    state["targetAmount"] = 63;
    
    // 可用硬币面值
    state["coins"] = {25, 10, 5, 1};
    
    // 当前已选硬币
    state["selectedCoins"] = json::array();
    
    // 当前剩余金额
    state["remainingAmount"] = 63;
    
    // 硬币元素
    state["elements"] = {
        {"coin-25", {
            {"value", 25},
            {"position", {{"x", 100}, {"y", 150}}},
            {"state", "normal"},
            {"size", 50}
        }},
        {"coin-10", {
            {"value", 10},
            {"position", {{"x", 200}, {"y", 150}}},
            {"state", "normal"},
            {"size", 40}
        }},
        {"coin-5", {
            {"value", 5},
            {"position", {{"x", 300}, {"y", 150}}},
            {"state", "normal"},
            {"size", 30}
        }},
        {"coin-1", {
            {"value", 1},
            {"position", {{"x", 400}, {"y", 150}}},
            {"state", "normal"},
            {"size", 20}
        }}
    };
    
    // 当前选择过程
    state["selections"] = json::array();
    
    return state;
} 