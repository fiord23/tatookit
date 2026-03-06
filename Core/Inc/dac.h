#ifndef DAC_H
#define DAC_H
#include "stm32l431xx.h"
#include "stm32l4xx_hal.h" 
void dac_init (void);
void dac_data_send (uint16_t data);
#endif