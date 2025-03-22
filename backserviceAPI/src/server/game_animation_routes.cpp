#include "game_animation_routes.h"
#include "animation/animation_manager.h"
#include "response_builder.h"
#include "maze_routes.h"
#include "greedy_routes.h"
#include "dp_routes.h"
#include "graph_routes.h"
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <numeric>
#include <queue>
#include <map>
#include <cmath>
#include <set>
#include <optional>

// 提取动画相关的查询参数
AnimationParams extractAnimationParams(const crow::request& req) {
    AnimationParams params;
    
    // 默认值
    params.animate = false;
    params.speed = 5;
    params.step = false;
    
    // 解析animate参数
    auto animateIt = req.url_params.get("animate");
    if (animateIt) {
        std::string animateStr = animateIt;
        params.animate = (animateStr == "true" || animateStr == "1");
    }
    
    // 解析speed参数
    auto speedIt = req.url_params.get("speed");
    if (speedIt) {
        try {
            params.speed = std::stoi(speedIt);
            // 确保速度在有效范围内 (1-10)
            if (params.speed < 1) params.speed = 1;
            if (params.speed > 10) params.speed = 10;
        } catch (const std::exception& e) {
            // 解析错误，使用默认值
        }
    }
    
    // 解析step参数
    auto stepIt = req.url_params.get("step");
    if (stepIt) {
        std::string stepStr = stepIt;
        params.step = (stepStr == "true" || stepStr == "1");
    }
    
    return params;
}

// 构建嵌套的树结构
json buildTreeStructure(const json& nodes, const json& edges, int rootId) {
    // 创建节点ID到节点的映射
    std::map<int, json> nodeMap;
    for (const auto& node : nodes) {
        nodeMap[node["id"]] = node;
    }

    // 创建父节点到子节点的映射
    std::map<int, std::vector<std::pair<int, std::string>>> childrenMap;
    for (const auto& edge : edges) {
        int source = edge["source"];
        int target = edge["target"];
        // 确保直接使用edge中的position属性
        std::string position = edge.contains("position") ? edge["position"].get<std::string>() : "";
        childrenMap[source].push_back({target, position});
    }

    // 递归构建树结构
    std::function<json(int)> buildSubtree = [&](int nodeId) -> json {
        if (nodeMap.find(nodeId) == nodeMap.end()) {
            return nullptr;
        }

        json result;
        result["id"] = nodeId;
        result["value"] = nodeMap[nodeId]["value"];
        result["left"] = nullptr;
        result["right"] = nullptr;

        // 处理子节点
        if (childrenMap.find(nodeId) != childrenMap.end()) {
            // 确保先处理所有显式标记了位置的子节点
            for (const auto& [childId, position] : childrenMap[nodeId]) {
                if (position == "left") {
                    result["left"] = buildSubtree(childId);
                } else if (position == "right") {
                    result["right"] = buildSubtree(childId);
                }
            }
            
            // 处理那些没有明确position的子节点（如果有的话）
            // 这部分逻辑是为了兼容旧数据
            for (const auto& [childId, position] : childrenMap[nodeId]) {
                if (position.empty()) {
                    // 如果没有明确的position，根据节点值比较决定左右
                    if (result["left"] == nullptr) {
                        result["left"] = buildSubtree(childId);
                    } else if (result["right"] == nullptr) {
                        result["right"] = buildSubtree(childId);
                    }
                }
            }
        }

        return result;
    };

    return buildSubtree(rootId);
}

// 链表游戏操作 - 插入节点
crow::response handleLinkedListInsert(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    // 解析请求体
    json requestBody;
    try {
        requestBody = json::parse(req.body);
    } catch (const std::exception& e) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "无效的JSON格式", "linkedlist").dump());
    }
    
    // 验证必需的参数
    if (!requestBody.contains("value") || !requestBody.contains("position")) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少必需的参数：value 或 position", "linkedlist").dump());
    }
    
    int value = requestBody["value"];
    int position = requestBody["position"];
    
    try {
        // 获取当前链表状态
        json currentState = db.getGameState("linkedlist");
        
        // 执行插入操作
        std::vector<int> linkedList;
        if (currentState.contains("list") && currentState["list"].is_array()) {
            linkedList = currentState["list"].get<std::vector<int>>();
        }
        
        // 验证位置是否有效
        if (position < 0 || position > linkedList.size()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "无效的插入位置", "linkedlist").dump());
        }
        
        // 如果需要动画效果
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("linkedlist", "insert");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 添加初始帧
            json initialFrame = {
                {"list", linkedList},
                {"currentIndex", -1},
                {"insertPosition", position},
                {"insertValue", value}
            };
            session.addFrame(initialFrame, "准备在位置 " + std::to_string(position) + " 插入值 " + std::to_string(value));
            
            // 添加遍历过程帧
            for (int i = 0; i <= position && i < linkedList.size(); i++) {
                json traverseFrame = {
                    {"list", linkedList},
                    {"currentIndex", i},
                    {"insertPosition", position},
                    {"insertValue", value}
                };
                session.addFrame(traverseFrame, "遍历到位置 " + std::to_string(i));
            }
            
            // 添加插入帧
            linkedList.insert(linkedList.begin() + position, value);
            json insertFrame = {
                {"list", linkedList},
                {"currentIndex", position},
                {"insertPosition", position},
                {"insertValue", value},
                {"inserted", true}
            };
            session.addFrame(insertFrame, "在位置 " + std::to_string(position) + " 插入值 " + std::to_string(value));
            
            // 添加最终帧
            json finalFrame = {
                {"list", linkedList},
                {"currentIndex", -1},
                {"completed", true}
            };
            session.addFrame(finalFrame, "插入操作完成");
            
            // 更新游戏状态
            json newState = {
                {"list", linkedList}
            };
            db.updateGameState("linkedlist", newState);
            session.updateGameState(newState);
            
            // 如果是单步执行模式，则返回第一帧
            if (params.step) {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "linkedlist").dump());
            } else {
                // 否则返回所有帧
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "linkedlist").dump());
            }
        } else {
            // 不需要动画，直接执行操作
            linkedList.insert(linkedList.begin() + position, value);
            
            // 更新游戏状态
            json newState = {
                {"list", linkedList}
            };
            db.updateGameState("linkedlist", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "linkedlist").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "linkedlist").dump());
    }
}

// 链表游戏操作 - 删除节点
crow::response handleLinkedListDelete(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    // 解析请求体
    json requestBody;
    try {
        requestBody = json::parse(req.body);
    } catch (const std::exception& e) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "无效的JSON格式", "linkedlist").dump());
    }
    
    // 验证必需的参数
    if (!requestBody.contains("position")) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少必需的参数：position", "linkedlist").dump());
    }
    
    int position = requestBody["position"];
    
    try {
        // 获取当前链表状态
        json currentState = db.getGameState("linkedlist");
        
        // 执行删除操作
        std::vector<int> linkedList;
        if (currentState.contains("list") && currentState["list"].is_array()) {
            linkedList = currentState["list"].get<std::vector<int>>();
        }
        
        // 验证位置是否有效
        if (position < 0 || position >= linkedList.size()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "无效的删除位置", "linkedlist").dump());
        }
        
        // 获取要删除的值以便在动画中显示
        int valueToDelete = linkedList[position];
        
        // 如果需要动画效果
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("linkedlist", "delete");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 添加初始帧
            json initialFrame = {
                {"list", linkedList},
                {"currentIndex", -1},
                {"deletePosition", position},
                {"deleteValue", valueToDelete}
            };
            session.addFrame(initialFrame, "准备删除位置 " + std::to_string(position) + " 的值");
            
            // 添加遍历过程帧
            for (int i = 0; i <= position && i < linkedList.size(); i++) {
                json traverseFrame = {
                    {"list", linkedList},
                    {"currentIndex", i},
                    {"deletePosition", position},
                    {"deleteValue", valueToDelete}
                };
                session.addFrame(traverseFrame, "遍历到位置 " + std::to_string(i));
            }
            
            // 添加删除帧
            linkedList.erase(linkedList.begin() + position);
            json deleteFrame = {
                {"list", linkedList},
                {"currentIndex", position},
                {"deletePosition", position},
                {"deleteValue", valueToDelete},
                {"deleted", true}
            };
            session.addFrame(deleteFrame, "删除位置 " + std::to_string(position) + " 的值 " + std::to_string(valueToDelete));
            
            // 添加最终帧
            json finalFrame = {
                {"list", linkedList},
                {"currentIndex", -1},
                {"completed", true}
            };
            session.addFrame(finalFrame, "删除操作完成");
            
            // 更新游戏状态
            json newState = {
                {"list", linkedList}
            };
            db.updateGameState("linkedlist", newState);
            session.updateGameState(newState);
            
            // 如果是单步执行模式，则返回第一帧
            if (params.step) {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "linkedlist").dump());
            } else {
                // 否则返回所有帧
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "linkedlist").dump());
            }
        } else {
            // 不需要动画，直接执行操作
            linkedList.erase(linkedList.begin() + position);
            
            // 更新游戏状态
            json newState = {
                {"list", linkedList}
            };
            db.updateGameState("linkedlist", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "linkedlist").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "linkedlist").dump());
    }
}

// 链表游戏操作 - 查找节点
crow::response handleLinkedListFind(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    // 解析请求体
    json requestBody;
    try {
        requestBody = json::parse(req.body);
    } catch (const std::exception& e) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "无效的JSON格式", "linkedlist").dump());
    }
    
    // 验证必需的参数
    if (!requestBody.contains("value")) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少必需的参数：value", "linkedlist").dump());
    }
    
    int value = requestBody["value"];
    
    try {
        // 获取当前链表状态
        json currentState = db.getGameState("linkedlist");
        
        // 执行查找操作
        std::vector<int> linkedList;
        if (currentState.contains("list") && currentState["list"].is_array()) {
            linkedList = currentState["list"].get<std::vector<int>>();
        }
        
        // 如果需要动画效果
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("linkedlist", "find");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 添加初始帧
            json initialFrame = {
                {"list", linkedList},
                {"currentIndex", -1},
                {"searchValue", value},
                {"found", false}
            };
            session.addFrame(initialFrame, "开始查找值 " + std::to_string(value));
            
            // 添加遍历过程帧
            bool found = false;
            int foundIndex = -1;
            
            for (int i = 0; i < linkedList.size(); i++) {
                json traverseFrame = {
                    {"list", linkedList},
                    {"currentIndex", i},
                    {"searchValue", value},
                    {"found", false}
                };
                session.addFrame(traverseFrame, "检查位置 " + std::to_string(i));
                
                if (linkedList[i] == value) {
                    found = true;
                    foundIndex = i;
                    json foundFrame = {
                        {"list", linkedList},
                        {"currentIndex", i},
                        {"searchValue", value},
                        {"found", true},
                        {"foundIndex", i}
                    };
                    session.addFrame(foundFrame, "在位置 " + std::to_string(i) + " 找到值 " + std::to_string(value));
                    break;
                }
            }
            
            // 添加最终帧
            json finalFrame = {
                {"list", linkedList},
                {"currentIndex", -1},
                {"searchValue", value},
                {"found", found},
                {"foundIndex", foundIndex},
                {"completed", true}
            };
            session.addFrame(finalFrame, found ? 
                           "查找完成：在位置 " + std::to_string(foundIndex) + " 找到值 " + std::to_string(value) :
                           "查找完成：未找到值 " + std::to_string(value));
            
            // 更新游戏状态
            json newState = {
                {"list", linkedList},
                {"searchResult", {
                    {"value", value},
                    {"found", found},
                    {"position", foundIndex}
                }}
            };
            db.updateGameState("linkedlist", newState);
            session.updateGameState(newState);
            
            // 如果是单步执行模式，则返回第一帧
            if (params.step) {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "linkedlist").dump());
            } else {
                // 否则返回所有帧
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "linkedlist").dump());
            }
        } else {
            // 不需要动画，直接执行查找
            bool found = false;
            int foundIndex = -1;
            
            for (int i = 0; i < linkedList.size(); i++) {
                if (linkedList[i] == value) {
                    found = true;
                    foundIndex = i;
                    break;
                }
            }
            
            // 更新游戏状态
            json newState = {
                {"list", linkedList},
                {"searchResult", {
                    {"value", value},
                    {"found", found},
                    {"position", foundIndex}
                }}
            };
            db.updateGameState("linkedlist", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "linkedlist").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "linkedlist").dump());
    }
}

