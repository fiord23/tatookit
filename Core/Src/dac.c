
#include "dac.h"
//#include "stm32l4xx_hal_spi.h"
void dac_init (void)
{ 
    GPIOA->MODER |= GPIO_MODER_MODE5_0 | GPIO_MODER_MODE5_1; //Analog Mode (reset state)
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT5; //Push-Pull (reset state)
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;

    /*Calibration*/
    DAC->CR &= ~DAC_CR_EN2;
    DAC->CR &= ~DAC_CR_CEN2;
    DAC->MCR |= 0 << DAC_MCR_MODE2_Pos; // DAC channel2 is connected to external pin with Buffer enabled
    DAC->CR |= DAC_CR_CEN2;





}