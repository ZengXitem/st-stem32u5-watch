# 消防员智能安全手表 API 文档

## 概述

消防员智能安全手表系统提供RESTful API和WebSocket接口，用于实时监控消防员状态、发送指令和处理警报。

## 基础信息

- **基础URL**: `http://localhost:8080/api/firefighter`
- **内容类型**: `application/json`
- **WebSocket**: `ws://localhost:8080/ws`

## 数据监控 API

### 获取所有设备最新数据

```http
GET /data/latest
```

**响应示例**:
```json
[
  {
    "id": 1,
    "deviceId": "D1920VU212U",
    "timestamp": "2024-12-19T10:30:00",
    "heartRate": 75.5,
    "bloodPressureSys": 120.0,
    "bloodPressureDia": 80.0,
    "bloodOxygen": 98.5,
    "bodyTemperature": 36.5,
    "latitude": 23.129110,
    "longitude": 113.264385,
    "altitude": 50.0,
    "speed": 0.0,
    "satellites": 8,
    "batteryLevel": 85,
    "status": 1
  }
]
```

### 获取指定设备最新数据

```http
GET /data/latest/{deviceId}
```

**参数**:
- `deviceId`: 设备ID

### 获取设备历史数据

```http
GET /data/history/{deviceId}?hours={hours}
```

**参数**:
- `deviceId`: 设备ID
- `hours`: 查询小时数 (默认: 24)

## 指令控制 API

### 发送撤退指令

```http
POST /command/evacuate/{deviceId}
```

### 发送状态查询指令

```http
POST /command/status/{deviceId}
```

### 发送自定义指令

```http
POST /command/custom/{deviceId}
```

**请求体**:
```json
{
  "command": "CUSTOM",
  "data": "指令内容",
  "timestamp": 1640000000000
}
```

## 设备管理 API

### 获取在线设备列表

```http
GET /devices/online
```

### 获取系统统计信息

```http
GET /stats
```

**响应示例**:
```json
{
  "totalDevices": 3,
  "onlineDevices": 2,
  "alertCount": 1,
  "lastUpdate": "2024-12-19T10:30:00"
}
```

## WebSocket 实时通信

### 连接端点

```javascript
const socket = new WebSocket('ws://localhost:8080/ws');
```

### 订阅主题

- `/topic/alerts` - 警报消息
- `/topic/notifications` - 系统通知
- `/topic/data` - 实时数据更新

### 消息格式

**警报消息**:
```json
{
  "deviceId": "D1920VU212U",
  "alertType": "HIGH_HEART_RATE",
  "message": "心率过高",
  "severity": "CRITICAL",
  "timestamp": "2024-12-19T10:30:00",
  "data": {...}
}
```

## 错误响应

```json
{
  "error": "设备未找到",
  "code": 404,
  "timestamp": "2024-12-19T10:30:00"
}
```

## 状态码

- `200` - 成功
- `400` - 请求错误
- `404` - 资源未找到
- `500` - 服务器错误 