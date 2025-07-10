#!/bin/bash

# 消防员智能安全手表系统构建脚本
# 作者: 挖野菜队

set -e

echo "🔥 开始构建消防员智能安全手表系统..."

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 检查必要工具
check_tool() {
    if ! command -v $1 &> /dev/null; then
        echo -e "${RED}❌ $1 未安装，请先安装${NC}"
        exit 1
    fi
}

echo "📋 检查必要工具..."
check_tool java
check_tool mvn
check_tool docker
check_tool arm-none-eabi-gcc

# 构建嵌入式固件
echo -e "${YELLOW}🛠️  构建嵌入式固件...${NC}"
cd firmware
make clean
make all
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ 固件构建成功${NC}"
else
    echo -e "${RED}❌ 固件构建失败${NC}"
    exit 1
fi
cd ..

# 构建物联网平台
echo -e "${YELLOW}🛠️  构建物联网平台...${NC}"
cd iot-platform
mvn clean package -DskipTests
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ 物联网平台构建成功${NC}"
else
    echo -e "${RED}❌ 物联网平台构建失败${NC}"
    exit 1
fi
cd ..

# 构建指挥官端应用
echo -e "${YELLOW}🛠️  构建指挥官端应用...${NC}"
cd commander-app
mvn clean package -DskipTests
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ 指挥官端应用构建成功${NC}"
else
    echo -e "${RED}❌ 指挥官端应用构建失败${NC}"
    exit 1
fi
cd ..

# 构建Docker镜像
echo -e "${YELLOW}🐳 构建Docker镜像...${NC}"
cd deployment
docker-compose build
if [ $? -eq 0 ]; then
    echo -e "${GREEN}✅ Docker镜像构建成功${NC}"
else
    echo -e "${RED}❌ Docker镜像构建失败${NC}"
    exit 1
fi
cd ..

echo -e "${GREEN}🎉 系统构建完成！${NC}"
echo "📦 构建产物："
echo "  - firmware/build/firefighter-watch.hex"
echo "  - iot-platform/target/firefighter-iot-platform-1.0.0.jar"
echo "  - commander-app/target/firefighter-commander-app-1.0.0.jar"
echo "  - Docker镜像已准备就绪"

echo -e "${YELLOW}🚀 部署说明：${NC}"
echo "1. 嵌入式固件：使用ST-Link或J-Link烧录到STM32U575"
echo "2. 物联网平台：运行 'cd deployment && docker-compose up -d'"
echo "3. 指挥官端：运行 'java -jar commander-app/target/firefighter-commander-app-1.0.0.jar'" 