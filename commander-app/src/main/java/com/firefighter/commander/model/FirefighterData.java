package com.firefighter.commander.model;

import java.sql.Timestamp;

public class FirefighterData {
    private Long id;
    private String deviceId;
    private Timestamp timestamp;
    
    // 生理参数
    private Float heartRate;
    private Float bloodPressureSys;
    private Float bloodPressureDia;
    private Float bloodOxygen;
    private Float bodyTemperature;
    
    // GPS数据
    private Double latitude;
    private Double longitude;
    private Float altitude;
    private Float speed;
    private Integer satellites;
    
    // 系统状态
    private Integer batteryLevel;
    private Integer status;
    
    // 构造函数
    public FirefighterData() {}
    
    // Getters and Setters
    public Long getId() { return id; }
    public void setId(Long id) { this.id = id; }
    
    public String getDeviceId() { return deviceId; }
    public void setDeviceId(String deviceId) { this.deviceId = deviceId; }
    
    public Timestamp getTimestamp() { return timestamp; }
    public void setTimestamp(Timestamp timestamp) { this.timestamp = timestamp; }
    
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
    
    public Integer getBatteryLevel() { return batteryLevel; }
    public void setBatteryLevel(Integer batteryLevel) { this.batteryLevel = batteryLevel; }
    
    public Integer getStatus() { return status; }
    public void setStatus(Integer status) { this.status = status; }
    
    @Override
    public String toString() {
        return "FirefighterData{" +
                "deviceId='" + deviceId + '\'' +
                ", heartRate=" + heartRate +
                ", bloodOxygen=" + bloodOxygen +
                ", bodyTemperature=" + bodyTemperature +
                ", batteryLevel=" + batteryLevel +
                '}';
    }
} 