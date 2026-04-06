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
#include "flash.h"
#include <stdbool.h>
#include "stdint.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//<#define MOTOR_SPEED_WAKEUP 53
#define MOTOR_SPEED_WAKEUP 68 // 0.1 step
#define ADDR_FLASH_PAGE ((uint32_t)0x0803F800)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t speed = MOTOR_SPEED_DEFAULT;
uint8_t flag_motor = 0;
volatile uint16_t ADC_Data[3] = {
    0,
};
uint16_t time = 0;
bool time_active = 1;
bool sleep = 0;
bool motor_init_flag = 0;
bool sleep_status = 0;
bool motor_speed_flag = 0;
bool flash_write_speed = false;
uint16_t time_sleep = 0;
uint8_t sec_to_min = 0;
uint16_t counter_first_start = 0;
float power_value;
volatile uint32_t button_counter_plus = 0;
volatile uint32_t button_counter_minus = 0;
uint8_t cycles = 0;
static uint32_t time_stall = 0;
static uint8_t buttons_pressed = 0;
static uint32_t buttons_time = 0;
uint16_t motor_strart_nopress = 0;
uint8_t ffirst_moment = 0;
uint8_t fw_version = 0;
uint8_t bug = 0;
char c_version_fw[4] = "0.0";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ShowVersion(void)
{
  switch (fw_version)
  {
  case 0:
    strcpy(c_version_fw, "3.2");
    break;
  case 1:
    strcpy(c_version_fw, "3.5");
    break;
  case 2:
    strcpy(c_version_fw, "3.8");
    break;
  case 3:
    strcpy(c_version_fw, "4.2");
    break;
  case 4:
    strcpy(c_version_fw, "4.7");
    break;
  }

  SSD1306_GotoXY(48, 8);
  SSD1306_Puts(c_version_fw, &Font_16x26, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();
}
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
  HAL_Delay(100);
  // CodeProtection_SetLevel(0);
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */
  /* Configure the system clock */
  SystemClock_Config();
  /* USER CODE BEGIN SysInit */
  MX_GPIO_Init();
  CodeProtection_SetLevel(1);
  HAL_Delay(100);
  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11)) // read power button
  {
    counter_first_start++;
    HAL_Delay(5);
    if (counter_first_start > 600)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    }
  }
  Flash_LoadOrInit();
  MX_SPI1_Init();
  adc_init();
  display_init();
  display_demo();

  SSD1306_Fill(SSD1306_COLOR_BLACK);
  SSD1306_UpdateScreen();

  Flash_FW_LoadOrInit();
  ShowVersion();

  uint32_t last_action = HAL_GetTick();
  uint8_t changed = 0;

  while (HAL_GetTick() - last_action < 700)
  {
    // "+"
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6))
    {
      HAL_Delay(50);

      if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6))
      {
        fw_version = (fw_version + 1) % 5;

        last_action = HAL_GetTick();
        changed = 1;

        uint32_t t = HAL_GetTick();
        while (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6))
        {
          if (HAL_GetTick() - t > 300)
            break;
        }

        ShowVersion();
      }
    }

    // "-"
    if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1))
    {
      HAL_Delay(50);

      if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1))
      {
        fw_version = (fw_version == 0) ? 4 : fw_version - 1;

        last_action = HAL_GetTick();
        changed = 1;

        uint32_t t = HAL_GetTick();
        while (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1))
        {
          if (HAL_GetTick() - t > 300)
            break;
        }

        ShowVersion();
      }
    }
  }

  // if (changed)
  // {
  //   Flash_FW_Save(fw_version);
  //}
  Flash_FW_Save(fw_version);
  Flash_FW_Save(fw_version);
  HAL_Delay(200);

  counter_first_start = 0;
  HAL_Delay(50);

  dac_init();
  MX_I2C3_Init();

  button_interrupt_init();
  HAL_Delay(50);
  motor_init();
  HAL_GPIO_WritePin(GPIOA, EN_IN1_Pin, GPIO_PIN_RESET);
  display_power_high();
  HAL_Delay(50);
  dac_data_send(993);
  HAL_Delay(50);
  display_test();
  time_init();

  // show_motor_speed();
  char speed_data_0[6] = {'0', '0', '0', 'H', 'z'};
  SSD1306_GotoXY(70, 8);
  SSD1306_Puts(speed_data_0, &Font_7x10, SSD1306_COLOR_WHITE);

  dac_data_send(993);
  HAL_Delay(10);
  dac_data_send(4000);
  HAL_Delay(10);
  dac_data_send(993);

  HAL_Delay(10);
  show_vbat();
  SSD1306_UpdateScreen();

  /* USER CODE END SysInit */
  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  Flash_FW_Save(fw_version);
  show_vbat();
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    // power_value = (((float)ADC_Data[2]) / 4095.0   ) * 3.3;
    if (time_sleep < 15)
    {
      if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)) // PB6 BUTTON +
      {
        button_counter_plus++;
        if (button_counter_plus > 12)
        {
          speed++;
          if (speed > MOTOR_SPEED_HIGH)
            speed = MOTOR_SPEED_HIGH;

          motor_speed_write(speed);
          flash_write_speed = true;
          show_motor_duty();
          HAL_Delay(1);
          if (bug == 0)
            SSD1306_UpdateScreen();
          if (button_counter_plus > 120)
            button_counter_plus = 0;
        }
      }
      if (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)) // PB1 BUTTON -
      {
        button_counter_minus++;
        if (button_counter_minus > 12)
        {
          speed--;
          if (speed < MOTOR_SPEED_LOW)
            speed = MOTOR_SPEED_LOW;
          motor_speed_write(speed);
          flash_write_speed = true;
          show_motor_duty();
          HAL_Delay(1);
          if (bug == 0)
            SSD1306_UpdateScreen();
          if (button_counter_minus > 120)
            button_counter_minus = 0;
        }
      }

      if ((HAL_GetTick() - time_stall > 1000) && ((motor_read(RC_STATUS1) < MOTOR_SPEED_STOP))) // 42 for divider x1
      {
        flag_motor = 0;
        display_power_low();
        motor_write(CONFIG0, 0x61);
        display_power_low();
        motor_write(CONFIG0, 0x61);
        time_active = 0;
        time_stall = 0;
      }


      if (buttons_pressed == 0)
      {
        show_motor_duty();
      }

      if (motor_speed_flag)
      {
        show_motor_speed();
        motor_speed_flag = 0;
      }
      show_vbat();
      // show_vbat();
      show_time();
      HAL_Delay(100);
      cycles++;
      if (cycles % 10 == 0)
      {
        cycles = 0;
        // show_vbat();
        if (bug == 0)
          SSD1306_UpdateScreen();
      }



      if ((!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6)) && (!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1)))
      {
        if (buttons_pressed == 0)
        {
          // кнопки только что нажали
          buttons_pressed = 1;
          buttons_time = HAL_GetTick();
        }
        else if (buttons_pressed == 1)
        {
          // ждём 1 секунду
          if (HAL_GetTick() - buttons_time >= 1000)
          {
            HAL_GPIO_TogglePin(GPIOA, PH_IN2_Pin);
            buttons_pressed = 2; // обработано, больше не выполняем
          }
        }
        // если buttons_pressed == 2 — ничего не делаем, ждём отпускания
      }
      else
      {
        // кнопки отпущены — сбрасываем состояние
        buttons_pressed = 0;
      }

      motor_show_direction(HAL_GPIO_ReadPin(GPIOA, PH_IN2_Pin));
      show_vbat();
      if (bug == 0)
        SSD1306_UpdateScreen();
      if (flash_write_speed == true)
      {
        flash_write_speed = false;
        Flash_Save(speed);
      }
      /* USER CODE END WHILE */
      if (ffirst_moment == 0)
      {
        dac_data_send(4000);
        HAL_Delay(50);
        dac_data_send(993);
        show_vbat();
        ffirst_moment = 1;
      }

      /* USER CODE BEGIN 3 */
    }

    else
    {
      display_power_low();
      SSD1306_Fill(SSD1306_COLOR_BLACK);
      SSD1306_UpdateScreen();
      sleep_status = 1;
      flag_motor = 1;

      if (time_sleep > 29)
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    }
  }

  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void EXTI1_IRQHandler(void) // PB1 BUTTON -
{
  EXTI->PR1 = EXTI_PR1_PIF1;
  speed--;
  if (speed < MOTOR_SPEED_LOW)
    speed = MOTOR_SPEED_LOW;
  motor_speed_write(speed);
  flash_write_speed = true;
  button_counter_minus = 0;
}

