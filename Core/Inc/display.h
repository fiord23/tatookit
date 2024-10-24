#ifndef DISPLAY_H
#define DISPLAY_H
#include "main.h"
#include "stm32l4xx_hal_spi.h"
void command (uint8_t* command);
void data (uint8_t* data);
#endif