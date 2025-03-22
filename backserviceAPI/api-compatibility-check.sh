#!/bin/bash

# API兼容性检查脚本
# 用于验证所有API端点是否正常工作
# 使用方法: ./api-compatibility-check.sh [host]
# 默认host为http://localhost:3000

# 设置颜色
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # 无颜色

# 检查是否提供了主机参数
if [ -z "$1" ]; then
  HOST="http://localhost:3000"
else
  HOST="$1"
fi

echo -e "${YELLOW}开始对 $HOST 进行API兼容性检查...${NC}\n"

# 计数器
TOTAL_TESTS=0
PASSED_TESTS=0

# 测试函数 - 参数: 方法, 路径, 描述, [请求体]
function test_endpoint() {
  METHOD=$1
  ENDPOINT=$2
  DESCRIPTION=$3
  BODY=$4
  
  TOTAL_TESTS=$((TOTAL_TESTS + 1))
  
  echo -ne "测试 ${METHOD} ${ENDPOINT}: ${DESCRIPTION}... "
  
  if [ "$METHOD" = "GET" ]; then
    RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" ${HOST}${ENDPOINT})
  else
    if [ -z "$BODY" ]; then
      RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X ${METHOD} ${HOST}${ENDPOINT})
    else
      RESPONSE=$(curl -s -o /dev/null -w "%{http_code}" -X ${METHOD} -H "Content-Type: application/json" -d "${BODY}" ${HOST}${ENDPOINT})
    fi
  fi
  
  if [ "$RESPONSE" = "200" ] || [ "$RESPONSE" = "201" ]; then
    echo -e "${GREEN}通过 ($RESPONSE)${NC}"
    PASSED_TESTS=$((PASSED_TESTS + 1))
  else
    echo -e "${RED}失败 ($RESPONSE)${NC}"
  fi
}

echo -e "${YELLOW}========== 基础API检查 ==========${NC}"
test_endpoint "GET" "/" "根路径"
test_endpoint "GET" "/health" "健康检查"

echo -e "\n${YELLOW}========== 数据结构API检查 ==========${NC}"

# 链表
echo -e "\n${YELLOW}--- 链表 API ---${NC}"
test_endpoint "GET" "/api/games/linkedlist/state" "获取链表状态"
test_endpoint "POST" "/api/games/linkedlist/reset" "重置链表"
test_endpoint "POST" "/api/games/linkedlist/insert" "插入节点" '{"value":42,"position":0}'
test_endpoint "POST" "/api/games/linkedlist/delete" "删除节点" '{"position":0}'
test_endpoint "POST" "/api/games/linkedlist/find" "查找节点" '{"value":42}'
test_endpoint "POST" "/api/games/linkedlist/reverse" "反转链表"

# 队列
echo -e "\n${YELLOW}--- 队列 API ---${NC}"
test_endpoint "GET" "/api/games/queue/state" "获取队列状态"
test_endpoint "POST" "/api/games/queue/reset" "重置队列"
test_endpoint "POST" "/api/games/queue/enqueue" "入队" '{"value":42}'
test_endpoint "POST" "/api/games/queue/dequeue" "出队"
test_endpoint "GET" "/api/games/queue/peek" "查看队首元素"

# 栈
echo -e "\n${YELLOW}--- 栈 API ---${NC}"
test_endpoint "GET" "/api/games/stack/state" "获取栈状态"
test_endpoint "POST" "/api/games/stack/reset" "重置栈"
test_endpoint "POST" "/api/games/stack/push" "入栈" '{"value":42}'
test_endpoint "POST" "/api/games/stack/pop" "出栈"
test_endpoint "GET" "/api/games/stack/peek" "查看栈顶元素"

# 二叉树
echo -e "\n${YELLOW}--- 二叉树 API ---${NC}"
test_endpoint "GET" "/api/games/binarytree/state" "获取二叉树状态"
test_endpoint "POST" "/api/games/binarytree/reset" "重置二叉树"
test_endpoint "POST" "/api/games/binarytree/insert" "插入节点" '{"value":50}'
test_endpoint "POST" "/api/games/binarytree/traversePreOrder" "前序遍历"
test_endpoint "POST" "/api/games/binarytree/traverseInOrder" "中序遍历"
test_endpoint "POST" "/api/games/binarytree/traversePostOrder" "后序遍历"
test_endpoint "POST" "/api/games/binarytree/traverseLevelOrder" "层序遍历"

