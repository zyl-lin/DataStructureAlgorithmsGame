# 二叉树API测试脚本
$apiHost = "192.168.56.128:3000"

Write-Host "开始测试二叉树API..." -ForegroundColor Green

# 测试二叉树状态API
Write-Host "`n测试获取二叉树状态..." -ForegroundColor Yellow
try {
    $response = Invoke-WebRequest -Uri "http://$apiHost/api/games/binarytree/state" -Method GET
    Write-Host "成功获取二叉树状态: $($response.StatusCode)" -ForegroundColor Green
    Write-Host $response.Content
} catch {
    Write-Host "获取二叉树状态失败: $($_.Exception.Message)" -ForegroundColor Red
}

# 测试二叉树插入API
Write-Host "`n测试插入节点..." -ForegroundColor Yellow
$valuesToInsert = @(50, 30, 70, 20, 40, 60, 80)
foreach ($value in $valuesToInsert) {
    try {
        $body = @{ value = $value } | ConvertTo-Json
        $response = Invoke-WebRequest -Uri "http://$apiHost/api/games/binarytree/insert" -Method POST -Body $body -ContentType "application/json"
        Write-Host "成功插入节点 $value: $($response.StatusCode)" -ForegroundColor Green
    } catch {
        Write-Host "插入节点 $value 失败: $($_.Exception.Message)" -ForegroundColor Red
    }
}

# 测试二叉树状态API（检查插入后的状态）
Write-Host "`n测试获取更新后的二叉树状态..." -ForegroundColor Yellow
try {
    $response = Invoke-WebRequest -Uri "http://$apiHost/api/games/binarytree/state" -Method GET
    Write-Host "成功获取二叉树状态: $($response.StatusCode)" -ForegroundColor Green
    Write-Host $response.Content
} catch {
    Write-Host "获取二叉树状态失败: $($_.Exception.Message)" -ForegroundColor Red
}

# 测试前序遍历API
Write-Host "`n测试前序遍历..." -ForegroundColor Yellow
try {
    $response = Invoke-WebRequest -Uri "http://$apiHost/api/games/binarytree/traversePreOrder?animate=true&speed=5" -Method POST -Body "{}" -ContentType "application/json"
    Write-Host "前序遍历成功: $($response.StatusCode)" -ForegroundColor Green
    $content = $response.Content | ConvertFrom-Json
    Write-Host "遍历结果: $($content.data.animation.frames[-1].traversalResult -join ', ')" -ForegroundColor Cyan
} catch {
    Write-Host "前序遍历失败: $($_.Exception.Message)" -ForegroundColor Red
}

# 测试中序遍历API
Write-Host "`n测试中序遍历..." -ForegroundColor Yellow
try {
    $response = Invoke-WebRequest -Uri "http://$apiHost/api/games/binarytree/traverseInOrder?animate=true&speed=5" -Method POST -Body "{}" -ContentType "application/json"
    Write-Host "中序遍历成功: $($response.StatusCode)" -ForegroundColor Green
    $content = $response.Content | ConvertFrom-Json
    Write-Host "遍历结果: $($content.data.animation.frames[-1].traversalResult -join ', ')" -ForegroundColor Cyan
} catch {
    Write-Host "中序遍历失败: $($_.Exception.Message)" -ForegroundColor Red
}

# 测试后序遍历API
Write-Host "`n测试后序遍历..." -ForegroundColor Yellow
try {
    $response = Invoke-WebRequest -Uri "http://$apiHost/api/games/binarytree/traversePostOrder?animate=true&speed=5" -Method POST -Body "{}" -ContentType "application/json"
    Write-Host "后序遍历成功: $($response.StatusCode)" -ForegroundColor Green
    $content = $response.Content | ConvertFrom-Json
    Write-Host "遍历结果: $($content.data.animation.frames[-1].traversalResult -join ', ')" -ForegroundColor Cyan
} catch {
    Write-Host "后序遍历失败: $($_.Exception.Message)" -ForegroundColor Red
}

# 测试层序遍历API
Write-Host "`n测试层序遍历..." -ForegroundColor Yellow
try {
    $response = Invoke-WebRequest -Uri "http://$apiHost/api/games/binarytree/traverseLevelOrder?animate=true&speed=5" -Method POST -Body "{}" -ContentType "application/json"
    Write-Host "层序遍历成功: $($response.StatusCode)" -ForegroundColor Green
    $content = $response.Content | ConvertFrom-Json
    Write-Host "遍历结果: $($content.data.animation.frames[-1].traversalResult -join ', ')" -ForegroundColor Cyan
} catch {
    Write-Host "层序遍历失败: $($_.Exception.Message)" -ForegroundColor Red
}

Write-Host "`n二叉树API测试完成" -ForegroundColor Green 