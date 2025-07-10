#!/bin/bash

# 消防员智能安全手表系统启动脚本
# 作者: 挖野菜队

set -e

echo "🚀 启动消防员智能安全手表系统..."

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# 检查Docker是否运行
check_docker() {
    if ! docker info >/dev/null 2>&1; then
        echo -e "${RED}❌ Docker未运行，请先启动Docker${NC}"
        exit 1
    fi
}

# 检查环境配置
check_env() {
    if [ ! -f "deployment/.env" ]; then
        echo -e "${YELLOW}⚠️  .env文件不存在，从示例创建...${NC}"
        cp deployment/.env.example deployment/.env
        echo -e "${BLUE}ℹ️  请编辑 deployment/.env 文件配置您的环境${NC}"
    fi
}

# 启动后端服务
start_backend() {
    echo -e "${YELLOW}🐳 启动后端服务...${NC}"
    cd deployment
    
    # 确保网络存在
    docker network create firefighter-network 2>/dev/null || true
    
    # 启动数据库和Redis
    echo "启动数据库和缓存..."
    docker-compose up -d mysql redis
    
    # 等待数据库就绪
    echo "等待数据库启动..."
    for i in {1..30}; do
        if docker exec firefighter-mysql mysqladmin ping -h localhost --silent; then
            echo -e "${GREEN}✅ 数据库已就绪${NC}"
            break
        fi
        sleep 2
        echo -n "."
    done
    
    # 启动MQTT服务
    echo "启动MQTT服务..."
    docker-compose up -d mqtt-broker
    
    # 等待MQTT就绪
    sleep 10
    
    # 启动应用服务
    echo "启动应用服务..."
    docker-compose up -d iot-platform
    
    # 启动Nginx
    echo "启动Web服务..."
    docker-compose up -d nginx
    
    cd ..
    echo -e "${GREEN}✅ 后端服务启动完成${NC}"
}

# 启动指挥官端应用
start_commander() {
    echo -e "${YELLOW}🖥️  启动指挥官端应用...${NC}"
    
    # 检查Java环境
    if ! command -v java &> /dev/null; then
        echo -e "${RED}❌ Java未安装，请先安装JDK 11+${NC}"
        return 1
    fi
    
    # 检查应用JAR文件
    JAR_FILE="commander-app/target/firefighter-commander-app-1.0.0.jar"
    if [ ! -f "$JAR_FILE" ]; then
        echo -e "${YELLOW}⚠️  应用JAR不存在，正在构建...${NC}"
        cd commander-app
        mvn clean package -DskipTests
        cd ..
    fi
    
    # 后台启动指挥官端应用
    echo "启动指挥官端应用..."
    nohup java -jar "$JAR_FILE" > logs/commander-app.log 2>&1 &
    COMMANDER_PID=$!
    echo $COMMANDER_PID > logs/commander-app.pid
    
    echo -e "${GREEN}✅ 指挥官端应用已启动 (PID: $COMMANDER_PID)${NC}"
}

# 验证服务状态
verify_services() {
    echo -e "${YELLOW}🔍 验证服务状态...${NC}"
    
    # 检查后端API
    for i in {1..30}; do
        if curl -s http://localhost:8080/api/firefighter/stats >/dev/null; then
            echo -e "${GREEN}✅ 后端API服务正常${NC}"
            break
        fi
        sleep 2
        echo -n "."
    done
    
    # 检查MQTT
    if docker exec firefighter-mqtt emqx ctl status >/dev/null 2>&1; then
        echo -e "${GREEN}✅ MQTT服务正常${NC}"
    else
        echo -e "${YELLOW}⚠️  MQTT服务状态未知${NC}"
    fi
    
    # 显示服务地址
    echo
    echo -e "${BLUE}📋 服务访问地址：${NC}"
    echo "  🌐 Web API: http://localhost:8080"
    echo "  📊 MQTT管理: http://localhost:18083 (admin/public)"
    echo "  📈 系统监控: http://localhost:8080/actuator/health"
    echo "  🖥️  指挥官端: 桌面应用已启动"
}

# 创建日志目录
create_log_dir() {
    mkdir -p logs
}

# 主执行流程
main() {
    create_log_dir
    check_docker
    check_env
    start_backend
    
    # 询问是否启动指挥官端
    read -p "是否启动指挥官端应用？(y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        start_commander
    fi
    
    verify_services
    
    echo
    echo -e "${GREEN}🎉 系统启动完成！${NC}"
    echo -e "${BLUE}💡 使用 './scripts/stop.sh' 停止系统${NC}"
    echo -e "${BLUE}💡 使用 './scripts/logs.sh' 查看日志${NC}"
}

# 执行主函数
main "$@" 