#!/bin/bash

# 消防员智能安全手表系统停止脚本
# 作者: 挖野菜队

set -e

echo "🛑 停止消防员智能安全手表系统..."

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# 停止指挥官端应用
stop_commander() {
    echo -e "${YELLOW}🖥️  停止指挥官端应用...${NC}"
    
    if [ -f "logs/commander-app.pid" ]; then
        PID=$(cat logs/commander-app.pid)
        if kill -0 $PID 2>/dev/null; then
            kill $PID
            echo -e "${GREEN}✅ 指挥官端应用已停止 (PID: $PID)${NC}"
        else
            echo -e "${YELLOW}⚠️  指挥官端应用进程不存在${NC}"
        fi
        rm -f logs/commander-app.pid
    else
        echo -e "${YELLOW}⚠️  未找到指挥官端应用PID文件${NC}"
        # 尝试通过进程名查找并停止
        pkill -f "firefighter-commander-app" || true
    fi
}

# 停止后端服务
stop_backend() {
    echo -e "${YELLOW}🐳 停止后端服务...${NC}"
    cd deployment
    
    # 停止所有服务
    docker-compose down
    
    # 检查是否需要清理
    read -p "是否清理数据库数据？(y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo -e "${YELLOW}🗑️  清理数据卷...${NC}"
        docker-compose down -v
        docker volume prune -f
        echo -e "${GREEN}✅ 数据已清理${NC}"
    fi
    
    cd ..
    echo -e "${GREEN}✅ 后端服务已停止${NC}"
}

# 清理临时文件
cleanup() {
    echo -e "${YELLOW}🧹 清理临时文件...${NC}"
    
    # 清理日志文件（可选）
    read -p "是否清理日志文件？(y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        rm -rf logs/*.log
        echo -e "${GREEN}✅ 日志文件已清理${NC}"
    fi
    
    # 清理构建缓存
    read -p "是否清理构建缓存？(y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "清理Maven缓存..."
        cd iot-platform && mvn clean && cd ..
        cd commander-app && mvn clean && cd ..
        echo -e "${GREEN}✅ 构建缓存已清理${NC}"
    fi
}

# 显示系统状态
show_status() {
    echo -e "${YELLOW}📊 检查剩余进程...${NC}"
    
    # 检查Docker容器
    CONTAINERS=$(docker ps --filter "name=firefighter" --format "table {{.Names}}\t{{.Status}}" 2>/dev/null || true)
    if [ -n "$CONTAINERS" ]; then
        echo "仍在运行的容器："
        echo "$CONTAINERS"
    else
        echo -e "${GREEN}✅ 没有运行的容器${NC}"
    fi
    
    # 检查Java进程
    JAVA_PROCESSES=$(pgrep -f "firefighter" || true)
    if [ -n "$JAVA_PROCESSES" ]; then
        echo "仍在运行的Java进程："
        ps aux | grep firefighter | grep -v grep || true
    else
        echo -e "${GREEN}✅ 没有运行的Java进程${NC}"
    fi
}

# 强制停止所有相关进程
force_stop() {
    echo -e "${YELLOW}🔨 强制停止所有相关进程...${NC}"
    
    # 强制停止Docker容器
    docker ps --filter "name=firefighter" -q | xargs -r docker kill
    docker ps -a --filter "name=firefighter" -q | xargs -r docker rm
    
    # 强制停止Java进程
    pkill -f "firefighter" || true
    
    echo -e "${GREEN}✅ 强制停止完成${NC}"
}

# 主执行流程
main() {
    # 检查是否需要强制停止
    if [ "$1" = "--force" ] || [ "$1" = "-f" ]; then
        force_stop
        return
    fi
    
    stop_commander
    stop_backend
    
    show_status
    
    # 询问是否清理
    echo
    read -p "是否进行清理操作？(y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        cleanup
    fi
    
    echo
    echo -e "${GREEN}🎉 系统已停止！${NC}"
    echo -e "${YELLOW}💡 使用 './scripts/start.sh' 重新启动系统${NC}"
    echo -e "${YELLOW}💡 使用 './scripts/stop.sh --force' 强制停止所有进程${NC}"
}

# 显示帮助信息
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "用法: $0 [选项]"
    echo
    echo "选项:"
    echo "  -f, --force    强制停止所有相关进程"
    echo "  -h, --help     显示此帮助信息"
    echo
    echo "示例:"
    echo "  $0              正常停止系统"
    echo "  $0 --force      强制停止所有进程"
    exit 0
fi

# 执行主函数
main "$@" 