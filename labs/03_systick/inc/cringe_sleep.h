#pragma once 

#include <stdint.h>

#define ONE_SEC_DELAY_TIME 3692308U // 48000000 / 13

static inline void more_precise_delay_forbidden_by_quantum_mechanics_1000ms()
{
   for (uint32_t i = 0; i < ONE_SEC_DELAY_TIME; ++i);
}