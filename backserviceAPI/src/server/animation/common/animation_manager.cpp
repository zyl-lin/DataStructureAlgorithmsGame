#include "animation_manager.h"
#include "animation_session.h"
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <mutex>

// 初始化静态成员
AnimationManager* AnimationManager::instance = nullptr;

// 获取单例实例
AnimationManager* AnimationManager::getInstance() {
    if (!instance) {
        instance = new AnimationManager();
    }
    return instance;
}

// 生成唯一的会话ID
std::string AnimationManager::generateSessionId() const {
    // 获取当前时间戳
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    );
    
    // 生成随机数
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 999999);
    int random = dis(gen);
    
    // 组合时间戳和随机数
    std::stringstream ss;
    ss << "session_" << now_ms.count() << "_" << std::setfill('0') << std::setw(6) << random;
    return ss.str();
}

// 创建新会话
std::string AnimationManager::createSession(const std::string& sessionId, const std::string& gameType) {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    
    // 如果会话ID已存在，返回错误
    if (sessions.find(sessionId) != sessions.end()) {
        throw std::runtime_error("Session already exists: " + sessionId);
    }

    // 创建新会话
    sessions.emplace(sessionId, AnimationSession(sessionId, gameType, ""));
    return sessionId;
}

// 获取会话
AnimationSession& AnimationManager::getSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    auto it = sessions.find(sessionId);
    if (it == sessions.end()) {
        throw std::runtime_error("Session not found: " + sessionId);
    }
    return it->second;
}

// 检查会话是否存在
bool AnimationManager::hasSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    return sessions.find(sessionId) != sessions.end();
}

// 删除会话
void AnimationManager::removeSession(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    sessions.erase(sessionId);
}

// 清空所有会话
void AnimationManager::clearSessions() {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    sessions.clear();
}

// 获取所有活动会话
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

// 动画控制方法
bool AnimationManager::startAnimation(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    session.setStatus(AnimationStatus::RUNNING);
    return true;
}

bool AnimationManager::pauseAnimation(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    session.setStatus(AnimationStatus::PAUSED);
    return true;
}

bool AnimationManager::resumeAnimation(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    session.setStatus(AnimationStatus::RUNNING);
    return true;
}

bool AnimationManager::stopAnimation(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    session.setStatus(AnimationStatus::COMPLETED);
    return true;
}

AnimationStatus AnimationManager::getAnimationStatus(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    return session.getStatus();
}

// 帧操作方法
json AnimationManager::getNextFrame(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    if (session.moveToNextFrame()) {
        return session.getCurrentFrame().data;
    }
    return json();
}

json AnimationManager::getPrevFrame(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    if (session.moveToPrevFrame()) {
        return session.getCurrentFrame().data;
    }
    return json();
}

json AnimationManager::getAllFrames(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    json frames = json::array();
    for (size_t i = 0; i < session.getFrameCount(); ++i) {
        frames.push_back(session.getFrame(i));
    }
    return frames;
}

json AnimationManager::getCurrentFrame(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    return session.getCurrentFrame().data;
}

// 速度控制方法
bool AnimationManager::setAnimationSpeed(const std::string& sessionId, int speed) {
    auto& session = getSession(sessionId);
    session.setSpeed(static_cast<double>(speed));
    return true;
}

int AnimationManager::getAnimationSpeed(const std::string& sessionId) {
    auto& session = getSession(sessionId);
    return static_cast<int>(session.getSpeed());
}

// 清理方法
void AnimationManager::cleanup() {
    std::lock_guard<std::mutex> lock(sessionsMutex);
    sessions.clear();
}

// AnimationSession方法实现在animation_session.cpp文件中
// 这里只保留与AnimationManager直接相关的方法 