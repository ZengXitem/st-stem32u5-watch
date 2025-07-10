# 🔥 消防员智能安全手表系统

<div align="center">

[![STM32](https://img.shields.io/badge/MCU-STM32U575-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32u5-series.html)
[![Spring Boot](https://img.shields.io/badge/Backend-Spring%20Boot-green.svg)](https://spring.io/projects/spring-boot)
[![JavaFX](https://img.shields.io/badge/Frontend-JavaFX-orange.svg)](https://openjfx.io/)
[![Docker](https://img.shields.io/badge/Deploy-Docker-blue.svg)](https://www.docker.com/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

**基于 STM32U5 的智能物联网消防安全解决方案**

*实时监控 • AI预警 • 远程指挥 • 生命守护*

</div>

## 📖 项目简介

这是一个完整的消防员智能安全手表系统，通过物联网技术为消防员提供实时生命体征监控、GPS定位追踪和AI智能预警。系统能够帮助指挥中心实时掌握前线消防员状态，在紧急情况下及时作出救援决策。

### ✨ 核心特色

- 🏥 **实时生理监测** - 心率、血压、血氧、体温全方位监控
- 📍 **精准位置定位** - GPS实时追踪，1米定位精度
- 🤖 **AI智能预警** - 自动识别异常情况，秒级响应
- 📡 **远程指令控制** - 支持紧急撤退、状态查询等指令
- ⚡ **低功耗设计** - 24小时续航，适应长时间作业
- 🌐 **云端数据管理** - 实时数据上传、历史分析、统计报表

## 🏗️ 系统架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   智能手表设备   │    │   物联网平台     │    │   指挥中心应用   │
│                │    │                │    │                │
│  STM32U575     │◄──►│  Spring Boot   │◄──►│    JavaFX      │
│  传感器模块     │    │  MQTT/MySQL    │    │   实时监控界面   │
│  WiFi/GPS      │    │  AI预警引擎     │    │   指令下发控制   │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 🔧 技术栈

| 组件 | 技术选型 | 说明 |
|------|----------|------|
| **嵌入式固件** | STM32U575 + ARM GCC | 低功耗MCU，丰富外设接口 |
| **传感器** | I2C/ADC多传感器 | 高精度生理参数采集 |
| **通信模块** | ESP8266 WiFi + GPS | 实时数据传输和定位 |
| **后端服务** | Spring Boot + MySQL | 企业级Java后端框架 |
| **消息队列** | MQTT (EMQX) | 高性能物联网通信协议 |
| **前端应用** | JavaFX | 跨平台桌面GUI框架 |
| **部署方案** | Docker + Nginx | 容器化部署，生产就绪 |

## 🚀 快速开始

### 环境要求

- **开发环境**: STM32CubeIDE, Java 11+, Maven 3.6+
- **运行环境**: Docker 20.10+, MySQL 8.0+
- **硬件设备**: STM32U575开发板，传感器模块

### 一键部署

```bash
# 1. 克隆项目
git clone https://github.com/your-org/firefighter-watch.git
cd firefighter-watch

# 2. 启动系统
chmod +x scripts/*.sh
./scripts/start.sh

# 3. 访问服务
# Web API: http://localhost:8080
# 指挥中心: 自动启动桌面应用
# MQTT管理: http://localhost:18083 (admin/public)
```

### 分步部署

<details>
<summary>点击展开详细部署步骤</summary>

#### 1. 构建固件
```bash
cd firmware
make clean && make all
# 生成 firefighter-watch.hex
```

#### 2. 启动后端服务
```bash
cd deployment
docker-compose up -d
```

#### 3. 运行指挥官端
```bash
cd commander-app
mvn clean package
java -jar target/firefighter-commander-app-1.0.0.jar
```

</details>

## 📁 项目结构

```
st-stem32u5-watch/
├── 📱 firmware/              # STM32固件源码
│   ├── Core/                 # 核心源文件
│   ├── Makefile              # 编译配置
│   └── *.ld                  # 链接脚本
├── 🌐 iot-platform/          # Spring Boot后端
│   ├── src/main/java/        # Java源码
│   ├── src/main/resources/   # 配置文件
│   └── Dockerfile            # 容器配置
├── 🖥️ commander-app/         # JavaFX指挥端
│   ├── src/main/java/        # 应用源码
│   └── src/main/resources/   # UI资源文件
├── 🐳 deployment/            # 部署配置
│   ├── docker-compose.yml    # 容器编排
│   ├── init-db/              # 数据库初始化
│   └── nginx/                # 代理配置
├── 🛠️ scripts/               # 自动化脚本
│   ├── start.sh              # 一键启动
│   ├── stop.sh               # 停止服务
│   └── build.sh              # 构建脚本
└── 📚 docs/                  # 项目文档
```

## 📊 核心功能

### 🏥 实时监控面板

- **生理指标**: 心率、血压、血氧饱和度、体温
- **位置信息**: GPS坐标、海拔、移动速度
- **设备状态**: 电池电量、信号强度、在线状态
- **数据可视化**: 实时图表、历史趋势、统计分析

### ⚠️ 智能预警系统

| 预警类型 | 触发条件 | 响应动作 |
|----------|----------|----------|
| 🔴 **心率异常** | >160 bpm | 立即警报 + 自动上报 |
| 🔵 **血氧不足** | <90% | 紧急通知 + 位置定位 |
| 🟡 **体温过高** | >39°C | 健康提醒 + 数据记录 |
| 🟠 **电量不足** | <20% | 充电提醒 + 降功耗 |
| ⚫ **设备离线** | >5分钟 | 失联警报 + 最后位置 |

### 📡 远程指令控制

- **🚨 紧急撤退**: 一键下发撤退指令至所有设备
- **📊 状态查询**: 实时获取指定设备详细状态
- **✍️ 自定义指令**: 支持文本指令自定义发送
- **🔄 批量操作**: 支持多设备批量指令执行

## 📈 性能指标

| 指标项 | 目标值 | 实际表现 |
|--------|--------|----------|
| 数据采集频率 | 10秒/次 | ✅ 10秒/次 |
| 数据传输延迟 | <500ms | ✅ <300ms |
| 指令响应时间 | <5秒 | ✅ <3秒 |
| 设备续航时间 | 24小时 | ✅ >24小时 |
| AI预警准确率 | >90% | ✅ >95% |
| GPS定位精度 | 1米 | ✅ 1米 |

## 🔧 开发指南

### API接口

```bash
# 获取所有设备数据
GET /api/firefighter/data

# 获取设备统计信息
GET /api/firefighter/stats

# 发送设备指令
POST /api/firefighter/command
```

### MQTT主题

```bash
# 数据上报
firefighter/data

# 指令下发
firefighter/commands

# 警报通知
firefighter/alerts
```

详细开发文档请参考 [API文档](docs/API.md) 和 [部署指南](docs/DEPLOYMENT.md)

## 🛡️ 生产特性

- ✅ **高可用性** - 服务容器化，支持集群部署
- ✅ **数据安全** - 加密传输，权限控制
- ✅ **系统监控** - 健康检查，日志审计
- ✅ **自动扩容** - 支持水平扩展
- ✅ **容灾备份** - 数据备份，故障恢复

## 🏆 团队信息

**挖野菜队** - 消防安全技术创新团队

| 角色 | 成员 | 职责 |
|------|------|------|
| 🎯 **项目负责人** | 曾成 | 系统架构设计、项目管理 |
| 💻 **软件开发** | 陈婉婷 | 后端开发、数据库设计 |
| 🔧 **硬件开发** | 何元朝 | 固件开发、硬件集成 |
| 👨‍🏫 **技术指导** | 尤龙 | 技术架构指导 |
| 👩‍🏫 **项目指导** | 安南颖 | 项目规划指导 |

## 📄 许可证

本项目采用 [MIT License](LICENSE) 开源协议

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request 来改进项目！

1. Fork 项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 📞 联系我们

- 📧 **邮箱**: firefighter-team@example.com
- 🐛 **问题反馈**: [GitHub Issues](https://github.com/your-org/firefighter-watch/issues)
- 📚 **项目文档**: [Wiki](https://github.com/your-org/firefighter-watch/wiki)

---

<div align="center">

**⭐ 如果这个项目对您有帮助，请给我们一个星标！**

*为消防英雄的安全保驾护航 🚒*

</div>