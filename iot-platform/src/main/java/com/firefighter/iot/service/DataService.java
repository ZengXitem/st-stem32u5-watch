package com.firefighter.iot.service;

import com.firefighter.iot.model.FirefighterData;
import com.firefighter.iot.repository.FirefighterDataRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
@Transactional
public class DataService {
    
    @Autowired
    private FirefighterDataRepository firefighterDataRepository;
    
    /**
     * 保存消防员数据
     */
    public FirefighterData saveFirefighterData(FirefighterData data) {
        return firefighterDataRepository.save(data);
    }
    
    /**
     * 获取所有设备的最新数据
     */
    public List<FirefighterData> getLatestData() {
        return firefighterDataRepository.findLatestDataForAllDevices();
    }
    
    /**
     * 获取指定设备的最新数据
     */
    public FirefighterData getLatestDataByDevice(String deviceId) {
        return firefighterDataRepository.findTopByDeviceIdOrderByTimestampDesc(deviceId)
                .orElse(null);
    }
    
    /**
     * 获取指定设备的历史数据
     */
    public List<FirefighterData> getHistoryData(String deviceId, int hours) {
        LocalDateTime endTime = LocalDateTime.now();
        LocalDateTime startTime = endTime.minusHours(hours);
        return firefighterDataRepository.findByDeviceIdAndTimestampBetweenOrderByTimestampDesc(
                deviceId, startTime, endTime);
    }
    
    /**
     * 获取在线设备列表
     */
    public List<String> getOnlineDevices() {
        LocalDateTime cutoffTime = LocalDateTime.now().minusMinutes(5);
        return firefighterDataRepository.findOnlineDevices(cutoffTime);
    }
    
    /**
     * 获取系统统计信息
     */
    public Map<String, Object> getSystemStats() {
        Map<String, Object> stats = new HashMap<>();
        
        // 总设备数
        Long totalDevices = firefighterDataRepository.countDistinctDevices();
        stats.put("totalDevices", totalDevices);
        
        // 在线设备数
        List<String> onlineDevices = getOnlineDevices();
        stats.put("onlineDevices", onlineDevices.size());
        
        // 异常数据数量
        List<FirefighterData> abnormalData = firefighterDataRepository.findAbnormalData();
        stats.put("alertCount", abnormalData.size());
        
        // 数据更新时间
        stats.put("lastUpdate", LocalDateTime.now());
        
        return stats;
    }
    
    /**
     * 获取异常数据
     */
    public List<FirefighterData> getAbnormalData() {
        return firefighterDataRepository.findAbnormalData();
    }
    
    /**
     * 定时清理历史数据（每天凌晨执行）
     */
    @Scheduled(cron = "0 0 2 * * ?")
    public void cleanupOldData() {
        LocalDateTime cutoffTime = LocalDateTime.now().minusDays(30);
        firefighterDataRepository.deleteByTimestampBefore(cutoffTime);
        System.out.println("已清理30天前的历史数据，截止时间：" + cutoffTime);
    }
} 