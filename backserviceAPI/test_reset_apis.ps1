# 数据结构与算法游戏API的reset接口测试脚本

# 设置基本URL
$baseUrl = "http://localhost:3000"

# 定义测试函数
function Test-ResetAPI {
    param (
        [string]$endpoint,
        [string]$description
    )
    
    Write-Host "测试 $description..." -ForegroundColor Cyan
    
    try {
        $response = Invoke-WebRequest -Uri "$baseUrl$endpoint" -Method Post -ContentType "application/json"
        $statusCode = $response.StatusCode
        
        if ($statusCode -eq 200) {
            Write-Host "✓ 成功 (状态码: $statusCode)" -ForegroundColor Green
            
            # 尝试解析JSON响应并显示重置后的状态
            $content = $response.Content | ConvertFrom-Json
            Write-Host "  重置后的状态: " -NoNewline
            Write-Host ($content.data.state | ConvertTo-Json -Compress) -ForegroundColor Gray
        } else {
            Write-Host "✗ 失败 (状态码: $statusCode)" -ForegroundColor Red
        }
    } catch {
        Write-Host "✗ 请求出错: $_" -ForegroundColor Red
    }
    
    Write-Host ""
}

# 显示测试开始
Write-Host "=====================================" -ForegroundColor Yellow
Write-Host "   数据结构与算法游戏reset接口测试   " -ForegroundColor Yellow
Write-Host "=====================================" -ForegroundColor Yellow
Write-Host ""

# 测试链表reset接口
Test-ResetAPI -endpoint "/api/games/linkedlist/reset" -description "链表重置接口"

# 测试队列reset接口
Test-ResetAPI -endpoint "/api/games/queue/reset" -description "队列重置接口"

# 测试栈reset接口
Test-ResetAPI -endpoint "/api/games/stack/reset" -description "栈重置接口"

# 测试二叉树reset接口
Test-ResetAPI -endpoint "/api/games/binarytree/reset" -description "二叉树重置接口"

# 测试图reset接口
Test-ResetAPI -endpoint "/api/games/graph/reset" -description "图重置接口"

# 测试排序reset接口
Test-ResetAPI -endpoint "/api/games/sorting/reset" -description "排序重置接口"

# 测试搜索reset接口
Test-ResetAPI -endpoint "/api/games/search/reset" -description "搜索重置接口"

# 测试迷宫reset接口
Test-ResetAPI -endpoint "/api/games/maze/reset" -description "迷宫重置接口"

# 测试动态规划reset接口
Test-ResetAPI -endpoint "/api/games/dp/reset" -description "动态规划重置接口"

# 测试贪心算法reset接口
Test-ResetAPI -endpoint "/api/games/greedy/reset" -description "贪心算法重置接口"

# 显示测试结束
Write-Host "=====================================" -ForegroundColor Yellow
Write-Host "             测试完成              " -ForegroundColor Yellow
Write-Host "=====================================" -ForegroundColor Yellow 