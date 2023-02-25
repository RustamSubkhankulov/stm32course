#include <stdint.h>
#include <stdbool.h>

//---------------------------------------------------------

#include "inc/seg7.h"
#include "inc/gpio.h"
#include "inc/rcc.h"

//=========================================================

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define ONE_MILLISECOND CPU_FREQENCY / 1000U

#define BUTTON_PIN 0

//=========================================================

static void board_clocking_init();
static void to_get_more_accuracy_pay_2202_2013_2410_3805_1ms();
static void board_gpio_init();

//=========================================================

static void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    SET_BIT(REG_RCC_CR, REG_RCC_CR_HSEON);
    while (CHECK_BIT(REG_RCC_CR, REG_RCC_CR_HSERDY))
        continue;

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    SET_REG_RCC_CFGR2_PREDIV(2);

    // (3) Select PREDIV output as PLL input (4 MHz):
    SET_REG_RCC_CFGR_PLLSRC(REG_RCC_CFGR_PLLSRC_HSE_PREDIV);

    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    SET_REG_RCC_CFGR_PLLMUL(12);

    // (5) Enable PLL:
    SET_BIT(REG_RCC_CR, REG_RCC_CR_PLLON);
    while(CHECK_BIT(REG_RCC_CR, REG_RCC_CR_PLLRDY))
        continue;

    // (6) Configure AHB frequency to 48 MHz:
    SET_REG_RCC_CFGR_HPRE_NOT_DIV();

    // (7) Select PLL as SYSCLK source:
    SET_REG_RCC_CFGR_SW(REG_RCC_CFGR_SW_PLL);

    while(GET_REG_RCC_CFGR_SWS() != REG_RCC_CFGR_SWS_PLL)
        continue;

    // (8) Set APB frequency to 24 MHz
    SET_REG_RCC_CFGR_PPRE(REG_RCC_CFGR_PPRE_DIV_2);
}

static void to_get_more_accuracy_pay_2202_2013_2410_3805_1ms()
{
    for (uint32_t i = 0; i < ONE_MILLISECOND/3U; ++i)
    {
        // Insert NOP for power consumption:
        __asm__ volatile("nop");
    }
}

//--------------------
// GPIO configuration
//--------------------

static void board_gpio_init()
{
    // (1) Configure PA1-PA12 as output:
    SET_BIT(REG_RCC_AHBENR, REG_RCC_AHBENR_IOPAEN);

    // Configure mode register:
    for (unsigned pin = 1; pin <= 12; pin++)
    {
        SET_GPIO_IOMODE(GPIOA, pin, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);
    }

    // Configure type register:
    for (unsigned pin = 1; pin <= 12; pin++)
    {
        SET_GPIO_OTYPE(GPIOA, pin, GPIO_OTYPE_PUSH_PULL);
    }

    // (2) Configure PA0 as button:
    SET_GPIO_IOMODE(GPIOA, BUTTON_PIN, GPIO_IOMODE_INPUT);

    // Configure PA0 as pull-down pin:
    SET_GPIO_PUPD(GPIOA, BUTTON_PIN, GPIO_PUPD_PD);
}

//------
// Main
//------

int main()
{
    board_clocking_init();
    board_gpio_init();

    // Init display rendering:
    struct Seg7Display seg7 =
    {
        .number = 0
    };

    uint32_t tick = 0;
    bool button_was_pressed = 0U;
    uint32_t saturation = 0U;
    while (1)
    {
        // Update button state:
        bool active = GPIO_IDR_GET_PIN(GPIOA, 0);

        if (active)
        {
            if (saturation < 5U)
            {
                saturation += 1U;
            }
            else
            {
                button_was_pressed = 1U;
            }
        }
        else
        {
            saturation = 0U;
        }

        // Update display state:
        if (!button_was_pressed && (tick % 100U) == 0U)
        {
            if (seg7.number < 9999U)
            {
                seg7.number = seg7.number + 1U;
            }
        }

        // Render display state:
        SEG7_set_number_quarter(&seg7, tick);

        SEG7_push_display_state_to_mc(&seg7);

        // Adjust ticks every ms:
        to_get_more_accuracy_pay_2202_2013_2410_3805_1ms();
        tick += 1;
    }
}
