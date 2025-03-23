#include "animation_routes.h"
#include "../implementations/sorting/sorting_animations.h"

void registerSortingRoutes(crow::App<crow::CORSHandler>& app) {
    // 注册冒泡排序算法路由
    CROW_ROUTE(app, "/api/sorting/bubbleSort")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleBubbleSortAnimation(req);
        });
    
    // 注册选择排序算法路由
    CROW_ROUTE(app, "/api/sorting/selectionSort")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleSelectionSortAnimation(req);
        });
    
    // 注册插入排序算法路由
    CROW_ROUTE(app, "/api/sorting/insertionSort")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleInsertionSortAnimation(req);
        });
    
    // 注册快速排序算法路由
    CROW_ROUTE(app, "/api/sorting/quickSort")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleQuickSortAnimation(req);
        });
    
    // 注册排序算法单步执行路由
    CROW_ROUTE(app, "/api/sorting/step")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleSortingStepAnimation(req);
        });
    
    // 注册获取排序状态路由
    CROW_ROUTE(app, "/api/sorting/state")
        .methods("GET"_method)
        ([](const crow::request& req) {
            return handleGetSortingState(req);
        });
    
    // 注册重置排序状态路由
    CROW_ROUTE(app, "/api/sorting/reset")
        .methods("POST"_method)
        ([](const crow::request& req) {
            return handleResetSortingState(req);
        });
} 