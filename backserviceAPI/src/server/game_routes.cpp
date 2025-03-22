#include "game_routes.h"
#include "jwt_utils.h"

using json = nlohmann::json;

void registerGameRoutes(crow::App<crow::CORSHandler>& app, Database& db) {
    // 获取所有游戏
    CROW_ROUTE(app, "/api/games").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        json games = db.getAllGames();
        return crow::response(200, games.dump());
    });
    
    // 获取特定游戏详情
    CROW_ROUTE(app, "/api/games/<int>").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req, int gameId) {
        json game = db.getGameDetails(gameId);
        if (game.empty()) {
            return crow::response(404, "{\"error\":\"游戏不存在\"}");
        }
        return crow::response(200, game.dump());
    });
    
    // 获取游戏关卡列表
    CROW_ROUTE(app, "/api/games/<int>/levels").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req, int gameId) {
        // 验证游戏是否存在
        json game = db.getGameDetails(gameId);
        if (game.empty()) {
            return crow::response(404, "{\"error\":\"游戏不存在\"}");
        }
        
        json levels = db.getGameLevels(gameId);
        return crow::response(200, levels.dump());
    });
    
    // 获取特定关卡详情
    CROW_ROUTE(app, "/api/games/<int>/levels/<int>").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req, int gameId, int levelId) {
        json level = db.getLevelDetails(gameId, levelId);
        if (level.empty()) {
            return crow::response(404, "{\"error\":\"关卡不存在\"}");
        }
        return crow::response(200, level.dump());
    });
    
    // 开始游戏关卡
    CROW_ROUTE(app, "/api/games/<int>/levels/<int>/start").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req, int gameId, int levelId) {
        // 验证令牌
        std::string token = req.get_header_value("Authorization");
        if (token.substr(0, 7) == "Bearer ") {
            token = token.substr(7);
        }
        
        auto payload = verifyJWT(token);
        if (!payload["valid"]) {
            return crow::response(401, "{\"error\":\"未授权或令牌已过期\"}");
        }
        
        int userId = std::stoi(payload["user_id"].get<std::string>());
        
        // 开始关卡
        bool success = db.startLevel(userId, gameId, levelId);
        if (!success) {
            return crow::response(404, "{\"error\":\"关卡不存在\"}");
        }
        
        return crow::response(200, "{\"message\":\"关卡已开始\"}");
    });
    
    // 提交游戏解答
    CROW_ROUTE(app, "/api/games/<int>/levels/<int>/submit").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req, int gameId, int levelId) {
        // 验证令牌
        std::string token = req.get_header_value("Authorization");
        if (token.substr(0, 7) == "Bearer ") {
            token = token.substr(7);
        }
        
        auto payload = verifyJWT(token);
        if (!payload["valid"]) {
            return crow::response(401, "{\"error\":\"未授权或令牌已过期\"}");
        }
        
        int userId = std::stoi(payload["user_id"].get<std::string>());
        
        // 解析请求体
        auto body = json::parse(req.body);
        if (!body.contains("solution")) {
            return crow::response(400, "{\"error\":\"缺少解答内容\"}");
        }
        
        std::string solution = body["solution"];
        
        // 提交解答
        bool success = db.submitSolution(userId, gameId, levelId, solution);
        if (!success) {
            return crow::response(404, "{\"error\":\"关卡不存在\"}");
        }
        
        // 这里可以添加判断解答是否正确的逻辑
        bool isCorrect = true; // 简化处理，假设解答正确
        
        json response = {
            {"success", isCorrect},
            {"message", isCorrect ? "解答正确！" : "解答错误，请重试。"}
        };
        
        return crow::response(200, response.dump());
    });
    
    // 获取游戏提示
    CROW_ROUTE(app, "/api/games/<int>/levels/<int>/hint").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req, int gameId, int levelId) {
        // 验证令牌
        std::string token = req.get_header_value("Authorization");
        if (token.substr(0, 7) == "Bearer ") {
            token = token.substr(7);
        }
        
        auto payload = verifyJWT(token);
        if (!payload["valid"]) {
            return crow::response(401, "{\"error\":\"未授权或令牌已过期\"}");
        }
        
        json hint = db.getLevelHint(gameId, levelId);
        return crow::response(200, hint.dump());
    });
    
    // 获取游戏解答
    CROW_ROUTE(app, "/api/games/<int>/levels/<int>/solution").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req, int gameId, int levelId) {
        // 验证令牌
        std::string token = req.get_header_value("Authorization");
        if (token.substr(0, 7) == "Bearer ") {
            token = token.substr(7);
        }
        
        auto payload = verifyJWT(token);
        if (!payload["valid"]) {
            return crow::response(401, "{\"error\":\"未授权或令牌已过期\"}");
        }
        
        json solution = db.getLevelSolution(gameId, levelId);
        return crow::response(200, solution.dump());
    });
} 