#include "main.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
extern ADC_HandleTypeDef hadc1;

/* 心率传感器读取 */
HAL_StatusTypeDef ReadHeartRate(float *heart_rate)
{
    uint8_t reg_addr = 0x02;  // 心率数据寄存器
    uint8_t data[2];
    HAL_StatusTypeDef status;
    
    /* 读取心率传感器数据 */
    status = HAL_I2C_Mem_Read(&hi2c1, HEART_RATE_I2C_ADDR << 1, reg_addr, 1, data, 2, HAL_MAX_DELAY);
    
    if (status == HAL_OK) {
        /* 将原始数据转换为心率值 */
        uint16_t raw_value = (data[0] << 8) | data[1];
        *heart_rate = (float)raw_value * 0.1f;  // 转换系数根据传感器规格调整
        
        /* 数据合理性检查 */
        if (*heart_rate < 40.0f || *heart_rate > 200.0f) {
            *heart_rate = 75.0f;  // 默认值
        }
    } else {
        *heart_rate = 75.0f;  // 读取失败时的默认值
    }
    
    return status;
}

/* 血压传感器读取 */
HAL_StatusTypeDef ReadBloodPressure(float *sys_pressure, float *dia_pressure)
{
    uint8_t reg_addr_sys = 0x10;  // 收缩压寄存器
    uint8_t reg_addr_dia = 0x12;  // 舒张压寄存器
    uint8_t data[2];
    HAL_StatusTypeDef status;
    
    /* 读取收缩压 */
    status = HAL_I2C_Mem_Read(&hi2c1, BLOOD_PRESSURE_I2C_ADDR << 1, reg_addr_sys, 1, data, 2, HAL_MAX_DELAY);
    if (status == HAL_OK) {
        uint16_t raw_sys = (data[0] << 8) | data[1];
        *sys_pressure = (float)raw_sys * 0.01f + 50.0f;  // 转换为mmHg
    } else {
        *sys_pressure = 120.0f;  // 默认值
    }
    
    /* 读取舒张压 */
    status = HAL_I2C_Mem_Read(&hi2c1, BLOOD_PRESSURE_I2C_ADDR << 1, reg_addr_dia, 1, data, 2, HAL_MAX_DELAY);
    if (status == HAL_OK) {
        uint16_t raw_dia = (data[0] << 8) | data[1];
        *dia_pressure = (float)raw_dia * 0.01f + 30.0f;  // 转换为mmHg
    } else {
        *dia_pressure = 80.0f;  // 默认值
    }
    
    /* 数据合理性检查 */
    if (*sys_pressure < 80.0f || *sys_pressure > 200.0f) {
        *sys_pressure = 120.0f;
    }
    if (*dia_pressure < 50.0f || *dia_pressure > 120.0f) {
        *dia_pressure = 80.0f;
    }
    
    return status;
}

/* 血氧饱和度传感器读取 */
HAL_StatusTypeDef ReadBloodOxygen(float *oxygen_level)
{
    uint8_t reg_addr = 0x05;  // 血氧数据寄存器
    uint8_t data[2];
    HAL_StatusTypeDef status;
    
    /* 读取血氧传感器数据 */
    status = HAL_I2C_Mem_Read(&hi2c1, BLOOD_OXYGEN_I2C_ADDR << 1, reg_addr, 1, data, 2, HAL_MAX_DELAY);
    
    if (status == HAL_OK) {
        /* 将原始数据转换为血氧饱和度 */
        uint16_t raw_value = (data[0] << 8) | data[1];
        *oxygen_level = (float)raw_value * 0.01f;  // 转换为百分比
        
        /* 数据合理性检查 */
        if (*oxygen_level < 70.0f || *oxygen_level > 100.0f) {
            *oxygen_level = 98.0f;  // 默认值
        }
    } else {
        *oxygen_level = 98.0f;  // 读取失败时的默认值
    }
    
    return status;
}

/* 体温传感器读取（通过ADC） */
HAL_StatusTypeDef ReadBodyTemperature(float *temperature)
{
    uint32_t adc_value;
    HAL_StatusTypeDef status;
    
    /* 启动ADC转换 */
    status = HAL_ADC_Start(&hadc1);
    if (status != HAL_OK) {
        *temperature = 36.5f;  // 默认体温
        return status;
    }
    
    /* 等待转换完成 */
    if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
        adc_value = HAL_ADC_GetValue(&hadc1);
        
        /* 将ADC值转换为温度 */
        float voltage = (float)adc_value * 3.3f / 4095.0f;  // 转换为电压
        *temperature = voltage * 10.0f + 30.0f;  // 根据传感器特性转换
        
        /* 数据合理性检查 */
        if (*temperature < 35.0f || *temperature > 42.0f) {
            *temperature = 36.5f;  // 默认值
        }
    } else {
        *temperature = 36.5f;  // 转换失败时的默认值
        status = HAL_ERROR;
    }
    
    HAL_ADC_Stop(&hadc1);
    return status;
}

