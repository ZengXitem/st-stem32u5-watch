package com.firefighter.iot.controller;

import com.firefighter.iot.model.FirefighterData;
import com.firefighter.iot.service.DataService;
import com.firefighter.iot.service.MqttService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/firefighter")
@CrossOrigin(origins = "*")
public class FirefighterController {
    
    @Autowired
    private DataService dataService;
    
    @Autowired
    private MqttService mqttService;
    
    /**
     * 获取所有消防员的最新数据
     */
    @GetMapping("/data/latest")
    public ResponseEntity<List<FirefighterData>> getLatestData() {
        List<FirefighterData> data = dataService.getLatestData();
        return ResponseEntity.ok(data);
    }
    
    /**
     * 获取指定设备的最新数据
     */
    @GetMapping("/data/latest/{deviceId}")
    public ResponseEntity<FirefighterData> getLatestDataByDevice(@PathVariable String deviceId) {
        FirefighterData data = dataService.getLatestDataByDevice(deviceId);
        if (data != null) {
            return ResponseEntity.ok(data);
        }
        return ResponseEntity.notFound().build();
    }
    
    /**
     * 获取指定设备的历史数据
     */
    @GetMapping("/data/history/{deviceId}")
    public ResponseEntity<List<FirefighterData>> getHistoryData(
            @PathVariable String deviceId,
            @RequestParam(defaultValue = "24") int hours) {
        List<FirefighterData> data = dataService.getHistoryData(deviceId, hours);
        return ResponseEntity.ok(data);
    }
    
    /**
     * 发送撤退指令
     */
    @PostMapping("/command/evacuate/{deviceId}")
    public ResponseEntity<String> sendEvacuateCommand(@PathVariable String deviceId) {
        String command = "{\"command\":\"EVACUATE\",\"message\":\"立即撤退到安全区域\"}";
        mqttService.sendCommand(deviceId, command);
        return ResponseEntity.ok("撤退指令已发送");
    }
    
    /**
     * 发送状态查询指令
     */
    @PostMapping("/command/status/{deviceId}")
    public ResponseEntity<String> sendStatusRequest(@PathVariable String deviceId) {
        String command = "{\"command\":\"STATUS_REQUEST\"}";
        mqttService.sendCommand(deviceId, command);
        return ResponseEntity.ok("状态查询指令已发送");
    }
    
    /**
     * 发送自定义指令
     */
    @PostMapping("/command/custom/{deviceId}")
    public ResponseEntity<String> sendCustomCommand(
            @PathVariable String deviceId,
            @RequestBody Map<String, Object> commandData) {
        try {
            String command = new com.fasterxml.jackson.databind.ObjectMapper()
                    .writeValueAsString(commandData);
            mqttService.sendCommand(deviceId, command);
            return ResponseEntity.ok("自定义指令已发送");
        } catch (Exception e) {
            return ResponseEntity.badRequest().body("指令格式错误: " + e.getMessage());
        }
    }
    
    /**
     * 获取在线设备列表
     */
    @GetMapping("/devices/online")
    public ResponseEntity<List<String>> getOnlineDevices() {
        List<String> devices = dataService.getOnlineDevices();
        return ResponseEntity.ok(devices);
    }
    
    /**
     * 获取设备统计信息
     */
    @GetMapping("/stats")
    public ResponseEntity<Map<String, Object>> getSystemStats() {
        Map<String, Object> stats = dataService.getSystemStats();
        return ResponseEntity.ok(stats);
    }
} 