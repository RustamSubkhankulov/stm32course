#pragma once 

//=========================================================

#include <stdint.h>

//=========================================================

#define BUZZER_MAX_FREQ 100000U

// Display state:
struct Buzzer
{
    uint32_t GPIOx;
    uint16_t pin;
    
    unsigned freq;
    unsigned tick_freq;

    bool state;
};

enum Buzzer_error
{
    BZR_INV_PTR  = -1,
    BZR_INV_GPIO = -2,
    BZR_INV_POS  = -3,
    BZR_INV_FREQ = -4
};

//=========================================================

int buzzer_setup(struct Buzzer* buzzer, uint32_t GPIOx, uint16_t pin, 
                                   unsigned freq, unsigned tick_freq);

int buzzer_do_routine(struct Buzzer* buzzer, uint32_t tick);