// 链表游戏操作 - 反转链表
crow::response handleLinkedListReverse(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前链表状态
        json currentState = db.getGameState("linkedlist");
        
        // 执行反转操作
        std::vector<int> linkedList;
        if (currentState.contains("list") && currentState["list"].is_array()) {
            linkedList = currentState["list"].get<std::vector<int>>();
        }
        
        // 如果需要动画效果
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("linkedlist", "reverse");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 添加初始帧
            json initialFrame = {
                {"list", linkedList},
                {"currentIndex", -1},
                {"leftIndex", -1},
                {"rightIndex", -1}
            };
            session.addFrame(initialFrame, "开始反转链表");
            
            // 添加反转过程帧
            int left = 0;
            int right = linkedList.size() - 1;
            
            while (left < right) {
                // 显示当前要交换的位置
                json beforeSwapFrame = {
                    {"list", linkedList},
                    {"currentIndex", -1},
                    {"leftIndex", left},
                    {"rightIndex", right}
                };
                session.addFrame(beforeSwapFrame, "准备交换位置 " + std::to_string(left) + " 和位置 " + std::to_string(right) + " 的元素");
                
                // 执行交换
                std::swap(linkedList[left], linkedList[right]);
                
                // 显示交换后的状态
                json afterSwapFrame = {
                    {"list", linkedList},
                    {"currentIndex", -1},
                    {"leftIndex", left},
                    {"rightIndex", right},
                    {"swapped", true}
                };
                session.addFrame(afterSwapFrame, "交换完成：位置 " + std::to_string(left) + " 和位置 " + std::to_string(right));
                
                left++;
                right--;
            }
            
            // 添加最终帧
            json finalFrame = {
                {"list", linkedList},
                {"currentIndex", -1},
                {"completed", true}
            };
            session.addFrame(finalFrame, "反转操作完成");
            
            // 更新游戏状态
            json newState = {
                {"list", linkedList}
            };
            db.updateGameState("linkedlist", newState);
            session.updateGameState(newState);
            
            // 如果是单步执行模式，则返回第一帧
            if (params.step) {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "linkedlist").dump());
            } else {
                // 否则返回所有帧
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "linkedlist").dump());
            }
        } else {
            // 不需要动画，直接执行反转
            std::reverse(linkedList.begin(), linkedList.end());
            
            // 更新游戏状态
            json newState = {
                {"list", linkedList}
            };
            db.updateGameState("linkedlist", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "linkedlist").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "linkedlist").dump());
    }
}

// 链表游戏操作 - 获取当前状态
crow::response handleLinkedListState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("linkedlist");
        
        // 如果链表不存在，则初始化一个空链表
        if (!state.contains("list")) {
            state["list"] = json::array();
            db.updateGameState("linkedlist", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "linkedlist").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "linkedlist").dump());
    }
}

// 链表游戏操作 - 重置状态
crow::response handleLinkedListReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的链表状态
        json emptyState = {
            {"list", json::array()}
        };
        
        // 更新数据库中的状态
        db.updateGameState("linkedlist", emptyState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "linkedlist").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置链表状态失败: ") + e.what(), "linkedlist").dump());
    }
}

// 队列游戏操作 - 入队
crow::response handleQueueEnqueue(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    // 解析请求体
    json requestBody;
    try {
        requestBody = json::parse(req.body);
    } catch (const std::exception& e) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "无效的JSON格式", "queue").dump());
    }
    
    // 验证必需的参数
    if (!requestBody.contains("value")) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少必需的参数：value", "queue").dump());
    }
    
    int value = requestBody["value"];
    
    try {
        // 获取当前队列状态
        json currentState = db.getGameState("queue");
        
        // 执行入队操作
        std::vector<int> queue;
        if (currentState.contains("queue") && currentState["queue"].is_array()) {
            queue = currentState["queue"].get<std::vector<int>>();
        }
        
        // 如果需要动画效果
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("queue", "enqueue");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 添加初始帧
            json initialFrame = {
                {"queue", queue},
                {"currentIndex", -1},
                {"enqueueValue", value}
            };
            session.addFrame(initialFrame, "准备将值 " + std::to_string(value) + " 入队");
            
            // 添加入队帧
            queue.push_back(value);
            json enqueueFrame = {
                {"queue", queue},
                {"currentIndex", queue.size() - 1},
                {"enqueueValue", value},
                {"enqueued", true}
            };
            session.addFrame(enqueueFrame, "将值 " + std::to_string(value) + " 入队到位置 " + std::to_string(queue.size() - 1));
            
            // 添加最终帧
            json finalFrame = {
                {"queue", queue},
                {"currentIndex", -1},
                {"completed", true}
            };
            session.addFrame(finalFrame, "入队操作完成");
            
            // 更新游戏状态
            json newState = {
                {"queue", queue}
            };
            db.updateGameState("queue", newState);
            session.updateGameState(newState);
            
            // 如果是单步执行模式，则返回第一帧
            if (params.step) {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "queue").dump());
            } else {
                // 否则返回所有帧
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "queue").dump());
            }
        } else {
            // 不需要动画，直接执行入队
            queue.push_back(value);
            
            // 更新游戏状态
            json newState = {
                {"queue", queue}
            };
            db.updateGameState("queue", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "queue").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "queue").dump());
    }
}

// 队列游戏操作 - 出队
crow::response handleQueueDequeue(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前队列状态
        json currentState = db.getGameState("queue");
        
        // 执行出队操作
        std::vector<int> queue;
        if (currentState.contains("queue") && currentState["queue"].is_array()) {
            queue = currentState["queue"].get<std::vector<int>>();
        }
        
        // 检查队列是否为空
        if (queue.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "队列为空，无法执行出队操作", "queue").dump());
        }
        
        // 获取要出队的值
        int dequeueValue = queue.front();
        
        // 如果需要动画效果
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("queue", "dequeue");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 添加初始帧
            json initialFrame = {
                {"queue", queue},
                {"currentIndex", 0},
                {"dequeueValue", dequeueValue}
            };
            session.addFrame(initialFrame, "准备将队首元素 " + std::to_string(dequeueValue) + " 出队");
            
            // 添加出队帧
            queue.erase(queue.begin());
            json dequeueFrame = {
                {"queue", queue},
                {"currentIndex", -1},
                {"dequeueValue", dequeueValue},
                {"dequeued", true}
            };
            session.addFrame(dequeueFrame, "将值 " + std::to_string(dequeueValue) + " 从队列中移除");
            
            // 添加最终帧
            json finalFrame = {
                {"queue", queue},
                {"currentIndex", -1},
                {"completed", true}
            };
            session.addFrame(finalFrame, "出队操作完成");
            
            // 更新游戏状态
            json newState = {
                {"queue", queue},
                {"dequeueValue", dequeueValue}
            };
            db.updateGameState("queue", newState);
            session.updateGameState(newState);
            
            // 如果是单步执行模式，则返回第一帧
            if (params.step) {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "queue").dump());
            } else {
                // 否则返回所有帧
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "queue").dump());
            }
        } else {
            // 不需要动画，直接执行出队
            queue.erase(queue.begin());
            
            // 更新游戏状态
            json newState = {
                {"queue", queue},
                {"dequeueValue", dequeueValue}
            };
            db.updateGameState("queue", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "queue").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "queue").dump());
    }
}

// 队列游戏操作 - 查看队首元素
crow::response handleQueuePeek(const crow::request& req, Database& db) {
    try {
        // 获取当前队列状态
        json state = db.getGameState("queue");
        
        // 获取队列
        std::vector<int> queue;
        if (state.contains("queue") && state["queue"].is_array()) {
            queue = state["queue"].get<std::vector<int>>();
        }
        
        // 如果队列为空，返回空状态
        if (queue.empty()) {
            json emptyState = {
                {"queue", queue},
                {"peek", nullptr}
            };
            return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "queue").dump());
        }
        
        // 返回队首元素
        json peekState = {
            {"queue", queue},
            {"peek", queue.front()}
        };
        return crow::response(200, ResponseBuilder::createSuccessResponse(peekState, "queue").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "queue").dump());
    }
}

// 队列游戏操作 - 获取当前状态
crow::response handleQueueState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("queue");
        
        // 如果状态为空，初始化一个空队列
        if (state.empty()) {
            state = {
                {"queue", json::array()}
            };
            db.updateGameState("queue", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "queue").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "queue").dump());
    }
}

// 队列游戏操作 - 重置状态
crow::response handleQueueReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的队列状态
        json emptyState = {
            {"queue", json::array()}
        };
        
        // 更新数据库中的状态
        db.updateGameState("queue", emptyState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "queue").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置队列状态失败: ") + e.what(), "queue").dump());
    }
}

// 栈游戏操作 - 入栈
crow::response handleStackPush(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    // 解析请求体
    json requestBody;
    try {
        requestBody = json::parse(req.body);
    } catch (const std::exception& e) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "无效的JSON格式", "stack").dump());
    }
    
    // 验证必需的参数
    if (!requestBody.contains("value")) {
        return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少必需的参数：value", "stack").dump());
    }
    
    int value = requestBody["value"];
    
    try {
        // 获取当前栈状态
        json currentState = db.getGameState("stack");
        
        // 执行入栈操作
        std::vector<int> stack;
        if (currentState.contains("stack") && currentState["stack"].is_array()) {
            stack = currentState["stack"].get<std::vector<int>>();
        }
        
        // 如果需要动画效果
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("stack", "push");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 添加初始帧
            json initialFrame = {
                {"stack", stack},
                {"currentIndex", -1},
                {"pushValue", value}
            };
            session.addFrame(initialFrame, "准备将值 " + std::to_string(value) + " 入栈");
            
            // 添加入栈帧
            stack.push_back(value);
            json pushFrame = {
                {"stack", stack},
                {"newValue", value},
                {"status", "completed"}
            };
            session.addFrame(pushFrame, "完成入栈值 " + std::to_string(value));
            
            // 更新游戏状态
            json newState = {
                {"stack", stack}
            };
            db.updateGameState("stack", newState);
            session.updateGameState(newState);
            
            // 根据模式返回帧
            if (params.step) {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "stack").dump());
            } else {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "stack").dump());
            }
        } else {
            // 不需要动画，直接执行操作
            stack.push_back(value);
            
            // 更新游戏状态
            json newState = {
                {"stack", stack}
            };
            db.updateGameState("stack", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "stack").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "stack").dump());
    }
}

// 栈游戏操作 - 出栈
crow::response handleStackPop(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前栈状态
        json currentState = db.getGameState("stack");
        
        // 执行出栈操作
        std::vector<int> stack;
        if (currentState.contains("stack") && currentState["stack"].is_array()) {
            stack = currentState["stack"].get<std::vector<int>>();
        }
        
        // 检查栈是否为空
        if (stack.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "栈为空，无法执行出栈操作", "stack").dump());
        }
        
        // 获取要出栈的值
        int popValue = stack.back();
        
        // 如果需要动画效果
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("stack", "pop");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 添加初始帧
            json initialFrame = {
                {"stack", stack},
                {"currentIndex", stack.size() - 1},
                {"popValue", popValue}
            };
            session.addFrame(initialFrame, "准备将栈顶元素 " + std::to_string(popValue) + " 出栈");
            
            // 添加出栈帧
            stack.pop_back();
            json popFrame = {
                {"stack", stack},
                {"currentIndex", -1},
                {"popValue", popValue},
                {"popped", true}
            };
            session.addFrame(popFrame, "将值 " + std::to_string(popValue) + " 从栈顶移除");
            
            // 添加最终帧
            json finalFrame = {
                {"stack", stack},
                {"currentIndex", -1},
                {"completed", true}
            };
            session.addFrame(finalFrame, "出栈操作完成");
            
            // 更新游戏状态
            json newState = {
                {"stack", stack},
                {"popValue", popValue}
            };
            db.updateGameState("stack", newState);
            session.updateGameState(newState);
            
            // 如果是单步执行模式，则返回第一帧
            if (params.step) {
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "stack").dump());
            } else {
                // 否则返回所有帧
                json frames = animManager->getAllFrames(sessionId);
                return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "stack").dump());
            }
        } else {
            // 不需要动画，直接执行出栈
            stack.pop_back();
            
            // 更新游戏状态
            json newState = {
                {"stack", stack},
                {"popValue", popValue}
            };
            db.updateGameState("stack", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "stack").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "stack").dump());
    }
}

// 栈游戏操作 - 查看栈顶元素
crow::response handleStackPeek(const crow::request& req, Database& db) {
    try {
        // 获取当前栈状态
        json state = db.getGameState("stack");
        
        // 获取栈
        std::vector<int> stack;
        if (state.contains("stack") && state["stack"].is_array()) {
            stack = state["stack"].get<std::vector<int>>();
        }
        
        // 如果栈为空，返回空状态
        if (stack.empty()) {
            json emptyState = {
                {"stack", stack},
                {"peek", nullptr}
            };
            return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "stack").dump());
        }
        
        // 返回栈顶元素
        json peekState = {
            {"stack", stack},
            {"peek", stack.back()}
        };
        return crow::response(200, ResponseBuilder::createSuccessResponse(peekState, "stack").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "stack").dump());
    }
}

