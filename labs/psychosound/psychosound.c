#include <stdint.h>

//---------------------------------------------------------

#include "seg7.h"
#include "button.h"
#include "inc/modregs.h"
#include "inc/gpio.h"
#include "inc/rcc.h"
#include "inc/sleep.h"
#include "buzzer.h"

//=========================================================

#define BLUE_LED_GPIOC_PIN  8U
#define GREEN_LED_GPIOC_PIN 9U

//=========================================================

static void board_clocking_init(void);
static void gpio_init_led(void);
static int enable_psychosound(void);

extern void timing_perfect_delay(uint32_t millis);

//=========================================================

int main(void)
{
    board_clocking_init();
    gpio_init_led();
    return enable_psychosound();
}

//---------------------------------------------------------

static void gpio_init_led(void)
{
    // (1) Enable GPIOC clocking:
    SET_BIT(REG_RCC_AHBENR, REG_RCC_AHBENR_IOPCEN);

    // (2) Configure PC9 mode:
    SET_GPIO_IOMODE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);

    // (3) Configure PC9 type - output push/pull 
    SET_GPIO_OTYPE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_OTYPE_PUSH_PULL);

    return;
}

//---------------------------------------------------------

static void board_clocking_init(void)
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

//---------------------------------------------------------

static int enable_psychosound(void)
{
    int err = 0;

    struct Seg7Display seg7 = { 0 };
    err = seg7_setup(&seg7, GPIOA);
    if (err < 0) return err;

    unsigned freq = 0;

    struct Buzzer buzzer = { 0 };
    err = buzzer_setup(&buzzer, GPIOC, 10, freq, 100000);
    if (err < 0) return err;

    struct Button up = { 0 };
    err = button_setup(&up, GPIOB, 6);
    if (err < 0) return err;

    struct Button down = { 0 };
    err = button_setup(&down, GPIOD, 2);
    if (err < 0) return err;

    uint32_t tick = 0U;
    seg7.number = 0U;

    // bool enable = false;

    while (1)
    {
        timing_perfect_delay(1);

        err = button_update(&up);
        if (err < 0) return err;

        err = button_update(&down);
        if (err < 0) return err;

        if ((tick % 1000) == 0)
        {
            if (button_is_pressed(&up))
            {
                if (buzzer.freq < BUZZER_MAX_FREQ - 100)
                    buzzer.freq += 100;
            }

            if (button_is_pressed(&down))
            {
                if (buzzer.freq > 100)
                    buzzer.freq -= 100;
            }
        }

        err = buzzer_do_routine(&buzzer, tick);
        if (err < 0) return err;

        seg7.number = buzzer.freq / 10;

        err = seg7_select_digit(&seg7, (tick % 4));
        if (err < 0) return err;

        err = seg7_push_display_state_to_mc(&seg7);
        tick += 1;

        // if (enable)
        // {
        //     GPIO_BSRR_SET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
        //     enable = false;
        // }
        // else 
        // {
        //     GPIO_BRR_RESET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
        //     enable = true;
        // }
    }

    return 0;
}