#include "main.h"
#include "stm32u5xx_hal.h"

/* 全局变量 */
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;  // GPS
UART_HandleTypeDef huart2;  // ESP8266
ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;

FirefighterData_t g_firefighter_data;
uint8_t g_data_collection_flag = 0;
uint8_t g_wifi_connected = 0;
uint8_t g_mqtt_connected = 0;

/* 主函数 */
int main(void)
{
    /* 初始化HAL库 */
    HAL_Init();
    
    /* 配置系统时钟 */
    SystemClock_Config();
    
    /* 初始化GPIO */
    MX_GPIO_Init();
    
    /* 初始化外设 */
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_ADC1_Init();
    MX_TIM2_Init();
    
    /* 设备ID初始化 */
    strcpy((char*)g_firefighter_data.device_id, DEVICE_ID);
    
    /* 启动定时器 */
    HAL_TIM_Base_Start_IT(&htim2);
    
    /* 初始化WiFi模块 */
    if (ESP8266_Init() == HAL_OK) {
        /* 连接WiFi */
        if (ESP8266_ConnectWiFi("FireStation_WiFi", "fire123456") == HAL_OK) {
            g_wifi_connected = 1;
            HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_SET);
            
            /* 连接MQTT */
            if (MQTT_Connect() == HAL_OK) {
                g_mqtt_connected = 1;
                MQTT_Subscribe("firefighter/commands");
            }
        }
    }
    
    /* 主循环 */
    while (1) {
        /* 数据采集 */
        if (g_data_collection_flag) {
            g_data_collection_flag = 0;
            
            /* 收集传感器数据 */
            CollectSensorData(&g_firefighter_data);
            
            /* AI分析数据 */
            AlertType_t alert = AI_AnalyzeData(&g_firefighter_data);
            if (alert != ALERT_NONE) {
                HandleEmergencyAlert(alert);
            }
            
            /* 发送数据到云端 */
            if (g_mqtt_connected) {
                SendDataToCloud(&g_firefighter_data);
            }
            
            /* 调试输出 */
            Debug_PrintData(&g_firefighter_data);
        }
        
        /* 处理远程指令 */
        ProcessRemoteCommands();
        
        /* 更新系统状态 */
        UpdateSystemStatus();
        
        /* 低功耗延时 */
        HAL_Delay(100);
    }
}

/* 系统时钟配置 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* 配置主振荡器 */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 10;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* 配置CPU、AHB和APB总线时钟 */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}

/* 收集传感器数据 */
void CollectSensorData(FirefighterData_t *data)
{
    /* 读取生理参数 */
    ReadHeartRate(&data->biometric.heart_rate);
    ReadBloodPressure(&data->biometric.blood_pressure_sys, &data->biometric.blood_pressure_dia);
    ReadBloodOxygen(&data->biometric.blood_oxygen);
    ReadBodyTemperature(&data->biometric.body_temperature);
    
    /* 读取GPS数据 */
    ReadGPSData(&data->gps);
    
    /* 读取系统状态 */
    data->battery_level = GetBatteryLevel();
    data->biometric.timestamp = HAL_GetTick();
    data->gps.timestamp = HAL_GetTick();
}

/* 发送数据到云端 */
void SendDataToCloud(FirefighterData_t *data)
{
    char json_buffer[512];
    
    /* 构建JSON数据 */
    snprintf(json_buffer, sizeof(json_buffer),
        "{"
        "\"deviceId\":\"%s\","
        "\"timestamp\":%lu,"
        "\"biometric\":{"
            "\"heartRate\":%.1f,"
            "\"bloodPressureSys\":%.1f,"
            "\"bloodPressureDia\":%.1f,"
            "\"bloodOxygen\":%.1f,"
            "\"bodyTemperature\":%.1f"
        "},"
        "\"gps\":{"
            "\"latitude\":%.6f,"
            "\"longitude\":%.6f,"
            "\"altitude\":%.1f,"
            "\"speed\":%.1f,"
            "\"satellites\":%d"
        "},"
        "\"battery\":%d,"
        "\"status\":%d"
        "}",
        data->device_id,
        data->biometric.timestamp,
        data->biometric.heart_rate,
        data->biometric.blood_pressure_sys,
        data->biometric.blood_pressure_dia,
        data->biometric.blood_oxygen,
        data->biometric.body_temperature,
        data->gps.latitude,
        data->gps.longitude,
        data->gps.altitude,
        data->gps.speed,
        data->gps.satellites,
        data->battery_level,
        data->system_status
    );
    
    /* 发布到MQTT */
    MQTT_Publish("firefighter/data", json_buffer);
}

