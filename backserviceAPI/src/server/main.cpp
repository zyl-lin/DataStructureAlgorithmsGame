#include "crow.h"
#include "nlohmann/json.hpp"
#include "database.h"
#include "user_routes.h"
#include "game_routes.h"
#include "achievement_routes.h"
#include "game_animation_routes.h"
#include "animation/animation_routes.h"
#include <crow/middlewares/cors.h>

using json = nlohmann::json;

int main() {
    // 初始化数据库
    Database db("game_database.db");
    db.initialize();
    
    // 创建Crow应用
    crow::App<crow::CORSHandler> app;
    
    // 启用CORS
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .methods(crow::HTTPMethod::POST)
        .methods(crow::HTTPMethod::GET)
        .methods(crow::HTTPMethod::PUT)
        .methods(crow::HTTPMethod::DELETE)
        .headers("Content-Type");
    
    // 注册路由
    registerUserRoutes(app, db);
    registerGameRoutes(app, db);
    registerAchievementRoutes(app, db);
    registerGameAnimationRoutes(app, db);
    registerAnimationRoutes(app);  // 注册动画和游戏元素交互路由
    
    // 健康检查端点
    CROW_ROUTE(app, "/health")
    ([]() {
        return crow::response(200, "{\"status\":\"ok\"}");
    });
    
    // 根路径端点
    CROW_ROUTE(app, "/")
    ([]() {
        return crow::response(200, "{\"message\":\"数据结构与算法游戏API服务\",\"version\":\"1.0\"}");
    });
    
    // 启动服务器
    app.port(3000).bindaddr("0.0.0.0").run();
    
    return 0;
} 