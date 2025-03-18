#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class SimpleJsonParser {
public:
    // 解析JSON字符串
    std::map<std::string, std::string> parse(const std::string& json) {
        std::map<std::string, std::string> result;
        std::istringstream stream(json);
        char ch;
        std::string key, value;

        // 跳过空白字符
        stream >> ch;
        if (ch != '{') {
            throw std::runtime_error("Invalid JSON: Expected '{'");
        }

        while (stream >> ch) {
            if (ch == '}') {
                break; // 结束解析
            }

            // 解析键
            if (ch == '"') {
                key = parseString(stream);
            } else {
                throw std::runtime_error("Invalid JSON: Expected '\"' for key");
            }

            // 跳过冒号
            stream >> ch;
            if (ch != ':') {
                throw std::runtime_error("Invalid JSON: Expected ':'");
            }

            // 解析值
            stream >> ch;
            if (ch == '"') {
                value = parseString(stream);
            } else {
                throw std::runtime_error("Invalid JSON: Expected '\"' for value");
            }

            // 存储键值对
            result[key] = value;

            // 跳过逗号或结束符
            stream >> ch;
            if (ch == '}') {
                break;
            } else if (ch != ',') {
                throw std::runtime_error("Invalid JSON: Expected ',' or '}'");
            }
        }

        return result;
    }

    // 生成JSON字符串
    std::string generate(const std::map<std::string, std::string>& data) {
        std::ostringstream stream;
        stream << "{";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) {
                stream << ",";
            }
            stream << "\"" << pair.first << "\":\"" << pair.second << "\"";
            first = false;
        }
        stream << "}";
        return stream.str();
    }

private:
    // 解析字符串
    std::string parseString(std::istringstream& stream) {
        std::string str;
        char ch;
        while (stream.get(ch)) {
            if (ch == '"') {
                break; // 字符串结束
            }
            str += ch;
        }
        return str;
    }
};

class SimpleHttpServer {
public:
    SimpleHttpServer(int port) : port(port) {}

    void start() {
        // 创建socket
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) {
            std::cerr << "Error: Failed to create socket" << std::endl;
            return;
        }

        // 绑定地址和端口
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Error: Failed to bind socket" << std::endl;
            close(server_fd);
            return;
        }

        // 监听连接
        if (listen(server_fd, 5) < 0) {
            std::cerr << "Error: Failed to listen on socket" << std::endl;
            close(server_fd);
            return;
        }

        std::cout << "Server started at http://localhost:" << port << std::endl;

        // 处理请求
        while (true) {
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
            if (client_fd < 0) {
                std::cerr << "Error: Failed to accept connection" << std::endl;
                continue;
            }

            handleRequest(client_fd);
            close(client_fd);
        }
    }

private:
    int port;
    int server_fd;

    void handleRequest(int client_fd) {
        char buffer[1024] = {0};
        read(client_fd, buffer, sizeof(buffer));

        std::string request(buffer);
        std::string method = parseMethod(request);
        std::string path = parsePath(request);

        std::string response;
        if (method == "GET" && path == "/api/hello") {
            response = handleGet();
        } else if (method == "POST" && path == "/api/greet") {
            std::string body = parseBody(request);
            response = handlePost(body);
        } else {
            response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        }

        send(client_fd, response.c_str(), response.size(), 0);
    }

    std::string parseMethod(const std::string& request) {
        size_t space = request.find(' ');
        if (space == std::string::npos) {
            return "";
        }
        return request.substr(0, space);
    }

    std::string parsePath(const std::string& request) {
        size_t start = request.find(' ');
        if (start == std::string::npos) {
            return "";
        }
        size_t end = request.find(' ', start + 1);
        if (end == std::string::npos) {
            return "";
        }
        return request.substr(start + 1, end - start - 1);
    }

    std::string parseBody(const std::string& request) {
        size_t body_start = request.find("\r\n\r\n");
        if (body_start == std::string::npos) {
            return "";
        }
        return request.substr(body_start + 4);
    }

    std::string handleGet() {
        std::string json = "{\"message\": \"Hello from C++ server!\", \"status\": \"success\"}";
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
               std::to_string(json.size()) + "\r\n\r\n" + json;
    }

    std::string handlePost(const std::string& body) {
        SimpleJsonParser parser;
        std::map<std::string, std::string> data;
        try {
            data = parser.parse(body);
            std::string name = data["name"];
            std::string json = "{\"message\": \"Hello, " + name + "!\", \"status\": \"success\"}";
            return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " +
                   std::to_string(json.size()) + "\r\n\r\n" + json;
        } catch (const std::exception& e) {
            std::string json = "{\"message\": \"Invalid JSON\", \"status\": \"error\"}";
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\nContent-Length: " +
                   std::to_string(json.size()) + "\r\n\r\n" + json;
        }
    }
};

int main() {
    SimpleHttpServer server(8080);
    server.start();
    return 0;
} 