// 获取栈状态
crow::response handleStackState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("stack");
        
        // 如果状态为空，初始化一个空栈
        if (state.empty()) {
            state = {
                {"stack", json::array()}
            };
            db.updateGameState("stack", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "stack").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "stack").dump());
    }
}

// 栈游戏操作 - 重置状态
crow::response handleStackReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的栈状态
        json emptyState = {
            {"stack", json::array()}
        };
        
        // 更新数据库中的状态
        db.updateGameState("stack", emptyState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "stack").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置栈状态失败: ") + e.what(), "stack").dump());
    }
}

// 二叉树游戏操作API处理函数实现
crow::response handleBinaryTreeInsert(const crow::request& req, Database& db) {
    try {
        // 声明value变量
        int value = 0;
        bool valueProvided = false;
        
        // 获取当前状态
        json currentState = db.getGameState("binarytree");
        if (currentState.is_null()) {
            currentState = json{
                {"nodes", json::array()},
                {"edges", json::array()},
                {"traversal", nullptr},
                {"root", nullptr}
            };
        }

        // 先检查请求体
        if (!req.body.empty()) {
            try {
                // 尝试解析请求体为JSON
                json requestBody = json::parse(req.body);
                // 检查value字段是否存在且不为null
                if (requestBody.contains("value")) {
                    if (requestBody["value"].is_number()) {
                        value = requestBody["value"].get<int>();
                        valueProvided = true;
                    } else if (requestBody["value"].is_string()) {
                        // 如果value是字符串，尝试转换为整数
                        try {
                            value = std::stoi(requestBody["value"].get<std::string>());
                            valueProvided = true;
                        } catch (...) {
                            return crow::response(400, ResponseBuilder::createErrorResponse(
                                400, "value参数必须是有效的整数", "binarytree").dump());
                        }
                    }
                }
            } catch (const json::parse_error& e) {
                // JSON解析失败，检查是否是表单数据
                std::string body = req.body;
                if (body.find("value=") != std::string::npos) {
                    size_t pos = body.find("value=") + 6;
                    std::string valueStr = body.substr(pos);
                    size_t endPos = valueStr.find('&');
                    if (endPos != std::string::npos) {
                        valueStr = valueStr.substr(0, endPos);
                    }
                    try {
                        value = std::stoi(valueStr);
                        valueProvided = true;
                    } catch (...) {
                        return crow::response(400, ResponseBuilder::createErrorResponse(
                            400, "value参数必须是有效的整数", "binarytree").dump());
                    }
                }
            }
        }
        
        // 如果请求体中没有value，则检查URL参数
        if (!valueProvided) {
            auto params = crow::query_string(req.url_params);
            std::string valueStr = params.get("value");
            if (!valueStr.empty()) {
                try {
                    value = std::stoi(valueStr);
                    valueProvided = true;
                } catch (...) {
                    return crow::response(400, ResponseBuilder::createErrorResponse(
                        400, "无效的value参数，必须是整数", "binarytree").dump());
                }
            }
        }
        
        // 如果没有提供value参数，返回错误
        if (!valueProvided) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少必要参数：value", "binarytree").dump());
        }

        // 获取动画参数
        auto params = crow::query_string(req.url_params);
        bool animate = params.get("animate") == "true";
        int speed = 1;
        std::string speedStr = params.get("speed");
        if (!speedStr.empty()) {
            try {
                speed = std::stoi(speedStr);
            } catch (...) {
                // 如果speed参数无效，使用默认值1
            }
        }

        // 获取现有节点和边
        std::vector<json> nodes = currentState["nodes"];
        std::vector<json> edges = currentState["edges"];

        // 创建新节点
        int newNodeId = nodes.empty() ? 0 : nodes.back()["id"].get<int>() + 1;
        
        // 如果是第一个节点
        if (nodes.empty()) {
            json newNode = json{
                {"id", newNodeId},
                {"value", value},
                {"x", 500},  // 根节点位置
                {"y", 50}
            };
            nodes.push_back(newNode);
            currentState["root"] = newNodeId;  // 设置根节点ID
        } else {
            // 找到插入位置
            int currentId = currentState["root"].get<int>();
            int parentId = -1;
            std::string position;
            int x = 500;  // 初始根节点位置
            int y = 50;   // 初始根节点位置
            int levelWidth = 800; // 初始层宽度
            
            // 使用队列进行广度优先搜索，找到第一个没有满子节点的节点
            std::queue<int> nodeQueue;
            nodeQueue.push(currentId);
            
            while (!nodeQueue.empty()) {
                currentId = nodeQueue.front();
                nodeQueue.pop();
                
                json& currentNode = *std::find_if(nodes.begin(), nodes.end(),
                    [currentId](const json& node) { return node["id"] == currentId; });
                
                // 检查当前节点是否已有左子节点
                bool hasLeftChild = false;
                bool hasRightChild = false;
                
                for (const auto& edge : edges) {
                    if (edge["source"] == currentId) {
                        if (edge.contains("position")) {
                            if (edge["position"] == "left") {
                                hasLeftChild = true;
                                int leftChildId = edge["target"];
                                nodeQueue.push(leftChildId);
                            } else if (edge["position"] == "right") {
                                hasRightChild = true;
                                int rightChildId = edge["target"];
                                nodeQueue.push(rightChildId);
                            }
                        }
                    }
                }
                
                // 如果没有左子节点，插入为左子节点
                // 否则，如果没有右子节点，插入为右子节点
                if (!hasLeftChild || !hasRightChild) {
                    parentId = currentId;
                    
                    // 计算当前层级
                    int level = (currentNode["y"].get<int>() - 50) / 100 + 1;
                    // 更新下一层垂直位置
                    y = 50 + level * 100;
                    // 计算水平间距
                    int horizontalOffset = std::max(100, static_cast<int>(levelWidth / std::pow(2, level)));
                    
                    if (!hasLeftChild) {
                        position = "left";
                        x = currentNode["x"].get<int>() - horizontalOffset;
                    } else {
                        position = "right";
                        x = currentNode["x"].get<int>() + horizontalOffset;
                    }
                    break;
                }
            }
            
            // 如果所有节点都有完整的左右子节点（不应该发生，除非是完全二叉树）
            // 那么添加到最后一个节点的右侧
            if (parentId == -1) {
                parentId = nodes.back()["id"].get<int>();
                json& lastNode = nodes.back();
                int level = (lastNode["y"].get<int>() - 50) / 100 + 1;
                y = 50 + level * 100;
                position = "right";
                x = lastNode["x"].get<int>() + 100;
            }

            // 创建新节点
            json newNode = json{
                {"id", newNodeId},
                {"value", value},
                {"x", x},
                {"y", y}
            };
            nodes.push_back(newNode);
            
            // 创建新边
            json newEdge = json{
                {"source", parentId},
                {"target", newNodeId},
                {"position", position}
            };
            edges.push_back(newEdge);
        }

        // 优化树的布局，重新计算所有节点的坐标
        if (currentState.contains("root") && !currentState["root"].is_null()) {
            recalculateTreeLayout(nodes, edges, currentState["root"].get<int>(), 500, 50, 100);
        }

        // 更新状态
        json updatedState = json{
            {"nodes", nodes},
            {"edges", edges},
            {"traversal", nullptr},
            {"root", currentState["root"]}  // 保持根节点ID
        };

        // 构建嵌套的树结构
        if (currentState.contains("root") && !currentState["root"].is_null()) {
            updatedState["tree"] = buildTreeStructure(nodes, edges, currentState["root"].get<int>());
        } else if (!nodes.empty()) {
            // 如果没有显式的root字段，默认使用第一个添加的节点作为根
            updatedState["tree"] = buildTreeStructure(nodes, edges, nodes[0]["id"].get<int>());
        } else {
            // 空树
            updatedState["tree"] = nullptr;
        }

        // 如果需要动画
        if (animate) {
            json insertFrame = json{
                {"type", "insert"},
                {"nodeId", newNodeId},
                {"value", value}
            };
            
            json animationData = json{
                {"frames", json::array({insertFrame})},
                {"currentFrame", 0},
                {"totalFrames", 1},
                {"speed", speed}
            };
            
            updatedState["animation"] = animationData;
        }

        // 更新数据库中的状态
        db.updateGameState("binarytree", updatedState);

        return crow::response(200, ResponseBuilder::createSuccessResponse(updatedState, "binarytree").dump());
    } catch (const std::invalid_argument& e) {
        // 处理参数错误
        return crow::response(400, ResponseBuilder::createErrorResponse(400, std::string("参数错误: ") + e.what(), "binarytree").dump());
    } catch (const std::out_of_range& e) {
        // 处理数值超出范围错误
        return crow::response(400, ResponseBuilder::createErrorResponse(400, std::string("数值超出范围: ") + e.what(), "binarytree").dump());
    } catch (const std::exception& e) {
        // 处理其他所有异常
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "binarytree").dump());
    }
}

// 二叉树游戏操作 - 前序遍历
crow::response handleBinaryTreeTraversePreOrder(const crow::request& req, Database& db) {
    // 提取动画参数
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前二叉树状态
        json currentState = db.getGameState("binarytree");
        
        // 确保树状态存在且有根节点
        if (currentState.is_null() || !currentState.contains("nodes") || !currentState.contains("edges") || 
            !currentState.contains("root") || currentState["nodes"].empty()) {
            json emptyState = {
                {"nodes", json::array()},
                {"edges", json::array()},
                {"traversal", {{"order", "preorder"}, {"sequence", json::array()}}},
                {"root", nullptr}
            };
            return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "binarytree").dump());
        }
        
        std::vector<json> nodes = currentState["nodes"].get<std::vector<json>>();
        std::vector<json> edges = currentState["edges"].get<std::vector<json>>();
        
        // 如果没有动画，直接执行遍历
        if (!params.animate) {
            std::vector<int> traversalResult;
            std::function<void(int)> preOrder = [&](int nodeId) {
                traversalResult.push_back(nodes[nodeId]["value"]);
                
                // 查找左子节点和右子节点
                std::optional<int> leftChildId;
                std::optional<int> rightChildId;
                
                // 优先使用position属性判断左右子节点
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                        if (edge.contains("position")) {
                            if (edge["position"] == "left") {
                                leftChildId = edge["target"].get<int>();
                            } else if (edge["position"] == "right") {
                                rightChildId = edge["target"].get<int>();
                            }
                        }
                    }
                }
                
                // 如果没有通过position找到子节点，则使用位置判断（兼容旧数据）
                if (!leftChildId.has_value() || !rightChildId.has_value()) {
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                            int targetId = edge["target"].get<int>();
                            json targetNode = *std::find_if(nodes.begin(), nodes.end(),
                                [targetId](const json& node) { return node["id"] == targetId; });
                            
                            if (targetNode["x"].get<int>() < nodes[nodeId]["x"].get<int>() && !leftChildId.has_value()) {
                                leftChildId = targetId;
                            } else if (targetNode["x"].get<int>() > nodes[nodeId]["x"].get<int>() && !rightChildId.has_value()) {
                                rightChildId = targetId;
                            }
                        }
                    }
                }
                
                // 先遍历左子树，后遍历右子树
                if (leftChildId.has_value()) {
                    int idx = std::distance(nodes.begin(), std::find_if(nodes.begin(), nodes.end(),
                        [&leftChildId](const json& node) { return node["id"] == leftChildId.value(); }));
                    preOrder(idx);
                }
                
                if (rightChildId.has_value()) {
                    int idx = std::distance(nodes.begin(), std::find_if(nodes.begin(), nodes.end(),
                        [&rightChildId](const json& node) { return node["id"] == rightChildId.value(); }));
                    preOrder(idx);
                }
            };
            
            // 从根节点开始遍历
            preOrder(0);
            
            // 更新遍历结果
            json updatedState = currentState;
            updatedState["traversal"] = traversalResult;
            
            // 添加树结构
            updatedState["tree"] = buildTreeStructure(nodes, edges, 0);
            
            // 更新数据库中的状态
            db.updateGameState("binarytree", updatedState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(updatedState, "binarytree").dump());
        }
        
        // 如果需要动画效果
        AnimationManager* animManager = AnimationManager::getInstance();
        std::string sessionId = animManager->createSession("binarytree", "traversePreOrder");
        AnimationSession& session = animManager->getSession(sessionId);
        
        // 添加初始帧
        json initialFrame = {
            {"nodes", nodes},
            {"edges", edges},
            {"currentNode", nullptr},
            {"visitedNodes", json::array()},
            {"traversalResult", json::array()}
        };
        session.addFrame(initialFrame, "开始前序遍历");
        
        std::vector<int> traversalResult;
        std::vector<int> visitedNodes;
        
        std::function<void(int)> preOrderWithAnimation = [&](int nodeId) {
            // 访问当前节点
            traversalResult.push_back(nodes[nodeId]["value"]);
            visitedNodes.push_back(nodeId);
            
            // 添加访问帧
            json visitFrame = {
                {"nodes", nodes},
                {"edges", edges},
                {"currentNode", nodeId},
                {"visitedNodes", visitedNodes},
                {"traversalResult", traversalResult}
            };
            session.addFrame(visitFrame, "访问节点 " + std::to_string(nodes[nodeId]["value"].get<int>()));
            
            // 查找左子节点和右子节点
            std::optional<int> leftChildId;
            std::optional<int> rightChildId;
            
            // 优先使用position属性判断左右子节点
            for (const auto& edge : edges) {
                if (edge["source"] == nodeId) {
                    if (edge.contains("position")) {
                        if (edge["position"] == "left") {
                            leftChildId = edge["target"].get<int>();
                        } else if (edge["position"] == "right") {
                            rightChildId = edge["target"].get<int>();
                        }
                    }
                }
            }
            
            // 如果没有通过position找到子节点，则使用位置判断（兼容旧数据）
            if (!leftChildId.has_value() || !rightChildId.has_value()) {
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                        int targetId = edge["target"].get<int>();
                        json targetNode = *std::find_if(nodes.begin(), nodes.end(),
                            [targetId](const json& node) { return node["id"] == targetId; });
                        
                        if (targetNode["x"].get<int>() < nodes[nodeId]["x"].get<int>() && !leftChildId.has_value()) {
                            leftChildId = targetId;
                        } else if (targetNode["x"].get<int>() > nodes[nodeId]["x"].get<int>() && !rightChildId.has_value()) {
                            rightChildId = targetId;
                        }
                    }
                }
            }
            
            // 先遍历左子树
            if (leftChildId.has_value()) {
                        // 添加移动到左子节点的帧
                        json moveFrame = {
                            {"nodes", nodes},
                            {"edges", edges},
                    {"currentNode", leftChildId.value()},
                            {"visitedNodes", visitedNodes},
                            {"traversalResult", traversalResult},
                            {"direction", "left"}
                        };
                        session.addFrame(moveFrame, "移动到左子节点");
                        
                preOrderWithAnimation(leftChildId.value());
            }
            
            // 后遍历右子树
            if (rightChildId.has_value()) {
                        // 添加移动到右子节点的帧
                        json moveFrame = {
                            {"nodes", nodes},
                            {"edges", edges},
                    {"currentNode", rightChildId.value()},
                            {"visitedNodes", visitedNodes},
                            {"traversalResult", traversalResult},
                            {"direction", "right"}
                        };
                        session.addFrame(moveFrame, "移动到右子节点");
                        
                preOrderWithAnimation(rightChildId.value());
            }
            
            // 如果是叶子节点，添加回溯帧
            bool isLeaf = !leftChildId.has_value() && !rightChildId.has_value();
            if (isLeaf) {
                json backtrackFrame = {
                    {"nodes", nodes},
                    {"edges", edges},
                    {"currentNode", nodeId},
                    {"visitedNodes", visitedNodes},
                    {"traversalResult", traversalResult},
                    {"backtrack", true}
                };
                session.addFrame(backtrackFrame, "从叶子节点回溯");
            }
        };
        
        // 从根节点开始遍历
        preOrderWithAnimation(0);
        
        // 添加最终帧
        json finalFrame = {
            {"nodes", nodes},
            {"edges", edges},
            {"currentNode", nullptr},
            {"visitedNodes", visitedNodes},
            {"traversalResult", traversalResult},
            {"completed", true}
        };
        session.addFrame(finalFrame, "前序遍历完成");
        
        // 更新遍历结果
        json updatedState = currentState;
        updatedState["traversal"] = traversalResult;
        
        // 添加树结构
        updatedState["tree"] = buildTreeStructure(nodes, edges, 0);
        
        // 更新数据库中的状态
        db.updateGameState("binarytree", updatedState);
        session.updateGameState(updatedState);
        
        // 计算总步数和速度
        int totalSteps = session.getFrameCount();
        int speed = 1000 / params.speed; // 将1-10的速度转换为毫秒
        
        return crow::response(200, ResponseBuilder::createAnimationResponse(updatedState, animManager->getAllFrames(sessionId), totalSteps, speed, "binarytree").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "binarytree").dump());
    }
}

