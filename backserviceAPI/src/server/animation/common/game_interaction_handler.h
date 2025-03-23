#ifndef GAME_INTERACTION_HANDLER_H
#define GAME_INTERACTION_HANDLER_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "animation_session.h"

using json = nlohmann::json;

// 游戏交互处理器类
class GameInteractionHandler {
public:
    // 处理点击事件
    static crow::response handleClick(const crow::request& req);
    
    // 处理拖拽事件
    static crow::response handleDrag(const crow::request& req);
    
    // 处理放置事件
    static crow::response handleDrop(const crow::request& req);
    
    // 处理悬停事件
    static crow::response handleHover(const crow::request& req);
    
    // 处理通用交互事件
    static crow::response handleInteract(const crow::request& req);

private:
    // 辅助函数：分割字符串
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // 辅助函数：验证会话ID
    static bool validateSession(const std::string& sessionId, AnimationSession& session);
    
    // 辅助函数：处理LCS单元格点击
    static json handleLCSCellClick(const std::string& cellId, AnimationSession& session);
    
    // 辅助函数：处理二叉树节点点击
    static json handleBinaryTreeNodeClick(const std::string& nodeId, AnimationSession& session);
    
    // 辅助函数：处理图节点点击
    static json handleGraphNodeClick(const std::string& nodeId, AnimationSession& session);
}; 

#endif // GAME_INTERACTION_HANDLER_H