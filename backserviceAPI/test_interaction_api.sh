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

# 测试统计变量
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

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
    
    # 增加测试计数
    TOTAL_TESTS=$((TOTAL_TESTS+1))
    
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
        PASSED_TESTS=$((PASSED_TESTS+1))
    else
        echo -e "${RED}测试失败!${NC}"
        echo -e "${GRAY}响应: $response${NC}"
        FAILED_TESTS=$((FAILED_TESTS+1))
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
test_api "/api/animation/create-session" "POST" "$dp_session_data" "创建动态规划(斐波那契)游戏会话"

# 测试LCS动态规划游戏会话创建
echo -e "${YELLOW}准备测试LCS动态规划游戏会话创建接口...${NC}"
lcs_session_data='{"gameType": "dp", "dpType": "lcs", "animationType": "step"}'
lcs_session_response=$(test_api "/api/animation/create-session" "POST" "$lcs_session_data" "创建LCS动态规划游戏会话")

# 提取LCS会话ID
if echo "$lcs_session_response" | grep -q '"success":true'; then
    lcs_session_id=$(echo "$lcs_session_response" | grep -o '"sessionId":"[^"]*"' | cut -d'"' -f4)
    echo -e "${GREEN}LCS会话创建成功，使用会话ID: $lcs_session_id${NC}"
    
    # 测试LCS动画生成
    lcs_anim_data='{"sessionId": "'$lcs_session_id'"}'
    test_api "/api/dp/lcs" "POST" "$lcs_anim_data" "生成LCS动画"
    
    # 测试LCS单步执行
    lcs_step_data='{"sessionId": "'$lcs_session_id'", "step": 1}'
    test_api "/api/dp/step" "POST" "$lcs_step_data" "LCS单步执行"
    
    # 测试LCS路径显示
    lcs_path_data='{"sessionId": "'$lcs_session_id'"}'
    test_api "/api/dp/lcs/path" "POST" "$lcs_path_data" "显示LCS路径"
    
    # 测试LCS单元格计算
    lcs_cell_data='{"sessionId": "'$lcs_session_id'", "cellId": "cell-2-3"}'
    test_api "/api/dp/lcs/cell" "POST" "$lcs_cell_data" "计算LCS单元格"
    
    # 测试点击LCS单元格
    lcs_click_data='{"sessionId": "'$lcs_session_id'", "elementId": "cell-1-1", "eventType": "click"}'
    test_api "/api/games/click" "POST" "$lcs_click_data" "点击LCS单元格"
    
    # 测试悬停在LCS单元格上
    lcs_hover_data='{"sessionId": "'$lcs_session_id'", "elementId": "cell-1-1", "eventType": "hover"}'
    test_api "/api/games/hover" "POST" "$lcs_hover_data" "悬停在LCS单元格上"
    
    # 测试LCS多步执行，准备保存状态
    echo -e "${YELLOW}准备测试LCS多步执行...${NC}"
    for i in {1..5}; do
        lcs_step_data='{"sessionId": "'$lcs_session_id'", "step": 1}'
        test_api "/api/dp/step" "POST" "$lcs_step_data" "LCS第$i次步骤执行"
    done
    
    # 测试LCS状态保存
    echo -e "${YELLOW}准备测试LCS状态保存功能...${NC}"
    lcs_save_data='{"sessionId": "'$lcs_session_id'"}'
    save_response=$(test_api "/api/dp/lcs/save" "POST" "$lcs_save_data" "保存LCS计算状态")
    
    # 继续执行几步操作
    echo -e "${YELLOW}继续执行几步操作...${NC}"
    for i in {6..8}; do
        lcs_step_data='{"sessionId": "'$lcs_session_id'", "step": 1}'
        test_api "/api/dp/step" "POST" "$lcs_step_data" "LCS第$i次步骤执行"
    done
    
    # 测试LCS状态恢复
    echo -e "${YELLOW}准备测试LCS状态恢复功能...${NC}"
    lcs_restore_data='{"sessionId": "'$lcs_session_id'"}'
    test_api "/api/dp/lcs/restore" "POST" "$lcs_restore_data" "恢复LCS计算状态"
    
    # 测试恢复后的状态验证
    echo -e "${YELLOW}验证恢复后的状态...${NC}"
    test_api "/api/animation/status?sessionId=$lcs_session_id" "GET" "" "获取恢复后的动画状态"
    
    # 测试LCS动画速度控制
    echo -e "${YELLOW}测试LCS动画速度控制...${NC}"
    lcs_speed_data='{"sessionId": "'$lcs_session_id'", "speed": 2.0}'
    test_api "/api/animation/speedControl" "POST" "$lcs_speed_data" "设置LCS动画速度为2倍"

    # 测试LCS执行回放功能
    echo -e "${YELLOW}测试LCS执行回放功能...${NC}"
    lcs_playback_data='{"sessionId": "'$lcs_session_id'", "step": 2}'
    test_api "/api/dp/lcs/playback" "POST" "$lcs_playback_data" "LCS执行记录回放"

    # 测试状态缺失时的回放功能
    echo -e "${YELLOW}测试状态缺失时的回放功能...${NC}"
    if echo "$save_response" | grep -q '"success":true'; then
        new_playback_data='{"sessionId": "'$lcs_session_id'", "step": 0}'
        test_api "/api/dp/lcs/playback" "POST" "$new_playback_data" "状态缺失时的执行记录回放"
    fi

    # 测试步骤超出范围的回放功能
    echo -e "${YELLOW}测试步骤超出范围的回放功能...${NC}"
    out_of_range_data='{"sessionId": "'$lcs_session_id'", "step": 999}'
    test_api "/api/dp/lcs/playback" "POST" "$out_of_range_data" "步骤超出范围的执行记录回放"
