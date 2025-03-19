#include "game_logic.h"
#include <random>
#include <algorithm>

namespace GameLogic {

// 链表节点结构
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// 生成链表关卡
json generateLinkedListLevel(int level) {
    json result;
    std::vector<int> values;
    int size = 3 + level; // 随关卡增加链表长度

    // 生成随机值
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    
    for (int i = 0; i < size; i++) {
        values.push_back(dis(gen));
    }

    // 构建链表
    ListNode* head = new ListNode(values[0]);
    ListNode* current = head;
    for (int i = 1; i < size; i++) {
        current->next = new ListNode(values[i]);
        current = current->next;
    }

    // 根据关卡生成不同的任务
    switch (level) {
        case 1:
            result["task"] = "在链表末尾插入一个新节点";
            result["target"] = dis(gen);
            break;
        case 2:
            result["task"] = "删除指定值的节点";
            result["target"] = values[std::uniform_int_distribution<>(0, size-1)(gen)];
            break;
        case 3:
            result["task"] = "反转整个链表";
            break;
        default:
            result["task"] = "在指定位置插入新节点";
            result["position"] = std::uniform_int_distribution<>(1, size)(gen);
            result["target"] = dis(gen);
    }

    // 将链表转换为JSON格式
    json nodes;
    current = head;
    while (current) {
        json node;
        node["value"] = current->val;
        nodes.push_back(node);
        current = current->next;
    }
    result["nodes"] = nodes;

    // 清理内存
    current = head;
    while (current) {
        ListNode* temp = current;
        current = current->next;
        delete temp;
    }

    return result;
}

// 生成迷宫关卡
json generateMazeLevel(int level) {
    json result;
    int size = 5 + level; // 随关卡增加迷宫大小
    std::vector<std::vector<int>> maze(size, std::vector<int>(size, 1));

    // 使用深度优先搜索生成迷宫
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, size-1);

    // 设置起点和终点
    maze[0][0] = 0;
    maze[size-1][size-1] = 0;

    // 生成迷宫路径
    for (int i = 1; i < size-1; i++) {
        for (int j = 1; j < size-1; j++) {
            if (dis(gen) % 3 == 0) { // 30%的概率生成墙
                maze[i][j] = 1;
            } else {
                maze[i][j] = 0;
            }
        }
    }

    // 确保迷宫可解
    maze[0][1] = 0;
    maze[1][0] = 0;
    maze[size-1][size-2] = 0;
    maze[size-2][size-1] = 0;

    result["maze"] = maze;
    result["start"] = {0, 0};
    result["end"] = {size-1, size-1};
    result["size"] = size;

    return result;
}

// 生成排序关卡
json generateSortingLevel(int level) {
    json result;
    int size = 5 + level; // 随关卡增加数组大小
    std::vector<int> array(size);

    // 生成随机数组
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    
    for (int i = 0; i < size; i++) {
        array[i] = dis(gen);
    }

    // 根据关卡选择排序算法
    switch (level) {
        case 1:
            result["algorithm"] = "bubble";
            result["task"] = "使用冒泡排序对数组进行升序排序";
            break;
        case 2:
            result["algorithm"] = "quick";
            result["task"] = "使用快速排序对数组进行升序排序";
            break;
        case 3:
            result["algorithm"] = "merge";
            result["task"] = "使用归并排序对数组进行升序排序";
            break;
        default:
            result["algorithm"] = "custom";
            result["task"] = "实现自定义排序算法";
    }

    result["array"] = array;
    result["size"] = size;

    return result;
}

} // namespace GameLogic 