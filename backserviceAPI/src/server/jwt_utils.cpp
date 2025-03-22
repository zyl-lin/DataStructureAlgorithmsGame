#include "jwt_utils.h"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include <ctime>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <vector>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;

// JWT密钥，实际应用中应该从配置文件或环境变量中读取
const std::string JWT_SECRET = "your-secret-key";

// 生成JWT令牌
std::string generateJWT(int userId, const std::string& username) {
    auto token = jwt::create()
        .set_issuer("auth0")
        .set_type("JWS")
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(24))
        .set_payload_claim("user_id", jwt::claim(std::to_string(userId)))
        .set_payload_claim("username", jwt::claim(username))
        .sign(jwt::algorithm::hs256{JWT_SECRET});
    
    return token;
}

// 验证JWT令牌
nlohmann::json verifyJWT(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{JWT_SECRET})
            .with_issuer("auth0");
        
        verifier.verify(decoded);
        
        // 创建包含有效载荷的JSON对象
        nlohmann::json payload;
        payload["user_id"] = decoded.get_payload_claim("user_id").as_string();
        payload["username"] = decoded.get_payload_claim("username").as_string();
        payload["valid"] = true;
        
        return payload;
    } catch (const std::exception& e) {
        // 返回错误JSON对象
        nlohmann::json error;
        error["valid"] = false;
        error["error"] = e.what();
        return error;
    }
} 