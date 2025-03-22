# API兼容性检查脚本
# 用于验证所有API端点是否正常工作
# 使用方法: .\api-compatibility-check.ps1 [HostUrl]
# 默认HostUrl为http://localhost:3000

# 检查是否提供了主机参数
param (
    [string]$HostUrl = "http://192.168.56.128:3000"
)

# 颜色函数
function Write-ColorOutput($ForegroundColor) {
    $fc = $host.UI.RawUI.ForegroundColor
    $host.UI.RawUI.ForegroundColor = $ForegroundColor
    if ($args) {
        Write-Output $args
    }
    else {
        $input | Write-Output
    }
    $host.UI.RawUI.ForegroundColor = $fc
}

Write-ColorOutput Yellow "开始对 $HostUrl 进行API兼容性检查...`n"

# 计数器
$totalTests = 0
$passedTests = 0

# 测试函数 - 参数: 方法, 路径, 描述, [请求体]
function Test-Endpoint {
    param (
        [string]$Method,
        [string]$Endpoint,
        [string]$Description,
        [string]$Body = $null
    )
    
    $global:totalTests++
    
    Write-Host "测试 $Method $Endpoint`: $Description... " -NoNewline
    
    try {
        if ($Method -eq "GET") {
            $response = Invoke-WebRequest -Uri "$HostUrl$Endpoint" -Method $Method -ErrorAction SilentlyContinue
        }
        else {
            if ([string]::IsNullOrEmpty($Body)) {
                $response = Invoke-WebRequest -Uri "$HostUrl$Endpoint" -Method $Method -ErrorAction SilentlyContinue
            }
            else {
                $response = Invoke-WebRequest -Uri "$HostUrl$Endpoint" -Method $Method -Body $Body -ContentType "application/json" -ErrorAction SilentlyContinue
            }
        }
        
        if ($response.StatusCode -eq 200 -or $response.StatusCode -eq 201) {
            Write-ColorOutput Green "通过 ($($response.StatusCode))"
            $global:passedTests++
        }
        else {
            Write-ColorOutput Red "失败 ($($response.StatusCode))"
        }
    }
    catch {
        $statusCode = $_.Exception.Response.StatusCode.value__
        if ($statusCode) {
            Write-ColorOutput Red "失败 ($statusCode)"
        }
        else {
            Write-ColorOutput Red "失败 (连接错误)"
        }
    }
}

Write-ColorOutput Yellow "========== 基础API检查 =========="
Test-Endpoint -Method "GET" -Endpoint "/" -Description "根路径"
Test-Endpoint -Method "GET" -Endpoint "/health" -Description "健康检查"

Write-ColorOutput Yellow "`n========== 数据结构API检查 =========="

# 链表
Write-ColorOutput Yellow "`n--- 链表 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/linkedlist/state" -Description "获取链表状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/linkedlist/reset" -Description "重置链表"
Test-Endpoint -Method "POST" -Endpoint "/api/games/linkedlist/insert" -Description "插入节点" -Body '{"value":42,"position":0}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/linkedlist/delete" -Description "删除节点" -Body '{"position":0}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/linkedlist/find" -Description "查找节点" -Body '{"value":42}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/linkedlist/reverse" -Description "反转链表"

# 队列
Write-ColorOutput Yellow "`n--- 队列 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/queue/state" -Description "获取队列状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/queue/reset" -Description "重置队列"
Test-Endpoint -Method "POST" -Endpoint "/api/games/queue/enqueue" -Description "入队" -Body '{"value":42}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/queue/dequeue" -Description "出队"
Test-Endpoint -Method "GET" -Endpoint "/api/games/queue/peek" -Description "查看队首元素"

# 栈
Write-ColorOutput Yellow "`n--- 栈 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/stack/state" -Description "获取栈状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/stack/reset" -Description "重置栈"
Test-Endpoint -Method "POST" -Endpoint "/api/games/stack/push" -Description "入栈" -Body '{"value":42}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/stack/pop" -Description "出栈"
Test-Endpoint -Method "GET" -Endpoint "/api/games/stack/peek" -Description "查看栈顶元素"

# 二叉树
Write-ColorOutput Yellow "`n--- 二叉树 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/binarytree/state" -Description "获取二叉树状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/binarytree/reset" -Description "重置二叉树"
Test-Endpoint -Method "POST" -Endpoint "/api/games/binarytree/insert" -Description "插入节点" -Body '{"value":50}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/binarytree/traversePreOrder" -Description "前序遍历"
Test-Endpoint -Method "POST" -Endpoint "/api/games/binarytree/traverseInOrder" -Description "中序遍历"
Test-Endpoint -Method "POST" -Endpoint "/api/games/binarytree/traversePostOrder" -Description "后序遍历"
Test-Endpoint -Method "POST" -Endpoint "/api/games/binarytree/traverseLevelOrder" -Description "层序遍历"