void EXTI9_5_IRQHandler(void) // PB6 BUTTON +
{
  EXTI->PR1 = EXTI_PR1_PIF6;
  speed++;
  if (speed > MOTOR_SPEED_HIGH)
    speed = MOTOR_SPEED_HIGH;
  motor_speed_write(speed);
  flash_write_speed = true;
  button_counter_plus = 0;
}

void EXTI15_10_IRQHandler(void)
{
  static uint32_t last_time = 0;

  if (HAL_GetTick() - last_time < 150)
  {
    EXTI->PR1 = EXTI_PR1_PIF11;
    return;
  }
  last_time = HAL_GetTick();

  EXTI->PR1 = EXTI_PR1_PIF11;

  // Первый запуск питания
  if (motor_init_flag == 0)
  {
    HAL_GPIO_WritePin(GPIOA, EN_IN1_Pin, GPIO_PIN_SET);
    motor_init_flag = 1;
    time_sleep = 0;
    time_stall = HAL_GetTick();
    dac_data_send(4000);
    dac_data_send(993);
    bug = 0;
  }

  // 1. Если мотор работает → выключаем
  if (flag_motor == 1)
  {
    flag_motor = 0;
    display_power_low();
    motor_write(CONFIG0, 0x61);
    time_active = 0;
    time_stall = 0;
    if (sleep_status == 1)
    {

      // display_power_high();
      sleep_status = 0;
      display_power_high();
      SSD1306_UpdateScreen();
      time_sleep = 0;
      // flag_motor = 0;
      // return;
    }
    return;
  }

  // 3. Иначе → запускаем мотор
  display_power_high();
  motor_write(CONFIG0, 0xE1);
  time_active = 1;

  if (speed < MOTOR_SPEED_WAKEUP)
  {
    motor_speed_write(MOTOR_SPEED_WAKEUP);
    for (volatile uint32_t mdelay = 0; mdelay < 2000000; mdelay++)
      ;
  }

  motor_speed_write(speed);
  flash_write_speed = true;
  time_stall = HAL_GetTick();
  flag_motor = 1;
  time_sleep = 0;
}
void ADC1_IRQHandler(void)
{
  if (ADC1->ISR & ADC_ISR_EOC)
  {
    ADC1->DR;
  }
}
void DMA1_Channel1_IRQHandler()
{
  if (DMA1->ISR & DMA_ISR_TCIF1)
    DMA1->IFCR |= DMA_IFCR_CGIF1;
  else if (DMA1->ISR & DMA_ISR_TEIF1)
    DMA1->IFCR |= DMA_IFCR_CGIF1;
}
void TIM7_IRQHandler(void)
{
  TIM7->SR &= ~TIM_SR_UIF;
  if (motor_init_flag == 1)
  {

    if (motor_speed_flag == 0)
    {
      motor_speed_flag = 1;
    }

    if (time_active) // motor is in run mode
    {
      sec_to_min++;
      //  if (sec_to_min > 59)
      if (sec_to_min > 59)
      {
        time++;
        sec_to_min = 0;
      }
    }
    else // time_active = 0, motor is in stop mode
    {
      sec_to_min++;
      if (sec_to_min > 59)
      //    if (sec_to_min > 59)
      {
        time_sleep++;
        sec_to_min = 0;
      }
    }
  }
  else // motor_init_flag = 0
  {

    motor_strart_nopress++;
    if (motor_strart_nopress > 900)
    {
      flag_motor = 1;
      sleep_status = 1;
      time_sleep = 16;
      if (bug == 0)
      {
        //    display_power_low();
        //   SSD1306_Fill(SSD1306_COLOR_BLACK);
        //   SSD1306_UpdateScreen(); // 106
        //   sleep_status = 1;
        bug = 1;
      }

      if (motor_strart_nopress > 1800)
      {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
      }

      // motor_strart_nopress = 0;
    }

    ;
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
#ifdef USE_FULL_ASSERT
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
