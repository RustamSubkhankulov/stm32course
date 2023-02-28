#include <stdbool.h>
#include <stdlib.h>

//---------------------------------------------------------

#include "inc/gpio.h"
#include "inc/rcc.h"
#include "seg7.h"

//=========================================================

// Pin Mapping:
#define A  (1 << 11)
#define B  (1 <<  7)
#define C  (1 <<  4)
#define D  (1 <<  2)
#define E  (1 <<  1)
#define F  (1 << 10)
#define G  (1 <<  5)
#define DP (1 <<  3) 

#define DIG0 (1 << 12)
#define DIG1 (1 <<  9)
#define DIG2 (1 <<  8)
#define DIG3 (1 <<  6)

#define POS0 DIG3
#define POS1 DIG2
#define POS2 DIG1
#define POS3 DIG0

//---------------------------------------------------------

static const uint32_t Pins_used = (A|B|C|D|E|F|G|DP|POS0|POS1|POS2|POS3);

// Digit composition
static const uint32_t Digits[10] =
{
    A|B|C|D|E|F,   // 0
    B|C,           // 1
    A|B|D|E|G,     // 2
    A|B|C|D|G,     // 3
    B|C|F|G,       // 4
    A|C|D|F|G,     // 5
    A|C|D|E|F|G,   // 6
    A|B|C,         // 7
    A|B|C|D|E|F|G, // 8
    A|B|C|D|F|G    // 9
};

static const uint32_t Positions[4] =
{
         POS1|POS2|POS3, // 0
    POS0     |POS2|POS3, // 1
    POS0|POS1     |POS3, // 2
    POS0|POS1|POS2       // 3
};

//=========================================================

static void seg7_setup_gpio(struct Seg7Display* seg7);

//=========================================================

int seg7_setup(struct Seg7Display* seg7, volatile uint32_t* GPIOx)
{
    if (seg7 == NULL)
        return SEG7_INV_PTR;

    if (GPIOx < GPIOA || GPIOx > GPIOF)
        return SEG7_INV_GPIO;

    seg7->display = 0U;
    seg7->number  = 0U;
    seg7->GPIOx = GPIOx;

    seg7_setup_gpio(seg7);

    return 0;
}

//---------------------------------------------------------

static void seg7_setup_gpio(struct Seg7Display* seg7)
{
    uint8_t bit = REG_RCC_AHBENR_IOPAEN + (seg7->GPIOx - GPIOA);
    bool clock_e = (bool) CHECK_BIT(REG_RCC_AHBENR, bit);

    if (clock_e == false)
    {
        SET_BIT(REG_RCC_AHBENR, bit);
    }

    // Configure mode register:
    for (uint8_t pin = 1U; pin <= 12U; pin++)
    {
        SET_GPIO_IOMODE(seg7->GPIOx, pin, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);
    }

    // Configure type register:
    for (uint8_t pin = 1U; pin <= 12U; pin++)
    {
        SET_GPIO_OTYPE(seg7->GPIOx, pin, GPIO_OTYPE_PUSH_PULL);
    }

    return;
}

//---------------------------------------------------------

int seg7_select_digit(struct Seg7Display* seg7, unsigned pos)
{
    if (seg7 == NULL)
        return SEG7_INV_PTR;

    if (pos > SEG7_MAX_POS)
        return SEG7_INV_POS;

    uint16_t divisors[4] = {1U, 10U, 100U, 1000U};
    seg7->display = Digits[(seg7->number / divisors[pos]) % 10U] | Positions[pos];

    return 0;
}

//---------------------------------------------------------

int seg7_push_display_state_to_mc(const struct Seg7Display* seg7)
{
    if (seg7 == NULL)
        return SEG7_INV_PTR;

    uint32_t surrounding_state = ~Pins_used & GPIO_ODR_READ(seg7->GPIOx);
    uint32_t to_write = Pins_used & seg7->display;
    
    GPIO_ODR_WRITE(seg7->GPIOx, surrounding_state | to_write);

    return 0;
}