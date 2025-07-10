-- 消防员智能安全手表数据库初始化脚本

-- 设置字符集
SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- 创建数据库（如果不存在）
CREATE DATABASE IF NOT EXISTS firefighter_iot DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE firefighter_iot;

-- 创建消防员数据表
CREATE TABLE IF NOT EXISTS firefighter_data (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL COMMENT '设备ID',
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '时间戳',
    
    -- 生理参数
    heart_rate FLOAT COMMENT '心率(bpm)',
    blood_pressure_sys FLOAT COMMENT '收缩压(mmHg)',
    blood_pressure_dia FLOAT COMMENT '舒张压(mmHg)',
    blood_oxygen FLOAT COMMENT '血氧饱和度(%)',
    body_temperature FLOAT COMMENT '体温(°C)',
    
    -- GPS数据
    latitude DOUBLE COMMENT '纬度',
    longitude DOUBLE COMMENT '经度',
    altitude FLOAT COMMENT '海拔(m)',
    speed FLOAT COMMENT '速度(km/h)',
    satellites INT COMMENT '卫星数量',
    
    -- 系统状态
    battery_level INT COMMENT '电池电量(%)',
    status INT DEFAULT 1 COMMENT '系统状态(1:正常,0:异常)',
    
    -- 索引
    INDEX idx_device_id (device_id),
    INDEX idx_timestamp (timestamp),
    INDEX idx_device_timestamp (device_id, timestamp)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='消防员数据表';

-- 创建设备信息表
CREATE TABLE IF NOT EXISTS devices (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) UNIQUE NOT NULL COMMENT '设备ID',
    device_name VARCHAR(100) COMMENT '设备名称',
    firefighter_name VARCHAR(50) COMMENT '消防员姓名',
    firefighter_id VARCHAR(50) COMMENT '消防员编号',
    department VARCHAR(100) COMMENT '所属部门',
    phone VARCHAR(20) COMMENT '联系电话',
    emergency_contact VARCHAR(50) COMMENT '紧急联系人',
    emergency_phone VARCHAR(20) COMMENT '紧急联系电话',
    device_status ENUM('ACTIVE', 'INACTIVE', 'MAINTENANCE') DEFAULT 'ACTIVE' COMMENT '设备状态',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
    
    INDEX idx_device_id (device_id),
    INDEX idx_firefighter_id (firefighter_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='设备信息表';

-- 创建警报记录表
CREATE TABLE IF NOT EXISTS alert_records (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL COMMENT '设备ID',
    alert_type VARCHAR(50) NOT NULL COMMENT '警报类型',
    alert_message TEXT COMMENT '警报消息',
    severity ENUM('LOW', 'MEDIUM', 'HIGH', 'CRITICAL') DEFAULT 'MEDIUM' COMMENT '严重程度',
    alert_data JSON COMMENT '警报相关数据',
    is_handled BOOLEAN DEFAULT FALSE COMMENT '是否已处理',
    handled_by VARCHAR(50) COMMENT '处理人',
    handled_at TIMESTAMP NULL COMMENT '处理时间',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
    
    INDEX idx_device_id (device_id),
    INDEX idx_alert_type (alert_type),
    INDEX idx_created_at (created_at),
    INDEX idx_severity (severity)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='警报记录表';

-- 创建指令记录表
CREATE TABLE IF NOT EXISTS command_records (
    id BIGINT AUTO_INCREMENT PRIMARY KEY,
    device_id VARCHAR(50) NOT NULL COMMENT '设备ID',
    command_type VARCHAR(50) NOT NULL COMMENT '指令类型',
    command_data TEXT COMMENT '指令内容',
    sender VARCHAR(50) COMMENT '发送者',
    status ENUM('SENT', 'DELIVERED', 'EXECUTED', 'FAILED') DEFAULT 'SENT' COMMENT '执行状态',
    sent_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP COMMENT '发送时间',
    executed_at TIMESTAMP NULL COMMENT '执行时间',
    response_data TEXT COMMENT '响应数据',
    
    INDEX idx_device_id (device_id),
    INDEX idx_command_type (command_type),
    INDEX idx_sent_at (sent_at),
    INDEX idx_status (status)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='指令记录表';

-- 插入测试设备数据
INSERT INTO devices (device_id, device_name, firefighter_name, firefighter_id, department, phone, emergency_contact, emergency_phone) VALUES
('D1920VU212U', '消防员智能安全手表-001', '张三', 'FF001', '第一消防大队', '13800138001', '李四', '13800138002'),
('D1920VU212V', '消防员智能安全手表-002', '王五', 'FF002', '第一消防大队', '13800138003', '赵六', '13800138004'),
('D1920VU212W', '消防员智能安全手表-003', '刘七', 'FF003', '第二消防大队', '13800138005', '孙八', '13800138006')
ON DUPLICATE KEY UPDATE updated_at = CURRENT_TIMESTAMP;

-- 插入测试数据
INSERT INTO firefighter_data (device_id, heart_rate, blood_pressure_sys, blood_pressure_dia, blood_oxygen, body_temperature, latitude, longitude, altitude, speed, satellites, battery_level, status) VALUES
('D1920VU212U', 75.5, 120.0, 80.0, 98.5, 36.5, 23.129110, 113.264385, 50.0, 0.0, 8, 85, 1),
('D1920VU212V', 82.3, 125.0, 82.0, 97.8, 36.8, 23.130000, 113.265000, 52.0, 2.5, 9, 92, 1),
('D1920VU212W', 78.1, 118.0, 78.0, 98.9, 36.3, 23.128000, 113.263000, 48.0, 0.0, 7, 76, 1);

-- 插入示例警报记录
INSERT INTO alert_records (device_id, alert_type, alert_message, severity, alert_data) VALUES
('D1920VU212U', 'LOW_BATTERY', '电池电量不足', 'MEDIUM', '{"batteryLevel": 15, "threshold": 20}'),
('D1920VU212V', 'HIGH_HEART_RATE', '心率过高', 'HIGH', '{"heartRate": 165, "threshold": 160}');

-- 插入示例指令记录
INSERT INTO command_records (device_id, command_type, command_data, sender, status) VALUES
('D1920VU212U', 'STATUS_REQUEST', '请求状态更新', '指挥中心', 'EXECUTED'),
('D1920VU212V', 'EVACUATE', '立即撤退到安全区域', '指挥中心', 'DELIVERED');

-- 创建视图：设备在线状态
CREATE OR REPLACE VIEW device_online_status AS
SELECT 
    d.device_id,
    d.device_name,
    d.firefighter_name,
    d.device_status,
    CASE 
        WHEN fd.timestamp > DATE_SUB(NOW(), INTERVAL 5 MINUTE) THEN 'ONLINE'
        ELSE 'OFFLINE'
    END as online_status,
    fd.timestamp as last_seen
FROM devices d
LEFT JOIN (
    SELECT device_id, MAX(timestamp) as timestamp
    FROM firefighter_data
    GROUP BY device_id
) fd ON d.device_id = fd.device_id;

-- 创建存储过程：清理历史数据
DELIMITER //
CREATE PROCEDURE CleanOldData(IN days_to_keep INT)
BEGIN
    DECLARE cutoff_date TIMESTAMP;
    SET cutoff_date = DATE_SUB(NOW(), INTERVAL days_to_keep DAY);
    
    -- 删除历史数据
    DELETE FROM firefighter_data WHERE timestamp < cutoff_date;
    DELETE FROM alert_records WHERE created_at < cutoff_date;
    DELETE FROM command_records WHERE sent_at < cutoff_date;
    
    -- 记录清理信息
    SELECT CONCAT('清理完成，删除了 ', ROW_COUNT(), ' 条记录') as result;
END //
DELIMITER ;

SET FOREIGN_KEY_CHECKS = 1;

-- 显示初始化完成信息
SELECT '数据库初始化完成！' as message; 