// 二叉树游戏操作 - 中序遍历
crow::response handleBinaryTreeTraverseInOrder(const crow::request& req, Database& db) {
    // 提取动画参数
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前二叉树状态
        json currentState = db.getGameState("binarytree");
        
        // 确保树状态存在且有根节点
        if (currentState.is_null() || !currentState.contains("nodes") || !currentState.contains("edges") || 
            !currentState.contains("root") || currentState["nodes"].empty()) {
            json emptyState = {
                {"nodes", json::array()},
                {"edges", json::array()},
                {"traversal", {{"order", "inorder"}, {"sequence", json::array()}}},
                {"root", nullptr}
            };
            return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "binarytree").dump());
        }
        
        // 获取节点和边的列表
        std::vector<json> nodes = currentState["nodes"].get<std::vector<json>>();
        std::vector<json> edges = currentState["edges"].get<std::vector<json>>();
        
        // 如果不需要动画，直接执行中序遍历
        if (!params.animate) {
            std::vector<int> result;
            std::function<void(int)> inOrder = [&](int nodeId) {
                // 查找左子节点和右子节点
                std::optional<int> leftChildId;
                std::optional<int> rightChildId;
                
                // 优先使用position属性判断左右子节点
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                        if (edge.contains("position")) {
                            if (edge["position"] == "left") {
                                leftChildId = edge["target"].get<int>();
                            } else if (edge["position"] == "right") {
                                rightChildId = edge["target"].get<int>();
                            }
                        }
                    }
                }
                
                // 如果没有通过position找到子节点，则使用位置判断（兼容旧数据）
                if (!leftChildId.has_value() || !rightChildId.has_value()) {
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                            int targetId = edge["target"].get<int>();
                            json targetNode = *std::find_if(nodes.begin(), nodes.end(),
                                [targetId](const json& node) { return node["id"] == targetId; });
                            
                            if (targetNode["x"].get<int>() < nodes[nodeId]["x"].get<int>() && !leftChildId.has_value()) {
                                leftChildId = targetId;
                            } else if (targetNode["x"].get<int>() > nodes[nodeId]["x"].get<int>() && !rightChildId.has_value()) {
                                rightChildId = targetId;
                            }
                        }
                    }
                }
                
                // 先遍历左子树
                if (leftChildId.has_value()) {
                    inOrder(leftChildId.value());
                }
                
                // 访问当前节点
                result.push_back(nodes[nodeId]["value"].get<int>());
                
                // 后遍历右子树
                if (rightChildId.has_value()) {
                    inOrder(rightChildId.value());
                }
            };
            
            // 从根节点开始遍历
            inOrder(0);
            
            // 更新游戏状态
            json updatedState = currentState;
            updatedState["traversal"] = {
                {"type", "inorder"},
                {"result", result}
            };
            
            // 添加树结构
            updatedState["tree"] = buildTreeStructure(nodes, edges, 0);
            
            db.updateGameState("binarytree", updatedState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(updatedState, "binarytree").dump());
        }
        
        // 需要动画效果
        AnimationManager* animManager = AnimationManager::getInstance();
        std::string sessionId = animManager->createSession("binarytree", "inorder");
        AnimationSession& session = animManager->getSession(sessionId);
        
        // 添加初始帧
        json initialFrame = {
            {"nodes", nodes},
            {"edges", edges},
            {"currentNode", nullptr},
            {"visited", json::array()},
            {"traversalResult", json::array()}
        };
        session.addFrame(initialFrame, "开始中序遍历");
        
        // 执行中序遍历并生成动画帧
        std::vector<int> result;
        std::function<void(int)> generateInOrderFrames = [&](int nodeId) {
            // 查找左子节点和右子节点
            std::optional<int> leftChildId;
            std::optional<int> rightChildId;
            
            // 优先使用position属性判断左右子节点
            for (const auto& edge : edges) {
                if (edge["source"] == nodeId) {
                    if (edge.contains("position")) {
                        if (edge["position"] == "left") {
                            leftChildId = edge["target"].get<int>();
                        } else if (edge["position"] == "right") {
                            rightChildId = edge["target"].get<int>();
                        }
                    }
                }
            }
            
            // 如果没有通过position找到子节点，则使用位置判断（兼容旧数据）
            if (!leftChildId.has_value() || !rightChildId.has_value()) {
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                        int targetId = edge["target"].get<int>();
                        json targetNode = *std::find_if(nodes.begin(), nodes.end(),
                            [targetId](const json& node) { return node["id"] == targetId; });
                        
                        if (targetNode["x"].get<int>() < nodes[nodeId]["x"].get<int>() && !leftChildId.has_value()) {
                            leftChildId = targetId;
                        } else if (targetNode["x"].get<int>() > nodes[nodeId]["x"].get<int>() && !rightChildId.has_value()) {
                            rightChildId = targetId;
                        }
                    }
                }
            }
            
            // 先遍历左子树
            if (leftChildId.has_value()) {
                        json searchFrame = {
                            {"nodes", nodes},
                            {"edges", edges},
                            {"currentNode", nodeId},
                    {"nextNode", leftChildId.value()},
                            {"visited", result},
                            {"traversalResult", result}
                        };
                        session.addFrame(searchFrame, "查找节点 " + std::to_string(nodes[nodeId]["value"].get<int>()) + " 的左子节点");
                        
                generateInOrderFrames(leftChildId.value());
            }
            
            // 访问当前节点
            json visitFrame = {
                {"nodes", nodes},
                {"edges", edges},
                {"currentNode", nodeId},
                {"visited", result},
                {"traversalResult", result}
            };
            session.addFrame(visitFrame, "访问节点 " + std::to_string(nodes[nodeId]["value"].get<int>()));
            
            result.push_back(nodes[nodeId]["value"].get<int>());
            
            json resultFrame = {
                {"nodes", nodes},
                {"edges", edges},
                {"currentNode", nodeId},
                {"visited", result},
                {"traversalResult", result}
            };
            session.addFrame(resultFrame, "将节点 " + std::to_string(nodes[nodeId]["value"].get<int>()) + " 添加到遍历结果");
            
            // 遍历右子树
            if (rightChildId.has_value()) {
                        json searchFrame = {
                            {"nodes", nodes},
                            {"edges", edges},
                            {"currentNode", nodeId},
                    {"nextNode", rightChildId.value()},
                            {"visited", result},
                            {"traversalResult", result}
                        };
                        session.addFrame(searchFrame, "查找节点 " + std::to_string(nodes[nodeId]["value"].get<int>()) + " 的右子节点");
                        
                generateInOrderFrames(rightChildId.value());
            }
        };
        
        // 从根节点开始遍历
        generateInOrderFrames(0);
        
        // 添加最终帧
        json finalFrame = {
            {"nodes", nodes},
            {"edges", edges},
            {"currentNode", nullptr},
            {"visited", result},
            {"traversalResult", result},
            {"completed", true}
        };
        session.addFrame(finalFrame, "中序遍历完成: " + 
                                   std::accumulate(
                                       result.begin(), 
                                       result.end(), 
                                       std::string(), 
                                       [](const std::string& a, int b) {
                                           return a + (a.empty() ? "" : ", ") + std::to_string(b);
                                       }));
        
        // 更新游戏状态
        json updatedState = currentState;
        updatedState["traversal"] = {
            {"type", "inorder"},
            {"result", result}
        };
        db.updateGameState("binarytree", updatedState);
        session.updateGameState(updatedState);
        
        // 添加树结构
        updatedState["tree"] = buildTreeStructure(nodes, edges, 0);
        
        // 计算总步数和速度
        int totalSteps = session.getFrameCount();
        int speed = 1000 / params.speed; // 将1-10的速度转换为毫秒
        
        return crow::response(200, ResponseBuilder::createAnimationResponse(updatedState, animManager->getAllFrames(sessionId), totalSteps, speed, "binarytree").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "binarytree").dump());
    }
}

