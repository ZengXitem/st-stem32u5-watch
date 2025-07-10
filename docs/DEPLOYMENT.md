# 消防员智能安全手表系统部署指南

## 系统概述

本系统是基于 STM32U5 的消防员智能安全手表解决方案，包含三个主要组成部分：
- **嵌入式固件**: STM32U575 微控制器上的固件程序
- **物联网平台**: Spring Boot 后端服务
- **指挥官端应用**: JavaFX 桌面客户端

## 系统要求

### 硬件要求
- **开发板**: STM32U575 Nucleo 或自定义PCB
- **传感器**: 心率、血压、血氧、温度传感器
- **通信模块**: ESP8266 WiFi模块
- **GPS模块**: UART接口GPS接收器
- **服务器**: 最低2GB RAM，20GB存储空间

### 软件要求
- **嵌入式开发**: STM32CubeIDE 1.10+, ARM GCC工具链
- **后端服务**: Docker 20.10+, Docker Compose 1.29+
- **桌面应用**: Java 11+, Maven 3.6+
- **操作系统**: Linux/Windows/macOS

## 快速开始

### 1. 克隆项目
```bash
git clone https://github.com/firefighter-team/st-stem32u5-watch.git
cd st-stem32u5-watch
```

### 2. 一键启动系统
```bash
chmod +x scripts/*.sh
./scripts/start.sh
```

### 3. 访问系统
- **Web API**: http://localhost:8080
- **MQTT管理**: http://localhost:18083 (admin/public)
- **指挥官端**: 自动启动桌面应用

## 详细部署步骤

### 步骤1: 环境准备

#### 安装Docker
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install docker.io docker-compose

# CentOS/RHEL
sudo yum install docker docker-compose

# 启动Docker服务
sudo systemctl start docker
sudo systemctl enable docker
```

#### 安装Java 11+
```bash
# Ubuntu/Debian
sudo apt-get install openjdk-11-jdk

# CentOS/RHEL
sudo yum install java-11-openjdk-devel

# 验证安装
java -version
mvn -version
```

### 步骤2: 配置环境变量

复制环境配置文件：
```bash
cp deployment/.env.example deployment/.env
```

编辑 `.env` 文件，配置以下参数：
```env
# 数据库配置
MYSQL_ROOT_PASSWORD=your_secure_password
MYSQL_DATABASE=firefighter_iot
MYSQL_USER=firefighter
MYSQL_PASSWORD=your_db_password

# MQTT配置
MQTT_BROKER_URL=tcp://mqtt-broker:1883

# 警报阈值
ALERT_HEART_RATE_MAX=160
ALERT_BLOOD_OXYGEN_MIN=90
ALERT_TEMPERATURE_MAX=39.0
ALERT_BATTERY_LOW=20
```

### 步骤3: 构建系统组件

#### 构建嵌入式固件
```bash
cd firmware
make clean
make all
```

#### 构建物联网平台
```bash
cd iot-platform
mvn clean package -DskipTests
```

#### 构建指挥官端应用
```bash
cd commander-app
mvn clean package -DskipTests
```

### 步骤4: 部署后端服务

#### 启动数据库和基础设施
```bash
cd deployment
docker-compose up -d mysql redis mqtt-broker
```

#### 等待服务就绪
```bash
# 检查数据库
docker exec firefighter-mysql mysqladmin ping -h localhost

# 检查MQTT
docker exec firefighter-mqtt emqx ctl status
```

#### 启动应用服务
```bash
docker-compose up -d iot-platform nginx
```

### 步骤5: 部署指挥官端应用

#### 后台启动
```bash
nohup java -jar commander-app/target/firefighter-commander-app-1.0.0.jar > logs/commander-app.log 2>&1 &
```

#### 前台启动（调试用）
```bash
java -jar commander-app/target/firefighter-commander-app-1.0.0.jar
```

### 步骤6: 烧录嵌入式固件

#### 使用STM32CubeIDE
1. 打开 `firmware/STM32CubeIDE/.project`
2. 配置目标芯片为 STM32U575ZIT6Q
3. 连接ST-Link调试器
4. 点击"Run"或"Debug"

#### 使用命令行工具
```bash
# 使用openocd
openocd -f interface/stlink-v2.cfg -f target/stm32u5x.cfg -c "program firmware/build/firefighter-watch.hex verify reset exit"

# 使用st-flash
st-flash write firmware/build/firefighter-watch.bin 0x08000000
```

## 系统配置

### 网络配置

#### WiFi设置
在嵌入式设备中配置WiFi：
```c
// 在main.c中修改WiFi配置
ESP8266_ConnectWiFi("Your_WiFi_SSID", "Your_WiFi_Password");
```

#### MQTT配置
```yaml
# iot-platform/src/main/resources/application.yml
mqtt:
  broker:
    url: tcp://your-mqtt-broker:1883
  topics:
    data: firefighter/data
    commands: firefighter/commands
    alerts: firefighter/alerts
```

### 数据库配置

#### 初始数据导入
```bash
# 连接到数据库
docker exec -it firefighter-mysql mysql -u root -p

