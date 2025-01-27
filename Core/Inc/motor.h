#ifndef MOTOR_H
#define MOTOR_H
#include "main.h"
#include "stm32l4xx_hal_i2c.h"
#define MOTOR_I2C_ADDRESS    0x60

/*REGISTERS*/
#define FAULT           0x00
#define RC_STATUS1      0x01
#define RC_STATUS2      0x02
#define RC_STATUS3      0x03
#define REG_STATUS1     0x04
#define REG_STATUS2     0x05
#define REG_STATUS3     0x06
#define CONFIG0         0x09
#define CONFIG1         0x0A
#define CONFIG2         0x0B
#define CONFIG3         0x0C
#define CONFIG4         0x0D
#define REG_CTRL0       0x0E
#define REG_CTRL1       0x0F
#define REG_CTRL2       0x10
#define RC_CTRL0        0x11
#define RC_CTRL1        0x12
#define RC_CTRL2        0x13
#define RC_CTRL3        0x14
#define RC_CTRL4        0x15
#define RC_CTRL5        0x16
#define RC_CTRL6        0x17
#define RC_CTRL7        0x18
#define RC_CTRL8        0x19

#define MOTOR_SPEED_LOW         3
#define MOTOR_SPEED_HIGH       63
#define MOTOR_SPEED_DEFAULT    40

void motor_write(uint8_t reg, uint8_t data);
void motor_init(void);
#endif