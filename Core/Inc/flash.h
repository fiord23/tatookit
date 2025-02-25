#ifndef FLASH_H
#define FLASH_H
#include "main.h"
void flash_rdp_level1(void);
HAL_StatusTypeDef CodeProtection_SetLevel(uint8_t Level);
#endif