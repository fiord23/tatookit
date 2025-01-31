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
#include "clocks.h"
#include "time.h"
#include <stdbool.h>
#include "stdint.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MOTOR_SPEED_WAKEUP 53
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t speed = 40;
bool flag_motor = 1;
volatile uint16_t ADC_Data[3] = { 0, };
uint16_t time = 0;
bool time_active = 1;
bool sleep = 0;
bool sleep_status = 0;
uint16_t time_sleep = 0;
uint8_t sec_to_min = 0;
float power_value;
volatile uint32_t button_counter_plus = 0;
volatile uint32_t button_counter_minus = 0;
uint8_t cycles = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
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
  MX_GPIO_Init();
  adc_init();
  dac_init();
  MX_I2C3_Init();
  MX_SPI1_Init();
  button_interrupt_init();
  motor_init();
  display_power_high();
  dac_data_send(993);
  display_init();
  display_test();
  time_init();
  show_motor_speed();


  /* USER CODE END SysInit */
  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
   // power_value = (((float)ADC_Data[2]) / 4095.0   ) * 3.3;

    if  ( time_sleep < 15 )
    {
      if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)) //PB6 BUTTON +
      {
        button_counter_plus ++;
        if (button_counter_plus > 12)
        {
          speed ++;
          if(speed > MOTOR_SPEED_HIGH)
            speed = MOTOR_SPEED_HIGH;
          motor_speed_write(speed);
          show_motor_speed();
          HAL_Delay(1);
          SSD1306_UpdateScreen();
          if (button_counter_plus > 1000)
            button_counter_plus = 0;
        }
      }
      if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)) //PB1 BUTTON -
      {
        button_counter_minus ++;
        if (button_counter_minus > 12)
        {
          speed --;
          if(speed < MOTOR_SPEED_LOW)
            speed = MOTOR_SPEED_LOW;
          motor_speed_write(speed);
          show_motor_speed();
          HAL_Delay(1);
          SSD1306_UpdateScreen();
          if (button_counter_minus > 1000)
            button_counter_minus = 0;
        }     

      }
      
      
      show_vbat();
      show_time();
      show_motor_duty(); 
      show_motor_speed();
      SSD1306_UpdateScreen();
    }
    else
    {
      display_power_low();
      SSD1306_Fill(SSD1306_COLOR_BLACK);
      sleep_status = 1;    

    }
    HAL_Delay(100);
    cycles ++;
    if (cycles % 10 == 0)
    {
      cycles = 0;
    //  show_vbat();
    SSD1306_UpdateScreen();
    }
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void EXTI1_IRQHandler (void)    //PB1 BUTTON - 
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
void EXTI15_10_IRQHandler (void) //BUTTON ON/OFF
{
  
  if (flag_motor)
  {
    flag_motor = 0;
    display_power_low();
    motor_write(CONFIG0, 0x61);
    time_active = 0;  
  }

  else
  {
    display_power_high();
    if (sleep_status)
    {
      sleep_status = 0;
      flag_motor = 0;
      time_sleep = 0;
      
    }
    else
    {
      motor_write(CONFIG0, 0xE1);
      time_active = 1;
      if (speed < MOTOR_SPEED_WAKEUP)
        {
          motor_speed_write(MOTOR_SPEED_WAKEUP);
          for (volatile uint32_t mdelay = 0; mdelay < 2000000; mdelay ++)
          ;
        }
      motor_speed_write(speed);
      time_sleep = 0;
      flag_motor = 1;
    }

  }

  EXTI->PR1 |= EXTI_PR1_PIF11; //PA11 BUTTON INT

}
void ADC1_IRQHandler(void)
{
    if (ADC1->ISR & ADC_ISR_EOC) { 
      ADC1->DR;
    }
}
void DMA1_Channel1_IRQHandler() {
  if(DMA1->ISR & DMA_ISR_TCIF1) 
		DMA1->IFCR |= DMA_IFCR_CGIF1; 
  else if (DMA1->ISR & DMA_ISR_TEIF1)	
		DMA1->IFCR |= DMA_IFCR_CGIF1;
}
void TIM7_IRQHandler (void)
{

  
  TIM7->SR &= ~TIM_SR_UIF;
  
  if (time_active)
  {
    sec_to_min++;
    if (sec_to_min > 59)
    {
      time++;
      sec_to_min = 0;
    }
    
  }
  else
  {
    sec_to_min++;
    if (sec_to_min > 59)
    {
      time_sleep ++;
      sec_to_min = 0;
    }
    
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
