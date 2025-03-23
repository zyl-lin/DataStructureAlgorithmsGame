cmake_minimum_required(VERSION 3.10)
project(DataStructureAlgorithmsGame)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加Crow依赖
include(FetchContent)
# 设置CROW_BUILD_EXAMPLES和CROW_BUILD_TESTS为OFF
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.1
)
FetchContent_MakeAvailable(Crow)

# 添加nlohmann/json依赖
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.10.5
)
FetchContent_MakeAvailable(json)

# 查找Boost (Crow依赖)
find_package(Boost REQUIRED)

# 查找SQLite3
find_package(SQLite3 REQUIRED)

# 查找OpenSSL (用于JWT签名)
find_package(OpenSSL REQUIRED)

# 查找线程库
find_package(Threads REQUIRED)

# jwt-cpp是仅头文件的库，设置包含路径
set(JWT_CPP_INCLUDE_DIR "/usr/local/include")

# 查找Crow
#find_package(Crow REQUIRED)

# 添加源文件
set(SOURCES
    src/server/main.cpp
    src/server/database.cpp
    src/server/user_routes.cpp
    src/server/game_routes.cpp
    src/server/achievement_routes.cpp
    src/server/game_animation_routes.cpp
    src/server/jwt_utils.cpp
    src/server/base64.cpp
    src/server/game_logic.cpp
    src/server/animation/routes/animation_routes.cpp
    src/server/animation/routes/dp_routes.cpp
    src/server/animation/routes/greedy_routes.cpp
    src/server/animation/routes/maze_routes.cpp
    src/server/animation/routes/graph_routes.cpp
    src/server/animation/implementations/dp/dp_animations.cpp
    src/server/animation/implementations/greedy/greedy_animations.cpp
    src/server/animation/implementations/maze/maze_animations.cpp
    src/server/animation/common/game_interaction_handler.cpp
    src/server/animation/common/animation_manager.cpp
    src/server/animation/common/animation_session.cpp
)

# 添加头文件目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server/animation
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server/animation/routes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server/animation/implementations
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server/animation/common
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 链接依赖库
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Crow::Crow
    nlohmann_json::nlohmann_json
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    Threads::Threads
)

# 添加包含目录
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server/animation
    ${crow_SOURCE_DIR}/include
    ${nlohmann_json_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${JWT_CPP_INCLUDE_DIR}
) 