# 定义API基础URL
$baseUrl = "http://localhost:3000"

# 测试API函数
function Test-API {
    param (
        [string]$Endpoint,
        [string]$Method = "GET",
        [string]$Data = "",
        [string]$Description = ""
    )
    
    Write-Host "=========================="
    Write-Host "测试: $Description" -ForegroundColor Cyan
    Write-Host "请求: $Method $Endpoint" -ForegroundColor Gray
    
    if ($Data -ne "") {
        Write-Host "数据: $Data" -ForegroundColor Gray
    }
    
    # 发送请求
    try {
        if ($Method -eq "GET") {
            $response = Invoke-RestMethod -Uri "$baseUrl$Endpoint" -Method $Method
        } else {
            $response = Invoke-RestMethod -Uri "$baseUrl$Endpoint" -Method $Method -Body $Data -ContentType "application/json"
        }
        
        # 检查是否成功
        if ($response.success -eq $true) {
            Write-Host "测试通过!" -ForegroundColor Green
        } else {
            Write-Host "测试失败!" -ForegroundColor Red
        }
        
        # 输出响应
        Write-Host "响应: $($response | ConvertTo-Json -Depth 3 -Compress)" -ForegroundColor Gray
        
        return $response
    } catch {
        Write-Host "测试失败! 错误: $_" -ForegroundColor Red
        return $null
    }
}

# 测试链表游戏会话创建
Write-Host "准备测试链表游戏会话创建接口..." -ForegroundColor Yellow
$sessionData = '{"gameType": "linkedlist", "animationType": "step"}'
$session = Test-API -Endpoint "/api/animation/create-session" -Method "POST" -Data $sessionData -Description "创建链表游戏会话"

# 获取会话ID
if ($session -and $session.success) {
    $sessionId = $session.data.sessionId
    Write-Host "会话创建成功，使用会话ID: $sessionId" -ForegroundColor Green
} else {
    # 使用固定会话ID进行测试
    $sessionId = "test-session-123"
    Write-Host "使用测试会话ID: $sessionId" -ForegroundColor Yellow
}

# 测试动态规划游戏会话创建
Write-Host "准备测试动态规划游戏会话创建接口..." -ForegroundColor Yellow
$dpSessionData = '{"gameType": "dp", "animationType": "step"}'
Test-API -Endpoint "/api/animation/create-session" -Method "POST" -Data $dpSessionData -Description "创建动态规划(斐波那契)游戏会话"

# 测试LCS动态规划游戏会话创建
Write-Host "准备测试LCS动态规划游戏会话创建接口..." -ForegroundColor Yellow
$lcsSessionData = '{"gameType": "dp", "dpType": "lcs", "animationType": "step"}'
$lcsSession = Test-API -Endpoint "/api/animation/create-session" -Method "POST" -Data $lcsSessionData -Description "创建LCS动态规划游戏会话"

