#include "tm4c123gh6pm.h"
#include "timer.h"
#include <stdint.h>

#define RELOAD (16000 - 1) // N+1 clock pulses, where N is any value from 1 to 0x00FF.FFFF
#define COUNT_NOT_ZERO (NVIC_ST_CTRL_R & (1<<16) == 0)
#define ENABLE_SYSTICK (1<<0)
#define CLK_SRC (1<<2) // System Clock
#define INTEN (0<<1) // Interrupt is disabled

/**
 * Uses the integrated system timer, SysTick
 * Default bus clock is 16MHz
 * 62.5ns for each clock tick
 */
void initTimer()
{
    NVIC_ST_CTRL_R &= ~ENABLE_SYSTICK; // Disable
    NVIC_ST_RELOAD_R = RELOAD; // 1 ms
    NVIC_ST_CURRENT_R = 1; // Write any value to clear
    NVIC_ST_CTRL_R |= CLK_SRC | INTEN | ENABLE_SYSTICK;
}

// time: number of ms to wait
void wait_1ms(uint32_t time)
{
    int i;
    for (i = 0; i < time; i++)
    {
        NVIC_ST_CURRENT_R = 1;
        while (COUNT_NOT_ZERO){}
    }
}
