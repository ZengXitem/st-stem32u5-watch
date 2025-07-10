#!/bin/bash

# 消防员智能安全手表系统日志查看脚本
# 作者: 挖野菜队

echo "📋 消防员智能安全手表系统日志"

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

# 显示菜单
show_menu() {
    echo
    echo -e "${BLUE}请选择要查看的日志：${NC}"
    echo "1. 物联网平台日志"
    echo "2. 指挥官端应用日志"
    echo "3. MQTT服务日志"
    echo "4. 数据库日志"
    echo "5. Nginx日志"
    echo "6. 所有Docker容器日志"
    echo "7. 实时跟踪所有日志"
    echo "0. 退出"
    echo
}

# 查看物联网平台日志
view_iot_logs() {
    echo -e "${YELLOW}📊 物联网平台日志：${NC}"
    docker logs firefighter-iot-platform --tail=50 -f
}

# 查看指挥官端日志
view_commander_logs() {
    echo -e "${YELLOW}🖥️  指挥官端应用日志：${NC}"
    if [ -f "logs/commander-app.log" ]; then
        tail -50 -f logs/commander-app.log
    else
        echo "指挥官端日志文件不存在"
    fi
}

# 查看MQTT日志
view_mqtt_logs() {
    echo -e "${YELLOW}📡 MQTT服务日志：${NC}"
    docker logs firefighter-mqtt --tail=50 -f
}

# 查看数据库日志
view_mysql_logs() {
    echo -e "${YELLOW}🗄️  数据库日志：${NC}"
    docker logs firefighter-mysql --tail=50 -f
}

# 查看Nginx日志
view_nginx_logs() {
    echo -e "${YELLOW}🌐 Nginx日志：${NC}"
    docker logs firefighter-nginx --tail=50 -f
}

# 查看所有容器日志
view_all_docker_logs() {
    echo -e "${YELLOW}🐳 所有Docker容器日志：${NC}"
    docker-compose -f deployment/docker-compose.yml logs --tail=20 -f
}

# 实时跟踪所有日志
tail_all_logs() {
    echo -e "${YELLOW}👁️  实时跟踪所有日志：${NC}"
    echo "按 Ctrl+C 停止"
    (
        docker-compose -f deployment/docker-compose.yml logs -f &
        if [ -f "logs/commander-app.log" ]; then
            tail -f logs/commander-app.log &
        fi
        wait
    )
}

# 主循环
while true; do
    show_menu
    read -p "请输入选择 (0-7): " choice
    
    case $choice in
        1)
            view_iot_logs
            ;;
        2)
            view_commander_logs
            ;;
        3)
            view_mqtt_logs
            ;;
        4)
            view_mysql_logs
            ;;
        5)
            view_nginx_logs
            ;;
        6)
            view_all_docker_logs
            ;;
        7)
            tail_all_logs
            ;;
        0)
            echo -e "${GREEN}👋 再见！${NC}"
            exit 0
            ;;
        *)
            echo "无效选择，请重新输入"
            ;;
    esac
    
    echo
    read -p "按回车键返回菜单..."
done 