# 获取LCS会话ID
if ($lcsSession -and $lcsSession.success) {
    $lcsSessionId = $lcsSession.data.sessionId
    Write-Host "LCS会话创建成功，使用会话ID: $lcsSessionId" -ForegroundColor Green
    
    # 测试LCS动画生成
    $lcsAnimData = "{`"sessionId`": `"$lcsSessionId`"}"
    Test-API -Endpoint "/api/dp/lcs" -Method "POST" -Data $lcsAnimData -Description "生成LCS动画"
    
    # 测试LCS单步执行
    $lcsStepData = "{`"sessionId`": `"$lcsSessionId`", `"step`": 1}"
    Test-API -Endpoint "/api/dp/step" -Method "POST" -Data $lcsStepData -Description "LCS单步执行"
    
    # 测试LCS路径显示
    $lcsPathData = "{`"sessionId`": `"$lcsSessionId`"}"
    Test-API -Endpoint "/api/dp/lcs/path" -Method "POST" -Data $lcsPathData -Description "显示LCS路径"
    
    # 测试LCS单元格计算
    $lcsCellData = "{`"sessionId`": `"$lcsSessionId`", `"cellId`": `"cell-2-3`"}"
    Test-API -Endpoint "/api/dp/lcs/cell" -Method "POST" -Data $lcsCellData -Description "计算LCS单元格"
    
    # 测试点击LCS单元格
    $lcsCellId = "cell-1-1"
    $lcsClickData = "{`"sessionId`": `"$lcsSessionId`", `"elementId`": `"$lcsCellId`", `"eventType`": `"click`"}"
    Test-API -Endpoint "/api/games/click" -Method "POST" -Data $lcsClickData -Description "点击LCS单元格"
    
    # 测试悬停在LCS单元格上
    $lcsHoverData = "{`"sessionId`": `"$lcsSessionId`", `"elementId`": `"$lcsCellId`", `"eventType`": `"hover`"}"
    Test-API -Endpoint "/api/games/hover" -Method "POST" -Data $lcsHoverData -Description "悬停在LCS单元格上"
}

# 测试贪心算法游戏会话创建
Write-Host "准备测试贪心算法游戏会话创建接口..." -ForegroundColor Yellow
$greedySessionData = '{"gameType": "greedy", "animationType": "step"}'
Test-API -Endpoint "/api/animation/create-session" -Method "POST" -Data $greedySessionData -Description "创建贪心算法游戏会话"

# 测试点击链表节点
$clickData = "{`"sessionId`": `"$sessionId`", `"elementId`": `"node-1`", `"eventType`": `"click`"}"
Test-API -Endpoint "/api/games/click" -Method "POST" -Data $clickData -Description "点击链表节点"

# 测试拖拽链表节点
$dragData = "{`"sessionId`": `"$sessionId`", `"elementId`": `"node-2`", `"eventType`": `"drag`", `"startPos`": {`"x`": 200, `"y`": 150}, `"endPos`": {`"x`": 250, `"y`": 150}}"
Test-API -Endpoint "/api/games/drag" -Method "POST" -Data $dragData -Description "拖拽链表节点"

# 测试放置链表节点
$dropData = "{`"sessionId`": `"$sessionId`", `"elementId`": `"node-2`", `"eventType`": `"drop`", `"position`": {`"x`": 250, `"y`": 150}}"
Test-API -Endpoint "/api/games/drop" -Method "POST" -Data $dropData -Description "放置链表节点"

# 测试悬停链表节点
$hoverData = "{`"sessionId`": `"$sessionId`", `"elementId`": `"node-3`", `"eventType`": `"hover`"}"
Test-API -Endpoint "/api/games/hover" -Method "POST" -Data $hoverData -Description "悬停在链表节点上"

# 测试通用交互接口
$interactData = "{`"sessionId`": `"$sessionId`", `"elementId`": `"edge-1`", `"eventType`": `"click`", `"additionalData`": {`"highlight`": true}}"
Test-API -Endpoint "/api/games/interact" -Method "POST" -Data $interactData -Description "点击链表边缘"

# 测试暂停动画
$pauseData = "{`"sessionId`": `"$sessionId`"}"
Test-API -Endpoint "/api/animation/pause" -Method "POST" -Data $pauseData -Description "暂停动画"

# 测试恢复动画
$resumeData = "{`"sessionId`": `"$sessionId`"}"
Test-API -Endpoint "/api/animation/resume" -Method "POST" -Data $resumeData -Description "恢复动画"

# 测试获取下一帧
$nextFrameData = "{`"sessionId`": `"$sessionId`"}"
Test-API -Endpoint "/api/animation/nextFrame" -Method "POST" -Data $nextFrameData -Description "获取下一帧"

# 测试获取动画状态
Test-API -Endpoint "/api/animation/status?sessionId=$sessionId" -Method "GET" -Description "获取动画状态"

Write-Host "`n所有测试完成!" -ForegroundColor Green 