# 图
echo -e "\n${YELLOW}--- 图 API ---${NC}"
test_endpoint "GET" "/api/games/graph/state" "获取图状态"
test_endpoint "POST" "/api/games/graph/reset" "重置图"
test_endpoint "POST" "/api/games/graph/traverseDFS" "DFS遍历" '{"startVertex":"A"}'
test_endpoint "POST" "/api/games/graph/traverseBFS" "BFS遍历" '{"startVertex":"A"}'
test_endpoint "POST" "/api/games/graph/findPath" "查找路径" '{"source":"A","target":"C"}'

echo -e "\n${YELLOW}========== 算法API检查 ==========${NC}"

# 排序
echo -e "\n${YELLOW}--- 排序算法 API ---${NC}"
test_endpoint "GET" "/api/games/sorting/state" "获取排序状态"
test_endpoint "POST" "/api/games/sorting/reset" "重置排序"
test_endpoint "POST" "/api/games/sorting/bubbleSort" "冒泡排序" '{"array":[5,3,8,4,2]}'
test_endpoint "POST" "/api/games/sorting/selectionSort" "选择排序" '{"array":[5,3,8,4,2]}'
test_endpoint "POST" "/api/games/sorting/insertionSort" "插入排序" '{"array":[5,3,8,4,2]}'
test_endpoint "POST" "/api/games/sorting/quickSort" "快速排序" '{"array":[5,3,8,4,2]}'

# 搜索
echo -e "\n${YELLOW}--- 搜索算法 API ---${NC}"
test_endpoint "GET" "/api/games/search/state" "获取搜索状态"
test_endpoint "POST" "/api/games/search/reset" "重置搜索"
test_endpoint "POST" "/api/games/search/linear" "线性搜索" '{"array":[5,3,8,4,2],"target":4}'
test_endpoint "POST" "/api/games/search/binary" "二分搜索" '{"array":[2,3,4,5,8],"target":4}'

# 迷宫
echo -e "\n${YELLOW}--- 迷宫算法 API ---${NC}"
test_endpoint "GET" "/api/games/maze/state" "获取迷宫状态"
test_endpoint "POST" "/api/games/maze/reset" "重置迷宫"
test_endpoint "GET" "/api/games/maze/generate" "生成迷宫"
test_endpoint "POST" "/api/games/maze/solveDFS" "DFS解迷宫"
test_endpoint "POST" "/api/games/maze/solveBFS" "BFS解迷宫"
test_endpoint "POST" "/api/games/maze/solveAStar" "A*解迷宫"

# 动态规划
echo -e "\n${YELLOW}--- 动态规划算法 API ---${NC}"
test_endpoint "GET" "/api/games/dp/state" "获取动态规划状态"
test_endpoint "POST" "/api/games/dp/reset" "重置动态规划"
test_endpoint "POST" "/api/games/dp/fibonacci" "斐波那契数列" '{"n":6}'
test_endpoint "POST" "/api/games/dp/knapsack" "背包问题" '{"weights":[2,3,4,5],"values":[3,4,5,6],"capacity":10}'
test_endpoint "POST" "/api/games/dp/lcs" "最长公共子序列" '{"str1":"ABCBDAB","str2":"BDCABA"}'

# 贪心算法
echo -e "\n${YELLOW}--- 贪心算法 API ---${NC}"
test_endpoint "GET" "/api/games/greedy/state" "获取贪心算法状态"
test_endpoint "POST" "/api/games/greedy/reset" "重置贪心算法"
test_endpoint "POST" "/api/games/greedy/coinChange" "零钱兑换" '{"amount":63,"coins":[25,10,5,1]}'
test_endpoint "POST" "/api/games/greedy/activitySelection" "活动安排" '{"start":[1,3,0,5,8,5],"finish":[2,4,6,7,9,9]}'
test_endpoint "POST" "/api/games/greedy/huffman" "哈夫曼编码" '{"chars":["a","b","c","d","e"],"freqs":[15,7,6,6,5]}'

# 结果统计
echo -e "\n${YELLOW}========== 测试结果 ==========${NC}"
PASS_RATE=$(echo "scale=2; $PASSED_TESTS * 100 / $TOTAL_TESTS" | bc)
echo -e "总测试: ${TOTAL_TESTS}"
echo -e "通过测试: ${PASSED_TESTS}"
echo -e "通过率: ${PASS_RATE}%"

if [ "$PASSED_TESTS" = "$TOTAL_TESTS" ]; then
  echo -e "\n${GREEN}所有API测试通过! ✅${NC}"
else
  echo -e "\n${RED}部分API测试失败，请检查服务器日志获取详细信息. ❌${NC}"
fi 