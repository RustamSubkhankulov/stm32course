#pragma once 

//---------------------------------------------------------

#include "modregs.h"

//=========================================================

// GPIO port C registers
#define GPIOC 0x48000800U

// GPIO port mode register
#define GPIOC_MODER (volatile uint32_t*)(uintptr_t)(GPIOC + 0x00) 

// GPIO port output type register
#define GPIOC_TYPER (volatile uint32_t*)(uintptr_t)(GPIOC + 0x04) 

// GPIO port output data register
#define GPIOC_ODR   (volatile uint32_t*)(uintptr_t)(GPIOC + 0x14) 
