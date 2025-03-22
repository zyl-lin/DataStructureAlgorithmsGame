#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <nlohmann/json.hpp>
#include <string>
#include <crow.h>

using json = nlohmann::json;

class ResponseBuilder {
public:
    static json createSuccessResponse(const json& data, const std::string& module = "") {
        json response;
        response["success"] = true;
        response["data"] = {
            {"state", data}
        };
        if (!module.empty()) {
            response["module"] = module;
        }
        return response;
    }

    static json createErrorResponse(int code, const std::string& message, const std::string& module = "") {
        json response;
        response["success"] = false;
        response["error"] = {
            {"code", code},
            {"message", message}
        };
        if (!module.empty()) {
            response["module"] = module;
        }
        return response;
    }

    static json createAnimationResponse(const json& state, const json& frames, int totalSteps, int speed, const std::string& module = "") {
        json response;
        response["success"] = true;
        response["data"] = {
            {"state", state},
            {"animation", {
                {"frames", frames},
                {"totalSteps", totalSteps},
                {"speed", speed}
            }}
        };
        if (!module.empty()) {
            response["module"] = module;
        }
        return response;
    }
    
    // 构建成功响应
    static crow::response buildSuccessResponse(const std::string& message, const json& data) {
        json response;
        response["success"] = true;
        response["message"] = message;
        
        if (!data.is_null()) {
            response["data"] = data;
        }
        
        return crow::response(200, response.dump());
    }
    
    // 构建错误响应
    static crow::response buildErrorResponse(const std::string& message, int statusCode = 400) {
        json response;
        response["success"] = false;
        response["message"] = message;
        
        return crow::response(statusCode, response.dump());
    }
    
    // 构建JSON响应
    static crow::response buildJsonResponse(const json& data, int statusCode = 200) {
        return crow::response(statusCode, data.dump());
    }
};

#endif // RESPONSE_BUILDER_H 