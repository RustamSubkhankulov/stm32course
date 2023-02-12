#include <stdint.h>

//-----------------
// Useful define's
//-----------------

#define SET_BIT(REG, BITNO) (*(REG) |=  (1 << (BITNO)))
#define CLEAR_BIT(REG, BITNO) (*(REG) &= ~(1 << (BITNO)))
#define CHECK_BIT(REG, BITNO) (*(REG) & (1 << BITNO))

#define MODIFY_REG(REG, MODIFYMASK, VALUE) (*(REG) |= ((MODIFYMASK) & (VALUE)))

//---------------
// RCC Registers
//---------------

#define REG_RCC 0x40021000U

#define REG_RCC_CR     (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x00) // Clock Control Register
#define REG_RCC_CFGR   (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x04) // PLL Configuration Register
#define REG_RCC_AHBENR (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x14) // AHB1 Peripheral Clock Enable Register
#define REG_RCC_CFGR2  (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x2C) // Clock configuration register 2

#define REG_RCC_CR_HSEON 16
#define REG_RCC_CR_HSERDY 17

#define REG_RCC_AHBENR_IOPCEN 19

//----------------
// GPIO Registers
//----------------

#define GPIOC 0x48000800U

#define GPIOC_MODER (volatile uint32_t*)(uintptr_t)(GPIOC + 0x00) // GPIO port mode register
#define GPIOC_TYPER (volatile uint32_t*)(uintptr_t)(GPIOC + 0x04) // GPIO port output type register
#define GPIOC_ODR   (volatile uint32_t*)(uintptr_t)(GPIOC + 0x14) // GPIO port output data register

//------
// Main
//------

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define ONE_MILLISECOND CPU_FREQENCY/1000U

//------
// LEDs
//------

#define BLUE_LED_GPIOC_PIN  8
#define GREEN_LED_GPIOC_PIN 9

void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    SET_BIT(REG_RCC_CR, REG_RCC_CR_HSEON);
    while (CHECK_BIT(REG_RCC_CR, REG_RCC_CR_HSERDY))
        continue;

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    *REG_RCC_CFGR2 |= 1U;

    // (3) Select PREDIV output as PLL input (4 MHz):
    *REG_RCC_CFGR |= 0x00010000U;

    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    *REG_RCC_CFGR |= (12U-2U) << 18U;

    // (5) Enable PLL:
    *REG_RCC_CR |= 0x01000000U;
    while ((*REG_RCC_CR & 0x02000000U) != 0x02000000U);

    // (6) Configure AHB frequency to 48 MHz:
    *REG_RCC_CFGR |= 0b000U << 4U;

    // (7) Select PLL as SYSCLK source:
    *REG_RCC_CFGR |= 0b10U;
    while ((*REG_RCC_CFGR & 0xCU) != 0x8U);

    // (8) Set APB frequency to 24 MHz
    *REG_RCC_CFGR |= 0b001U << 8U;
}

void board_gpio_init()
{
    // (1) Enable GPIOC clocking:
    SET_BIT(REG_RCC_AHBENR, REG_RCC_AHBENR_IOPCEN);

    // (2) Configure PC8 & PC9 mode:
    //(value 01 => General purpose output mode)
    MODIFY_REG(GPIOC_MODER, 0b11U << (2 * GREEN_LED_GPIOC_PIN), 0b01U << (2 * GREEN_LED_GPIOC_PIN));
    MODIFY_REG(GPIOC_MODER, 0b11U << (2 * BLUE_LED_GPIOC_PIN), 0b01U << (2 * BLUE_LED_GPIOC_PIN));

    // (3) Configure PC8 & PC9 type:
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
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms(); // TODO: XOR

        SET_BIT(GPIOC_ODR, BLUE_LED_GPIOC_PIN);
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();

        CLEAR_BIT(GPIOC_ODR, BLUE_LED_GPIOC_PIN);
        totally_accurate_quantum_femtosecond_precise_super_delay_3000_1000ms();
    }
}
