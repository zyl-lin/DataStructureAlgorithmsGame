#include "animation_manager.h"
#include <chrono>
#include <random>
#include <stdexcept>

using namespace std::chrono;

// 初始化静态成员变量
AnimationManager* AnimationManager::instance = nullptr;

// AnimationSession 实现
AnimationSession::AnimationSession(const std::string& id, const std::string& type, const std::string& animation)
    : sessionId(id), gameType(type), animationType(animation), 
      status(AnimationStatus::IDLE), currentFrameIndex(-1), speed(500) {
    // 默认动画速度为500毫秒每帧
}

std::string AnimationSession::getSessionId() const {
    return sessionId;
}

std::string AnimationSession::getGameType() const {
    return gameType;
}

std::string AnimationSession::getAnimationType() const {
    return animationType;
}

AnimationStatus AnimationSession::getStatus() const {
    return status;
}

void AnimationSession::setStatus(AnimationStatus newStatus) {
    status = newStatus;
}

bool AnimationSession::isActive() const {
    return status == AnimationStatus::RUNNING || status == AnimationStatus::PAUSED;
}

void AnimationSession::addFrame(const json& frameData, const std::string& description) {
    int index = frames.size();
    frames.emplace_back(index, frameData, description);
    
    // 如果是第一帧，设置当前帧索引
    if (currentFrameIndex == -1) {
        currentFrameIndex = 0;
    }
}

const AnimationFrame& AnimationSession::getCurrentFrame() const {
    if (currentFrameIndex < 0 || currentFrameIndex >= frames.size()) {
        throw std::out_of_range("当前帧索引无效");
    }
    return frames[currentFrameIndex];
}

const AnimationFrame& AnimationSession::getFrame(int index) const {
    if (index < 0 || index >= frames.size()) {
        throw std::out_of_range("帧索引超出范围");
    }
    return frames[index];
}

int AnimationSession::getFrameCount() const {
    return frames.size();
}

bool AnimationSession::moveToNextFrame() {
    if (currentFrameIndex + 1 < frames.size()) {
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
    if (index >= 0 && index < frames.size()) {
        currentFrameIndex = index;
        return true;
    }
    return false;
}

int AnimationSession::getCurrentFrameIndex() const {
    return currentFrameIndex;
}

int AnimationSession::getSpeed() const {
    return speed;
}

void AnimationSession::setSpeed(int newSpeed) {
    if (newSpeed > 0) {
        speed = newSpeed;
    }
}

json AnimationSession::getGameState() const {
    return gameState;
}

void AnimationSession::updateGameState(const json& newState) {
    gameState = newState;
}

json AnimationSession::toJson() const {
    json result;
    result["sessionId"] = sessionId;
    result["gameType"] = gameType;
    result["animationType"] = animationType;
    
    // 转换动画状态为字符串
    std::string statusStr;
    switch (status) {
        case AnimationStatus::IDLE: statusStr = "idle"; break;
        case AnimationStatus::RUNNING: statusStr = "running"; break;
        case AnimationStatus::PAUSED: statusStr = "paused"; break;
        case AnimationStatus::COMPLETED: statusStr = "completed"; break;
    }
    result["status"] = statusStr;
    
    result["currentFrameIndex"] = currentFrameIndex;
    result["totalFrames"] = frames.size();
    result["speed"] = speed;
    
    // 只包含当前帧信息，避免过大的响应
    if (currentFrameIndex >= 0 && currentFrameIndex < frames.size()) {
        result["currentFrame"] = {
            {"index", frames[currentFrameIndex].frameIndex},
            {"data", frames[currentFrameIndex].data},
            {"description", frames[currentFrameIndex].description}
        };
    }
    
    return result;
}

// AnimationManager 实现
AnimationManager* AnimationManager::getInstance() {
    if (instance == nullptr) {
        instance = new AnimationManager();
    }
    return instance;
}

// 生成随机会话ID
std::string generateSessionId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static const char* digits = "0123456789abcdef";
    
    std::string id;
    for (int i = 0; i < 16; ++i) {
        id += digits[dis(gen)];
        if (i == 3 || i == 7 || i == 11) {
            id += '-';
        }
    }
    return id;
}

