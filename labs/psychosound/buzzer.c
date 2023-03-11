#include <stdbool.h>
#include <stdlib.h>

//---------------------------------------------------------

#include "inc/gpio.h"
#include "inc/rcc.h"
#include "buzzer.h"

//=========================================================

static void buzzer_setup_gpio(struct Buzzer* buzzer);

//=========================================================

int buzzer_setup(struct Buzzer* buzzer, uint32_t GPIOx, uint16_t pin, unsigned freq, unsigned tick_freq)
{
    if (buzzer == NULL)
        return BZR_INV_PTR;

    if (GPIOx < GPIOA || GPIOx > GPIOF)
        return BZR_INV_GPIO;

    buzzer->GPIOx = GPIOx;
    buzzer->pin = pin;
    buzzer->state = false;

    if (freq > BUZZER_MAX_FREQ)
        return BZR_INV_FREQ;

    buzzer->freq = freq;
    buzzer->tick_freq = tick_freq;

    buzzer_setup_gpio(buzzer);

    return 0;
}

//---------------------------------------------------------

static void buzzer_setup_gpio(struct Buzzer* buzzer)
{
    uint8_t bit = REG_RCC_AHBENR_IOPAEN + (buzzer->GPIOx - GPIOA) / GPIO_offs;
    SET_BIT(REG_RCC_AHBENR, bit);
    
    SET_GPIO_IOMODE(buzzer->GPIOx, buzzer->pin, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);
    SET_GPIO_OTYPE(buzzer->GPIOx, buzzer->pin, GPIO_OTYPE_PUSH_PULL);

    return;
}

//---------------------------------------------------------

int buzzer_do_routine(struct Buzzer* buzzer, uint32_t tick)
{
    if (buzzer == NULL)
        return BZR_INV_PTR;

    unsigned rate = buzzer->tick_freq / buzzer->freq;

    if ((tick % rate) == 0)
    {
        if (buzzer->state == false)
        {
            GPIO_BSRR_SET_PIN(buzzer->GPIOx, buzzer->pin);
            buzzer->state = true;
        }
        else 
        {
            GPIO_BRR_RESET_PIN(buzzer->GPIOx, buzzer->pin);
            buzzer->state = false;
        }
    }

    return 0;
}

