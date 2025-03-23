#ifndef ANIMATION_SESSION_H
#define ANIMATION_SESSION_H

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 动画帧结构体
struct AnimationFrame {
    int frameIndex;
    json data;
    std::string description;

    AnimationFrame() = default;
    AnimationFrame(int index, const json& frameData, const std::string& desc)
        : frameIndex(index), data(frameData), description(desc) {}
};

// 动画状态枚举
enum class AnimationStatus {
    IDLE,
    RUNNING,
    PAUSED,
    COMPLETED
};

// 动画会话类
class AnimationSession {
private:
    std::string sessionId;
    std::string gameType;
    std::string animationType;
    std::vector<AnimationFrame> frames;
    int currentFrameIndex = -1;
    double speed = 1.0;
    AnimationStatus status = AnimationStatus::IDLE;
    json gameState;
    std::map<std::string, json> savedStates;

public:
    // 构造函数
    AnimationSession() = default;
    explicit AnimationSession(const std::string& type) : gameType(type) {}
    AnimationSession(const std::string& id, const std::string& type, const std::string& animation)
        : sessionId(id), gameType(type), animationType(animation) {}

    // 帧操作方法
    void addFrame(const json& frameData, const std::string& description);
    const AnimationFrame& getCurrentFrame() const;
    json getFrame(size_t index) const;
    bool moveToNextFrame();
    bool moveToPrevFrame();
    bool moveToFrame(int index);
    int getCurrentFrameIndex() const;
    size_t getFrameCount() const { return frames.size(); }

    // 速度控制
    void setSpeed(double newSpeed);
    double getSpeed() const;

    // 状态管理
    void setStatus(AnimationStatus newStatus);
    AnimationStatus getStatus() const;
    bool isActive() const;

    // 游戏状态管理
    const std::string& getGameType() const { return gameType; }
    const std::string& getSessionId() const { return sessionId; }
    const std::string& getAnimationType() const { return animationType; }
    json getGameState() const { return gameState; }
    void updateGameState(const json& newState);

    // 状态保存和加载
    void saveState(const std::string& key, const json& state) { savedStates[key] = state; }
    json loadState(const std::string& key) const { 
        auto it = savedStates.find(key);
        return it != savedStates.end() ? it->second : json(); 
    }

    // 序列化
    json toJson() const;
};

#endif // ANIMATION_SESSION_H