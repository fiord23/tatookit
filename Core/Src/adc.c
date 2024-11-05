/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "adc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;

/* ADC1 init function */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
    PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
    PeriphClkInit.PLLSAI1.PLLSAI1N = 16;
    PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
    PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
    PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* ADC1 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA6     ------> ADC1_IN11
    PB0     ------> ADC1_IN15
    */
    GPIO_InitStruct.Pin = IPROPI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(IPROPI_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = POWER_ADC_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(POWER_ADC_GPIO_Port, &GPIO_InitStruct);

    /* ADC1 interrupt Init */
    ADC1->IER |= ADC_IER_EOCIE;
    HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA6     ------> ADC1_IN11
    PB0     ------> ADC1_IN15
    */
    HAL_GPIO_DeInit(IPROPI_GPIO_Port, IPROPI_Pin);

    HAL_GPIO_DeInit(POWER_ADC_GPIO_Port, POWER_ADC_Pin);

    /* ADC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC1_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void adc_init (void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN; 
    RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1;
    // Ждем, пока ADC1 будет готов

    ADC1->CR &= ~ADC_CR_ADEN; // Отключаем ADC
    while (ADC1->CR & ADC_CR_ADEN); // Ждем, пока ADC отключится

    ADC1_COMMON->CCR |= 0xB << 18; //ADC Prescaler
    // Калибровка ADC
   // ADC1->CR |= ADC_CR_ADCAL; // Запускаем калибровку
    //while (ADC1->CR & ADC_CR_ADCAL); // Ждем завершения калибровки
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    // Настройка разрешения и режима работы
    ADC1->CFGR |= ADC_CFGR_CONT; // Режим непрерывного преобразования
    ADC1->SMPR2 |= 7 << 15; // 640.5 ADC clock cycles
  // Настройка канала
    ADC1->SQR1 &= ~ADC_SQR1_L; // Устанавливаем количество последовательных преобразований в 1
    ADC1->SQR1 |= (15 << ADC_SQR1_SQ1_Pos); // Устанавливаем 15 канал в SQ1

    // Включаем прерывания
    ADC1->IER |= ADC_IER_EOCIE; //enable interrupt bit
    //NVIC_EnableIRQ(ADC1_IRQn); // Разрешаем прерывание для ADC1
    NVIC_EnableIRQ(ADC1_IRQn); 
    ADC1->ISR |= ADC_ISR_ADRDY;
   // ADC1->CR &= ~ADC_CR_ADDIS; // Отключаем ADC, если он был включен
    ADC1->CR &= ~ADC_CR_DEEPPWD;
    ADC1->CFGR |= ADC_CFGR_JQDIS;

    // Включаем ADC
//Enable ADC
  ADC1->ISR |= ADC_ISR_ADRDY;  //Clear the ADRDY bit in the ADC_ISR register by writing ‘1’
  ADC1->CR |= ADC_CR_ADEN; //Enable ADC
  while(ADC1->ISR & ADC_ISR_ADRDY)
  ;
  ADC1->ISR |= ADC_ISR_ADRDY;
  ADC1->CR |= ADC_CR_ADVREGEN;
  HAL_Delay(10);
  ADC1->CR |= ADC_CR_ADSTART;
  HAL_Delay(10);
}


/* USER CODE END 1 */
