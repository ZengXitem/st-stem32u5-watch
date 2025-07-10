package com.firefighter.iot.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.firefighter.iot.model.FirefighterData;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Service;

import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.Map;

@Service
public class AlertService {
    
    @Autowired
    private SimpMessagingTemplate messagingTemplate;
    
    private final ObjectMapper objectMapper = new ObjectMapper();
    
    /**
     * 检查数据是否触发警报
     */
    public void checkAlerts(FirefighterData data) {
        // 检查心率异常
        if (data.getHeartRate() != null && data.getHeartRate() > 160) {
            sendAlert(data, "HIGH_HEART_RATE", "心率过高", "CRITICAL");
        }
        
        // 检查血氧异常
        if (data.getBloodOxygen() != null && data.getBloodOxygen() < 90) {
            sendAlert(data, "LOW_BLOOD_OXYGEN", "血氧不足", "CRITICAL");
        }
        
        // 检查体温异常
        if (data.getBodyTemperature() != null && data.getBodyTemperature() > 39.0) {
            sendAlert(data, "HIGH_TEMPERATURE", "体温过高", "HIGH");
        }
        
        // 检查电池电量
        if (data.getBatteryLevel() != null && data.getBatteryLevel() < 20) {
            sendAlert(data, "LOW_BATTERY", "电池电量不足", "MEDIUM");
        }
        
        // 检查设备离线
        LocalDateTime lastUpdate = data.getTimestamp();
        if (lastUpdate != null && lastUpdate.isBefore(LocalDateTime.now().minusMinutes(5))) {
            sendAlert(data, "DEVICE_OFFLINE", "设备离线", "HIGH");
        }
    }
    
    /**
     * 发送警报
     */
    private void sendAlert(FirefighterData data, String alertType, String message, String severity) {
        try {
            Map<String, Object> alert = new HashMap<>();
            alert.put("deviceId", data.getDeviceId());
            alert.put("alertType", alertType);
            alert.put("message", message);
            alert.put("severity", severity);
            alert.put("timestamp", LocalDateTime.now());
            alert.put("data", data);
            
            // 通过WebSocket发送实时警报
            messagingTemplate.convertAndSend("/topic/alerts", alert);
            
            // 记录日志
            System.out.println("警报发送 - 设备: " + data.getDeviceId() + 
                             ", 类型: " + alertType + 
                             ", 消息: " + message + 
                             ", 严重程度: " + severity);
            
        } catch (Exception e) {
            System.err.println("发送警报失败: " + e.getMessage());
        }
    }
    
    /**
     * 处理来自设备的警报
     */
    public void processAlert(String alertPayload) {
        try {
            Map<String, Object> alertData = objectMapper.readValue(alertPayload, Map.class);
            
            // 转发警报到前端
            messagingTemplate.convertAndSend("/topic/alerts", alertData);
            
            System.out.println("处理设备警报: " + alertPayload);
            
        } catch (Exception e) {
            System.err.println("处理警报失败: " + e.getMessage());
        }
    }
    
    /**
     * 发送系统通知
     */
    public void sendSystemNotification(String message, String type) {
        try {
            Map<String, Object> notification = new HashMap<>();
            notification.put("message", message);
            notification.put("type", type);
            notification.put("timestamp", LocalDateTime.now());
            
            messagingTemplate.convertAndSend("/topic/notifications", notification);
            
        } catch (Exception e) {
            System.err.println("发送系统通知失败: " + e.getMessage());
        }
    }
} 