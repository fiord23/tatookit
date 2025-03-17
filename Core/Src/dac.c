
#include "dac.h"
void dac_init(void)
{
    // PA5 Pin
    GPIOA->MODER |= GPIO_MODER_MODE5_0 | GPIO_MODER_MODE5_1; // Analog Mode (reset state)
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;                       // Push-Pull (reset state)
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;

    /*Calibration*/
    DAC->CR &= ~DAC_CR_EN2;
    DAC->CR &= ~DAC_CR_CEN2;
    DAC->MCR |= 2 << DAC_MCR_MODE2_Pos; // DAC channel2 is connected to external pin with Buffer enabled
    DAC->SHSR2 = 500;
    DAC->CR |= DAC_CR_CEN2;

    for (volatile uint8_t calibr = 0; calibr < 31; calibr++)
    {
        DAC->CCR |= calibr << DAC_CCR_OTRIM2_Pos;
        HAL_Delay(5);
        if (DAC->SR & DAC_SR_CAL_FLAG2)
            break;
    }
    DAC->CR &= ~DAC_CR_CEN2;
    /*End calibration*/

    DAC->CR |= DAC_CR_EN2;
    DAC->DHR12R2 = 0;
}

void dac_data_send(uint16_t data)
{
    DAC->DHR12R2 = data;
}