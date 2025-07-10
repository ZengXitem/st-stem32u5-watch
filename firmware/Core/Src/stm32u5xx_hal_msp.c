#include "main.h"

/**
 * 初始化全局MSP
 */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
}

/**
 * I2C MSP初始化
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if(hi2c->Instance == I2C1)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_I2C1_CLK_ENABLE();
        
        /**I2C1 GPIO配置
        PB8     ------> I2C1_SCL
        PB9     ------> I2C1_SDA
        */
        GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

/**
 * I2C MSP反初始化
 */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
    if(hi2c->Instance == I2C1)
    {
        __HAL_RCC_I2C1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);
    }
}

/**
 * UART MSP初始化
 */
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if(huart->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        /**USART1 GPIO配置
        PA9     ------> USART1_TX
        PA10    ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if(huart->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        /**USART2 GPIO配置
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
 * UART MSP反初始化
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if(huart->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
    }
    else if(huart->Instance == USART2)
    {
        __HAL_RCC_USART2_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
    }
}

/**
 * ADC MSP初始化
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if(hadc->Instance == ADC1)
    {
        __HAL_RCC_ADC12_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        /**ADC1 GPIO配置
        PA1     ------> ADC1_IN2 (体温传感器)
        PA4     ------> ADC1_IN9 (电池电压检测)
        */
        GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/**
 * ADC MSP反初始化
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        __HAL_RCC_ADC12_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_4);
    }
}

/**
 * TIM MSP初始化
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    if(htim->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
        
        /* TIM2 中断配置 */
        HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}

/**
 * TIM MSP反初始化
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim)
{
    if(htim->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_DISABLE();
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
} 