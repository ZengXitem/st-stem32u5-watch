#include "main.h"
#include "stm32u5xx_it.h"

extern TIM_HandleTypeDef htim2;

/**
 * 不可屏蔽中断处理函数
 */
void NMI_Handler(void)
{
    while (1)
    {
    }
}

/**
 * 硬件错误处理函数
 */
void HardFault_Handler(void)
{
    while (1)
    {
    }
}

/**
 * 内存管理错误处理函数
 */
void MemManage_Handler(void)
{
    while (1)
    {
    }
}

/**
 * 总线错误处理函数
 */
void BusFault_Handler(void)
{
    while (1)
    {
    }
}

/**
 * 使用错误处理函数
 */
void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

/**
 * SVCall处理函数
 */
void SVC_Handler(void)
{
}

/**
 * 调试监控处理函数
 */
void DebugMon_Handler(void)
{
}

/**
 * PendSV处理函数
 */
void PendSV_Handler(void)
{
}

/**
 * SysTick处理函数
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 * TIM2中断处理函数
 */
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
} 