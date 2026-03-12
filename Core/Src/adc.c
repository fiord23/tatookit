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
#include "stm32l431xx.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ADC_HandleTypeDef hadc1;
extern volatile uint16_t ADC_Data[];
/* ADC1 init function */
/* USER CODE BEGIN 1 */

void adc_init(void)
/*
PA1 IN6 VBAT 10k - 10k
PA6 IN11 MOTOR ... - 5.1k
PB0 IN15 POWER 180k - 39k
*/

{
  RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
  RCC->CCIPR |= RCC_CCIPR_ADCSEL_0 | RCC_CCIPR_ADCSEL_1; // CLOCK FROM SYSCLK

  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
  DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);
  DMA1_Channel1->CMAR = (uint32_t)ADC_Data;
  DMA1_Channel1->CNDTR = 3;
  DMA1_Channel1->CCR &= ~DMA_CCR_DIR;                        // Direction from peripheral to memory
  DMA1_Channel1->CCR |= DMA_CCR_CIRC;                        // circular mode
  DMA1_Channel1->CCR |= (DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0); // Memory and peripheral 16 bits
  DMA1_Channel1->CCR |= DMA_CCR_TCIE;                        // interrupt transmission complete
  DMA1_Channel1->CCR |= DMA_CCR_PL;
  DMA1_Channel1->CCR |= DMA_CCR_MINC; // memory increment
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  DMA1_Channel1->CCR |= DMA_CCR_EN;

  ADC1->CR &= ~ADC_CR_ADEN; // ADC off
  while (ADC1->CR & ADC_CR_ADEN)
    ;                            //
  ADC1_COMMON->CCR |= 0xB << 18; // ADC Prescaler
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  // Настройка разрешения и режима работы
  ADC1->CFGR |= ADC_CFGR_CONT | ADC_CFGR_DMAEN | ADC_CFGR_DMACFG;                          // Continious mode, DMA ON, DMA Circular mode
  ADC1->SMPR2 |= 7 << 15;                                                                  // 640.5 ADC clock cycles
  ADC1->SQR1 |= 2 << ADC_SQR1_L_Pos;                                                       // 3 conversions: VBAT ADC, POWER, MOTOR CURRENT
  ADC1->SQR1 |= (6 << ADC_SQR1_SQ1_Pos | 11 << ADC_SQR1_SQ2_Pos | 15 << ADC_SQR1_SQ3_Pos); // Set channels

  //
  ADC1->IER |= ADC_IER_EOCIE; // enable interrupt bit
  NVIC_EnableIRQ(ADC1_IRQn);
  ADC1->ISR |= ADC_ISR_ADRDY;
  ADC1->CR &= ~ADC_CR_DEEPPWD;
  ADC1->CFGR |= ADC_CFGR_JQDIS;

  // Enable ADC
  ADC1->ISR |= ADC_ISR_ADRDY; // Clear the ADRDY bit in the ADC_ISR register by writing ‘1’
  ADC1->CR |= ADC_CR_ADEN;    // Enable ADC
  while (ADC1->ISR & ADC_ISR_ADRDY)
    ;
  ADC1->ISR |= ADC_ISR_ADRDY;
  ADC1->CR |= ADC_CR_ADVREGEN;
  HAL_Delay(10);
  ADC1->CR |= ADC_CR_ADSTART;
  HAL_Delay(10);
}

/* USER CODE END 1 */