fi

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

# ==============================================
# 添加LCS状态管理综合测试模块
# ==============================================
echo -e "\n${YELLOW}开始LCS状态管理综合测试...${NC}"

# 创建新的LCS会话用于综合测试
echo -e "${YELLOW}为综合测试创建新的LCS会话...${NC}"
lcs_test_data='{"gameType": "dp", "dpType": "lcs", "animationType": "step"}'
lcs_test_response=$(test_api "/api/animation/create-session" "POST" "$lcs_test_data" "创建LCS综合测试会话")

# 提取综合测试会话ID
if echo "$lcs_test_response" | grep -q '"success":true'; then
    lcs_test_id=$(echo "$lcs_test_response" | grep -o '"sessionId":"[^"]*"' | cut -d'"' -f4)
    echo -e "${GREEN}LCS综合测试会话创建成功，使用会话ID: $lcs_test_id${NC}"
    
    # 测试生成初始动画
    lcs_gen_data='{"sessionId": "'$lcs_test_id'"}'
    test_api "/api/dp/lcs" "POST" "$lcs_gen_data" "生成LCS综合测试动画"
    
    # 测试执行3个步骤后保存状态
    echo -e "${YELLOW}执行3个步骤...${NC}"
    for i in {1..3}; do
        lcs_step_data='{"sessionId": "'$lcs_test_id'", "step": 1}'
        test_api "/api/dp/step" "POST" "$lcs_step_data" "LCS综合测试步骤$i"
    done
    
    # 保存状态1
    echo -e "${YELLOW}保存第一个状态点...${NC}"
    lcs_save_data='{"sessionId": "'$lcs_test_id'"}'
    save_response1=$(test_api "/api/dp/lcs/save" "POST" "$lcs_save_data" "保存LCS第一个状态点")
    
    # 继续执行3个步骤
    echo -e "${YELLOW}继续执行3个步骤...${NC}"
    for i in {4..6}; do
        lcs_step_data='{"sessionId": "'$lcs_test_id'", "step": 1}'
        test_api "/api/dp/step" "POST" "$lcs_step_data" "LCS综合测试步骤$i"
    done
    
    # 保存状态2
    echo -e "${YELLOW}保存第二个状态点...${NC}"
    save_response2=$(test_api "/api/dp/lcs/save" "POST" "$lcs_save_data" "保存LCS第二个状态点")
    
    # 恢复到状态1
    echo -e "${YELLOW}恢复到第一个状态点...${NC}"
    test_api "/api/dp/lcs/restore" "POST" "$lcs_save_data" "恢复到第一个状态点"
    
    # 检查恢复的状态
    echo -e "${YELLOW}检查恢复后的状态...${NC}"
    test_api "/api/animation/status?sessionId=$lcs_test_id" "GET" "" "检查恢复后的状态"
    
    # 在恢复状态上继续执行
    echo -e "${YELLOW}在恢复状态上继续执行...${NC}"
    lcs_step_data='{"sessionId": "'$lcs_test_id'", "step": 1}'
    test_api "/api/dp/step" "POST" "$lcs_step_data" "从恢复点继续执行"
    
    # 测试单元格计算
    echo -e "${YELLOW}在恢复状态后测试单元格计算...${NC}"
    lcs_cell_data='{"sessionId": "'$lcs_test_id'", "cellId": "cell-3-3"}'
    test_api "/api/dp/lcs/cell" "POST" "$lcs_cell_data" "恢复后计算单元格"
    
    # 测试步骤回退
    echo -e "${YELLOW}测试步骤回退功能...${NC}"
    step_back_data='{"sessionId": "'$lcs_test_id'"}'
    test_api "/api/animation/stepBack" "POST" "$step_back_data" "步骤回退"

    # 测试步骤回退到第一步后继续回退
    echo -e "${YELLOW}测试回退到第一步后继续回退...${NC}"
    # 先回退到第一步
    for i in {1..10}; do
        test_api "/api/animation/stepBack" "POST" "$step_back_data" "回退第$i次"
    done
    # 尝试再次回退
    test_api "/api/animation/stepBack" "POST" "$step_back_data" "尝试超出范围回退"
    
    echo -e "${GREEN}LCS状态管理综合测试完成!${NC}"
