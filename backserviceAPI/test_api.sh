#!/bin/bash

# 设置服务器地址
SERVER="http://localhost:3000"
# SERVER="http://你的远程服务器IP:3000" # 如果是远程服务器，取消注释此行

# 定义颜色
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 测试健康状态
echo -e "${BLUE}测试健康状态...${NC}"
curl -s $SERVER/api/health
echo -e "\n"

# 用户注册
echo -e "${BLUE}测试用户注册...${NC}"
curl -s -X POST $SERVER/api/user/register \
  -H "Content-Type: application/json" \
  -d '{"username": "testuser", "password": "password123", "email": "test@example.com"}'
echo -e "\n"

# 用户登录并获取token
echo -e "${BLUE}测试用户登录...${NC}"
TOKEN=$(curl -s -X POST $SERVER/api/user/login \
  -H "Content-Type: application/json" \
  -d '{"username": "testuser", "password": "password123"}' | grep -o '"token":"[^"]*' | cut -d'"' -f4)

if [ -z "$TOKEN" ]
then
  echo -e "${RED}登录失败，无法获取token${NC}"
  exit 1
else
  echo -e "${GREEN}获取token成功: $TOKEN${NC}"
fi
echo -e "\n"

# 获取用户信息
echo -e "${BLUE}测试获取用户信息...${NC}"
curl -s -X GET $SERVER/api/user/profile \
  -H "Authorization: Bearer $TOKEN"
echo -e "\n"

# 更新用户信息
echo -e "${BLUE}测试更新用户信息...${NC}"
curl -s -X PUT $SERVER/api/user/profile \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"nickname": "新昵称", "avatar": "avatar.png"}'
echo -e "\n"

# 获取所有游戏
echo -e "${BLUE}测试获取所有游戏...${NC}"
curl -s -X GET $SERVER/api/games
echo -e "\n"

# 获取特定游戏详情
echo -e "${BLUE}测试获取特定游戏详情...${NC}"
curl -s -X GET $SERVER/api/games/1
echo -e "\n"

# 获取游戏关卡列表
echo -e "${BLUE}测试获取游戏关卡列表...${NC}"
curl -s -X GET $SERVER/api/games/1/levels
echo -e "\n"

# 获取特定关卡详情
echo -e "${BLUE}测试获取特定关卡详情...${NC}"
curl -s -X GET $SERVER/api/games/1/levels/1
echo -e "\n"

# 开始游戏关卡
echo -e "${BLUE}测试开始游戏关卡...${NC}"
curl -s -X POST $SERVER/api/games/1/levels/1/start \
  -H "Authorization: Bearer $TOKEN"
echo -e "\n"

# 提交游戏解答
echo -e "${BLUE}测试提交游戏解答...${NC}"
curl -s -X POST $SERVER/api/games/1/levels/1/submit \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"solution": "用户的解答内容"}'
echo -e "\n"

# 获取游戏提示
echo -e "${BLUE}测试获取游戏提示...${NC}"
curl -s -X GET $SERVER/api/games/1/levels/1/hint \
  -H "Authorization: Bearer $TOKEN"
echo -e "\n"

# 获取游戏解答
echo -e "${BLUE}测试获取游戏解答...${NC}"
curl -s -X GET $SERVER/api/games/1/levels/1/solution \
  -H "Authorization: Bearer $TOKEN"
echo -e "\n"

# 获取所有成就列表
echo -e "${BLUE}测试获取所有成就列表...${NC}"
curl -s -X GET $SERVER/api/achievements
echo -e "\n"

# 获取用户已获得的成就
echo -e "${BLUE}测试获取用户已获得的成就...${NC}"
curl -s -X GET $SERVER/api/user/achievements \
  -H "Authorization: Bearer $TOKEN"
echo -e "\n"

# 获取用户游戏进度
echo -e "${BLUE}测试获取用户游戏进度...${NC}"
curl -s -X GET $SERVER/api/user/progress \
  -H "Authorization: Bearer $TOKEN"
echo -e "\n"

# 同步用户游戏进度
echo -e "${BLUE}测试同步用户游戏进度...${NC}"
curl -s -X POST $SERVER/api/user/progress/sync \
  -H "Content-Type: application/json" \
  -H "Authorization: Bearer $TOKEN" \
  -d '{"gameId": 1, "levelId": 1, "completed": true, "score": 100}'
echo -e "\n"

echo -e "${GREEN}所有API测试完成!${NC}" 