// 二叉树游戏操作 - 后序遍历
crow::response handleBinaryTreeTraversePostOrder(const crow::request& req, Database& db) {
    // 提取动画参数
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前二叉树状态
        json currentState = db.getGameState("binarytree");
        
        // 确保树状态存在且有根节点
        if (currentState.is_null() || !currentState.contains("nodes") || !currentState.contains("edges") || 
            !currentState.contains("root") || currentState["nodes"].empty()) {
            json emptyState = {
                {"nodes", json::array()},
                {"edges", json::array()},
                {"traversal", {{"order", "postorder"}, {"sequence", json::array()}}},
                {"root", nullptr}
            };
            return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "binarytree").dump());
        }
        
        // 获取节点和边的列表
        std::vector<json> nodes = currentState["nodes"].get<std::vector<json>>();
        std::vector<json> edges = currentState["edges"].get<std::vector<json>>();
        
        // 如果不需要动画，直接执行后序遍历
        if (!params.animate) {
            std::vector<int> result;
            std::function<void(int)> postOrder = [&](int nodeId) {
                // 查找左子节点和右子节点
                std::optional<int> leftChildId;
                std::optional<int> rightChildId;
                
                // 优先使用position属性判断左右子节点
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                        if (edge.contains("position")) {
                            if (edge["position"] == "left") {
                                leftChildId = edge["target"].get<int>();
                            } else if (edge["position"] == "right") {
                                rightChildId = edge["target"].get<int>();
                            }
                        }
                    }
                }
                
                // 如果没有通过position找到子节点，则使用位置判断（兼容旧数据）
                if (!leftChildId.has_value() || !rightChildId.has_value()) {
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                            int targetId = edge["target"].get<int>();
                            json targetNode = *std::find_if(nodes.begin(), nodes.end(),
                                [targetId](const json& node) { return node["id"] == targetId; });
                            
                            if (targetNode["x"].get<int>() < nodes[nodeId]["x"].get<int>() && !leftChildId.has_value()) {
                                leftChildId = targetId;
                            } else if (targetNode["x"].get<int>() > nodes[nodeId]["x"].get<int>() && !rightChildId.has_value()) {
                                rightChildId = targetId;
                            }
                        }
                    }
                }
                
                // 先遍历左子树
                if (leftChildId.has_value()) {
                    postOrder(leftChildId.value());
                }
                
                // 再遍历右子树
                if (rightChildId.has_value()) {
                    postOrder(rightChildId.value());
                }
                
                // 最后访问当前节点
                result.push_back(nodes[nodeId]["value"].get<int>());
            };
            
            // 从根节点开始遍历
            postOrder(0);
            
            // 更新游戏状态
            json updatedState = currentState;
            updatedState["traversal"] = {
                {"type", "postorder"},
                {"result", result}
            };
            
            // 添加树结构
            updatedState["tree"] = buildTreeStructure(nodes, edges, 0);
            
            db.updateGameState("binarytree", updatedState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(updatedState, "binarytree").dump());
        }
        
        // 需要动画效果
        AnimationManager* animManager = AnimationManager::getInstance();
        std::string sessionId = animManager->createSession("binarytree", "postorder");
        AnimationSession& session = animManager->getSession(sessionId);
        
        // 添加初始帧
        json initialFrame = {
            {"nodes", nodes},
            {"edges", edges},
            {"currentNode", nullptr},
            {"visited", json::array()},
            {"traversalResult", json::array()}
        };
        session.addFrame(initialFrame, "开始后序遍历");
        
        // 执行后序遍历并生成动画帧
        std::vector<int> result;
        std::function<void(int)> generatePostOrderFrames = [&](int nodeId) {
            // 查找左子节点和右子节点
            std::optional<int> leftChildId;
            std::optional<int> rightChildId;
            
            // 优先使用position属性判断左右子节点
            for (const auto& edge : edges) {
                if (edge["source"] == nodeId) {
                    if (edge.contains("position")) {
                        if (edge["position"] == "left") {
                            leftChildId = edge["target"].get<int>();
                        } else if (edge["position"] == "right") {
                            rightChildId = edge["target"].get<int>();
                        }
                    }
                }
            }
            
            // 如果没有通过position找到子节点，则使用位置判断（兼容旧数据）
            if (!leftChildId.has_value() || !rightChildId.has_value()) {
                for (const auto& edge : edges) {
                    if (edge["source"] == nodeId) {
                        int targetId = edge["target"].get<int>();
                        json targetNode = *std::find_if(nodes.begin(), nodes.end(),
                            [targetId](const json& node) { return node["id"] == targetId; });
                        
                        if (targetNode["x"].get<int>() < nodes[nodeId]["x"].get<int>() && !leftChildId.has_value()) {
                            leftChildId = targetId;
                        } else if (targetNode["x"].get<int>() > nodes[nodeId]["x"].get<int>() && !rightChildId.has_value()) {
                            rightChildId = targetId;
                        }
                    }
                }
            }
            
            // 先遍历左子树
            if (leftChildId.has_value()) {
                        json searchFrame = {
                            {"nodes", nodes},
                            {"edges", edges},
                            {"currentNode", nodeId},
                    {"nextNode", leftChildId.value()},
                            {"visited", result},
                            {"traversalResult", result}
                        };
                        session.addFrame(searchFrame, "查找节点 " + std::to_string(nodes[nodeId]["value"].get<int>()) + " 的左子节点");
                        
                generatePostOrderFrames(leftChildId.value());
            }
            
            // 再遍历右子树
            if (rightChildId.has_value()) {
                        json searchFrame = {
                            {"nodes", nodes},
                            {"edges", edges},
                            {"currentNode", nodeId},
                    {"nextNode", rightChildId.value()},
                            {"visited", result},
                            {"traversalResult", result}
                        };
                        session.addFrame(searchFrame, "查找节点 " + std::to_string(nodes[nodeId]["value"].get<int>()) + " 的右子节点");
                        
                generatePostOrderFrames(rightChildId.value());
            }
            
            // 最后访问当前节点
            json visitFrame = {
                {"nodes", nodes},
                {"edges", edges},
                {"currentNode", nodeId},
                {"visited", result},
                {"traversalResult", result}
            };
            session.addFrame(visitFrame, "访问节点 " + std::to_string(nodes[nodeId]["value"].get<int>()));
            
            result.push_back(nodes[nodeId]["value"].get<int>());
            
            json resultFrame = {
                {"nodes", nodes},
                {"edges", edges},
                {"currentNode", nodeId},
                {"visited", result},
                {"traversalResult", result}
            };
            session.addFrame(resultFrame, "将节点 " + std::to_string(nodes[nodeId]["value"].get<int>()) + " 添加到遍历结果");
        };
        
        // 从根节点开始遍历
        generatePostOrderFrames(0);
        
        // 添加最终帧
        json finalFrame = {
            {"nodes", nodes},
            {"edges", edges},
            {"currentNode", nullptr},
            {"visited", result},
            {"traversalResult", result},
            {"completed", true}
        };
        session.addFrame(finalFrame, "后序遍历完成: " + 
                                   std::accumulate(
                                       result.begin(), 
                                       result.end(), 
                                       std::string(), 
                                       [](const std::string& a, int b) {
                                           return a + (a.empty() ? "" : ", ") + std::to_string(b);
                                       }));
        
        // 更新游戏状态
        json updatedState = currentState;
        updatedState["traversal"] = {
            {"type", "postorder"},
            {"result", result}
        };
        db.updateGameState("binarytree", updatedState);
        session.updateGameState(updatedState);
        
        // 添加树结构
        updatedState["tree"] = buildTreeStructure(nodes, edges, 0);
        
        // 计算总步数和速度
        int totalSteps = session.getFrameCount();
        int speed = 1000 / params.speed; // 将1-10的速度转换为毫秒
        
        return crow::response(200, ResponseBuilder::createAnimationResponse(updatedState, animManager->getAllFrames(sessionId), totalSteps, speed, "binarytree").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "binarytree").dump());
    }
}

// 从旧格式的树结构构建新格式
json buildTreeFromOldFormat(const json& treeState, int nodeId) {
    if (nodeId < 0 || !treeState["nodes"].contains(std::to_string(nodeId))) {
        return nullptr;
    }
    
    const json& node = treeState["nodes"][std::to_string(nodeId)];
    
    json newNode = {
        {"value", node["value"]},
        {"id", nodeId},
        {"left", nullptr},
        {"right", nullptr}
    };
    
    // 递归构建左子树
    if (node.contains("left") && !node["left"].is_null()) {
        int leftId = node["left"].get<int>();
        newNode["left"] = buildTreeFromOldFormat(treeState, leftId);
    }
    
    // 递归构建右子树
    if (node.contains("right") && !node["right"].is_null()) {
        int rightId = node["right"].get<int>();
        newNode["right"] = buildTreeFromOldFormat(treeState, rightId);
    }
    
    return newNode;
}

crow::response handleBinaryTreeTraverseLevelOrder(const crow::request& req, Database& db) {
    // 提取动画参数
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 获取当前二叉树状态
        json currentState = db.getGameState("binarytree");
        
        // 确保树状态存在且有根节点
        if (currentState.is_null() || !currentState.contains("nodes") || !currentState.contains("edges") || 
            !currentState.contains("root") || currentState["nodes"].empty()) {
            json emptyState = {
                {"nodes", json::array()},
                {"edges", json::array()},
                {"traversal", {{"order", "levelorder"}, {"sequence", json::array()}}},
                {"root", nullptr}
            };
            return crow::response(200, ResponseBuilder::createSuccessResponse(emptyState, "binarytree").dump());
        }
        
        // 获取节点和边的列表
        std::vector<json> nodes = currentState["nodes"].get<std::vector<json>>();
        std::vector<json> edges = currentState["edges"].get<std::vector<json>>();
        
        // 获取根节点ID
        int rootId = 0;
        if (currentState.contains("root") && !currentState["root"].is_null()) {
            rootId = currentState["root"].get<int>();
        }
        
        // 创建节点ID到索引的映射
        std::map<int, size_t> nodeIdToIndex;
        for (size_t i = 0; i < nodes.size(); i++) {
            nodeIdToIndex[nodes[i]["id"].get<int>()] = i;
        }
        
        // 创建父节点到子节点的映射
        std::map<int, std::vector<std::pair<int, std::string>>> childrenMap;
        for (const auto& edge : edges) {
            int source = edge["source"].get<int>();
            int target = edge["target"].get<int>();
            std::string position = edge.contains("position") ? edge["position"].get<std::string>() : "";
            childrenMap[source].push_back({target, position});
        }
        
        // 如果不需要动画，直接执行层序遍历
        if (!params.animate) {
            std::vector<int> result;
            std::queue<int> q;
            q.push(rootId);
            
            while (!q.empty()) {
                int nodeId = q.front();
                q.pop();
                
                // 当前节点的索引
                size_t idx = nodeIdToIndex[nodeId];
                
                // 添加当前节点值到结果
                result.push_back(nodes[idx]["value"].get<int>());
                
                // 获取子节点，优先使用左右位置信息
                std::optional<int> leftChildId;
                std::optional<int> rightChildId;
                
                for (const auto& [childId, position] : childrenMap[nodeId]) {
                    if (position == "left") {
                        leftChildId = childId;
                    } else if (position == "right") {
                        rightChildId = childId;
                    }
                }
                
                // 先左子节点入队
                if (leftChildId.has_value()) {
                    q.push(leftChildId.value());
                }
                
                // 再右子节点入队
                if (rightChildId.has_value()) {
                    q.push(rightChildId.value());
                }
                
                // 处理没有明确位置的子节点（兼容旧数据）
                for (const auto& [childId, position] : childrenMap[nodeId]) {
                    if (position.empty()) {
                        q.push(childId);
                    }
                }
            }
            
            // 更新游戏状态
            json updatedState = currentState;
            updatedState["traversal"] = {
                {"type", "levelorder"},
                {"result", result}
            };
            
            // 添加树结构
            updatedState["tree"] = buildTreeStructure(nodes, edges, rootId);
            
            db.updateGameState("binarytree", updatedState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(updatedState, "binarytree").dump());
        }
        
        // 需要动画效果
        AnimationManager* animManager = AnimationManager::getInstance();
        std::string sessionId = animManager->createSession("binarytree", "levelorder");
        AnimationSession& session = animManager->getSession(sessionId);
        
        // 添加初始帧
        json initialFrame = {
            {"nodes", nodes},
            {"edges", edges},
            {"currentNode", nullptr},
            {"visited", json::array()},
            {"traversalResult", json::array()},
            {"queue", json::array()}
        };
        session.addFrame(initialFrame, "开始层序遍历：使用队列，逐层访问节点");
        
        // 执行层序遍历并生成动画帧
        std::vector<int> result;
        std::vector<int> visited;
        std::queue<int> q;
        q.push(rootId);
        
        // 根节点入队帧
        json queueFrame = initialFrame;
        json queueArray = json::array();
        queueArray.push_back(rootId);
        queueFrame["queue"] = queueArray;
        session.addFrame(queueFrame, "将根节点加入队列");
        
        while (!q.empty()) {
            int nodeId = q.front();
            q.pop();
            
            // 更新队列状态帧
            json queueStatusFrame = {
                {"nodes", nodes},
                {"edges", edges},
                {"currentNode", nodeId},
                {"visited", visited},
                {"traversalResult", result}
            };
            
            // 更新队列（复制当前队列状态，移除第一个元素）
            std::queue<int> tempQueue = q;
            json updatedQueue = json::array();
            while (!tempQueue.empty()) {
                updatedQueue.push_back(tempQueue.front());
                tempQueue.pop();
            }
            queueStatusFrame["queue"] = updatedQueue;
            
            session.addFrame(queueStatusFrame, "从队列中取出节点 " + std::to_string(nodes[nodeIdToIndex[nodeId]]["value"].get<int>()));
            
            // 访问当前节点
            visited.push_back(nodeId);
            result.push_back(nodes[nodeIdToIndex[nodeId]]["value"].get<int>());
            
            json visitFrame = {
                {"nodes", nodes},
                {"edges", edges},
                {"currentNode", nodeId},
                {"visited", visited},
                {"traversalResult", result},
                {"queue", updatedQueue}
            };
            
            session.addFrame(visitFrame, "访问节点 " + std::to_string(nodes[nodeIdToIndex[nodeId]]["value"].get<int>()) + 
                                         " 并将其添加到结果");
            
            // 获取子节点，优先使用左右位置信息
            std::vector<std::pair<int, std::string>> children;
            for (const auto& [childId, position] : childrenMap[nodeId]) {
                if (position == "left" || position == "right") {
                    children.push_back({childId, position});
                }
            }
            
            // 没有明确位置的子节点放在最后
            for (const auto& [childId, position] : childrenMap[nodeId]) {
                if (position.empty()) {
                    children.push_back({childId, position});
                }
            }
            
            // 子节点按位置入队（先左后右）
            for (const auto& [childId, position] : children) {
                q.push(childId);
                
                // 更新队列
                tempQueue = q;
                updatedQueue = json::array();
                while (!tempQueue.empty()) {
                    updatedQueue.push_back(tempQueue.front());
                    tempQueue.pop();
                }
                
                json enqueueFrame = {
                    {"nodes", nodes},
                    {"edges", edges},
                    {"currentNode", childId},
                    {"visited", visited},
                    {"traversalResult", result},
                    {"queue", updatedQueue}
                };
                
                std::string positionStr = position.empty() ? "" : position + " ";
                session.addFrame(enqueueFrame, "将" + positionStr + "子节点 " + 
                                             std::to_string(nodes[nodeIdToIndex[childId]]["value"].get<int>()) + 
                                             " 加入队列");
            }
        }
        
        // 添加最终帧
        json finalFrame = {
            {"nodes", nodes},
            {"edges", edges},
            {"currentNode", nullptr},
            {"visited", visited},
            {"traversalResult", result},
            {"queue", json::array()},
            {"completed", true}
        };
        
        session.addFrame(finalFrame, "层序遍历完成: " + 
                                   std::accumulate(
                                       result.begin(), 
                                       result.end(), 
                                       std::string(), 
                                       [](const std::string& a, int b) {
                                           return a + (a.empty() ? "" : ", ") + std::to_string(b);
                                       }));
        
        // 更新游戏状态
        json updatedState = currentState;
        updatedState["traversal"] = {
            {"type", "levelorder"},
            {"result", result}
        };
        
        // 添加树结构
        updatedState["tree"] = buildTreeStructure(nodes, edges, rootId);
        
        db.updateGameState("binarytree", updatedState);
        session.updateGameState(updatedState);
        
        // 计算总步数和速度
        int totalSteps = session.getFrameCount();
        int speed = 1000 / params.speed; // 将1-10的速度转换为毫秒
        
        return crow::response(200, ResponseBuilder::createAnimationResponse(updatedState, animManager->getAllFrames(sessionId), totalSteps, speed, "binarytree").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "binarytree").dump());
    }
}

