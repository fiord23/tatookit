#include "display.h"
#include "stm32l4xx_hal_spi.h"
#define cs_low()                HAL_GPIO_WritePin(GPIOA, DISPLAY_CS_Pin, GPIO_PIN_RESET) //PA9 LOW
#define cs_high()               HAL_GPIO_WritePin(GPIOA, DISPLAY_CS_Pin, GPIO_PIN_SET) //PA9 HIGH
#define display_data()          HAL_GPIO_WritePin(GPIOA, DISPLAY_DC_Pin, GPIO_PIN_SET) //PA10 HIGH
#define display_command()       HAL_GPIO_WritePin(GPIOA, DISPLAY_DC_Pin, GPIO_PIN_RESET) // PA10 LOW
#define display_reset_low()     HAL_GPIO_WritePin(GPIOA, POWER_VOLTAGE_REMOTE_Pin, GPIO_PIN_RESET) // PA8 LOW
#define display_reset_high()    HAL_GPIO_WritePin(GPIOA, POWER_VOLTAGE_REMOTE_Pin, GPIO_PIN_SET) // PA8 HIGH
extern SPI_HandleTypeDef hspi1;

void command (uint8_t* command)
{
    display_command();
    cs_low();
    HAL_SPI_Transmit(&hspi1, command, 1, 100);
    cs_high(); 

}

void data (uint8_t* data)
{
    display_data();
    cs_low();
    HAL_SPI_Transmit(&hspi1, data, 1, 100);
    cs_high(); 

}