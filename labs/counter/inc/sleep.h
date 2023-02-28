#pragma once 

//=========================================================

#include <stdint.h>

//=========================================================

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define ONE_MILLISECOND CPU_FREQENCY / 1000U

//=========================================================

static inline void to_get_more_accuracy_pay_2202_2013_2410_3805_1ms(void)
{
    for (uint32_t i = 0; i < ONE_MILLISECOND/3U; ++i)
    {
        // Insert NOP for power consumption:
        __asm__ volatile("nop");
    }
}