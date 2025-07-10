#include "main.h"

extern UART_HandleTypeDef huart2;
extern uint8_t g_wifi_connected;
extern uint8_t g_mqtt_connected;

static char wifi_rx_buffer[256];
static uint16_t wifi_rx_index = 0;

/* ESP8266初始化 */
HAL_StatusTypeDef ESP8266_Init(void)
{
    HAL_StatusTypeDef status;
    
    /* 硬件复位ESP8266 */
    HAL_GPIO_WritePin(ESP8266_RESET_PORT, ESP8266_RESET_PIN, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(ESP8266_RESET_PORT, ESP8266_RESET_PIN, GPIO_PIN_SET);
    HAL_Delay(2000);
    
    /* 发送AT指令测试连接 */
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT\r\n", 4, 1000);
    HAL_Delay(500);
    
    /* 关闭回显 */
    HAL_UART_Transmit(&huart2, (uint8_t*)"ATE0\r\n", 6, 1000);
    HAL_Delay(500);
    
    /* 设置WiFi模式为Station模式 */
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+CWMODE=1\r\n", 13, 1000);
    HAL_Delay(1000);
    
    return HAL_OK;
}

/* 连接WiFi */
HAL_StatusTypeDef ESP8266_ConnectWiFi(const char *ssid, const char *password)
{
    char cmd_buffer[128];
    
    /* 构建WiFi连接指令 */
    snprintf(cmd_buffer, sizeof(cmd_buffer), "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    
    /* 发送连接指令 */
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd_buffer, strlen(cmd_buffer), 1000);
    
    /* 等待连接完成 */
    HAL_Delay(5000);
    
    /* 检查连接状态 */
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+CWJAP?\r\n", 11, 1000);
    HAL_Delay(1000);
    
    return HAL_OK;
}

/* MQTT连接 */
HAL_StatusTypeDef MQTT_Connect(void)
{
    char cmd_buffer[256];
    
    /* 设置MQTT用户配置 */
    snprintf(cmd_buffer, sizeof(cmd_buffer), 
        "AT+MQTTUSERCFG=0,1,\"firefighter_%s\",\"firefighter\",\"fire123\",0,0,\"\"\r\n", 
        DEVICE_ID);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd_buffer, strlen(cmd_buffer), 1000);
    HAL_Delay(1000);
    
    /* 连接MQTT服务器 */
    snprintf(cmd_buffer, sizeof(cmd_buffer), 
        "AT+MQTTCONN=0,\"%s\",%d,1\r\n", 
        MQTT_BROKER_HOST, MQTT_BROKER_PORT);
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd_buffer, strlen(cmd_buffer), 1000);
    HAL_Delay(3000);
    
    return HAL_OK;
}

/* MQTT发布消息 */
HAL_StatusTypeDef MQTT_Publish(const char *topic, const char *data)
{
    char cmd_buffer[512];
    
    if (!g_mqtt_connected) {
        return HAL_ERROR;
    }
    
    /* 构建MQTT发布指令 */
    snprintf(cmd_buffer, sizeof(cmd_buffer), 
        "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", 
        topic, data);
    
    /* 发送发布指令 */
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd_buffer, strlen(cmd_buffer), 2000);
    HAL_Delay(100);
    
    return HAL_OK;
}

/* MQTT订阅主题 */
HAL_StatusTypeDef MQTT_Subscribe(const char *topic)
{
    char cmd_buffer[256];
    
    if (!g_mqtt_connected) {
        return HAL_ERROR;
    }
    
    /* 构建MQTT订阅指令 */
    snprintf(cmd_buffer, sizeof(cmd_buffer), 
        "AT+MQTTSUB=0,\"%s\",1\r\n", 
        topic);
    
    /* 发送订阅指令 */
    HAL_UART_Transmit(&huart2, (uint8_t*)cmd_buffer, strlen(cmd_buffer), 1000);
    HAL_Delay(500);
    
    return HAL_OK;
}

/* 检查WiFi连接状态 */
HAL_StatusTypeDef ESP8266_CheckConnection(void)
{
    /* 发送状态查询指令 */
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+CWJAP?\r\n", 11, 1000);
    HAL_Delay(500);
    
    /* 这里应该解析返回的状态，简化实现直接返回OK */
    return HAL_OK;
}

/* 检查MQTT连接状态 */
HAL_StatusTypeDef MQTT_CheckConnection(void)
{
    if (!g_wifi_connected) {
        return HAL_ERROR;
    }
    
    /* 发送MQTT状态查询 */
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+MQTTCONN?\r\n", 14, 1000);
    HAL_Delay(500);
    
    return HAL_OK;
}

/* 进入低功耗模式 */
void EnterLowPowerMode(void)
{
    /* 关闭不必要的外设 */
    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ALERT_LED_PORT, ALERT_LED_PIN, GPIO_PIN_RESET);
    
    /* 通知云端设备进入低功耗模式 */
    if (g_mqtt_connected) {
        MQTT_Publish("firefighter/status", "{\"status\":\"SLEEP\"}");
        HAL_Delay(1000);
    }
    
    /* 断开WiFi连接以节约电量 */
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT+CWQAP\r\n", 10, 1000);
    HAL_Delay(500);
    
    /* 进入Stop模式 */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
}

/* 退出低功耗模式 */
void ExitLowPowerMode(void)
{
    /* 重新配置系统时钟 */
    SystemClock_Config();
    
    /* 重新初始化WiFi连接 */
    ESP8266_Init();
    ESP8266_ConnectWiFi("FireStation_WiFi", "fire123456");
    
    /* 重新连接MQTT */
    MQTT_Connect();
    MQTT_Subscribe("firefighter/commands");
    
    /* 通知云端设备已唤醒 */
    MQTT_Publish("firefighter/status", "{\"status\":\"AWAKE\"}");
    
    /* 恢复状态LED */
    HAL_GPIO_WritePin(STATUS_LED_PORT, STATUS_LED_PIN, GPIO_PIN_SET);
} 