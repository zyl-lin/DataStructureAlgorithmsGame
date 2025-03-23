#include "animation_session.h"
#include <stdexcept>

// 帧操作方法
void AnimationSession::addFrame(const json& frameData, const std::string& description) {
    int nextIndex = frames.size();
    frames.emplace_back(nextIndex, frameData, description);
}

const AnimationFrame& AnimationSession::getCurrentFrame() const {
    if (currentFrameIndex < 0 || currentFrameIndex >= static_cast<int>(frames.size())) {
        throw std::out_of_range("Current frame index is out of range");
    }
    return frames[currentFrameIndex];
}

json AnimationSession::getFrame(size_t index) const {
    if (index >= frames.size()) {
        return json();
    }
    return frames[index].data;
}

bool AnimationSession::moveToNextFrame() {
    if (currentFrameIndex + 1 < static_cast<int>(frames.size())) {
        currentFrameIndex++;
        return true;
    }
    return false;
}

bool AnimationSession::moveToPrevFrame() {
    if (currentFrameIndex > 0) {
        currentFrameIndex--;
        return true;
    }
    return false;
}

bool AnimationSession::moveToFrame(int index) {
    if (index >= 0 && index < static_cast<int>(frames.size())) {
        currentFrameIndex = index;
        return true;
    }
    return false;
}

int AnimationSession::getCurrentFrameIndex() const {
    return currentFrameIndex;
}

// 速度控制
void AnimationSession::setSpeed(double newSpeed) {
    if (newSpeed <= 0) {
        throw std::invalid_argument("Speed must be greater than 0");
    }
    speed = newSpeed;
}

double AnimationSession::getSpeed() const {
    return speed;
}

// 状态管理
void AnimationSession::setStatus(AnimationStatus newStatus) {
    status = newStatus;
}

AnimationStatus AnimationSession::getStatus() const {
    return status;
}

bool AnimationSession::isActive() const {
    return status == AnimationStatus::RUNNING || status == AnimationStatus::PAUSED;
}

// 游戏状态管理
void AnimationSession::updateGameState(const json& newState) {
    gameState = newState;
}

// 序列化
json AnimationSession::toJson() const {
    json result;
    result["sessionId"] = sessionId;
    result["gameType"] = gameType;
    result["animationType"] = animationType;
    result["currentFrameIndex"] = currentFrameIndex;
    result["speed"] = speed;
    result["status"] = static_cast<int>(status);
    result["frameCount"] = frames.size();
    result["gameState"] = gameState;
    return result;
} 