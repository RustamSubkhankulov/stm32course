#include <stdint.h>

//---------------------------------------------------------

#include "inc/modregs.h"
#include "inc/rcc.h"
#include "inc/gpio.h"

//=========================================================

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define ONE_MILLISECOND ( (CPU_FREQENCY) / 1000U)

//---------------------------------------------------------

#define BLUE_LED_GPIOC_PIN  8
#define GREEN_LED_GPIOC_PIN 9

//=========================================================

void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    SET_BIT(REG_RCC_CR, REG_RCC_CR_HSEON);
    while (CHECK_BIT(REG_RCC_CR, REG_RCC_CR_HSERDY))
        continue;

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    MODIFY_REG(REG_RCC_CFGR2, 0b1111 << REG_RCC_CFGR2_PREDIV, REG_RCC_CFGR2_PREDIV_DIV_2 << REG_RCC_CFGR2_PREDIV);

    // (3) Select PREDIV output as PLL input (4 MHz):
    MODIFY_REG(REG_RCC_CFGR, 0b11 << REG_RCC_CFGR_PLLSRC, REG_RCC_CFGR_PLLSRC_HSE_PREDIV << REG_RCC_CFGR_PLLSRC);

    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    MODIFY_REG(REG_RCC_CFGR, 0b1111 << REG_RCC_CFGR_PLLMUL, REG_RCC_CFGR_PLLMUL_12 << REG_RCC_CFGR_PLLMUL);

    // (5) Enable PLL:
    SET_BIT(REG_RCC_CR, REG_RCC_CR_PLLON);
    while(CHECK_BIT(REG_RCC_CR, REG_RCC_CR_PLLRDY))
        continue;

    // (6) Configure AHB frequency to 48 MHz:
    MODIFY_REG(REG_RCC_CFGR, 0b1111 << REG_RCC_CFGR_HPRE, REG_RCC_CFGR_HPRE_NOT_DIV << REG_RCC_CFGR_HPRE);

    // (7) Select PLL as SYSCLK source:
    MODIFY_REG(REG_RCC_CFGR, 0b11 << REG_RCC_CFGR_SW, REG_RCC_CFGR_SW_PLL << REG_RCC_CFGR_SW);
    while(CHECK_REG(REG_RCC_CFGR, 0b11 << REG_RCC_CFGR_SWS) != REG_RCC_CFGR_SWS_PLL)
        continue;

    // (8) Set APB frequency to 24 MHz
    MODIFY_REG(REG_RCC_CFGR, 0b111 << REG_RCC_CFGR_PPRE, REG_RCC_CFGR_PPRE_DIV_2 << REG_RCC_CFGR_PPRE);
}

void board_gpio_init()
{
    // (1) Enable GPIOC clocking:
    SET_BIT(REG_RCC_AHBENR, REG_RCC_AHBENR_IOPCEN);

    // (2) Configure PC8 & PC9 mode:
    //(value 01 => General purpose output mode)
    MODIFY_REG(GPIOC_MODER, 0b11U << (2 * GREEN_LED_GPIOC_PIN), 0b01U << (2 * GREEN_LED_GPIOC_PIN)); // TODO add DSL
    MODIFY_REG(GPIOC_MODER, 0b11U << (2 * BLUE_LED_GPIOC_PIN), 0b01U << (2 * BLUE_LED_GPIOC_PIN));

    // (3) Configure PC8 & PC9 type - output push/pull 
    CLEAR_BIT(GPIOC_TYPER, GREEN_LED_GPIOC_PIN);
    CLEAR_BIT(GPIOC_TYPER, BLUE_LED_GPIOC_PIN);
}

void totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms()
{
    for (uint32_t i = 0; i < 1000U * ONE_MILLISECOND; ++i)
    {
        // Insert NOP for power consumption:
        __asm__ volatile("nop");
    }
}

int main()
{
#ifndef INSIDE_QEMU
    board_clocking_init();
#endif

    board_gpio_init();

    while (1)
    {
        SET_BIT(GPIOC_ODR, GREEN_LED_GPIOC_PIN);
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();

        CLEAR_BIT(GPIOC_ODR, GREEN_LED_GPIOC_PIN);
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms(); 

        // SET_BIT(GPIOC_ODR, BLUE_LED_GPIOC_PIN);
        // totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();

        // CLEAR_BIT(GPIOC_ODR, BLUE_LED_GPIOC_PIN);
        // totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();
    }
}