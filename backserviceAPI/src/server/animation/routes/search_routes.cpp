#include "animation_routes.h"
#include "../implementations/search/search_animations.h"

void registerSearchRoutes(crow::App<crow::CORSHandler>& app) {
    // 注册线性搜索算法路由
    CROW_ROUTE(app, "/api/search/linear")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleLinearSearchAnimation(req);
        });
    
    // 注册二分搜索算法路由
    CROW_ROUTE(app, "/api/search/binary")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleBinarySearchAnimation(req);
        });
    
    // 注册搜索算法单步执行路由
    CROW_ROUTE(app, "/api/search/step")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleSearchStepAnimation(req);
        });
    
    // 注册获取搜索状态路由
    CROW_ROUTE(app, "/api/search/state")
        .methods("GET"_method)
        ([](const crow::request& req) {
            return handleGetSearchState(req);
        });
    
    // 注册重置搜索状态路由
    CROW_ROUTE(app, "/api/search/reset")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleResetSearchState(req);
        });
} 