#include "jwt_utils.h"
#include <ctime>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <base64.h>

using json = nlohmann::json;

// JWT密钥 - 在实际应用中应该存储在安全的环境变量中
const std::string JWT_SECRET = "your_jwt_secret_key_change_this_in_production";

// Base64 URL 编码
std::string base64UrlEncode(const std::string& input) {
    std::string base64 = base64_encode(reinterpret_cast<const unsigned char*>(input.c_str()), input.length());
    
    // 将标准Base64转换为URL安全的Base64
    std::replace(base64.begin(), base64.end(), '+', '-');
    std::replace(base64.begin(), base64.end(), '/', '_');
    
    // 移除填充字符
    while (!base64.empty() && base64.back() == '=') {
        base64.pop_back();
    }
    
    return base64;
}

// HMAC-SHA256签名
std::string hmacSha256(const std::string& data, const std::string& key) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    
    HMAC(EVP_sha256(), key.c_str(), key.length(),
         reinterpret_cast<const unsigned char*>(data.c_str()), data.length(),
         hash, &hashLen);
    
    std::string result(reinterpret_cast<char*>(hash), hashLen);
    return result;
}

// 生成JWT令牌
std::string generateJWT(int userId, const std::string& username) {
    // 头部
    json header = {
        {"alg", "HS256"},
        {"typ", "JWT"}
    };
    
    // 当前时间和过期时间（1小时后）
    time_t now = time(nullptr);
    time_t exp = now + 3600; // 1小时后过期
    
    // 载荷
    json payload = {
        {"user_id", userId},
        {"username", username},
        {"iat", now},      // 签发时间
        {"exp", exp}       // 过期时间
    };
    
    // 编码头部和载荷
    std::string encodedHeader = base64UrlEncode(header.dump());
    std::string encodedPayload = base64UrlEncode(payload.dump());
    
    // 创建签名
    std::string data = encodedHeader + "." + encodedPayload;
    std::string signature = hmacSha256(data, JWT_SECRET);
    std::string encodedSignature = base64UrlEncode(signature);
    
    // 组合JWT
    return data + "." + encodedSignature;
}

// Base64 URL 解码
std::string base64UrlDecode(const std::string& input) {
    std::string base64 = input;
    
    // 将URL安全的Base64转换回标准Base64
    std::replace(base64.begin(), base64.end(), '-', '+');
    std::replace(base64.begin(), base64.end(), '_', '/');
    
    // 添加填充字符
    switch (base64.length() % 4) {
        case 2: base64 += "=="; break;
        case 3: base64 += "="; break;
        default: break;
    }
    
    return base64_decode(base64);
}

// 验证JWT令牌
json verifyJWT(const std::string& token) {
    // 解析JWT部分
    std::vector<std::string> parts;
    std::istringstream stream(token);
    std::string part;
    
    while (std::getline(stream, part, '.')) {
        parts.push_back(part);
    }
    
    if (parts.size() != 3) {
        return json(); // 无效的令牌格式
    }
    
    // 验证签名
    std::string data = parts[0] + "." + parts[1];
    std::string expectedSignature = hmacSha256(data, JWT_SECRET);
    std::string actualSignature = base64UrlDecode(parts[2]);
    
    if (expectedSignature != actualSignature) {
        return json(); // 签名不匹配
    }
    
    // 解码载荷
    std::string decodedPayload = base64UrlDecode(parts[1]);
    json payload = json::parse(decodedPayload);
    
    // 验证过期时间
    time_t now = time(nullptr);
    if (payload.contains("exp") && now > payload["exp"]) {
        return json(); // 令牌已过期
    }
    
    return payload;
} 