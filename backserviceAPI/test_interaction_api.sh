#!/bin/bash

# 定义API基础URL
API_URL="http://localhost:3000"

# 定义颜色变量
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

# 测试API函数
test_api() {
    local endpoint=$1
    local method=$2
    local data=$3
    local description=$4
    
    echo -e "=========================="
    echo -e "${CYAN}测试: $description${NC}"
    echo -e "${GRAY}请求: $method $endpoint${NC}"
    
    if [ "$data" != "" ]; then
        echo -e "${GRAY}数据: $data${NC}"
    fi
    
    # 发送请求
    if [ "$method" = "GET" ]; then
        response=$(curl -s "$API_URL$endpoint")
    else
        response=$(curl -s -X $method -H "Content-Type: application/json" -d "$data" "$API_URL$endpoint")
    fi
    
    # 检查是否包含success字段
    if echo "$response" | grep -q '"success":true'; then
        echo -e "${GREEN}测试通过!${NC}"
        echo -e "${GRAY}响应: $response${NC}"
    else
        echo -e "${RED}测试失败!${NC}"
        echo -e "${GRAY}响应: $response${NC}"
    fi
    
    # 返回响应以便其他函数使用
    echo "$response"
}

# 测试链表游戏会话创建
echo -e "${YELLOW}准备测试链表游戏会话创建接口...${NC}"
session_data='{"gameType": "linkedlist", "animationType": "step"}'
session_response=$(test_api "/api/animation/create-session" "POST" "$session_data" "创建链表游戏会话")

# 提取会话ID
if echo "$session_response" | grep -q '"success":true'; then
    session_id=$(echo "$session_response" | grep -o '"sessionId":"[^"]*"' | cut -d'"' -f4)
    echo -e "${GREEN}会话创建成功，使用会话ID: $session_id${NC}"
else
    # 使用固定会话ID进行测试
    session_id="test-session-123"
    echo -e "${YELLOW}使用测试会话ID: $session_id${NC}"
fi

# 测试动态规划游戏会话创建
echo -e "${YELLOW}准备测试动态规划游戏会话创建接口...${NC}"
dp_session_data='{"gameType": "dp", "animationType": "step"}'
test_api "/api/animation/create-session" "POST" "$dp_session_data" "创建动态规划游戏会话"

# 测试贪心算法游戏会话创建
echo -e "${YELLOW}准备测试贪心算法游戏会话创建接口...${NC}"
greedy_session_data='{"gameType": "greedy", "animationType": "step"}'
test_api "/api/animation/create-session" "POST" "$greedy_session_data" "创建贪心算法游戏会话"

# 测试点击链表节点
click_data='{"sessionId": "'$session_id'", "elementId": "node-1", "eventType": "click"}'
test_api "/api/games/click" "POST" "$click_data" "点击链表节点"

# 测试拖拽链表节点
drag_data='{"sessionId": "'$session_id'", "elementId": "node-2", "eventType": "drag", "startPos": {"x": 200, "y": 150}, "endPos": {"x": 250, "y": 150}}'
test_api "/api/games/drag" "POST" "$drag_data" "拖拽链表节点"

# 测试放置链表节点
drop_data='{"sessionId": "'$session_id'", "elementId": "node-2", "eventType": "drop", "position": {"x": 250, "y": 150}}'
test_api "/api/games/drop" "POST" "$drop_data" "放置链表节点"

# 测试悬停链表节点
hover_data='{"sessionId": "'$session_id'", "elementId": "node-3", "eventType": "hover"}'
test_api "/api/games/hover" "POST" "$hover_data" "悬停在链表节点上"

# 测试通用交互接口
interact_data='{"sessionId": "'$session_id'", "elementId": "edge-1", "eventType": "click", "additionalData": {"highlight": true}}'
test_api "/api/games/interact" "POST" "$interact_data" "点击链表边缘"

# 测试暂停动画
pause_data='{"sessionId": "'$session_id'"}'
test_api "/api/animation/pause" "POST" "$pause_data" "暂停动画"

# 测试恢复动画
resume_data='{"sessionId": "'$session_id'"}'
test_api "/api/animation/resume" "POST" "$resume_data" "恢复动画"

# 测试获取下一帧
next_frame_data='{"sessionId": "'$session_id'"}'
test_api "/api/animation/nextFrame" "POST" "$next_frame_data" "获取下一帧"

# 测试获取动画状态
test_api "/api/animation/status?sessionId=$session_id" "GET" "" "获取动画状态"

echo -e "\n${GREEN}所有测试完成!${NC}" 