#include "user_routes.h"
#include "jwt_utils.h"
#include <iostream>
#include <crow/middlewares/cors.h>

using json = nlohmann::json;

void registerUserRoutes(crow::App<crow::CORSHandler>& app, Database& db) {
    // 用户注册
    CROW_ROUTE(app, "/api/user/register")
        .methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        auto body = json::parse(req.body);
        
        // 验证必填字段
        if (!body.contains("username") || !body.contains("password")) {
            return crow::response(400, "{\"error\":\"用户名和密码为必填项\"}");
        }
        
        std::string username = body["username"];
        std::string password = body["password"];
        std::string email = body.contains("email") ? body["email"].get<std::string>() : "";
        
        // 注册用户
        bool success = db.registerUser(username, password, email);
        if (!success) {
            return crow::response(409, "{\"error\":\"用户名已存在\"}");
        }
        
        return crow::response(201, "{\"message\":\"注册成功\"}");
    });
    
    // 用户登录
    CROW_ROUTE(app, "/api/user/login")
        .methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        auto body = json::parse(req.body);
        
        // 验证必填字段
        if (!body.contains("username") || !body.contains("password")) {
            return crow::response(400, "{\"error\":\"用户名和密码为必填项\"}");
        }
        
        std::string username = body["username"];
        std::string password = body["password"];
        
        // 验证用户凭据
        bool authenticated = db.authenticateUser(username, password);
        if (!authenticated) {
            return crow::response(401, "{\"error\":\"用户名或密码错误\"}");
        }
        
        // 获取用户ID
        json user = db.executeSelect("SELECT id FROM users WHERE username = '" + username + "'");
        if (user.empty()) {
            return crow::response(500, "{\"error\":\"系统错误\"}");
        }
        
        int userId = user[0]["id"];
        
        // 生成JWT令牌
        std::string token = generateJWT(userId, username);
        
        // 返回带令牌的响应
        json response = {
            {"token", token},
            {"user_id", userId},
            {"username", username}
        };
        
        return crow::response(200, response.dump());
    });
    
    // 获取用户信息
    CROW_ROUTE(app, "/api/user/profile").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        // 从请求头获取并验证令牌
        std::string token = req.get_header_value("Authorization");
        if (token.substr(0, 7) == "Bearer ") {
            token = token.substr(7);
        }
        
        auto payload = verifyJWT(token);
        if (!payload["valid"]) {
            return crow::response(401, "{\"error\":\"未授权或令牌已过期\"}");
        }
        
        int userId = std::stoi(payload["user_id"].get<std::string>());
        
        // 获取用户资料
        json profile = db.getUserProfile(userId);
        if (profile.empty()) {
            return crow::response(404, "{\"error\":\"用户不存在\"}");
        }
        
        return crow::response(200, profile.dump());
    });
    
    // 更新用户信息
    CROW_ROUTE(app, "/api/user/profile").methods(crow::HTTPMethod::PUT)
    ([&db](const crow::request& req) {
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
        
        // 更新用户资料
        bool success = db.updateUserProfile(userId, body);
        if (!success) {
            return crow::response(400, "{\"error\":\"更新失败，请检查提供的数据\"}");
        }
        
        return crow::response(200, "{\"message\":\"资料更新成功\"}");
    });
} 