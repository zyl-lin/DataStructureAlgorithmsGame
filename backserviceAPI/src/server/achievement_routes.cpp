#include "achievement_routes.h"
#include "jwt_utils.h"

using json = nlohmann::json;

void registerAchievementRoutes(crow::App<crow::CORSHandler>& app, Database& db) {
    // 获取所有成就
    CROW_ROUTE(app, "/api/achievements").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        json achievements = db.getAllAchievements();
        return crow::response(200, achievements.dump());
    });
    
    // 获取用户成就
    CROW_ROUTE(app, "/api/user/achievements").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        // 验证令牌
        std::string token = req.get_header_value("Authorization");
        if (token.substr(0, 7) == "Bearer ") {
            token = token.substr(7);
        }
        
        auto payload = verifyJWT(token);
        if (payload.empty()) {
            return crow::response(401, "{\"error\":\"未授权或令牌已过期\"}");
        }
        
        int userId = payload["user_id"];
        
        json achievements = db.getUserAchievements(userId);
        return crow::response(200, achievements.dump());
    });
    
    // 获取用户进度
    CROW_ROUTE(app, "/api/user/progress").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        // 验证令牌
        std::string token = req.get_header_value("Authorization");
        if (token.substr(0, 7) == "Bearer ") {
            token = token.substr(7);
        }
        
        auto payload = verifyJWT(token);
        if (payload.empty()) {
            return crow::response(401, "{\"error\":\"未授权或令牌已过期\"}");
        }
        
        int userId = payload["user_id"];
        
        json progress = db.getUserProgress(userId);
        return crow::response(200, progress.dump());
    });
    
    // 同步用户进度
    CROW_ROUTE(app, "/api/user/progress/sync").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        // 验证令牌
        std::string token = req.get_header_value("Authorization");
        if (token.substr(0, 7) == "Bearer ") {
            token = token.substr(7);
        }
        
        auto payload = verifyJWT(token);
        if (payload.empty()) {
            return crow::response(401, "{\"error\":\"未授权或令牌已过期\"}");
        }
        
        int userId = payload["user_id"];
        
        // 解析请求体
        auto body = json::parse(req.body);
        
        // 同步进度
        bool success = db.syncUserProgress(userId, body);
        if (!success) {
            return crow::response(400, "{\"error\":\"同步失败，请检查数据格式\"}");
        }
        
        return crow::response(200, "{\"message\":\"进度同步成功\"}");
    });
} 