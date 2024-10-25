#ifndef DISPLAY_H
#define DISPLAY_H
#include "main.h"
#include "stm32l4xx_hal_spi.h"
#define cs_low()                HAL_GPIO_WritePin(GPIOA, DISPLAY_CS_Pin, GPIO_PIN_RESET) //PA9 LOW
#define cs_high()               HAL_GPIO_WritePin(GPIOA, DISPLAY_CS_Pin, GPIO_PIN_SET) //PA9 HIGH
#define display_data()          HAL_GPIO_WritePin(GPIOA, DISPLAY_DC_Pin, GPIO_PIN_SET) //PA10 HIGH
#define display_command()       HAL_GPIO_WritePin(GPIOA, DISPLAY_DC_Pin, GPIO_PIN_RESET) // PA10 LOW
#define display_reset_low()     HAL_GPIO_WritePin(GPIOA, POWER_VOLTAGE_REMOTE_Pin, GPIO_PIN_RESET) // PA8 LOW
#define display_reset_high()    HAL_GPIO_WritePin(GPIOA, POWER_VOLTAGE_REMOTE_Pin, GPIO_PIN_SET) // PA8 HIGH
#define display_power_low()     HAL_GPIO_WritePin(GPIOA, POWER_ON_OFF_Pin, GPIO_PIN_RESET) // P0 LOW
#define display_power_high()    HAL_GPIO_WritePin(GPIOA, POWER_ON_OFF_Pin, GPIO_PIN_SET) // PA0 HIGH
#define OLED_X_MAXPIXEL 128  //OLED width maximum memory 
#define OLED_Y_MAXPIXEL  72 //OLED height maximum memory

#define WIDTH 128/2
#define HEIGHT 72

void command (uint8_t command);
void data (uint8_t data);
void display_init (void);
void OLED_ColorTurn(uint8_t i);
void er_oled_SetWindow(uint8_t Xstart, uint8_t Ystart, uint8_t Xend, uint8_t Yend);
void er_oled_clear(uint8_t a);
void er_oled_bitmap(const uint8_t * pBuf);

#endif