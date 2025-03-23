#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class ResponseBuilder {
public:
    // 创建成功响应
    static json createSuccessResponse(const json& data, const std::string& module = "") {
        json response = {
            {"success", true},
            {"state", data}
        };
        
        if (!module.empty()) {
            response["module"] = module;
        }
        
        return response;
    }
    
    // 创建错误响应
    static json createErrorResponse(int code, const std::string& message, const std::string& module = "") {
        json response = {
            {"success", false},
            {"code", code},
            {"message", message}
        };
        
        if (!module.empty()) {
            response["module"] = module;
        }
        
        return response;
    }
    
    // 创建动画响应
    static json createAnimationResponse(const std::vector<json>& frames, const std::string& module = "") {
        json response = {
            {"success", true},
            {"frames", frames}
        };
        
        if (!module.empty()) {
            response["module"] = module;
        }
        
        return response;
    }
}; 

#endif // RESPONSE_BUILDER_H