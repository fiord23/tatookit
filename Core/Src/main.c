/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"



/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "display.h"
#include "dac.h"
#include "motor.h"
#include "fonts.h"
#include <stdbool.h>
#include "clocks.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t adc_value11 = 0;
volatile uint16_t adc_value15 = 0;
char vbatbuf[3] = {'0', '.', '0'};
float vbat_value = 0.0;
uint8_t speed = 40;
uint8_t flag_motor = 3;
volatile uint16_t ADC_Data[3] = { 0, };




/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  //MX_ADC1_Init();
  adc_init();
  dac_init();
  MX_I2C3_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  //
  button_interrupt_init();
  motor_init();
  display_power_high();
  dac_data_send(993);
  display_init();
  display_test();
  /* USER CODE END 2 */
  
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    show_motor_duty();
    show_vbat();
    SSD1306_UpdateScreen();

    HAL_Delay(1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void  EXTI1_IRQHandler (void)    //PB1 BUTTON - 
{
  EXTI->PR1 |= EXTI_PR1_PIF1;
  speed--;
  motor_speed_write(speed);

}
void EXTI9_5_IRQHandler (void) //PB6 BUTTON +
{
  EXTI->PR1 |= EXTI_PR1_PIF6; 
  speed ++;
  motor_speed_write(speed);
}
void EXTI15_10_IRQHandler (void)
{
  
  if (flag_motor)
  {
    flag_motor = 0;
    display_power_low();
    motor_write(CONFIG0, 0x61);

    
  }
  else
  {
    flag_motor = 1;
    display_power_high();
    motor_write(CONFIG0, 0xE1);
  }

  EXTI->PR1 |= EXTI_PR1_PIF11; //PA11 BUTTON INT

}
void ADC1_IRQHandler(void)
{
    if (ADC1->ISR & ADC_ISR_EOC) { // Проверяем флаг конца преобразования
      ADC1->DR;
    }
}
void DMA1_Channel1_IRQHandler() {
  
  if(DMA1->ISR & DMA_ISR_TCIF1) {

		    DMA1->IFCR |= DMA_IFCR_CGIF1; //Сбросим глобальный флаг.

	} else if (DMA1->ISR & DMA_ISR_TEIF1) {
		/*Здесь можно сделать какой-то обработчик ошибок*/
		DMA1->IFCR |= DMA_IFCR_CGIF1; //Сбросим глобальный флаг.
	}
}

/* USER CODE END 4 */

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
