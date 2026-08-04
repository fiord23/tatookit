#ifndef EEPROM_H
#define EEPROM_H
#include "main.h"
#include "stm32l4xx_hal_i2c.h"
#include "stdint.h"
#include <stdbool.h>
#include <string.h>

#define AT24CXX_I2C hi2c3
extern I2C_HandleTypeDef AT24CXX_I2C;

#define EEPROM_ARRAY_ADDRESS_1 0x55
#define EEPROM_PAGE_ADDRESS 0xBA
#define EEPROM_ARRAY_ADDRESS_2 0x55

#define EEPROM_I2C_ADDR (0x55 << 1)

#define AT24CXX_I2C_ADDR (0x55 << 1)

#define EEPROM_PAGE_SPEED 1
#define EEPROM_PAGE_FW 2
#define MOTOR_DIRECTION_PAGE 3
#define DISPLAY_ORIENTATION_PAGE 4

/*-----------------------Указываем тип памяти(нужное раcкомментировать)--------------------------------*/
//#define AT24C01
//#define AT24C02
//#define AT24C04
//#define AT24C08
//#define AT24C16
//#define AT24C32
//#define AT24C64
#define AT24C128
//#define AT24C256
//#define AT24C512


#define WP_PORT GPIOC
#define WP_Pin GPIO_PIN_14

#if defined(AT24C01)
#define AT24CXX_PAGE_BYTE 8
#define AT24CXX_MAX_MEM_ADDRESS 128
#elif defined(AT24C02)
#define AT24CXX_PAGE_BYTE 8
#define AT24CXX_MAX_MEM_ADDRESS 256
#elif defined(AT24C04)
#define AT24CXX_PAGE_BYTE 16
#define AT24CXX_MAX_MEM_ADDRESS 512
#elif defined(AT24C08)
#define AT24CXX_PAGE_BYTE 16
#define AT24CXX_MAX_MEM_ADDRESS 1024
#elif defined(AT24C16)
#define AT24CXX_PAGE_BYTE 16
#define AT24CXX_MAX_MEM_ADDRESS 2048
#elif defined(AT24C32)
#define AT24CXX_PAGE_BYTE 32
#define AT24CXX_MAX_MEM_ADDRESS 4096
#elif defined(AT24C64)
#define AT24CXX_PAGE_BYTE 32
#define AT24CXX_MAX_MEM_ADDRESS 8192
#elif defined(AT24C128)
#define AT24CXX_PAGE_BYTE 64
#define AT24CXX_MAX_MEM_ADDRESS 16384
#elif defined(AT24C256)
#define AT24CXX_PAGE_BYTE 64
#define AT24CXX_MAX_MEM_ADDRESS 32768
#elif defined(AT24C512)
#define AT24CXX_PAGE_BYTE 128
#define AT24CXX_MAX_MEM_ADDRESS 65536
#endif



bool AT24xx_Connect_test(void);
void AT24Cxx_erase_chip(void);
uint16_t AT24Cxx_write(uint16_t addMem_write, uint8_t *data_write, uint16_t size_write);
uint16_t AT24Cxx_read(uint16_t addMem_read, uint8_t *data_read, uint16_t size_read);
bool AT24Cxx_write_data(uint16_t addMem_write, uint8_t *data, uint8_t len);
bool AT24Cxx_read_data(uint16_t addMem_read, uint8_t *data, uint8_t len);

void eeprom_speed_init(void);
void eeprom_fw_init(void);
void eeprom_fw_save(uint8_t value);
void eeprom_speed_save(uint8_t value);
void motor_direction_init(void);
void motor_direction_save(uint8_t value);
void display_orientation_init(void);
void display_orientation_save(uint8_t value);
#endif
