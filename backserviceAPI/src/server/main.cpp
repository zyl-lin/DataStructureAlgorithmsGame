#include <crow.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>

using json = nlohmann::json;

// 数据库连接类
class Database {
private:
    sqlite3* db;
    static Database* instance;

    Database() {
        int rc = sqlite3_open("game.db", &db);
        if (rc) {
            throw std::runtime_error("无法打开数据库");
        }
        initTables();
    }

    void initTables() {
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                username TEXT UNIQUE NOT NULL,
                password TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );

            CREATE TABLE IF NOT EXISTS game_progress (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER,
                game_id INTEGER,
                level INTEGER,
                score INTEGER,
                completed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users(id)
            );

            CREATE TABLE IF NOT EXISTS achievements (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER,
                achievement_id INTEGER,
                unlocked_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (user_id) REFERENCES users(id)
            );
        )";

        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string error = errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error("数据库初始化失败: " + error);
        }
    }

public:
    static Database* getInstance() {
        if (instance == nullptr) {
            instance = new Database();
        }
        return instance;
    }

    sqlite3* getConnection() { return db; }
};

Database* Database::instance = nullptr;

// 游戏逻辑类
class GameLogic {
public:
    static json generateLinkedListLevel(int level) {
        json result;
        // 根据关卡生成链表题目
        return result;
    }

    static json generateMazeLevel(int level) {
        json result;
        // 根据关卡生成迷宫题目
        return result;
    }

    static json generateSortingLevel(int level) {
        json result;
        // 根据关卡生成排序题目
        return result;
    }
};

int main() {
    crow::SimpleApp app;
    Database* db = Database::getInstance();

    // 用户注册
    CROW_ROUTE(app, "/api/user/register")
        .methods("POST"_method)
        ([](const crow::request& req) {
            auto x = json::parse(req.body);
            // 处理用户注册逻辑
            return crow::response(json{{"status", "success"}}.dump());
        });

    // 用户登录
    CROW_ROUTE(app, "/api/user/login")
        .methods("POST"_method)
        ([](const crow::request& req) {
            auto x = json::parse(req.body);
            // 处理用户登录逻辑
            return crow::response(json{{"status", "success"}}.dump());
        });

    // 获取游戏关卡
    CROW_ROUTE(app, "/api/games/<int>/levels/<int>")
        .methods("GET"_method)
        ([](int gameId, int levelId) {
            json result;
            switch (gameId) {
                case 1: // 链表操作
                    result = GameLogic::generateLinkedListLevel(levelId);
                    break;
                case 3: // 迷宫寻路
                    result = GameLogic::generateMazeLevel(levelId);
                    break;
                case 4: // 排序可视化
                    result = GameLogic::generateSortingLevel(levelId);
                    break;
                default:
                    return crow::response(404);
            }
            return crow::response(result.dump());
        });

    // 提交游戏答案
    CROW_ROUTE(app, "/api/games/<int>/levels/<int>/submit")
        .methods("POST"_method)
        ([](const crow::request& req, int gameId, int levelId) {
            auto x = json::parse(req.body);
            // 处理答案提交逻辑
            return crow::response(json{{"status", "success"}}.dump());
        });

    app.port(3000).multithreaded().run();
    return 0;
} 