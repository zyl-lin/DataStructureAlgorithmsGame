#include "animation_routes.h"
#include "../implementations/maze/maze_animations.h"
#include "../common/response_builder.h"

void registerMazeRoutes(crow::App<crow::CORSHandler>& app) {
    // 注册迷宫生成路由
    CROW_ROUTE(app, "/api/maze/generate")
        .methods("GET"_method)
        ([](const crow::request& req) {
            return handleMazeGenerate(req);
        });
    
    // 注册DFS求解路由
    CROW_ROUTE(app, "/api/maze/solveDFS")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleMazeSolveDFS(req);
        });
    
    // 注册BFS求解路由
    CROW_ROUTE(app, "/api/maze/solveBFS")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleMazeSolveBFS(req);
        });
    
    // 注册A*求解路由
    CROW_ROUTE(app, "/api/maze/solveAStar")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleMazeSolveAStar(req);
        });
    
    // 注册获取迷宫状态路由
    CROW_ROUTE(app, "/api/maze/state")
        .methods("GET"_method)
        ([](const crow::request& req) {
            return handleGetMazeState(req);
        });
    
    // 注册重置迷宫状态路由
    CROW_ROUTE(app, "/api/maze/reset")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleResetMazeState(req);
        });
} 