# 图
Write-ColorOutput Yellow "`n--- 图 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/graph/state" -Description "获取图状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/graph/reset" -Description "重置图"
Test-Endpoint -Method "POST" -Endpoint "/api/games/graph/traverseDFS" -Description "DFS遍历" -Body '{"startVertex":"A"}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/graph/traverseBFS" -Description "BFS遍历" -Body '{"startVertex":"A"}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/graph/findPath" -Description "查找路径" -Body '{"source":"A","target":"C"}'

Write-ColorOutput Yellow "`n========== 算法API检查 =========="

# 排序
Write-ColorOutput Yellow "`n--- 排序算法 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/sorting/state" -Description "获取排序状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/sorting/reset" -Description "重置排序"
Test-Endpoint -Method "POST" -Endpoint "/api/games/sorting/bubbleSort" -Description "冒泡排序" -Body '{"array":[5,3,8,4,2]}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/sorting/selectionSort" -Description "选择排序" -Body '{"array":[5,3,8,4,2]}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/sorting/insertionSort" -Description "插入排序" -Body '{"array":[5,3,8,4,2]}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/sorting/quickSort" -Description "快速排序" -Body '{"array":[5,3,8,4,2]}'

# 搜索
Write-ColorOutput Yellow "`n--- 搜索算法 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/search/state" -Description "获取搜索状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/search/reset" -Description "重置搜索"
Test-Endpoint -Method "POST" -Endpoint "/api/games/search/linear" -Description "线性搜索" -Body '{"array":[5,3,8,4,2],"target":4}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/search/binary" -Description "二分搜索" -Body '{"array":[2,3,4,5,8],"target":4}'

# 迷宫
Write-ColorOutput Yellow "`n--- 迷宫算法 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/maze/state" -Description "获取迷宫状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/maze/reset" -Description "重置迷宫"
Test-Endpoint -Method "GET" -Endpoint "/api/games/maze/generate" -Description "生成迷宫"
Test-Endpoint -Method "POST" -Endpoint "/api/games/maze/solveDFS" -Description "DFS解迷宫"
Test-Endpoint -Method "POST" -Endpoint "/api/games/maze/solveBFS" -Description "BFS解迷宫"
Test-Endpoint -Method "POST" -Endpoint "/api/games/maze/solveAStar" -Description "A*解迷宫"

# 动态规划
Write-ColorOutput Yellow "`n--- 动态规划算法 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/dp/state" -Description "获取动态规划状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/dp/reset" -Description "重置动态规划"
Test-Endpoint -Method "POST" -Endpoint "/api/games/dp/fibonacci" -Description "斐波那契数列" -Body '{"n":6}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/dp/knapsack" -Description "背包问题" -Body '{"weights":[2,3,4,5],"values":[3,4,5,6],"capacity":10}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/dp/lcs" -Description "最长公共子序列" -Body '{"str1":"ABCBDAB","str2":"BDCABA"}'

# 贪心算法
Write-ColorOutput Yellow "`n--- 贪心算法 API ---"
Test-Endpoint -Method "GET" -Endpoint "/api/games/greedy/state" -Description "获取贪心算法状态"
Test-Endpoint -Method "POST" -Endpoint "/api/games/greedy/reset" -Description "重置贪心算法"
Test-Endpoint -Method "POST" -Endpoint "/api/games/greedy/coinChange" -Description "零钱兑换" -Body '{"amount":63,"coins":[25,10,5,1]}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/greedy/activitySelection" -Description "活动安排" -Body '{"start":[1,3,0,5,8,5],"finish":[2,4,6,7,9,9]}'
Test-Endpoint -Method "POST" -Endpoint "/api/games/greedy/huffman" -Description "哈夫曼编码" -Body '{"chars":["a","b","c","d","e"],"freqs":[15,7,6,6,5]}'

# 结果统计
Write-ColorOutput Yellow "`n========== 测试结果 =========="
$passRate = [math]::Round(($passedTests / $totalTests) * 100, 2)
Write-Host "总测试: $totalTests"
Write-Host "通过测试: $passedTests"
Write-Host "通过率: $passRate%"

if ($passedTests -eq $totalTests) {
    Write-ColorOutput Green "`n所有API测试通过! ✅"
}
else {
    Write-ColorOutput Red "`n部分API测试失败，请检查服务器日志获取详细信息. ❌"
} 