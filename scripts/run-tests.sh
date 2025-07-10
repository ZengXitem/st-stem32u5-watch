#!/bin/bash

# 消防员智能安全手表系统测试脚本
# 作者: 挖野菜队

set -e

echo "🧪 开始运行系统测试..."

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# 测试计数器
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

run_test() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "${YELLOW}🔍 运行测试: $1${NC}"
    if $2; then
        echo -e "${GREEN}✅ 测试通过: $1${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}❌ 测试失败: $1${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# 测试物联网平台
test_iot_platform() {
    cd iot-platform
    mvn test
    cd ..
}

# 测试指挥官端应用
test_commander_app() {
    cd commander-app
    mvn test
    cd ..
}

# 测试API连通性
test_api_connectivity() {
    echo "启动测试环境..."
    cd deployment
    docker-compose up -d mysql mqtt-broker iot-platform
    sleep 30
    
    # 等待服务启动
    echo "等待服务就绪..."
    for i in {1..30}; do
        if curl -f http://localhost:8080/api/firefighter/stats &>/dev/null; then
            echo "API服务已就绪"
            break
        fi
        sleep 2
    done
    
    # 测试API端点
    curl -f http://localhost:8080/api/firefighter/stats || return 1
    curl -f http://localhost:8080/api/firefighter/data/latest || return 1
    
    docker-compose down
    cd ..
}

# 运行测试
run_test "物联网平台单元测试" test_iot_platform
run_test "指挥官端应用测试" test_commander_app
run_test "API连通性测试" test_api_connectivity

# 输出测试结果
echo
echo "📊 测试结果汇总："
echo "  总测试数: $TOTAL_TESTS"
echo "  通过数: $PASSED_TESTS"
echo "  失败数: $FAILED_TESTS"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 所有测试通过！${NC}"
    exit 0
else
    echo -e "${RED}❌ 有 $FAILED_TESTS 个测试失败${NC}"
    exit 1
fi 