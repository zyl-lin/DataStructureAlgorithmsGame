#!/bin/bash

# 数据结构与算法游戏API的reset接口测试脚本

# 设置基本URL
BASE_URL="http://localhost:3000"

# 定义颜色
GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

# 定义测试函数
test_reset_api() {
    local endpoint=$1
    local description=$2
    
    echo -e "${CYAN}测试 $description...${NC}"
    
    response=$(curl -s -w "\n%{http_code}" -X POST -H "Content-Type: application/json" ${BASE_URL}${endpoint})
    status_code=$(echo "$response" | tail -n1)
    content=$(echo "$response" | sed '$d')
    
    if [ "$status_code" -eq 200 ]; then
        echo -e "${GREEN}✓ 成功 (状态码: $status_code)${NC}"
        
        # 尝试解析JSON响应并显示重置后的状态
        echo -n "  重置后的状态: "
        echo -e "${GRAY}$(echo $content | jq -c '.data.state')${NC}"
    else
        echo -e "${RED}✗ 失败 (状态码: $status_code)${NC}"
    fi
    
    echo ""
}

# 显示测试开始
echo -e "${YELLOW}=====================================${NC}"
echo -e "${YELLOW}   数据结构与算法游戏reset接口测试   ${NC}"
echo -e "${YELLOW}=====================================${NC}"
echo ""

# 测试链表reset接口
test_reset_api "/api/games/linkedlist/reset" "链表重置接口"

# 测试队列reset接口
test_reset_api "/api/games/queue/reset" "队列重置接口"

# 测试栈reset接口
test_reset_api "/api/games/stack/reset" "栈重置接口"

# 测试二叉树reset接口
test_reset_api "/api/games/binarytree/reset" "二叉树重置接口"

# 测试图reset接口
test_reset_api "/api/games/graph/reset" "图重置接口"

# 测试排序reset接口
test_reset_api "/api/games/sorting/reset" "排序重置接口"

# 测试搜索reset接口
test_reset_api "/api/games/search/reset" "搜索重置接口"

# 测试迷宫reset接口
test_reset_api "/api/games/maze/reset" "迷宫重置接口"

# 测试动态规划reset接口
test_reset_api "/api/games/dp/reset" "动态规划重置接口"

# 测试贪心算法reset接口
test_reset_api "/api/games/greedy/reset" "贪心算法重置接口"

# 显示测试结束
echo -e "${YELLOW}=====================================${NC}"
echo -e "${YELLOW}             测试完成              ${NC}"
echo -e "${YELLOW}=====================================${NC}" 