#include "inc/gpio.h"
#include "seg7.h"

//=========================================================

void SEG7_set_number_quarter(struct Seg7Display* seg7, unsigned tick)
{
    uint32_t divisors[4] = {1, 10, 100, 1000};

    unsigned quarter = tick % 4;
    unsigned divisor = divisors[quarter];

    seg7->display = Digits[(seg7->number / divisor) % 10] | Positions[quarter];
}

//---------------------------------------------------------

// Write changes to microcontroller:
void SEG7_push_display_state_to_mc(struct Seg7Display* seg7)
{
    uint32_t surrounding_state = ~Pins_used & GPIO_ODR_READ(GPIOA);
    uint32_t to_write = Pins_used & seg7->display;

    GPIO_ODR_WRITE(GPIOA, surrounding_state | to_write);
}