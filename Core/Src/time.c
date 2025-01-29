#include "time.h"
#include "stm32l431xx.h"

void time_init(void)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;
    TIM7->CR1 |= TIM_CR1_CEN | TIM_CR1_URS;
    TIM7->CR2 |= 2<< TIM_CR2_MMS2;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->PSC |= 1499; 
    TIM7->ARR |= 1999;
    
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ (TIM7_IRQn);

}