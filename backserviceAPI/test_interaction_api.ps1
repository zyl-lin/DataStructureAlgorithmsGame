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
    
    try {
        $params = @{
            Uri = $Endpoint
            Method = $Method
            ContentType = "application/json"
        }
        
        if ($Method -eq "POST" -and $Data -ne "") {
            $params.Body = $Data
        }
        
        $response = Invoke-RestMethod @params
        
        if ($response.success -eq $true) {
            Write-Host "测试通过!" -ForegroundColor Green
            Write-Host "响应: " -ForegroundColor Gray
            $response | ConvertTo-Json -Depth 5 | Write-Host -ForegroundColor Gray
        } else {
            Write-Host "测试失败: $($response.message)" -ForegroundColor Red
        }
    } catch {
        Write-Host "请求错误: $_" -ForegroundColor Red
    }
    
    return $response
}

# 测试链表游戏会话创建
Write-Host "准备测试链表游戏会话创建接口..." -ForegroundColor Yellow
$sessionData = '{"gameType": "linkedlist", "animationType": "step"}'
$sessionResponse = Test-API -Endpoint "$baseUrl/api/animation/create-session" -Method "POST" -Data $sessionData -Description "创建链表游戏会话"

# 如果会话创建成功，使用返回的会话ID，否则使用一个固定的测试会话ID
if ($sessionResponse -and $sessionResponse.success) {
    $sessionId = $sessionResponse.data.sessionId
    Write-Host "会话创建成功，使用会话ID: $sessionId" -ForegroundColor Green
} else {
    $sessionId = "test-session-123"
    Write-Host "使用测试会话ID: $sessionId" -ForegroundColor Yellow
}

# 测试动态规划游戏会话创建
Write-Host "准备测试动态规划游戏会话创建接口..." -ForegroundColor Yellow
$dpSessionData = '{"gameType": "dp", "animationType": "step"}'
$dpSessionResponse = Test-API -Endpoint "$baseUrl/api/animation/create-session" -Method "POST" -Data $dpSessionData -Description "创建动态规划游戏会话"

# 测试贪心算法游戏会话创建
Write-Host "准备测试贪心算法游戏会话创建接口..." -ForegroundColor Yellow
$greedySessionData = '{"gameType": "greedy", "animationType": "step"}'
$greedySessionResponse = Test-API -Endpoint "$baseUrl/api/animation/create-session" -Method "POST" -Data $greedySessionData -Description "创建贪心算法游戏会话"

# 测试点击链表节点
$clickData = '{"sessionId": "' + $sessionId + '", "elementId": "node-1", "eventType": "click"}'
Test-API -Endpoint "$baseUrl/api/games/click" -Method "POST" -Data $clickData -Description "点击链表节点"

# 测试拖拽链表节点
$dragData = '{"sessionId": "' + $sessionId + '", "elementId": "node-2", "eventType": "drag", "startPos": {"x": 200, "y": 150}, "endPos": {"x": 250, "y": 150}}'
Test-API -Endpoint "$baseUrl/api/games/drag" -Method "POST" -Data $dragData -Description "拖拽链表节点"

# 测试放置链表节点
$dropData = '{"sessionId": "' + $sessionId + '", "elementId": "node-2", "eventType": "drop", "position": {"x": 250, "y": 150}}'
Test-API -Endpoint "$baseUrl/api/games/drop" -Method "POST" -Data $dropData -Description "放置链表节点"

# 测试悬停链表节点
$hoverData = '{"sessionId": "' + $sessionId + '", "elementId": "node-3", "eventType": "hover"}'
Test-API -Endpoint "$baseUrl/api/games/hover" -Method "POST" -Data $hoverData -Description "悬停在链表节点上"

# 测试通用交互接口
$interactData = '{"sessionId": "' + $sessionId + '", "elementId": "edge-1", "eventType": "click", "additionalData": {"highlight": true}}'
Test-API -Endpoint "$baseUrl/api/games/interact" -Method "POST" -Data $interactData -Description "点击链表边缘"

# 测试暂停动画
$pauseData = '{"sessionId": "' + $sessionId + '"}'
Test-API -Endpoint "$baseUrl/api/animation/pause" -Method "POST" -Data $pauseData -Description "暂停动画"

# 测试恢复动画
$resumeData = '{"sessionId": "' + $sessionId + '"}'
Test-API -Endpoint "$baseUrl/api/animation/resume" -Method "POST" -Data $resumeData -Description "恢复动画"

# 测试获取下一帧
$nextFrameData = '{"sessionId": "' + $sessionId + '"}'
Test-API -Endpoint "$baseUrl/api/animation/nextFrame" -Method "POST" -Data $nextFrameData -Description "获取下一帧"

# 测试获取动画状态
Test-API -Endpoint "$baseUrl/api/animation/status?sessionId=$sessionId" -Method "GET" -Description "获取动画状态"

Write-Host "`n所有测试完成!" -ForegroundColor Green 