#include "delay.h"


void Delay_Init(void) {
    SysTick->LOAD = (uint32_t)((SystemCoreClock / 1000UL) - 1UL);
    NVIC_SetPriority(SysTick_IRQn,(1UL << __NVIC_PRIO_BITS) - 1UL);
    SysTick->VAL = 0UL;
    SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk);
}


// Do a delay for a specified number of milliseconds
// input:
//   ms - number of milliseconds to wait
void Delay_ms(uint32_t ms) {
    __IO uint32_t delay_counter = ms;

    while (delay_counter) {
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
            delay_counter--;
        }
    }
}
