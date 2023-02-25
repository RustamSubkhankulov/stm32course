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

void board_gpio_init()
{
    // (1) Enable GPIOC clocking:
    SET_BIT(REG_RCC_AHBENR, REG_RCC_AHBENR_IOPCEN);

    // (2) Configure PC8 & PC9 mode:
    SET_GPIO_IOMODE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_MODE_GEN_PURPOSE_OUTPUT);
    SET_GPIO_IOMODE(GPIOC, BLUE_LED_GPIOC_PIN, GPIO_MODE_GEN_PURPOSE_OUTPUT);

    // (3) Configure PC8 & PC9 type - output push/pull 
    SET_GPIO_IOTYPE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_TYPE_PUSH_PULL);
    SET_GPIO_IOTYPE(GPIOC, BLUE_LED_GPIOC_PIN, GPIO_TYPE_PUSH_PULL);
}

void totally_accurate_quantum_femtosecond_precise_super_delay_3000_100ms()
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
        SET_BIT(GPIO_ODR(GPIOC), GREEN_LED_GPIOC_PIN);
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_100ms();

        CLEAR_BIT(GPIO_ODR(GPIOC), GREEN_LED_GPIOC_PIN);
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_100ms(); 

        SET_BIT(GPIO_ODR(GPIOC), BLUE_LED_GPIOC_PIN);
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_100ms();

        CLEAR_BIT(GPIO_ODR(GPIOC), BLUE_LED_GPIOC_PIN);
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_100ms();
    }
}