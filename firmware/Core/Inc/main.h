#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32u5xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* 系统配置 */
#define SYSTEM_VERSION "1.0.0"
#define DATA_COLLECTION_INTERVAL 10000  // 10秒采集一次数据
#define MAX_RETRY_COUNT 3
#define MQTT_BROKER_HOST "iot.firefighter.com"
#define MQTT_BROKER_PORT 1883
#define DEVICE_ID "D1920VU212U"

/* 传感器配置 */
#define HEART_RATE_I2C_ADDR 0x57
#define BLOOD_PRESSURE_I2C_ADDR 0x48
#define BLOOD_OXYGEN_I2C_ADDR 0x39
#define GPS_UART_INSTANCE USART1

/* WiFi模块配置 */
#define ESP8266_UART_INSTANCE USART2
#define ESP8266_RESET_PIN GPIO_PIN_0
#define ESP8266_RESET_PORT GPIOA

/* LED指示灯 */
#define STATUS_LED_PIN GPIO_PIN_5
#define STATUS_LED_PORT GPIOA
#define ALERT_LED_PIN GPIO_PIN_6
#define ALERT_LED_PORT GPIOA

/* 数据结构定义 */
typedef struct {
    float heart_rate;        // 心率 (bpm)
    float blood_pressure_sys; // 收缩压 (mmHg)
    float blood_pressure_dia; // 舒张压 (mmHg)
    float blood_oxygen;      // 血氧饱和度 (%)
    float body_temperature;  // 体温 (°C)
    uint32_t timestamp;      // 时间戳
} BiometricData_t;

typedef struct {
    float latitude;          // 纬度
    float longitude;         // 经度
    float altitude;          // 海拔
    float speed;            // 速度 (km/h)
    uint32_t timestamp;      // 时间戳
    uint8_t satellites;      // 卫星数量
} GPSData_t;

typedef struct {
    uint8_t device_id[32];
    BiometricData_t biometric;
    GPSData_t gps;
    uint8_t battery_level;   // 电池电量 (%)
    uint8_t signal_strength; // 信号强度
    uint8_t system_status;   // 系统状态
} FirefighterData_t;

typedef struct {
    uint8_t command_type;    // 指令类型
    uint8_t command_data[64]; // 指令数据
    uint32_t timestamp;      // 时间戳
} RemoteCommand_t;

/* AI预警类型 */
typedef enum {
    ALERT_NONE = 0,
    ALERT_HIGH_HEART_RATE,
    ALERT_LOW_BLOOD_OXYGEN,
    ALERT_HIGH_TEMPERATURE,
    ALERT_EMERGENCY_LOCATION,
    ALERT_DEVICE_MALFUNCTION
} AlertType_t;

/* 函数声明 */
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_I2C1_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_ADC1_Init(void);
void MX_TIM2_Init(void);

/* 传感器函数 */
HAL_StatusTypeDef ReadHeartRate(float *heart_rate);
HAL_StatusTypeDef ReadBloodPressure(float *sys_pressure, float *dia_pressure);
HAL_StatusTypeDef ReadBloodOxygen(float *oxygen_level);
HAL_StatusTypeDef ReadBodyTemperature(float *temperature);
HAL_StatusTypeDef ReadGPSData(GPSData_t *gps_data);

/* WiFi/MQTT函数 */
HAL_StatusTypeDef ESP8266_Init(void);
HAL_StatusTypeDef ESP8266_ConnectWiFi(const char *ssid, const char *password);
HAL_StatusTypeDef MQTT_Connect(void);
HAL_StatusTypeDef MQTT_Publish(const char *topic, const char *data);
HAL_StatusTypeDef MQTT_Subscribe(const char *topic);
void MQTT_MessageHandler(const char *topic, const char *message);

/* AI处理函数 */
AlertType_t AI_AnalyzeData(FirefighterData_t *data);
void AI_ProcessAlert(AlertType_t alert_type);

/* 系统功能函数 */
void CollectSensorData(FirefighterData_t *data);
void SendDataToCloud(FirefighterData_t *data);
void ProcessRemoteCommands(void);
void UpdateSystemStatus(void);
void HandleEmergencyAlert(AlertType_t alert);

/* 电源管理 */
void EnterLowPowerMode(void);
void ExitLowPowerMode(void);
uint8_t GetBatteryLevel(void);

/* 调试函数 */
void Debug_PrintData(FirefighterData_t *data);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */ 