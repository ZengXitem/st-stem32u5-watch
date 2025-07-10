#include "stm32u5xx.h"

#if !defined  (HSE_VALUE) 
  #define HSE_VALUE    16000000U  /* 外部高速振荡器频率 */
#endif

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    16000000U  /* 内部高速振荡器频率 */
#endif

/* 系统时钟频率变量 */
uint32_t SystemCoreClock = 4000000U;

const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8] =  {0, 0, 0, 0, 1, 2, 3, 4};
const uint32_t MSIRangeTable[16] = {100000U,  200000U,  400000U,  800000U,
                                    1000000U, 2000000U, 4000000U, 8000000U,
                                    16000000U,24000000U,32000000U,48000000U,
                                    0U,       0U,       0U,       0U};

/**
 * @brief  设置微控制器系统
 *         初始化嵌入式Flash接口，PLL和更新SystemCoreClock变量
 */
void SystemInit(void)
{
    /* 复位RCC时钟配置为默认复位状态(用于调试目的) */
    /* 设置MSION位 */
    RCC->CR |= RCC_CR_MSISON;

    /* 复位CFGR寄存器 */
    RCC->CFGR1 = 0x00000000U;
    RCC->CFGR2 = 0x00000000U;

    /* 复位HSEON, CSSON, HSIKON, HSI48ON位 */
    RCC->CR &= 0xFEF6FFFFU;

    /* 复位PLLCFGR寄存器 */
    RCC->PLLCFGR = 0x00001000U;

    /* 复位HSEBYP位 */
    RCC->CR &= 0xFFFBFFFFU;

    /* 禁用所有中断 */
    RCC->CIER = 0x00000000U;

    /* 更新SystemCoreClock全局变量 */
    SystemCoreClockUpdate();
}

/**
 * @brief  更新SystemCoreClock变量
 */
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0U, msirange = 0U, pllvco = 0U, pllsource = 0U, pllm = 0U, pllr = 0U;

    /* 获取SYSCLK源 */
    switch (RCC->CFGR1 & RCC_CFGR1_SWS)
    {
        case 0x00:  /* MSI用作系统时钟源 */
            msirange = (RCC->ICSCR1 & RCC_ICSCR1_MSISRANGE) >> RCC_ICSCR1_MSISRANGE_Pos;
            SystemCoreClock = MSIRangeTable[msirange];
            break;

        case 0x04:  /* HSI用作系统时钟源 */
            SystemCoreClock = HSI_VALUE;
            break;

        case 0x08:  /* HSE用作系统时钟源 */
            SystemCoreClock = HSE_VALUE;
            break;

        case 0x0C:  /* PLL用作系统时钟源 */
            /* PLL_VCO = (HSE_VALUE or HSI_VALUE or MSI_VALUE/ PLLM) * PLLN
               SYSCLK = PLL_VCO / PLLR
               */
            pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC);
            pllm = ((RCC->PLLCFGR & RCC_PLLCFGR_PLLM) >> RCC_PLLCFGR_PLLM_Pos) + 1U;

            switch (pllsource)
            {
                case 0x00:  /* 无时钟发送到PLL */
                    SystemCoreClock = 0U;
                    break;

                case 0x02:  /* MSI用作PLL时钟源 */
                    msirange = (RCC->ICSCR1 & RCC_ICSCR1_MSISRANGE) >> RCC_ICSCR1_MSISRANGE_Pos;
                    pllvco = (MSIRangeTable[msirange] / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos);
                    break;

                case 0x03:  /* HSI用作PLL时钟源 */
                    pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos);
                    break;

                case 0x04:  /* HSE用作PLL时钟源 */
                    pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> RCC_PLLCFGR_PLLN_Pos);
                    break;

                default:
                    SystemCoreClock = 0U;
                    break;
            }

            pllr = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >> RCC_PLLCFGR_PLLR_Pos) + 1U);
            SystemCoreClock = pllvco / pllr;
            break;

        default:
            SystemCoreClock = MSIRangeTable[6];  /* 4MHz */
            break;
    }

    /* 计算HCLK频率 */
    /* 获取HCLK预分频器 */
    tmp = AHBPrescTable[((RCC->CFGR2 & RCC_CFGR2_HPRE) >> RCC_CFGR2_HPRE_Pos)];
    /* HCLK频率 */
    SystemCoreClock >>= tmp;
}

/**
 * @brief  配置系统时钟频率、HCLK、PCLK1和PCLK2预分频器
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* 配置主内部调节器输出电压 */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
    {
        Error_Handler();
    }

    /* 初始化RCC振荡器根据RCC_OscInitStruct中指定的参数 */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;  /* 48MHz */
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 3;
    RCC_OscInitStruct.PLL.PLLN = 10;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* 初始化CPU、AHB和APB总线时钟 */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                |RCC_CLOCKTYPE_PCLK3;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
} 