/* GPS数据读取 */
HAL_StatusTypeDef ReadGPSData(GPSData_t *gps_data)
{
    uint8_t gps_buffer[128];
    uint16_t bytes_received = 0;
    HAL_StatusTypeDef status;
    
    /* 从GPS模块读取NMEA数据 */
    status = HAL_UART_Receive(&huart1, gps_buffer, sizeof(gps_buffer), 1000);
    
    if (status == HAL_OK) {
        /* 解析NMEA格式的GPS数据 */
        char *gprmc_start = strstr((char*)gps_buffer, "$GPRMC");
        
        if (gprmc_start != NULL) {
            /* 简化的GPRMC解析 */
            char *token = strtok(gprmc_start, ",");
            int field = 0;
            
            while (token != NULL && field < 12) {
                switch (field) {
                    case 3:  // 纬度
                        if (strlen(token) > 0) {
                            gps_data->latitude = atof(token) / 100.0f;  // 简化转换
                        }
                        break;
                    case 5:  // 经度
                        if (strlen(token) > 0) {
                            gps_data->longitude = atof(token) / 100.0f;  // 简化转换
                        }
                        break;
                    case 7:  // 速度
                        if (strlen(token) > 0) {
                            gps_data->speed = atof(token) * 1.852f;  // 节转km/h
                        }
                        break;
                }
                token = strtok(NULL, ",");
                field++;
            }
            
            /* 设置其他默认值 */
            gps_data->altitude = 100.0f;  // 默认海拔
            gps_data->satellites = 8;     // 默认卫星数
            gps_data->timestamp = HAL_GetTick();
        } else {
            /* 没有有效的GPS数据，使用默认值 */
            gps_data->latitude = 23.129110f;   // 广州默认坐标
            gps_data->longitude = 113.264385f;
            gps_data->altitude = 50.0f;
            gps_data->speed = 0.0f;
            gps_data->satellites = 0;
            gps_data->timestamp = HAL_GetTick();
            status = HAL_ERROR;
        }
    } else {
        /* GPS读取失败，使用默认坐标 */
        gps_data->latitude = 23.129110f;   // 广州默认坐标
        gps_data->longitude = 113.264385f;
        gps_data->altitude = 50.0f;
        gps_data->speed = 0.0f;
        gps_data->satellites = 0;
        gps_data->timestamp = HAL_GetTick();
    }
    
    return status;
}

/* 获取电池电量 */
uint8_t GetBatteryLevel(void)
{
    uint32_t adc_value;
    uint8_t battery_level;
    
    /* 启动ADC转换读取电池电压 */
    if (HAL_ADC_Start(&hadc1) == HAL_OK) {
        if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {
            adc_value = HAL_ADC_GetValue(&hadc1);
            
            /* 将ADC值转换为电池电量百分比 */
            float voltage = (float)adc_value * 3.3f / 4095.0f;
            float battery_voltage = voltage * 2.0f;  // 电压分压电路
            
            /* 锂电池电压范围 3.0V-4.2V */
            if (battery_voltage >= 4.0f) {
                battery_level = 100;
            } else if (battery_voltage >= 3.8f) {
                battery_level = 75;
            } else if (battery_voltage >= 3.6f) {
                battery_level = 50;
            } else if (battery_voltage >= 3.4f) {
                battery_level = 25;
            } else {
                battery_level = 10;
            }
        } else {
            battery_level = 50;  // 默认值
        }
        HAL_ADC_Stop(&hadc1);
    } else {
        battery_level = 50;  // 默认值
    }
    
    return battery_level;
}

/* 获取信号强度 */
uint8_t GetSignalStrength(void)
{
    /* 简化实现，返回固定值 */
    /* 实际应用中应该通过AT+CWLAP或类似指令获取WiFi信号强度 */
    return 75;  // 返回75%的信号强度
}

/* 传感器校准函数 */
HAL_StatusTypeDef CalibrateSensors(void)
{
    /* 心率传感器校准 */
    uint8_t calibration_cmd[] = {0x06, 0x01};  // 校准指令
    HAL_I2C_Master_Transmit(&hi2c1, HEART_RATE_I2C_ADDR << 1, calibration_cmd, 2, 1000);
    HAL_Delay(100);
    
    /* 血氧传感器校准 */
    calibration_cmd[0] = 0x08;
    HAL_I2C_Master_Transmit(&hi2c1, BLOOD_OXYGEN_I2C_ADDR << 1, calibration_cmd, 2, 1000);
    HAL_Delay(100);
    
    /* 血压传感器校准 */
    calibration_cmd[0] = 0x0A;
    HAL_I2C_Master_Transmit(&hi2c1, BLOOD_PRESSURE_I2C_ADDR << 1, calibration_cmd, 2, 1000);
    HAL_Delay(100);
    
    return HAL_OK;
} 