/* AI数据分析 */
AlertType_t AI_AnalyzeData(FirefighterData_t *data)
{
    /* 心率异常检测 */
    if (data->biometric.heart_rate > 160.0f) {
        return ALERT_HIGH_HEART_RATE;
    }
    
    /* 血氧异常检测 */
    if (data->biometric.blood_oxygen < 90.0f) {
        return ALERT_LOW_BLOOD_OXYGEN;
    }
    
    /* 体温异常检测 */
    if (data->biometric.body_temperature > 39.0f) {
        return ALERT_HIGH_TEMPERATURE;
    }
    
    /* 位置异常检测（示例：检查是否在危险区域） */
    // 这里可以集成更复杂的AI算法
    
    return ALERT_NONE;
}

/* 处理紧急警报 */
void HandleEmergencyAlert(AlertType_t alert)
{
    char alert_message[256];
    
    /* 点亮警报LED */
    HAL_GPIO_WritePin(ALERT_LED_PORT, ALERT_LED_PIN, GPIO_PIN_SET);
    
    /* 构建警报消息 */
    switch (alert) {
        case ALERT_HIGH_HEART_RATE:
            strcpy(alert_message, "{\"alertType\":\"HIGH_HEART_RATE\",\"severity\":\"HIGH\"}");
            break;
        case ALERT_LOW_BLOOD_OXYGEN:
            strcpy(alert_message, "{\"alertType\":\"LOW_BLOOD_OXYGEN\",\"severity\":\"CRITICAL\"}");
            break;
        case ALERT_HIGH_TEMPERATURE:
            strcpy(alert_message, "{\"alertType\":\"HIGH_TEMPERATURE\",\"severity\":\"HIGH\"}");
            break;
        default:
            strcpy(alert_message, "{\"alertType\":\"UNKNOWN\",\"severity\":\"MEDIUM\"}");
            break;
    }
    
    /* 发送警报到云端 */
    if (g_mqtt_connected) {
        MQTT_Publish("firefighter/alerts", alert_message);
    }
}

/* 定时器中断回调 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        /* 设置数据采集标志 */
        g_data_collection_flag = 1;
    }
}

/* MQTT消息处理 */
void MQTT_MessageHandler(const char *topic, const char *message)
{
    if (strcmp(topic, "firefighter/commands") == 0) {
        /* 处理远程指令 */
        if (strstr(message, "EVACUATE")) {
            /* 撤退指令 */
            HAL_GPIO_WritePin(ALERT_LED_PORT, ALERT_LED_PIN, GPIO_PIN_SET);
            // 触发蜂鸣器、振动等提醒
        }
        else if (strstr(message, "STATUS_REQUEST")) {
            /* 状态查询指令 */
            SendDataToCloud(&g_firefighter_data);
        }
        else if (strstr(message, "SHUTDOWN")) {
            /* 关机指令 */
            EnterLowPowerMode();
        }
    }
}

/* 处理远程指令 */
void ProcessRemoteCommands(void)
{
    // 检查是否有新的MQTT消息
    // 这个函数会在MQTT回调中处理指令
}

/* 更新系统状态 */
void UpdateSystemStatus(void)
{
    /* 更新LED状态 */
    if (g_wifi_connected && g_mqtt_connected) {
        HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_TogglePin(STATUS_LED_PORT, STATUS_LED_PIN);
    }
    
    /* 检查电池电量 */
    if (g_firefighter_data.battery_level < 20) {
        HAL_GPIO_TogglePin(ALERT_LED_PORT, ALERT_LED_PIN);
    }
}

/* 进入低功耗模式 */
void EnterLowPowerMode(void)
{
    /* 关闭不必要的外设 */
    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ALERT_LED_PORT, ALERT_LED_PIN, GPIO_PIN_RESET);
    
    /* 进入停止模式 */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    
    /* 唤醒后重新配置时钟 */
    SystemClock_Config();
}

/* 调试输出 */
void Debug_PrintData(FirefighterData_t *data)
{
    printf("=== 消防员数据 ===\n");
    printf("心率: %.1f bpm\n", data->biometric.heart_rate);
    printf("血压: %.1f/%.1f mmHg\n", data->biometric.blood_pressure_sys, data->biometric.blood_pressure_dia);
    printf("血氧: %.1f%%\n", data->biometric.blood_oxygen);
    printf("体温: %.1f°C\n", data->biometric.body_temperature);
    printf("位置: %.6f, %.6f\n", data->gps.latitude, data->gps.longitude);
    printf("电池: %d%%\n", data->battery_level);
    printf("==================\n");
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
        /* 错误指示LED闪烁 */
        HAL_GPIO_TogglePin(ALERT_LED_PORT, ALERT_LED_PIN);
        HAL_Delay(200);
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */ 