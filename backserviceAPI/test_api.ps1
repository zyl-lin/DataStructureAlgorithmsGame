# 设置服务器地址
$SERVER="http://192.168.56.128:3000"
# $SERVER="http://你的远程服务器IP:3000" # 如果是远程服务器，取消注释此行

# 设置控制台编码为UTF-8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

# 格式化输出函数
function Format-Response {
    param (
        [string]$response
    )
    try {
        $json = $response | ConvertFrom-Json
        return ($json | ConvertTo-Json -Depth 3)
    } catch {
        return $response
    }
}

$OutputEncoding = [console]::InputEncoding = [console]::OutputEncoding = New-Object System.Text.UTF8Encoding

chcp 65001

Write-Host "测试健康状态..." -ForegroundColor Blue
$response = Invoke-WebRequest -Uri "$SERVER/api/health" -Method GET | Select-Object -ExpandProperty Content
Write-Host (Format-Response $response)
Write-Host ""

Write-Host "测试用户注册..." -ForegroundColor Blue
$response = Invoke-WebRequest -Uri "$SERVER/api/user/register" -Method POST -ContentType "application/json" -Body '{"username": "testuser", "password": "password123", "email": "test@example.com"}' | Select-Object -ExpandProperty Content
Write-Host (Format-Response $response)
Write-Host ""

Write-Host "测试用户登录..." -ForegroundColor Blue
$loginResponse = Invoke-WebRequest -Uri "$SERVER/api/user/login" -Method POST -ContentType "application/json" -Body '{"username": "testuser", "password": "password123"}' | Select-Object -ExpandProperty Content
$loginData = $loginResponse | ConvertFrom-Json
$TOKEN = $loginData.token

if ([string]::IsNullOrEmpty($TOKEN)) {
    Write-Host "登录失败，无法获取token" -ForegroundColor Red
    exit 1
} else {
    Write-Host "获取token成功: $TOKEN" -ForegroundColor Green
}
Write-Host ""

Write-Host "测试获取用户信息..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/user/profile" -Method GET -Headers @{"Authorization"="Bearer $TOKEN"} | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试更新用户信息..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/user/profile" -Method PUT -ContentType "application/json" -Headers @{"Authorization"="Bearer $TOKEN"} -Body '{"nickname": "新昵称", "avatar": "avatar.png"}' | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取所有游戏..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/games" -Method GET | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取特定游戏详情..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/games/1" -Method GET | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取游戏关卡列表..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/games/1/levels" -Method GET | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取特定关卡详情..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/games/1/levels/1" -Method GET | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试开始游戏关卡..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/games/1/levels/1/start" -Method POST -Headers @{"Authorization"="Bearer $TOKEN"} | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试提交游戏解答..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/games/1/levels/1/submit" -Method POST -ContentType "application/json" -Headers @{"Authorization"="Bearer $TOKEN"} -Body '{"solution": "用户的解答内容"}' | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取游戏提示..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/games/1/levels/1/hint" -Method GET -Headers @{"Authorization"="Bearer $TOKEN"} | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取游戏解答..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/games/1/levels/1/solution" -Method GET -Headers @{"Authorization"="Bearer $TOKEN"} | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取所有成就列表..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/achievements" -Method GET | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取用户已获得的成就..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/user/achievements" -Method GET -Headers @{"Authorization"="Bearer $TOKEN"} | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试获取用户游戏进度..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/user/progress" -Method GET -Headers @{"Authorization"="Bearer $TOKEN"} | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "测试同步用户游戏进度..." -ForegroundColor Blue
Invoke-WebRequest -Uri "$SERVER/api/user/progress/sync" -Method POST -ContentType "application/json" -Headers @{"Authorization"="Bearer $TOKEN"} -Body '{"gameId": 1, "levelId": 1, "completed": true, "score": 100}' | Select-Object -ExpandProperty Content
Write-Host ""

Write-Host "所有API测试完成!" -ForegroundColor Green 