std::string AnimationManager::createSession(const std::string& gameType, const std::string& animationType) {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    
    // 生成唯一会话ID
    std::string sessionId = generateSessionId();
    while (sessions.find(sessionId) != sessions.end()) {
        sessionId = generateSessionId();
    }
    
    // 创建并存储会话
    sessions.emplace(sessionId, AnimationSession(sessionId, gameType, animationType));
    return sessionId;
}

bool AnimationManager::hasSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    return sessions.find(sessionId) != sessions.end();
}

AnimationSession& AnimationManager::getSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    auto it = sessions.find(sessionId);
    if (it == sessions.end()) {
        throw std::runtime_error("未找到会话: " + sessionId);
    }
    return it->second;
}

void AnimationManager::removeSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    sessions.erase(sessionId);
}

std::vector<std::string> AnimationManager::getActiveSessions() {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    std::vector<std::string> activeSessions;
    
    for (const auto& pair : sessions) {
        if (pair.second.isActive()) {
            activeSessions.push_back(pair.first);
        }
    }
    
    return activeSessions;
}

bool AnimationManager::startAnimation(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        session.setStatus(AnimationStatus::RUNNING);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool AnimationManager::pauseAnimation(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        if (session.getStatus() == AnimationStatus::RUNNING) {
            session.setStatus(AnimationStatus::PAUSED);
            return true;
        }
        return false;
    } catch (const std::exception&) {
        return false;
    }
}

bool AnimationManager::resumeAnimation(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        if (session.getStatus() == AnimationStatus::PAUSED) {
            session.setStatus(AnimationStatus::RUNNING);
            return true;
        }
        return false;
    } catch (const std::exception&) {
        return false;
    }
}

bool AnimationManager::stopAnimation(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        session.setStatus(AnimationStatus::COMPLETED);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

AnimationStatus AnimationManager::getAnimationStatus(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        return session.getStatus();
    } catch (const std::exception&) {
        return AnimationStatus::IDLE;
    }
}

json AnimationManager::getNextFrame(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        if (session.moveToNextFrame()) {
            return session.getCurrentFrame().data;
        }
        return json::object();
    } catch (const std::exception&) {
        return json::object();
    }
}

json AnimationManager::getPrevFrame(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        if (session.moveToPrevFrame()) {
            return session.getCurrentFrame().data;
        }
        return json::object();
    } catch (const std::exception&) {
        return json::object();
    }
}

json AnimationManager::getAllFrames(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        json frames = json::array();
        
        for (int i = 0; i < session.getFrameCount(); ++i) {
            const auto& frame = session.getFrame(i);
            frames.push_back({
                {"index", frame.frameIndex},
                {"data", frame.data},
                {"description", frame.description}
            });
        }
        
        return frames;
    } catch (const std::exception&) {
        return json::array();
    }
}

json AnimationManager::getCurrentFrame(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        const auto& frame = session.getCurrentFrame();
        return {
            {"index", frame.frameIndex},
            {"data", frame.data},
            {"description", frame.description}
        };
    } catch (const std::exception&) {
        return json::object();
    }
}

bool AnimationManager::setAnimationSpeed(const std::string& sessionId, int speed) {
    try {
        auto& session = getSession(sessionId);
        session.setSpeed(speed);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

int AnimationManager::getAnimationSpeed(const std::string& sessionId) {
    try {
        auto& session = getSession(sessionId);
        return session.getSpeed();
    } catch (const std::exception&) {
        return 0;
    }
}

void AnimationManager::cleanup() {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    
    // 删除所有已完成或闲置超过1小时的会话
    auto now = system_clock::now();
    auto it = sessions.begin();
    
    while (it != sessions.end()) {
        if (it->second.getStatus() == AnimationStatus::COMPLETED) {
            it = sessions.erase(it);
        } else {
            ++it;
        }
    }
} 