#ifndef GAME_INTERACTION_HANDLER_H
#define GAME_INTERACTION_HANDLER_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include "animation_manager.h"

using json = nlohmann::json;

// 主要交互处理函数
crow::response handleGameInteract(const crow::request& req);

// 各种交互类型处理函数
json handleElementClick(AnimationSession& session, const json& currentState, const std::string& elementId, const json& data);
json handleElementDrag(AnimationSession& session, const json& currentState, const std::string& elementId, const json& data);
json handleElementDrop(AnimationSession& session, const json& currentState, const std::string& elementId, const json& data);
json handleElementHover(AnimationSession& session, const json& currentState, const std::string& elementId, const json& data);

// 特定游戏元素状态更新函数
void updateLinkedListElementState(json& state, const std::string& elementId, const std::string& newState);
void updateLinkedListElementPosition(json& state, const std::string& elementId, int x, int y);
void updateStackElementState(json& state, const std::string& elementId, const std::string& newState);
void updateQueueElementState(json& state, const std::string& elementId, const std::string& newState);
void updateBinaryTreeElementState(json& state, const std::string& elementId, const std::string& newState);
void updateBinaryTreeElementPosition(json& state, const std::string& elementId, int x, int y);
void updateGraphElementState(json& state, const std::string& elementId, const std::string& newState);
void updateGraphElementPosition(json& state, const std::string& elementId, int x, int y);
void updateSortingElementState(json& state, const std::string& elementId, const std::string& newState);
void updateSearchElementState(json& state, const std::string& elementId, const std::string& newState);
void updateMazeElementState(json& state, const std::string& elementId, const std::string& newState, int x, int y);

// 用于注册到路由的API处理函数
crow::response handleGameElementClick(const crow::request& req);
crow::response handleGameElementDrag(const crow::request& req);
crow::response handleGameElementDrop(const crow::request& req);
crow::response handleGameElementHover(const crow::request& req);

#endif // GAME_INTERACTION_HANDLER_H 