#include "animation_routes.h"
#include "../common/response_builder.h"
#include <chrono>
#include <thread>


// Helper function to get session ID from request
std::string getSessionIdFromRequest(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        if (bodyData.contains("sessionId")) {
            return bodyData["sessionId"].get<std::string>();
        }
        // Try to get from URL parameters if not in body
        auto sessionId = req.url_params.get("sessionId");
        if (sessionId) {
            return std::string(sessionId);
        }
        throw std::runtime_error("Missing sessionId parameter");
    } catch (const json::parse_error& e) {
        // If body parsing fails, try URL parameters
        auto sessionId = req.url_params.get("sessionId");
        if (sessionId) {
            return std::string(sessionId);
        }
        throw std::runtime_error("Missing sessionId parameter");
    }
}

// 注册所有动画相关的路由
void registerAnimationRoutes(crow::App<crow::CORSHandler>& app) {
    // 会话管理
    CROW_ROUTE(app, "/api/animation/create-session")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleCreateSession(req);
        });
    
    // 动画控制
    CROW_ROUTE(app, "/api/animation/pause")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handlePauseAnimation(req);
        });
    
    CROW_ROUTE(app, "/api/animation/resume")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleResumeAnimation(req);
        });
    
    CROW_ROUTE(app, "/api/animation/stop")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleStopAnimation(req);
        });
    
    CROW_ROUTE(app, "/api/animation/status")
        .methods("GET"_method)
        ([](const crow::request& req) {
            return handleGetAnimationStatus(req);
        });
    
    // 动画帧控制
    CROW_ROUTE(app, "/api/animation/nextFrame")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleNextFrame(req);
        });
    
    CROW_ROUTE(app, "/api/animation/prevFrame")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handlePrevFrame(req);
        });
    
    CROW_ROUTE(app, "/api/animation/frames")
        .methods("GET"_method)
        ([](const crow::request& req) {
            return handleGetFrames(req);
        });
    
    CROW_ROUTE(app, "/api/animation/setSpeed")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleSetSpeed(req);
        });
    
    // 动画速度控制
    CROW_ROUTE(app, "/api/animation/speedControl")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleSpeedControl(req);
        });
    
    // 步骤回退
    CROW_ROUTE(app, "/api/animation/stepBack")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleStepBack(req);
        });
    
    // 游戏元素交互
    CROW_ROUTE(app, "/api/games/interact")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return GameInteractionHandler::handleInteract(req);
        });
    
    CROW_ROUTE(app, "/api/games/click")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return GameInteractionHandler::handleClick(req);
        });
    
    CROW_ROUTE(app, "/api/games/drag")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return GameInteractionHandler::handleDrag(req);
        });
    
    CROW_ROUTE(app, "/api/games/drop")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return GameInteractionHandler::handleDrop(req);
        });
    
    CROW_ROUTE(app, "/api/games/hover")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return GameInteractionHandler::handleHover(req);
        });
}

// 会话管理相关的处理函数实现
crow::response handleCreateSession(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("gameType") || !bodyData.contains("animationType")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing required parameters", "animation").dump());
        }
        
        std::string gameType = bodyData["gameType"];
        std::string animationType = bodyData["animationType"];
        
        auto animationManager = AnimationManager::getInstance();
        std::string sessionId = animationManager->createSession(gameType, animationType);
        
        json responseData = {
            {"sessionId", sessionId},
            {"gameType", gameType},
            {"animationType", animationType}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Create session error: ") + e.what(), "animation").dump());
    }
}

// 动画控制相关的处理函数实现
crow::response handlePauseAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "animation").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "animation").dump());
        }
        
        json responseData = {
            {"sessionId", sessionId},
            {"status", "paused"}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Pause animation error: ") + e.what(), "animation").dump());
    }
}

crow::response handleResumeAnimation(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "animation").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "animation").dump());
        }
        
        json responseData = {
            {"sessionId", sessionId},
            {"status", "playing"}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Resume animation error: ") + e.what(), "animation").dump());
    }
}