else
    echo -e "${RED}LCS综合测试会话创建失败，跳过综合测试。${NC}"
fi

echo -e "\n${GREEN}全部测试完成!${NC}"

# ==============================================
# LCS状态管理错误处理测试
# ==============================================
echo -e "\n${YELLOW}开始LCS状态管理错误处理测试...${NC}"

# 测试无效会话ID
echo -e "${YELLOW}测试无效会话ID...${NC}"
invalid_session_data='{"sessionId": "invalid-session-id"}'
test_api "/api/dp/lcs/save" "POST" "$invalid_session_data" "使用无效会话ID保存状态"
test_api "/api/dp/lcs/restore" "POST" "$invalid_session_data" "使用无效会话ID恢复状态"

# 测试非LCS类型会话
if echo "$session_response" | grep -q '"success":true'; then
    echo -e "${YELLOW}测试非LCS类型会话...${NC}"
    non_lcs_data='{"sessionId": "'$session_id'"}'
    test_api "/api/dp/lcs/save" "POST" "$non_lcs_data" "对非LCS会话保存状态"
    test_api "/api/dp/lcs/restore" "POST" "$non_lcs_data" "对非LCS会话恢复状态"
fi

# 测试在未计算完成时恢复状态
if echo "$lcs_test_response" | grep -q '"success":true'; then
    echo -e "${YELLOW}测试未保存状态时恢复...${NC}"
    new_session_data='{"gameType": "dp", "dpType": "lcs", "animationType": "step"}'
    new_session_response=$(test_api "/api/animation/create-session" "POST" "$new_session_data" "创建新LCS会话")
    
    if echo "$new_session_response" | grep -q '"success":true'; then
        new_session_id=$(echo "$new_session_response" | grep -o '"sessionId":"[^"]*"' | cut -d'"' -f4)
        no_state_data='{"sessionId": "'$new_session_id'"}'
        test_api "/api/dp/lcs/restore" "POST" "$no_state_data" "未保存状态时恢复"
    fi
fi

# 测试参数缺失
echo -e "${YELLOW}测试参数缺失...${NC}"
missing_param_data='{}'
test_api "/api/dp/lcs/save" "POST" "$missing_param_data" "保存状态缺少sessionId参数"
test_api "/api/dp/lcs/restore" "POST" "$missing_param_data" "恢复状态缺少sessionId参数"