crow::response handleBinaryTreeState(const crow::request& req, Database& db) {
    try {
        json currentState = db.getGameState("binarytree");
        
        if (currentState.is_null() || currentState.empty()) {
            currentState = {
                {"nodes", json::array()},
                {"edges", json::array()},
                {"traversal", nullptr},
                {"root", nullptr}
            };
        }
        
        // 如果有节点数据，优化树的布局
        if (currentState.contains("nodes") && currentState.contains("edges") && 
            currentState.contains("root") && !currentState["root"].is_null() &&
            !currentState["nodes"].empty()) {
            
            std::vector<json> nodes = currentState["nodes"].get<std::vector<json>>();
            std::vector<json> edges = currentState["edges"].get<std::vector<json>>();
            
            // 重新计算布局
            recalculateTreeLayout(nodes, edges, currentState["root"].get<int>(), 500, 50, 100);
            
            // 更新节点和边
            currentState["nodes"] = nodes;
            currentState["edges"] = edges;
            
            // 更新树结构
            currentState["tree"] = buildTreeStructure(nodes, edges, currentState["root"].get<int>());
            
            // 更新数据库
            db.updateGameState("binarytree", currentState);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(currentState, "binarytree").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "binarytree").dump());
    }
}

// 二叉树游戏操作 - 重置状态
crow::response handleBinaryTreeReset(const crow::request& req, Database& db) {
    try {
        // 创建一个空的二叉树状态
        json emptyState = {
            {"nodes", json::array()},
            {"edges", json::array()},
            {"traversal", nullptr},
            {"tree", nullptr}
        };
        
        // 更新数据库中的状态
        db.updateGameState("binarytree", emptyState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(
            {{"message", "二叉树状态已重置"}}, "binarytree").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置二叉树状态失败: ") + e.what(), "binarytree").dump());
    }
}

// 注册游戏动画路由
void registerGameAnimationRoutes(crow::App<crow::CORSHandler>& app, Database& db) {
    // 链表游戏操作
    CROW_ROUTE(app, "/api/games/linkedlist/insert").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleLinkedListInsert(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/linkedlist/delete").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleLinkedListDelete(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/linkedlist/find").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleLinkedListFind(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/linkedlist/reverse").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleLinkedListReverse(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/linkedlist/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleLinkedListState(req, db);
    });
    
    // 添加重置链表状态路由
    CROW_ROUTE(app, "/api/games/linkedlist/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleLinkedListReset(req, db);
    });
    
    // 队列游戏操作
    CROW_ROUTE(app, "/api/games/queue/enqueue").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleQueueEnqueue(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/queue/dequeue").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleQueueDequeue(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/queue/peek").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleQueuePeek(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/queue/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleQueueState(req, db);
    });
    
    // 添加重置队列状态路由
    CROW_ROUTE(app, "/api/games/queue/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleQueueReset(req, db);
    });
    
    // 栈游戏操作
    CROW_ROUTE(app, "/api/games/stack/push").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleStackPush(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/stack/pop").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleStackPop(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/stack/peek").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleStackPeek(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/stack/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleStackState(req, db);
    });
    
    // 添加重置栈状态路由
    CROW_ROUTE(app, "/api/games/stack/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleStackReset(req, db);
    });
    
    // 二叉树游戏操作
    CROW_ROUTE(app, "/api/games/binarytree/insert").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleBinaryTreeInsert(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/binarytree/traversePreOrder").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleBinaryTreeTraversePreOrder(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/binarytree/traverseInOrder").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleBinaryTreeTraverseInOrder(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/binarytree/traversePostOrder").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleBinaryTreeTraversePostOrder(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/binarytree/traverseLevelOrder").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleBinaryTreeTraverseLevelOrder(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/binarytree/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleBinaryTreeState(req, db);
    });
    
    // 添加重置二叉树状态路由
    CROW_ROUTE(app, "/api/games/binarytree/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleBinaryTreeReset(req, db);
    });
    
    // 图游戏操作
    CROW_ROUTE(app, "/api/games/graph/traverseDFS").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleGraphTraverseDFS(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/graph/traverseBFS").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleGraphTraverseBFS(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/graph/findPath").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleGraphFindPath(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/graph/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleGraphState(req, db);
    });
    
    // 添加重置图状态路由
    CROW_ROUTE(app, "/api/games/graph/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleGraphReset(req, db);
    });
    
    // 搜索游戏操作
    CROW_ROUTE(app, "/api/games/search/linear").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleSearchLinear(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/search/binary").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleSearchBinary(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/search/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleSearchState(req, db);
    });
    
    // 添加重置搜索状态路由
    CROW_ROUTE(app, "/api/games/search/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleSearchReset(req, db);
    });
    
    // 迷宫游戏操作
    CROW_ROUTE(app, "/api/games/maze/generate").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleMazeGenerate(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/maze/solveDFS").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleMazeSolveDFS(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/maze/solveBFS").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleMazeSolveBFS(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/maze/solveAStar").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleMazeSolveAStar(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/maze/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleMazeState(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/maze/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleMazeReset(req, db);
    });
    
    // 动态规划游戏操作
    CROW_ROUTE(app, "/api/games/dp/fibonacci").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleDPFibonacci(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/dp/knapsack").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleDPKnapsack(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/dp/lcs").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleDPLCS(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/dp/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleDPState(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/dp/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleDPReset(req, db);
    });
    
    // 贪心算法游戏操作
    CROW_ROUTE(app, "/api/games/greedy/coinChange").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleGreedyCoinChange(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/greedy/activitySelection").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleGreedyActivitySelection(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/greedy/huffman").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleGreedyHuffman(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/greedy/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleGreedyState(req, db);
    });
    
    CROW_ROUTE(app, "/api/games/greedy/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleGreedyReset(req, db);
    });
    
    // 获取排序状态
    CROW_ROUTE(app, "/api/games/sorting/state").methods(crow::HTTPMethod::GET)
    ([&db](const crow::request& req) {
        return handleSortingState(req, db);
    });
    CROW_ROUTE(app, "/api/games/sorting/reset").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleSortingReset(req, db);
    });
    CROW_ROUTE(app, "/api/games/sorting/bubbleSort").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleSortingBubbleSort(req, db);
    });
    CROW_ROUTE(app, "/api/games/sorting/selectionSort").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleSortingSelectionSort(req, db);
    });
    CROW_ROUTE(app, "/api/games/sorting/insertionSort").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleSortingInsertionSort(req, db);
    });
    CROW_ROUTE(app, "/api/games/sorting/quickSort").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        return handleSortingQuickSort(req, db);
    });
} 

// 重新计算二叉树所有节点的x坐标，使布局更加平衡
void recalculateTreeLayout(std::vector<json>& nodes, const std::vector<json>& edges, int rootId, int baseX, int baseY, int levelHeight) {
    if (nodes.empty()) return;
    
    // 创建节点ID到节点索引的映射
    std::map<int, size_t> nodeIdToIndex;
    for (size_t i = 0; i < nodes.size(); i++) {
        nodeIdToIndex[nodes[i]["id"].get<int>()] = i;
    }
    
    // 创建父子关系映射
    std::map<int, std::vector<std::pair<int, std::string>>> childrenMap;
    for (const auto& edge : edges) {
        int source = edge["source"].get<int>();
        int target = edge["target"].get<int>();
        
        // 安全获取position属性
        std::string position = "unknown";
        if (edge.contains("position") && edge["position"].is_string()) {
            position = edge["position"].get<std::string>();
        } else {
            // 如果没有position属性，则通过节点位置判断左右关系
            size_t sourceIdx = nodeIdToIndex[source];
            size_t targetIdx = nodeIdToIndex[target];
            
            if (nodes[targetIdx]["x"].get<int>() < nodes[sourceIdx]["x"].get<int>()) {
                position = "left";
            } else {
                position = "right";
            }
        }
        
        childrenMap[source].push_back({target, position});
    }
    
    // 计算每个节点的层级
    std::map<int, int> levels;
    std::function<void(int, int)> calculateLevels = [&](int nodeId, int level) {
        if (nodeIdToIndex.find(nodeId) == nodeIdToIndex.end()) return;
        
        levels[nodeId] = level;
        
        for (const auto& [childId, _] : childrenMap[nodeId]) {
            calculateLevels(childId, level + 1);
        }
    };
    
    calculateLevels(rootId, 0);
    
    // 计算最大层级
    int maxLevel = 0;
    for (const auto& [_, level] : levels) {
        maxLevel = std::max(maxLevel, level);
    }
    
    // 计算树的每个节点的x坐标
    struct NodeInfo {
        int x;
        int y;
        int width;
    };
    
    std::map<int, NodeInfo> nodeLayout;
    
    // 自底向上计算节点位置
    std::function<NodeInfo(int, int)> calculateNodePosition = [&](int nodeId, int level) -> NodeInfo {
        int leftWidth = 0;
        int rightWidth = 0;
        int nodeX = 0;
        int nodeY = baseY + level * levelHeight;
        
        // 获取左右子节点
        std::vector<int> leftChildren, rightChildren;
        for (const auto& [childId, pos] : childrenMap[nodeId]) {
            if (pos == "left") {
                leftChildren.push_back(childId);
            } else if (pos == "right") {
                rightChildren.push_back(childId);
            }
        }
        
        // 计算左子树位置
        for (int leftChildId : leftChildren) {
            NodeInfo leftInfo = calculateNodePosition(leftChildId, level + 1);
            leftWidth += leftInfo.width;
        }
        
        // 计算右子树位置
        for (int rightChildId : rightChildren) {
            NodeInfo rightInfo = calculateNodePosition(rightChildId, level + 1);
            rightWidth += rightInfo.width;
        }
        
        // 如果是叶子节点
        if (leftChildren.empty() && rightChildren.empty()) {
            nodeLayout[nodeId] = {baseX, nodeY, 100};  // 基础宽度100
            return {baseX, nodeY, 100};
        }
        
        // 计算当前节点的x坐标
        if (!leftChildren.empty() && !rightChildren.empty()) {
            // 如果有左右子树，放在中间
            int leftChildId = leftChildren.front();
            int rightChildId = rightChildren.front();
            nodeX = (nodeLayout[leftChildId].x + nodeLayout[rightChildId].x) / 2;
        } else if (!leftChildren.empty()) {
            // 只有左子树，向右偏移
            int leftChildId = leftChildren.front();
            nodeX = nodeLayout[leftChildId].x + 100;
        } else if (!rightChildren.empty()) {
            // 只有右子树，向左偏移
            int rightChildId = rightChildren.front();
            nodeX = nodeLayout[rightChildId].x - 100;
        }
        
        // 总宽度 = 左子树宽度 + 右子树宽度 + 当前节点宽度
        int totalWidth = leftWidth + rightWidth;
        if (totalWidth == 0) totalWidth = 100;  // 最小宽度
        
        nodeLayout[nodeId] = {nodeX, nodeY, totalWidth};
        return {nodeX, nodeY, totalWidth};
    };
    
    // 修正：先自顶向下分配每层位置
    std::function<void(int, int, int, int)> assignNodePositions = [&](int nodeId, int level, int minX, int maxX) {
        if (nodeIdToIndex.find(nodeId) == nodeIdToIndex.end()) return;
        
        int nodeY = baseY + level * levelHeight;
        int nodeX = (minX + maxX) / 2;
        
        // 更新节点坐标
        size_t idx = nodeIdToIndex[nodeId];
        nodes[idx]["x"] = nodeX;
        nodes[idx]["y"] = nodeY;
        
        // 获取左右子节点
        std::vector<int> leftChildren, rightChildren;
        for (const auto& [childId, pos] : childrenMap[nodeId]) {
            if (pos == "left") {
                leftChildren.push_back(childId);
            } else if (pos == "right") {
                rightChildren.push_back(childId);
            }
        }
        
        // 递归处理左子树
        if (!leftChildren.empty()) {
            assignNodePositions(leftChildren[0], level + 1, minX, nodeX);
        }
        
        // 递归处理右子树
        if (!rightChildren.empty()) {
            assignNodePositions(rightChildren[0], level + 1, nodeX, maxX);
        }
    };
    
    // 设置布局范围，确保足够的空间
    int layoutWidth = 800 * (maxLevel + 1);
    assignNodePositions(rootId, 0, baseX - layoutWidth/2, baseX + layoutWidth/2);
} 

