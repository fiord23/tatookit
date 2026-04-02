#ifndef FLASH_H
#define FLASH_H
#include "main.h"

#define FLASH_USER_ADDR  0x0803F800

void flash_rdp_level1(void);
HAL_StatusTypeDef CodeProtection_SetLevel(uint8_t Level);

void Flash_Save(uint8_t value);
void Flash_LoadOrInit(void);
void Flash_FW_LoadOrInit(void);
void Flash_FW_Save(uint8_t value);
#endif