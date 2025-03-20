#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 动画状态枚举
enum class AnimationStatus {
    IDLE,       // 空闲状态
    RUNNING,    // 正在运行
    PAUSED,     // 已暂停
    COMPLETED   // 已完成
};

// 动画帧结构体
struct AnimationFrame {
    int frameIndex;                // 帧索引
    json data;                     // 帧数据（包含游戏元素的位置、状态等信息）
    std::string description;       // 帧描述（用于解释当前动画步骤）
    
    AnimationFrame(int index, const json& frameData, const std::string& desc)
        : frameIndex(index), data(frameData), description(desc) {}
};

// 动画会话类 - 管理单个动画实例
class AnimationSession {
private:
    std::string sessionId;                 // 会话ID
    std::string gameType;                  // 游戏类型
    std::string animationType;             // 动画类型
    AnimationStatus status;                // 当前状态
    std::vector<AnimationFrame> frames;    // 动画帧列表
    int currentFrameIndex;                 // 当前帧索引
    int speed;                             // 动画速度 (毫秒/帧)
    json gameState;                        // 游戏状态

public:
    AnimationSession(const std::string& id, const std::string& type, const std::string& animation);
    
    // 会话管理
    std::string getSessionId() const;
    std::string getGameType() const;
    std::string getAnimationType() const;
    
    // 状态管理
    AnimationStatus getStatus() const;
    void setStatus(AnimationStatus newStatus);
    bool isActive() const;
    
    // 帧管理
    void addFrame(const json& frameData, const std::string& description);
    const AnimationFrame& getCurrentFrame() const;
    const AnimationFrame& getFrame(int index) const;
    int getFrameCount() const;
    
    // 帧导航
    bool moveToNextFrame();
    bool moveToPrevFrame();
    bool moveToFrame(int index);
    int getCurrentFrameIndex() const;
    
    // 速度控制
    int getSpeed() const;
    void setSpeed(int newSpeed);
    
    // 游戏状态管理
    json getGameState() const;
    void updateGameState(const json& newState);
    
    // 序列化
    json toJson() const;
};

// 动画管理器类 - 全局管理所有动画会话
class AnimationManager {
private:
    std::map<std::string, AnimationSession> sessions;    // 会话映射表
    std::mutex sessionsMutex;                          // 会话互斥锁，保证线程安全
    static AnimationManager* instance;                 // 单例实例

    // 私有构造函数，确保单例模式
    AnimationManager() = default;

public:
    // 获取单例实例
    static AnimationManager* getInstance();
    
    // 会话管理
    std::string createSession(const std::string& gameType, const std::string& animationType);
    bool hasSession(const std::string& sessionId);
    AnimationSession& getSession(const std::string& sessionId);
    void removeSession(const std::string& sessionId);
    std::vector<std::string> getActiveSessions();
    
    // 动画控制
    bool startAnimation(const std::string& sessionId);
    bool pauseAnimation(const std::string& sessionId);
    bool resumeAnimation(const std::string& sessionId);
    bool stopAnimation(const std::string& sessionId);
    AnimationStatus getAnimationStatus(const std::string& sessionId);
    
    // 帧导航
    json getNextFrame(const std::string& sessionId);
    json getPrevFrame(const std::string& sessionId);
    json getAllFrames(const std::string& sessionId);
    json getCurrentFrame(const std::string& sessionId);
    
    // 速度控制
    bool setAnimationSpeed(const std::string& sessionId, int speed);
    int getAnimationSpeed(const std::string& sessionId);
    
    // 清理
    void cleanup();
};

#endif // ANIMATION_MANAGER_H 