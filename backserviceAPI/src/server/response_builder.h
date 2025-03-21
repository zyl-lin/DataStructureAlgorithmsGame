#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <nlohmann/json.hpp>
#include <string>

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
};

#endif // RESPONSE_BUILDER_H 