crow::response handleStopAnimation(const crow::request& req) {
    try {
        auto sessionId = getSessionIdFromRequest(req);
        auto& session = AnimationManager::getInstance()->getSession(sessionId);
        
        // 停止动画并重置状态
        AnimationManager::getInstance()->stopAnimation(sessionId);
        
        // 返回成功响应
        return crow::response(200, R"({"status": "success"})");
    } catch (const std::exception& e) {
        return crow::response(400, R"({"error": ")" + std::string(e.what()) + R"("})");
    }
}

crow::response handleGetAnimationStatus(const crow::request& req) {
    try {
        std::string sessionId = req.url_params.get("sessionId") ? req.url_params.get("sessionId") : "";
        
        if (sessionId.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "animation").dump());
        }
        
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "animation").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        
        json responseData = {
            {"sessionId", sessionId},
            {"gameType", session.getGameType()},
            {"animationType", session.getAnimationType()},
            {"frameCount", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Get animation status error: ") + e.what(), "animation").dump());
    }
}

// 动画帧控制相关的处理函数实现
crow::response handleNextFrame(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "animation").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "animation").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        size_t currentFrame = bodyData.value("currentFrame", 0);
        
        if (currentFrame >= session.getFrameCount()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Frame index out of range", "animation").dump());
        }
        
        json frame = session.getFrame(currentFrame);
        
        json responseData = {
            {"sessionId", sessionId},
            {"frame", frame},
            {"frameIndex", currentFrame},
            {"totalFrames", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Next frame error: ") + e.what(), "animation").dump());
    }
}

crow::response handlePrevFrame(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "animation").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "animation").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        size_t currentFrame = bodyData.value("currentFrame", 0);
        
        if (currentFrame == 0) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Already at first frame", "animation").dump());
        }
        
        json frame = session.getFrame(currentFrame - 1);
        
        json responseData = {
            {"sessionId", sessionId},
            {"frame", frame},
            {"frameIndex", currentFrame - 1},
            {"totalFrames", session.getFrameCount()}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Previous frame error: ") + e.what(), "animation").dump());
    }
}

crow::response handleGetFrames(const crow::request& req) {
    try {
        auto sessionId = getSessionIdFromRequest(req);
        auto& session = AnimationManager::getInstance()->getSession(sessionId);
        
        // 获取所有帧数据
        json response = json{
            {"sessionId", sessionId},
            {"frames", AnimationManager::getInstance()->getAllFrames(sessionId)},
            {"totalFrames", session.getFrameCount()}
        };
        
        return crow::response(200, response.dump());
    } catch (const std::exception& e) {
        return crow::response(400, R"({"error": ")" + std::string(e.what()) + R"("})");
    }
}

crow::response handleSetSpeed(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("speed")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing required parameters", "animation").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        double speed = bodyData["speed"];
        
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "animation").dump());
        }
        
        json responseData = {
            {"sessionId", sessionId},
            {"speed", speed}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Set speed error: ") + e.what(), "animation").dump());
    }
}

// 动画速度控制相关的处理函数实现
crow::response handleSpeedControl(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId") || !bodyData.contains("speed")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing required parameters", "animation").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        double speed = bodyData["speed"];
        
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "animation").dump());
        }
        
        if (speed < 0.5 || speed > 2.0) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Speed must be between 0.5 and 2.0", "animation").dump());
        }
        
        json responseData = {
            {"sessionId", sessionId},
            {"speed", speed}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Speed control error: ") + e.what(), "animation").dump());
    }
}

// 步骤回退相关的处理函数实现
crow::response handleStepBack(const crow::request& req) {
    try {
        auto bodyData = json::parse(req.body);
        
        if (!bodyData.contains("sessionId")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Missing sessionId", "animation").dump());
        }
        
        std::string sessionId = bodyData["sessionId"];
        auto animationManager = AnimationManager::getInstance();
        
        if (!animationManager->hasSession(sessionId)) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Invalid session ID", "animation").dump());
        }
        
        auto& session = animationManager->getSession(sessionId);
        size_t currentStep = bodyData.value("currentStep", 0);
        
        if (currentStep == 0) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "Already at first step", "animation").dump());
        }
        
        json state = session.getGameState();
        
        json responseData = {
            {"sessionId", sessionId},
            {"step", currentStep - 1},
            {"state", state}
        };
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(responseData, "animation").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("Step back error: ") + e.what(), "animation").dump());
    }
} 