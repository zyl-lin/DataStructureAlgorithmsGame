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
    
    // 创建Crow应用 - 移除 CORSHandler
    crow::App app;
    
    // 手动添加 CORS 头部
    app.after_handle([](crow::request& req, crow::response& res){
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        res.add_header("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE");
    });
    
    // 添加 OPTIONS 方法的处理
    CROW_ROUTE(app, "/<path>")
    .methods(crow::HTTPMethod::OPTIONS)
    ([](const crow::request&, std::string path) {
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Headers", "Content-Type");
        res.add_header("Access-Control-Allow-Methods", "POST, GET, PUT, DELETE");
        res.code = 204;
        return res;
    });
    
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