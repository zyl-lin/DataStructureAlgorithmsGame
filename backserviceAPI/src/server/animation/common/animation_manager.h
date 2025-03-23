#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include "animation_session.h"
#include <string>
#include <map>
#include <mutex>
#include <vector>

// 动画管理器类，管理所有游戏会话
class AnimationManager {
private:
    static AnimationManager* instance;  // 改为指针类型
    std::map<std::string, AnimationSession> sessions;
    std::mutex sessionsMutex;  // 添加互斥锁保护会话访问

public:
    // 获取单例实例
    static AnimationManager* getInstance();
    
    // 会话管理方法
    bool hasSession(const std::string& sessionId);
    AnimationSession& getSession(const std::string& sessionId);
    std::string createSession(const std::string& sessionId, const std::string& gameType);
    void removeSession(const std::string& sessionId);
    void clearSessions();
    std::string generateSessionId() const;
    std::vector<std::string> getActiveSessions();

    // 动画控制方法
    bool startAnimation(const std::string& sessionId);
    bool pauseAnimation(const std::string& sessionId);
    bool resumeAnimation(const std::string& sessionId);
    bool stopAnimation(const std::string& sessionId);
    AnimationStatus getAnimationStatus(const std::string& sessionId);

    // 帧操作方法
    json getNextFrame(const std::string& sessionId);
    json getPrevFrame(const std::string& sessionId);
    json getAllFrames(const std::string& sessionId);
    json getCurrentFrame(const std::string& sessionId);

    // 速度控制方法
    bool setAnimationSpeed(const std::string& sessionId, int speed);
    int getAnimationSpeed(const std::string& sessionId);

    // 清理方法
    void cleanup();

private:
    AnimationManager() = default;  // 私有构造函数
    ~AnimationManager() = default;
    AnimationManager(const AnimationManager&) = delete;
    AnimationManager& operator=(const AnimationManager&) = delete;
};

#endif // ANIMATION_MANAGER_H