echo -e "${GREEN}LCS状态管理错误处理测试完成!${NC}"


# 测试搜索算法API
echo "测试搜索算法API..."

# 创建会话
SESSION_ID=$(curl -s -X POST "http://localhost:3000/api/animation/create-session" \
    -H "Content-Type: application/json" \
    -d '{"gameType":"search"}' | jq -r '.data.sessionId')

echo "创建会话成功，会话ID: $SESSION_ID"

# 测试线性搜索
echo "测试线性搜索..."
curl -s -X POST "http://localhost:3000/api/search/linear" \
    -H "Content-Type: application/json" \
    -d "{\"sessionId\":\"$SESSION_ID\"}"

# 测试二分搜索
echo "测试二分搜索..."
curl -s -X POST "http://localhost:3000/api/search/binary" \
    -H "Content-Type: application/json" \
    -d "{\"sessionId\":\"$SESSION_ID\"}"

# 测试单步执行
echo "测试单步执行..."
curl -s -X POST "http://localhost:3000/api/search/step" \
    -H "Content-Type: application/json" \
    -d "{\"sessionId\":\"$SESSION_ID\"}"

# 测试获取状态
echo "测试获取状态..."
curl -s -X GET "http://localhost:3000/api/search/state?sessionId=$SESSION_ID"

# 测试重置状态
echo "测试重置状态..."
curl -s -X POST "http://localhost:3000/api/search/reset" \
    -H "Content-Type: application/json" \
    -d "{\"sessionId\":\"$SESSION_ID\"}" 



# 测试迷宫算法API
echo "测试迷宫算法API..."

# 创建会话
SESSION_ID=$(curl -s -X POST "http://localhost:3000/api/animation/create-session" \
    -H "Content-Type: application/json" \
    -d '{"gameType":"maze"}' | jq -r '.data.sessionId')

echo "创建会话成功，会话ID: $SESSION_ID"

# 测试生成迷宫
echo "测试生成迷宫..."
curl -s -X GET "http://localhost:3000/api/maze/generate?sessionId=$SESSION_ID"

# 测试DFS求解
echo "测试DFS求解迷宫..."
curl -s -X POST "http://localhost:3000/api/maze/solveDFS" \
    -H "Content-Type: application/json" \
    -d "{\"sessionId\":\"$SESSION_ID\"}"

# 测试BFS求解
echo "测试BFS求解迷宫..."
curl -s -X POST "http://localhost:3000/api/maze/solveBFS" \
    -H "Content-Type: application/json" \
    -d "{\"sessionId\":\"$SESSION_ID\"}"

# 测试A*求解
echo "测试A*求解迷宫..."
curl -s -X POST "http://localhost:3000/api/maze/solveAStar" \
    -H "Content-Type: application/json" \
    -d "{\"sessionId\":\"$SESSION_ID\"}"

# 测试获取状态
echo "测试获取迷宫状态..."
curl -s -X GET "http://localhost:3000/api/maze/state?sessionId=$SESSION_ID"

# 测试重置状态
echo "测试重置迷宫状态..."
curl -s -X POST "http://localhost:3000/api/maze/reset" \
    -H "Content-Type: application/json" \
    -d "{\"sessionId\":\"$SESSION_ID\"}" 


# ==============================================
# 测试结果汇总
# ==============================================
echo -e "\n${YELLOW}测试结果汇总${NC}"
echo -e "=========================="
echo -e "总测试数: ${CYAN}$TOTAL_TESTS${NC}"
echo -e "通过测试: ${GREEN}$PASSED_TESTS${NC}"
echo -e "失败测试: ${RED}$FAILED_TESTS${NC}"
echo -e "测试通过率: ${CYAN}$(( (PASSED_TESTS * 100) / TOTAL_TESTS ))%${NC}"
echo -e "=========================="

# 根据测试结果设置退出码
if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "\n${GREEN}所有测试均已通过!${NC}" 
    exit 0
else
    echo -e "\n${RED}有 $FAILED_TESTS 个测试失败!${NC}" 
    exit 1
fi 