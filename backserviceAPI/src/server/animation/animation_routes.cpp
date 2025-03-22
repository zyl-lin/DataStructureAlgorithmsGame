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