// 搜索游戏操作 - 重置状态
crow::response handleSearchReset(const crow::request& req, Database& db) {
    try {
        // 创建一个初始的搜索状态
        json initialState = {
            {"array", json::array({10, 20, 30, 40, 50, 60, 70, 80, 90, 100})},
            {"target", nullptr},
            {"result", nullptr},
            {"algorithm", "binary"}
        };
        
        // 更新数据库中的状态
        db.updateGameState("search", initialState);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(initialState, "search").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("重置搜索状态失败: ") + e.what(), "search").dump());
    }
}

// 搜索游戏操作 - 获取当前状态
crow::response handleSearchState(const crow::request& req, Database& db) {
    try {
        json state = db.getGameState("search");
        
        // 如果状态为空，初始化一个搜索状态
        if (state.empty()) {
            state = {
                {"array", json::array({10, 20, 30, 40, 50, 60, 70, 80, 90, 100})},
                {"target", nullptr},
                {"result", nullptr},
                {"algorithm", "binary"}
            };
            db.updateGameState("search", state);
        }
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "search").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("服务器错误: ") + e.what(), "search").dump());
    }
}

// 搜索游戏操作 - 线性搜索
crow::response handleSearchLinear(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 解析JSON请求体
        json requestBody = json::parse(req.body);
        
        if (!requestBody.contains("array") || !requestBody["array"].is_array()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "请求必须包含一个数组字段", "search").dump());
        }
        
        if (!requestBody.contains("target") || !requestBody["target"].is_number()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "请求必须包含一个目标值字段", "search").dump());
        }
        
        std::vector<int> array = requestBody["array"].get<std::vector<int>>();
        int target = requestBody["target"].get<int>();
        
        // 执行线性搜索
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("search", "linear");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 初始帧
            json initialFrame = {
                {"array", array},
                {"target", target},
                {"currentIndex", -1},
                {"found", false},
                {"completed", false}
            };
            session.addFrame(initialFrame, "开始线性搜索，目标值: " + std::to_string(target));
            
            // 执行搜索步骤
            bool found = false;
            int foundIndex = -1;
            
            for (int i = 0; i < array.size(); i++) {
                // 当前检查的元素
                json stepFrame = {
                    {"array", array},
                    {"target", target},
                    {"currentIndex", i},
                    {"found", false},
                    {"completed", false}
                };
                session.addFrame(stepFrame, "检查索引 " + std::to_string(i) + " 处的值 " + std::to_string(array[i]));
                
                // 如果找到目标
                if (array[i] == target) {
                    found = true;
                    foundIndex = i;
                    
                    json foundFrame = {
                        {"array", array},
                        {"target", target},
                        {"currentIndex", i},
                        {"found", true},
                        {"foundIndex", i},
                        {"completed", false}
                    };
                    session.addFrame(foundFrame, "在索引 " + std::to_string(i) + " 处找到目标值 " + std::to_string(target));
                    break;
                }
            }
            
            // 最终帧
            json finalFrame = {
                {"array", array},
                {"target", target},
                {"currentIndex", found ? foundIndex : array.size()},
                {"found", found},
                {"foundIndex", foundIndex},
                {"completed", true}
            };
            
            if (found) {
                session.addFrame(finalFrame, "搜索完成，在索引 " + std::to_string(foundIndex) + " 处找到目标值");
            } else {
                session.addFrame(finalFrame, "搜索完成，未找到目标值");
            }
            
            // 更新游戏状态
            json newState = {
                {"array", array},
                {"target", target},
                {"algorithm", "linear"},
                {"found", found},
                {"foundIndex", foundIndex}
            };
            db.updateGameState("search", newState);
            session.updateGameState(newState);
            
            // 返回响应
            json frames = animManager->getAllFrames(sessionId);
            return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "search").dump());
        } else {
            // 不使用动画时的直接搜索
            bool found = false;
            int foundIndex = -1;
            
            for (int i = 0; i < array.size(); i++) {
                if (array[i] == target) {
                    found = true;
                    foundIndex = i;
                    break;
                }
            }
            
            // 更新游戏状态
            json newState = {
                {"array", array},
                {"target", target},
                {"algorithm", "linear"},
                {"found", found},
                {"foundIndex", foundIndex}
            };
            db.updateGameState("search", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "search").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("线性搜索错误: ") + e.what(), "search").dump());
    }
}

// 搜索游戏操作 - 二分搜索
crow::response handleSearchBinary(const crow::request& req, Database& db) {
    AnimationParams params = extractAnimationParams(req);
    
    try {
        // 解析JSON请求体
        json requestBody = json::parse(req.body);
        
        if (!requestBody.contains("array") || !requestBody["array"].is_array()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "请求必须包含一个数组字段", "search").dump());
        }
        
        if (!requestBody.contains("target") || !requestBody["target"].is_number()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "请求必须包含一个目标值字段", "search").dump());
        }
        
        std::vector<int> array = requestBody["array"].get<std::vector<int>>();
        int target = requestBody["target"].get<int>();
        
        // 排序数组，二分搜索要求
        std::sort(array.begin(), array.end());
        
        // 执行二分搜索
        if (params.animate) {
            AnimationManager* animManager = AnimationManager::getInstance();
            std::string sessionId = animManager->createSession("search", "binary");
            AnimationSession& session = animManager->getSession(sessionId);
            
            // 初始帧
            json initialFrame = {
                {"array", array},
                {"target", target},
                {"left", 0},
                {"right", static_cast<int>(array.size() - 1)},
                {"mid", -1},
                {"found", false},
                {"completed", false}
            };
            session.addFrame(initialFrame, "开始二分搜索，目标值: " + std::to_string(target));
            
            // 执行搜索步骤
            int left = 0;
            int right = array.size() - 1;
            bool found = false;
            int foundIndex = -1;
            
            while (left <= right) {
                int mid = left + (right - left) / 2;
                
                // 当前检查的元素
                json stepFrame = {
                    {"array", array},
                    {"target", target},
                    {"left", left},
                    {"right", right},
                    {"mid", mid},
                    {"found", false},
                    {"completed", false}
                };
                session.addFrame(stepFrame, "检查中间索引 " + std::to_string(mid) + " 处的值 " + std::to_string(array[mid]));
                
                // 如果找到目标
                if (array[mid] == target) {
                    found = true;
                    foundIndex = mid;
                    
                    json foundFrame = {
                        {"array", array},
                        {"target", target},
                        {"left", left},
                        {"right", right},
                        {"mid", mid},
                        {"found", true},
                        {"foundIndex", mid},
                        {"completed", false}
                    };
                    session.addFrame(foundFrame, "在索引 " + std::to_string(mid) + " 处找到目标值 " + std::to_string(target));
                    break;
                }
                
                // 调整搜索范围
                if (array[mid] < target) {
                    left = mid + 1;
                    json rangeFrame = {
                        {"array", array},
                        {"target", target},
                        {"left", left},
                        {"right", right},
                        {"mid", mid},
                        {"direction", "right"},
                        {"found", false},
                        {"completed", false}
                    };
                    session.addFrame(rangeFrame, "目标值大于中间值，搜索右半部分");
                } else {
                    right = mid - 1;
                    json rangeFrame = {
                        {"array", array},
                        {"target", target},
                        {"left", left},
                        {"right", right},
                        {"mid", mid},
                        {"direction", "left"},
                        {"found", false},
                        {"completed", false}
                    };
                    session.addFrame(rangeFrame, "目标值小于中间值，搜索左半部分");
                }
            }
            
            // 最终帧
            json finalFrame = {
                {"array", array},
                {"target", target},
                {"left", left},
                {"right", right},
                {"mid", found ? foundIndex : -1},
                {"found", found},
                {"foundIndex", foundIndex},
                {"completed", true}
            };
            
            if (found) {
                session.addFrame(finalFrame, "搜索完成，在索引 " + std::to_string(foundIndex) + " 处找到目标值");
            } else {
                session.addFrame(finalFrame, "搜索完成，未找到目标值");
            }
            
            // 更新游戏状态
            json newState = {
                {"array", array},
                {"target", target},
                {"algorithm", "binary"},
                {"found", found},
                {"foundIndex", foundIndex}
            };
            db.updateGameState("search", newState);
            session.updateGameState(newState);
            
            // 返回响应
            json frames = animManager->getAllFrames(sessionId);
            return crow::response(200, ResponseBuilder::createAnimationResponse(newState, frames, session.getFrameCount(), params.speed, "search").dump());
        } else {
            // 不使用动画时的直接搜索
            int left = 0;
            int right = array.size() - 1;
            bool found = false;
            int foundIndex = -1;
            
            while (left <= right) {
                int mid = left + (right - left) / 2;
                
                if (array[mid] == target) {
                    found = true;
                    foundIndex = mid;
                    break;
                }
                
                if (array[mid] < target) {
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }
            
            // 更新游戏状态
            json newState = {
                {"array", array},
                {"target", target},
                {"algorithm", "binary"},
                {"found", found},
                {"foundIndex", foundIndex}
            };
            db.updateGameState("search", newState);
            
            return crow::response(200, ResponseBuilder::createSuccessResponse(newState, "search").dump());
        }
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("二分搜索错误: ") + e.what(), "search").dump());
    }
}

// 获取排序状态
crow::response handleSortingState(const crow::request& req, Database& db) {
    try {
        json state = {
            {"array", {5, 3, 8, 4, 2}},  // 默认数组
            {"sorted", false},
            {"currentIndex", -1},
            {"comparingIndex", -1}
        };
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "sorting").dump());
    } catch (...) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, "获取排序状态时发生错误", "sorting").dump());
    }
}

// 重置排序状态
crow::response handleSortingReset(const crow::request& req, Database& db) {
    try {
        json state = {
            {"array", {5, 3, 8, 4, 2}},  // 重置为默认数组
            {"sorted", false},
            {"currentIndex", -1},
            {"comparingIndex", -1}
        };
        return crow::response(200, ResponseBuilder::createSuccessResponse(state, "sorting").dump());
    } catch (...) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, "重置排序状态时发生错误", "sorting").dump());
    }
}

