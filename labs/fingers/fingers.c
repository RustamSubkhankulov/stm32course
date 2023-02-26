#include <stdint.h>

//---------------------------------------------------------

#include "seg7.h"
#include "button.h"
#include "inc/modregs.h"
#include "inc/gpio.h"
#include "inc/rcc.h"
#include "inc/sleep.h"

//=========================================================

static void board_clocking_init(void);
static int play_fingers_game(void);

//=========================================================

int main(void)
{
    board_clocking_init();
    return play_fingers_game();
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

static int play_fingers_game(void)
{
    struct Seg7Display seg7 = { 0 };
    
    int err = seg7_setup(&seg7, GPIOA);
    if (err < 0) return err;

    struct Button turn_off_button = { 0 };

    err = button_setup(&turn_off_button, GPIOA, 0);
    if (err < 0) return err;

    uint32_t tick = 0U;
    seg7.number = 0U;

    while (1)
    {
        err = button_update(&turn_off_button);
        if (err < 0) return err;

        bool is_pressed = button_is_pressed(&turn_off_button);
        if (is_pressed == true)
            break;

        if ((tick % 100U) == 0U)
        {
            if (seg7.number < SEG7_MAX_NUMBER)
            {
                seg7.number += 1U;
            }
        }

        err = seg7_select_digit(&seg7, (tick % 4));
        if (err < 0) return err;

        err = seg7_push_display_state_to_mc(&seg7);

        to_get_more_accuracy_pay_2202_2013_2410_3805_1ms();
        tick += 1;
    }

    return 0;
}