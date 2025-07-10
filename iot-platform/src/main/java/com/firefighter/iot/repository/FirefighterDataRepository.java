package com.firefighter.iot.repository;

import com.firefighter.iot.model.FirefighterData;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.query.Param;
import org.springframework.stereotype.Repository;

import java.time.LocalDateTime;
import java.util.List;
import java.util.Optional;

@Repository
public interface FirefighterDataRepository extends JpaRepository<FirefighterData, Long> {
    
    /**
     * 根据设备ID查找最新数据
     */
    Optional<FirefighterData> findTopByDeviceIdOrderByTimestampDesc(String deviceId);
    
    /**
     * 查找指定设备在指定时间范围内的数据
     */
    List<FirefighterData> findByDeviceIdAndTimestampBetweenOrderByTimestampDesc(
            String deviceId, LocalDateTime startTime, LocalDateTime endTime);
    
    /**
     * 查找所有设备的最新数据
     */
    @Query("SELECT f FROM FirefighterData f WHERE f.timestamp = " +
           "(SELECT MAX(f2.timestamp) FROM FirefighterData f2 WHERE f2.deviceId = f.deviceId)")
    List<FirefighterData> findLatestDataForAllDevices();
    
    /**
     * 查找在线设备（最近5分钟内有数据的设备）
     */
    @Query("SELECT DISTINCT f.deviceId FROM FirefighterData f WHERE f.timestamp > :cutoffTime")
    List<String> findOnlineDevices(@Param("cutoffTime") LocalDateTime cutoffTime);
    
    /**
     * 删除指定时间之前的历史数据
     */
    void deleteByTimestampBefore(LocalDateTime cutoffTime);
    
    /**
     * 统计总设备数
     */
    @Query("SELECT COUNT(DISTINCT f.deviceId) FROM FirefighterData f")
    Long countDistinctDevices();
    
    /**
     * 查找异常数据（心率>160或血氧<90）
     */
    @Query("SELECT f FROM FirefighterData f WHERE f.heartRate > 160 OR f.bloodOxygen < 90 " +
           "ORDER BY f.timestamp DESC")
    List<FirefighterData> findAbnormalData();
} 