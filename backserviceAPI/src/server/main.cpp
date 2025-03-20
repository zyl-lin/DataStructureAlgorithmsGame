#include "crow.h"
#include "nlohmann/json.hpp"
#include "database.h"
#include "user_routes.h"
#include "game_routes.h"
#include "achievement_routes.h"

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
        .headers("Content-Type")
        .methods("POST", "GET", "PUT", "DELETE");
    
    // 注册路由
    registerUserRoutes(app, db);
    registerGameRoutes(app, db);
    registerAchievementRoutes(app, db);
    
    // 健康检查端点
    CROW_ROUTE(app, "/api/health")
    ([]() {
        return crow::response(200, "{\"status\":\"ok\"}");
    });
    
    // 启动服务器
    app.port(3000).bindaddr("0.0.0.0").run();
    
    return 0;
} 