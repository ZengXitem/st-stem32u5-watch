package com.firefighter.iot.service;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.firefighter.iot.model.FirefighterData;
import org.eclipse.paho.client.mqttv3.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import javax.annotation.PostConstruct;
import java.time.LocalDateTime;
import java.util.UUID;

@Service
public class MqttService implements MqttCallback {
    
    @Value("${mqtt.broker.url:tcp://localhost:1883}")
    private String brokerUrl;
    
    @Value("${mqtt.client.id:firefighter-server}")
    private String clientId;
    
    @Autowired
    private DataService dataService;
    
    @Autowired
    private AlertService alertService;
    
    private MqttClient mqttClient;
    private ObjectMapper objectMapper = new ObjectMapper();
    
    @PostConstruct
    public void initialize() {
        try {
            mqttClient = new MqttClient(brokerUrl, clientId + "_" + UUID.randomUUID().toString());
            MqttConnectOptions options = new MqttConnectOptions();
            options.setCleanSession(true);
            options.setConnectionTimeout(10);
            options.setKeepAliveInterval(20);
            
            mqttClient.setCallback(this);
            mqttClient.connect(options);
            
            // 订阅相关主题
            mqttClient.subscribe("firefighter/data", 1);
            mqttClient.subscribe("firefighter/alerts", 1);
            mqttClient.subscribe("firefighter/status", 1);
            
            System.out.println("MQTT服务已连接到: " + brokerUrl);
            
        } catch (MqttException e) {
            System.err.println("MQTT连接失败: " + e.getMessage());
        }
    }
    
    @Override
    public void connectionLost(Throwable cause) {
        System.err.println("MQTT连接丢失: " + cause.getMessage());
        // 实现重连逻辑
        try {
            Thread.sleep(5000);
            mqttClient.reconnect();
        } catch (Exception e) {
            System.err.println("MQTT重连失败: " + e.getMessage());
        }
    }
    
    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        String payload = new String(message.getPayload());
        System.out.println("收到MQTT消息 - 主题: " + topic + ", 内容: " + payload);
        
        switch (topic) {
            case "firefighter/data":
                handleFirefighterData(payload);
                break;
            case "firefighter/alerts":
                handleAlert(payload);
                break;
            case "firefighter/status":
                handleStatusUpdate(payload);
                break;
        }
    }
    
    @Override
    public void deliveryComplete(IMqttDeliveryToken token) {
        // 消息发送完成回调
    }
    
    private void handleFirefighterData(String payload) {
        try {
            // 解析JSON数据
            FirefighterDataDto dto = objectMapper.readValue(payload, FirefighterDataDto.class);
            
            // 转换为实体对象
            FirefighterData data = new FirefighterData();
            data.setDeviceId(dto.getDeviceId());
            data.setTimestamp(LocalDateTime.now());
            data.setHeartRate(dto.getBiometric().getHeartRate());
            data.setBloodPressureSys(dto.getBiometric().getBloodPressureSys());
            data.setBloodPressureDia(dto.getBiometric().getBloodPressureDia());
            data.setBloodOxygen(dto.getBiometric().getBloodOxygen());
            data.setBodyTemperature(dto.getBiometric().getBodyTemperature());
            data.setLatitude(dto.getGps().getLatitude());
            data.setLongitude(dto.getGps().getLongitude());
            data.setAltitude(dto.getGps().getAltitude());
            data.setSpeed(dto.getGps().getSpeed());
            data.setSatellites(dto.getGps().getSatellites());
            data.setBatteryLevel(dto.getBattery());
            data.setStatus(dto.getStatus());
            
            // 保存数据
            dataService.saveFirefighterData(data);
            
            // 检查是否需要发送警报
            alertService.checkAlerts(data);
            
        } catch (Exception e) {
            System.err.println("处理消防员数据失败: " + e.getMessage());
        }
    }
    
    private void handleAlert(String payload) {
        try {
            alertService.processAlert(payload);
        } catch (Exception e) {
            System.err.println("处理警报失败: " + e.getMessage());
        }
    }
    
    private void handleStatusUpdate(String payload) {
        System.out.println("设备状态更新: " + payload);
    }
    
    public void sendCommand(String deviceId, String command) {
        try {
            String topic = "firefighter/commands";
            MqttMessage message = new MqttMessage(command.getBytes());
            message.setQos(1);
            mqttClient.publish(topic, message);
            System.out.println("发送指令到设备 " + deviceId + ": " + command);
        } catch (MqttException e) {
            System.err.println("发送指令失败: " + e.getMessage());
        }
    }
    
    // 内部DTO类
    public static class FirefighterDataDto {
        private String deviceId;
        private BiometricData biometric;
        private GPSData gps;
        private Integer battery;
        private Integer status;
        
        // Getters and setters
        public String getDeviceId() { return deviceId; }
        public void setDeviceId(String deviceId) { this.deviceId = deviceId; }
        
        public BiometricData getBiometric() { return biometric; }
        public void setBiometric(BiometricData biometric) { this.biometric = biometric; }
        
        public GPSData getGps() { return gps; }
        public void setGps(GPSData gps) { this.gps = gps; }
        
        public Integer getBattery() { return battery; }
        public void setBattery(Integer battery) { this.battery = battery; }
        
        public Integer getStatus() { return status; }
        public void setStatus(Integer status) { this.status = status; }
    }
    
    public static class BiometricData {
        private Float heartRate;
        private Float bloodPressureSys;
        private Float bloodPressureDia;
        private Float bloodOxygen;
        private Float bodyTemperature;
        
        // Getters and setters
        public Float getHeartRate() { return heartRate; }
        public void setHeartRate(Float heartRate) { this.heartRate = heartRate; }
        
        public Float getBloodPressureSys() { return bloodPressureSys; }
        public void setBloodPressureSys(Float bloodPressureSys) { this.bloodPressureSys = bloodPressureSys; }
        
        public Float getBloodPressureDia() { return bloodPressureDia; }
        public void setBloodPressureDia(Float bloodPressureDia) { this.bloodPressureDia = bloodPressureDia; }
        
        public Float getBloodOxygen() { return bloodOxygen; }
        public void setBloodOxygen(Float bloodOxygen) { this.bloodOxygen = bloodOxygen; }
        
        public Float getBodyTemperature() { return bodyTemperature; }
        public void setBodyTemperature(Float bodyTemperature) { this.bodyTemperature = bodyTemperature; }
    }
    
    public static class GPSData {
        private Double latitude;
        private Double longitude;
        private Float altitude;
        private Float speed;
        private Integer satellites;
        
        // Getters and setters
        public Double getLatitude() { return latitude; }
        public void setLatitude(Double latitude) { this.latitude = latitude; }
        
        public Double getLongitude() { return longitude; }
        public void setLongitude(Double longitude) { this.longitude = longitude; }
        
        public Float getAltitude() { return altitude; }
        public void setAltitude(Float altitude) { this.altitude = altitude; }
        
        public Float getSpeed() { return speed; }
        public void setSpeed(Float speed) { this.speed = speed; }
        
        public Integer getSatellites() { return satellites; }
        public void setSatellites(Integer satellites) { this.satellites = satellites; }
    }
} 