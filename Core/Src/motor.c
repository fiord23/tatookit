#include "motor.h"
#include "stm32l4xx_hal_i2c.h"
extern I2C_HandleTypeDef hi2c3;

void motor_write(uint8_t reg, uint8_t data)
{
    uint8_t tdata[2] = {reg, data};
    HAL_I2C_Master_Transmit(&hi2c3, MOTOR_I2C_ADDRESS, tdata, 2,  100);
}


void motor_init(void)
{
    HAL_GPIO_WritePin(NSLEEP_GPIO_Port, NSLEEP_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA,  POWER_ON_OFF_Pin |EN_IN1_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    motor_write(CONFIG4, 0x30);
    motor_write(CONFIG0, 0xE1);
    motor_write(REG_CTRL2, 40);
    HAL_GPIO_WritePin(GPIOA, EN_IN1_Pin, GPIO_PIN_SET);  
    HAL_Delay(100);
}
