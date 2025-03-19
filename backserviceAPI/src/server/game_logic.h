#pragma once

#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

namespace GameLogic {

// 生成链表关卡
json generateLinkedListLevel(int level);

// 生成迷宫关卡
json generateMazeLevel(int level);

// 生成排序关卡
json generateSortingLevel(int level);

} // namespace GameLogic 