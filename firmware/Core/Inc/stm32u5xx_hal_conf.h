#ifndef __STM32U5xx_HAL_CONF_H
#define __STM32U5xx_HAL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* 模块启用定义 */
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED

/* 振荡器配置 */
#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    16000000U  /* 外部高速振荡器频率16MHz */
#endif

#if !defined  (HSE_STARTUP_TIMEOUT)
  #define HSE_STARTUP_TIMEOUT    100U
#endif

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    16000000U  /* 内部高速振荡器频率16MHz */
#endif

#if !defined  (LSE_VALUE)
  #define LSE_VALUE  32768U       /* 外部低速振荡器频率32.768kHz */
#endif

#if !defined  (LSI_VALUE) 
  #define LSI_VALUE  32000U       /* 内部低速振荡器频率32kHz */
#endif

/* 系统配置 */
#define VDD_VALUE    3300U       /* VDD电压3.3V */
#define TICK_INT_PRIORITY    0x0FU  /* SysTick中断优先级 */
#define USE_RTOS     0U
#define PREFETCH_ENABLE    1U
#define INSTRUCTION_CACHE_ENABLE  1U
#define DATA_CACHE_ENABLE  1U

/* 断言宏定义 */
#ifdef  USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif

/* 包含HAL模块头文件 */
#ifdef HAL_RCC_MODULE_ENABLED
  #include "stm32u5xx_hal_rcc.h"
#endif

#ifdef HAL_GPIO_MODULE_ENABLED
  #include "stm32u5xx_hal_gpio.h"
#endif

#ifdef HAL_DMA_MODULE_ENABLED
  #include "stm32u5xx_hal_dma.h"
#endif

#ifdef HAL_CORTEX_MODULE_ENABLED
  #include "stm32u5xx_hal_cortex.h"
#endif

#ifdef HAL_ADC_MODULE_ENABLED
  #include "stm32u5xx_hal_adc.h"
#endif

#ifdef HAL_FLASH_MODULE_ENABLED
  #include "stm32u5xx_hal_flash.h"
#endif

#ifdef HAL_I2C_MODULE_ENABLED
  #include "stm32u5xx_hal_i2c.h"
#endif

#ifdef HAL_PWR_MODULE_ENABLED
  #include "stm32u5xx_hal_pwr.h"
#endif

#ifdef HAL_TIM_MODULE_ENABLED
  #include "stm32u5xx_hal_tim.h"
#endif

#ifdef HAL_UART_MODULE_ENABLED
  #include "stm32u5xx_hal_uart.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __STM32U5xx_HAL_CONF_H */ 