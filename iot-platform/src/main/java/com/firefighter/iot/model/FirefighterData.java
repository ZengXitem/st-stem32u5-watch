package com.firefighter.iot.model;

import com.fasterxml.jackson.annotation.JsonProperty;
import javax.persistence.*;
import java.time.LocalDateTime;

@Entity
@Table(name = "firefighter_data")
public class FirefighterData {
    
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    
    @Column(name = "device_id")
    private String deviceId;
    
    @Column(name = "timestamp")
    private LocalDateTime timestamp;
    
    // 生理参数
    @Column(name = "heart_rate")
    private Float heartRate;
    
    @Column(name = "blood_pressure_sys")
    private Float bloodPressureSys;
    
    @Column(name = "blood_pressure_dia")
    private Float bloodPressureDia;
    
    @Column(name = "blood_oxygen")
    private Float bloodOxygen;
    
    @Column(name = "body_temperature")
    private Float bodyTemperature;
    
    // GPS数据
    @Column(name = "latitude")
    private Double latitude;
    
    @Column(name = "longitude")
    private Double longitude;
    
    @Column(name = "altitude")
    private Float altitude;
    
    @Column(name = "speed")
    private Float speed;
    
    @Column(name = "satellites")
    private Integer satellites;
    
    // 系统状态
    @Column(name = "battery_level")
    private Integer batteryLevel;
    
    @Column(name = "status")
    private Integer status;
    
    // 构造函数
    public FirefighterData() {}
    
    // Getters and Setters
    public Long getId() { return id; }
    public void setId(Long id) { this.id = id; }
    
    public String getDeviceId() { return deviceId; }
    public void setDeviceId(String deviceId) { this.deviceId = deviceId; }
    
    public LocalDateTime getTimestamp() { return timestamp; }
    public void setTimestamp(LocalDateTime timestamp) { this.timestamp = timestamp; }
    
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
} 