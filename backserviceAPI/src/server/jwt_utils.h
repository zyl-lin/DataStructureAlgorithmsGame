#pragma once

#include <string>
#include "nlohmann/json.hpp"

// 生成JWT令牌
std::string generateJWT(int userId, const std::string& username);

// 验证JWT令牌并返回有效载荷
nlohmann::json verifyJWT(const std::string& token); 