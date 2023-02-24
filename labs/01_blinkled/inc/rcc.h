#pragma once 

//---------------------------------------------------------

#include "modregs.h"

//=========================================================

// RESET AND CLOCK CONTROL
#define REG_RCC 0x40021000U

#define REG_RCC_CR       (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x00) // Clock Control Register
#define REG_RCC_CFGR     (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x04) // Clock configuration register
#define REG_RCC_CIR      (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x08) // Clock interrupt register
#define REG_RCC_APB2RSTR (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x0C) // APB peripheral reset register 2
#define REG_RCC_APB1RSTR (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x10) // APB peripheral reset register 1
#define REG_RCC_AHBENR   (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x14) // AHB1 Peripheral Clock Enable Register
#define REG_RCC_APB2ENR  (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x18) // APB peripheral clock enable register 2
#define REG_RCC_APB1ENR  (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x1C) // APB peripheral clock enable register 1
#define REG_RCC_BDCR     (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x20) // RTC domain control register
#define REG_RCC_CSR      (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x24) // Control/status register
#define REG_RCC_AHBRSTR  (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x28) // AHB peripheral reset register
#define REG_RCC_CFGR2    (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x2C) // Clock configuration register 2
#define REG_RCC_CFGR3    (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x30) // Clock configuration register 3
#define REG_RCC_CR2      (volatile uint32_t*)(uintptr_t)(REG_RCC + 0x34) // Clock Control Register 2

//---------------------------------------------------------

// CR

#define REG_RCC_CR_HSION  0  // HSI clock enable
#define REG_RCC_CR_HSIRDY 1  // HSI clock ready flag
// Bit 2 reserved
#define REG_RCC_CR_HSEON  16 // HSE clock enable
#define REG_RCC_CR_HSERDY 17 // HSE clock ready flag
#define REG_RCC_CR_HSEBYP 18 // HSE crystal oscillator bypass
#define REG_RCC_CR_CSSON  19 // Clock security system enable
// Bits 23:20 reserved
#define REG_RCC_CR_PLLON  24 // PLL enable
#define REG_RCC_CR_PLLRDY 25 // PLL clock ready flag

#define REG_RCC_CR_HSICAL 8  // HSI clock calibration
#define GET_REG_RCC_CR_HSICAL(value) MODIFY_REG(REG_RCC_CR, 0b1111111 << REG_RCC_CR_HSICAL, value << REG_RCC_CR_HSICAL)

#define REG_RCC_CR_HSITRIM 3
// #define GET_REG_RCC_CR_HSITRIM(value)  
// #define SET_REG_RCC_CR_HSITRIM(value) 

//---------------------------------------------------------

// AHBENR

// I/O port C clock enable
#define REG_RCC_AHBENR_IOPCEN 19 

//---------------------------------------------------------

// CFGR2

// PREDIV division factor
#define REG_RCC_CFGR2_PREDIV 0            
#define REG_RCC_CFGR2_PREDIV_DIV_2 0b0001  

//---------------------------------------------------------

// CFGR

// PLL input clock source
#define REG_RCC_CFGR_PLLSRC 15               
#define REG_RCC_CFGR_PLLSRC_HSE_PREDIV 0b10 

// PLL multiplication factor
#define REG_RCC_CFGR_PLLMUL 18         
#define REG_RCC_CFGR_PLLMUL_12 0b1010 

// HCLK prescaler
#define REG_RCC_CFGR_HPRE 4               
#define REG_RCC_CFGR_HPRE_NOT_DIV 0b0000 

// System clock switch
#define REG_RCC_CFGR_SW 0       
#define REG_RCC_CFGR_SW_PLL 10 

// System clock switch status
#define REG_RCC_CFGR_SWS 2       
#define REG_RCC_CFGR_SWS_PLL 10  

// PCLK prescaler
#define REG_RCC_CFGR_PPRE 8          
#define REG_RCC_CFGR_PPRE_DIV_2 100  