# 导入初始数据
mysql> SOURCE /docker-entrypoint-initdb.d/01-init.sql;
```

#### 数据清理策略
```sql
-- 设置数据保留期（天）
CALL CleanOldData(30);
```

### 安全配置

#### 防火墙设置
```bash
# 开放必要端口
sudo ufw allow 8080/tcp  # Web API
sudo ufw allow 1883/tcp  # MQTT
sudo ufw allow 18083/tcp # MQTT管理
```

#### SSL/TLS配置
```nginx
# deployment/nginx/nginx.conf
server {
    listen 443 ssl;
    ssl_certificate /path/to/certificate.crt;
    ssl_certificate_key /path/to/private.key;
    # ... 其他SSL配置
}
```

## 监控和维护

### 健康检查

#### 系统状态检查
```bash
# 检查所有服务状态
curl http://localhost:8080/actuator/health

# 检查MQTT状态
docker exec firefighter-mqtt emqx ctl status

# 检查数据库连接
docker exec firefighter-mysql mysqladmin ping -h localhost
```

#### 日志监控
```bash
# 实时查看所有日志
./scripts/logs.sh

# 查看特定服务日志
docker logs firefighter-iot-platform -f
```

### 性能监控

#### 指标收集
```bash
# Prometheus指标
curl http://localhost:8080/actuator/prometheus

# JVM指标
curl http://localhost:8080/actuator/metrics/jvm.memory.used
```

#### 数据库性能
```sql
-- 查看慢查询
SHOW FULL PROCESSLIST;

-- 查看表大小
SELECT 
    table_name,
    ROUND(((data_length + index_length) / 1024 / 1024), 2) AS 'Size in MB'
FROM information_schema.tables 
WHERE table_schema = 'firefighter_iot';
```

### 备份和恢复

#### 数据库备份
```bash
# 创建备份
docker exec firefighter-mysql mysqldump -u root -p firefighter_iot > backup_$(date +%Y%m%d).sql

# 恢复备份
docker exec -i firefighter-mysql mysql -u root -p firefighter_iot < backup_20240101.sql
```

#### 配置备份
```bash
# 备份配置文件
tar -czf config_backup_$(date +%Y%m%d).tar.gz deployment/.env iot-platform/src/main/resources/application.yml
```

## 故障排除

### 常见问题

#### 1. 数据库连接失败
```bash
# 检查数据库容器状态
docker ps | grep mysql

# 查看数据库日志
docker logs firefighter-mysql

# 测试连接
docker exec firefighter-mysql mysql -u firefighter -p -e "SELECT 1;"
```

#### 2. MQTT连接失败
```bash
# 检查MQTT服务状态
docker exec firefighter-mqtt emqx ctl status

# 查看MQTT日志
docker logs firefighter-mqtt

# 测试MQTT连接
mosquitto_pub -h localhost -p 1883 -t test -m "hello"
```

#### 3. 固件烧录失败
```bash
# 检查USB连接
lsusb | grep STMicroelectronics

# 检查权限
sudo chmod 666 /dev/ttyACM0

# 重新烧录
make flash
```

### 性能优化

#### JVM调优
```bash
# 设置JVM参数
export JAVA_OPTS="-Xms1g -Xmx2g -XX:+UseG1GC"
java $JAVA_OPTS -jar iot-platform.jar
```

#### 数据库优化
```sql
-- 添加索引
CREATE INDEX idx_device_timestamp ON firefighter_data(device_id, timestamp);

-- 优化查询
EXPLAIN SELECT * FROM firefighter_data WHERE device_id = 'D1920VU212U' ORDER BY timestamp DESC LIMIT 10;
```

## 扩展部署

### 集群部署

#### 负载均衡配置
```nginx
upstream iot_backend {
    server iot-platform-1:8080;
    server iot-platform-2:8080;
    server iot-platform-3:8080;
}
```

#### 数据库主从复制
```yaml
# docker-compose-cluster.yml
mysql-master:
  image: mysql:8.0
  environment:
    MYSQL_REPLICATION_MODE: master
    MYSQL_REPLICATION_USER: repl_user
    
mysql-slave:
  image: mysql:8.0
  environment:
    MYSQL_REPLICATION_MODE: slave
    MYSQL_MASTER_HOST: mysql-master
```

### 云部署

#### Kubernetes部署
```yaml
# k8s/deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: firefighter-iot-platform
spec:
  replicas: 3
  selector:
    matchLabels:
      app: firefighter-iot
  template:
    metadata:
      labels:
        app: firefighter-iot
    spec:
      containers:
      - name: iot-platform
        image: firefighter/iot-platform:latest
        ports:
        - containerPort: 8080
```

## 联系支持

如遇到部署问题，请联系：

- **技术支持**: support@firefighter-team.com
- **项目地址**: https://github.com/firefighter-team/st-stem32u5-watch
- **文档中心**: https://docs.firefighter-team.com

---

**挖野菜队** - 专注于消防安全物联网解决方案 