// 冒泡排序
crow::response handleSortingBubbleSort(const crow::request& req, Database& db) {
    try {
        auto reqJson = json::parse(req.body);
        if (!reqJson.contains("array")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少array参数", "sorting").dump());
        }

        std::vector<int> array = reqJson["array"];
        std::vector<json> frames;
        
        // 生成排序过程的动画帧
        for (size_t i = 0; i < array.size() - 1; i++) {
            for (size_t j = 0; j < array.size() - i - 1; j++) {
                // 添加比较帧
                frames.push_back({
                    {"array", array},
                    {"currentIndex", j},
                    {"comparingIndex", j + 1},
                    {"sorted", false}
                });

                if (array[j] > array[j + 1]) {
                    std::swap(array[j], array[j + 1]);
                    // 添加交换帧
                    frames.push_back({
                        {"array", array},
                        {"currentIndex", j},
                        {"comparingIndex", j + 1},
                        {"sorted", false}
                    });
                }
            }
        }

        // 添加最终排序完成帧
        frames.push_back({
            {"array", array},
            {"currentIndex", -1},
            {"comparingIndex", -1},
            {"sorted", true}
        });

        json response = {
            {"state", {
                {"array", array},
                {"sorted", true},
                {"currentIndex", -1},
                {"comparingIndex", -1}
            }},
            {"animation", {
                {"frames", frames},
                {"totalSteps", frames.size()},
                {"speed", 5}
            }}
        };

        return crow::response(200, ResponseBuilder::createSuccessResponse(response, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("冒泡排序执行错误: ") + e.what(), "sorting").dump());
    }
}

// 选择排序
crow::response handleSortingSelectionSort(const crow::request& req, Database& db) {
    try {
        auto reqJson = json::parse(req.body);
        if (!reqJson.contains("array")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少array参数", "sorting").dump());
        }

        std::vector<int> array = reqJson["array"];
        std::vector<json> frames;
        
        for (size_t i = 0; i < array.size() - 1; i++) {
            size_t minIdx = i;
            frames.push_back({
                {"array", array},
                {"currentIndex", i},
                {"comparingIndex", minIdx},
                {"sorted", false}
            });

            for (size_t j = i + 1; j < array.size(); j++) {
                frames.push_back({
                    {"array", array},
                    {"currentIndex", i},
                    {"comparingIndex", j},
                    {"sorted", false}
                });

                if (array[j] < array[minIdx]) {
                    minIdx = j;
                }
            }

            if (minIdx != i) {
                std::swap(array[i], array[minIdx]);
                frames.push_back({
                    {"array", array},
                    {"currentIndex", i},
                    {"comparingIndex", minIdx},
                    {"sorted", false}
                });
            }
        }

        frames.push_back({
            {"array", array},
            {"currentIndex", -1},
            {"comparingIndex", -1},
            {"sorted", true}
        });

        json response = {
            {"state", {
                {"array", array},
                {"sorted", true},
                {"currentIndex", -1},
                {"comparingIndex", -1}
            }},
            {"animation", {
                {"frames", frames},
                {"totalSteps", frames.size()},
                {"speed", 5}
            }}
        };

        return crow::response(200, ResponseBuilder::createSuccessResponse(response, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("选择排序执行错误: ") + e.what(), "sorting").dump());
    }
}

// 插入排序
crow::response handleSortingInsertionSort(const crow::request& req, Database& db) {
    try {
        auto reqJson = json::parse(req.body);
        if (!reqJson.contains("array")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少array参数", "sorting").dump());
        }

        std::vector<int> array = reqJson["array"];
        std::vector<json> frames;
        
        for (size_t i = 1; i < array.size(); i++) {
            int key = array[i];
            int j = i - 1;
            
            frames.push_back({
                {"array", array},
                {"currentIndex", i},
                {"comparingIndex", j},
                {"sorted", false}
            });

            while (j >= 0 && array[j] > key) {
                array[j + 1] = array[j];
                frames.push_back({
                    {"array", array},
                    {"currentIndex", j + 1},
                    {"comparingIndex", j},
                    {"sorted", false}
                });
                j--;
            }
            array[j + 1] = key;
            frames.push_back({
                {"array", array},
                {"currentIndex", j + 1},
                {"comparingIndex", -1},
                {"sorted", false}
            });
        }

        frames.push_back({
            {"array", array},
            {"currentIndex", -1},
            {"comparingIndex", -1},
            {"sorted", true}
        });

        json response = {
            {"state", {
                {"array", array},
                {"sorted", true},
                {"currentIndex", -1},
                {"comparingIndex", -1}
            }},
            {"animation", {
                {"frames", frames},
                {"totalSteps", frames.size()},
                {"speed", 5}
            }}
        };

        return crow::response(200, ResponseBuilder::createSuccessResponse(response, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("插入排序执行错误: ") + e.what(), "sorting").dump());
    }
}

// 快速排序辅助函数
void quickSortHelper(std::vector<int>& array, int low, int high, std::vector<json>& frames) {
    if (low < high) {
        int pivot = array[high];
        int i = low - 1;
        
        frames.push_back({
            {"array", array},
            {"currentIndex", high},
            {"comparingIndex", -1},
            {"sorted", false},
            {"pivot", pivot}
        });

        for (int j = low; j < high; j++) {
            frames.push_back({
                {"array", array},
                {"currentIndex", j},
                {"comparingIndex", high},
                {"sorted", false},
                {"pivot", pivot}
            });

            if (array[j] <= pivot) {
                i++;
                std::swap(array[i], array[j]);
                frames.push_back({
                    {"array", array},
                    {"currentIndex", i},
                    {"comparingIndex", j},
                    {"sorted", false},
                    {"pivot", pivot}
                });
            }
        }
        
        std::swap(array[i + 1], array[high]);
        frames.push_back({
            {"array", array},
            {"currentIndex", i + 1},
            {"comparingIndex", high},
            {"sorted", false},
            {"pivot", pivot}
        });

        quickSortHelper(array, low, i, frames);
        quickSortHelper(array, i + 2, high, frames);
    }
}

// 快速排序
crow::response handleSortingQuickSort(const crow::request& req, Database& db) {
    try {
        auto reqJson = json::parse(req.body);
        if (!reqJson.contains("array")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(400, "缺少array参数", "sorting").dump());
        }

        std::vector<int> array = reqJson["array"];
        std::vector<json> frames;
        
        quickSortHelper(array, 0, array.size() - 1, frames);

        frames.push_back({
            {"array", array},
            {"currentIndex", -1},
            {"comparingIndex", -1},
            {"sorted", true}
        });

        json response = {
            {"state", {
                {"array", array},
                {"sorted", true},
                {"currentIndex", -1},
                {"comparingIndex", -1}
            }},
            {"animation", {
                {"frames", frames},
                {"totalSteps", frames.size()},
                {"speed", 5}
            }}
        };

        return crow::response(200, ResponseBuilder::createSuccessResponse(response, "sorting").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(500, std::string("快速排序执行错误: ") + e.what(), "sorting").dump());
    }
}

// 图游戏操作 - 查找路径
crow::response handleGraphFindPath(const crow::request& req, Database& db) {
    try {
        // 检查请求体是否为空
        if (req.body.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "请求体不能为空", "graph").dump());
        }
        
        json reqJson;
        try {
            reqJson = json::parse(req.body);
        } catch (const json::parse_error& e) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, std::string("JSON解析错误: ") + e.what(), "graph").dump());
        }
        
        // 检查必要参数
        if (!reqJson.contains("source") || !reqJson.contains("target")) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "缺少source或target参数", "graph").dump());
        }
        
        // 获取source和target参数，支持字符串或数字类型
        std::string source;
        std::string target;
        if (reqJson["source"].is_string()) {
            source = reqJson["source"].get<std::string>();
        } else if (reqJson["source"].is_number()) {
            source = std::to_string(reqJson["source"].get<int>());
        } else {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "source参数必须是字符串或数字", "graph").dump());
        }
        
        if (reqJson["target"].is_string()) {
            target = reqJson["target"].get<std::string>();
        } else if (reqJson["target"].is_number()) {
            target = std::to_string(reqJson["target"].get<int>());
        } else {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "target参数必须是字符串或数字", "graph").dump());
        }
        
        // 获取当前图状态
        json currentState = db.getGameState("graph");
        if (currentState.is_null()) {
            // 初始化一个空图状态
            currentState = json{
                {"nodes", json::array()},
                {"edges", json::array()},
                {"algorithm", nullptr}
            };
        }
        
        if (!currentState.contains("nodes") || !currentState.contains("edges")) {
            currentState["nodes"] = json::array();
            currentState["edges"] = json::array();
        }
        
        auto nodes = currentState["nodes"];
        auto edges = currentState["edges"];
        
        // 如果图是空的，返回适当的错误信息
        if (nodes.empty()) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "图中没有节点，请先添加节点", "graph").dump());
        }
        
        // 检查source和target节点是否存在
        bool sourceExists = false;
        bool targetExists = false;
        int sourceId = -1;
        int targetId = -1;

        // 尝试将source和target转换为整数
        try {
            sourceId = std::stoi(source);
            targetId = std::stoi(target);
        } catch (...) {
            // 如果转换失败，可能是使用了字母标签，尝试通过label查找
            for (const auto& node : nodes) {
                if (node["label"] == source) {
                    sourceId = node["id"].get<int>();
                    sourceExists = true;
                }
                if (node["label"] == target) {
                    targetId = node["id"].get<int>();
                    targetExists = true;
                }
            }
        }

        // 如果没有通过label找到，再尝试直接用ID查找
        if (!sourceExists || !targetExists) {
            for (const auto& node : nodes) {
                if (node["id"] == sourceId) {
                    sourceExists = true;
                }
                if (node["id"] == targetId) {
                    targetExists = true;
                }
            }
        }
        
        if (!sourceExists || !targetExists) {
            return crow::response(400, ResponseBuilder::createErrorResponse(
                400, "起点或终点节点不存在", "graph").dump());
        }

        // 将source和target转换为字符串形式的ID
        source = std::to_string(sourceId);
        target = std::to_string(targetId);
        
        // 构建邻接表
        std::map<std::string, std::vector<std::string>> adjacencyList;
        for (const auto& node : nodes) {
            adjacencyList[node["id"].get<std::string>()] = std::vector<std::string>();
        }
        
        for (const auto& edge : edges) {
            std::string from = edge["source"];
            std::string to = edge["target"];
            adjacencyList[from].push_back(to);
            // 如果是无向图，添加反向边
            if (!edge.contains("directed") || !edge["directed"].get<bool>()) {
                adjacencyList[to].push_back(from);
            }
        }
        
        // 使用BFS查找路径
        std::map<std::string, std::string> parent;
        std::queue<std::string> queue;
        std::set<std::string> visited;
        
        queue.push(source);
        visited.insert(source);
        
        bool found = false;
        while (!queue.empty() && !found) {
            std::string current = queue.front();
            queue.pop();
            
            if (current == target) {
                found = true;
                break;
            }
            
            for (const auto& neighbor : adjacencyList[current]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    parent[neighbor] = current;
                    queue.push(neighbor);
                }
            }
        }
        
        // 构建路径
        std::vector<std::string> path;
        if (found) {
            std::string current = target;
            while (current != source) {
                path.push_back(current);
                current = parent[current];
            }
            path.push_back(source);
            std::reverse(path.begin(), path.end());
        }
        
        // 提取动画参数
        AnimationParams animParams = extractAnimationParams(req);
        
        // 生成动画帧
        std::vector<json> frames;
        
        // 初始帧
        frames.push_back(json{
            {"nodes", nodes},
            {"edges", edges},
            {"visited", json::array()},
            {"current", nullptr},
            {"path", nullptr}
        });
        
        // 仅当需要动画时添加动画帧
        if (animParams.animate) {
            // 遍历过程帧
            for (const auto& vertex : visited) {
                if (vertex != source && vertex != target) {
                    frames.push_back(json{
                        {"nodes", nodes},
                        {"edges", edges},
                        {"visited", json(visited)},
                        {"current", vertex},
                        {"path", nullptr}
                    });
                }
            }
        }
        
        // 结果帧
        frames.push_back(json{
            {"nodes", nodes},
            {"edges", edges},
            {"visited", json(visited)},
            {"current", nullptr},
            {"path", path}
        });
        
        // 构建响应
        json response = json{
            {"state", json{
                {"nodes", nodes},
                {"edges", edges},
                {"algorithm", "findPath"},
                {"source", source},
                {"target", target},
                {"path", path},
                {"found", found}
            }}
        };
        
        // 如果需要动画，添加动画数据
        if (animParams.animate) {
            response["animation"] = json{
                {"frames", frames},
                {"totalSteps", frames.size()},
                {"speed", animParams.speed}
            };
        }
        
        // 更新状态
        db.updateGameState("graph", response["state"]);
        
        return crow::response(200, ResponseBuilder::createSuccessResponse(response, "graph").dump());
    } catch (const std::exception& e) {
        return crow::response(500, ResponseBuilder::createErrorResponse(
            500, std::string("路径查找失败: ") + e.what(), "graph").dump());
    }
}