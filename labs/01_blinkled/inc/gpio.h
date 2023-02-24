#pragma once 

//---------------------------------------------------------

#include "modregs.h"

//=========================================================

// GPIO ports
#define GPIOA 0x48000000U
#define GPIOB 0x48000400U
#define GPIOC 0x48000800U
#define GPIOD 0x48000C00U
#define GPIOE 0x48001000U
#define GPIOF 0x48001400U

#define GPIO_MODER(GPIO)   (volatile uint32_t*)(uintptr_t)(GPIO + 0x00) 
#define GPIO_OTYPER(GPIO)  (volatile uint32_t*)(uintptr_t)(GPIO + 0x04) 
#define GPIO_OSPEEDR(GPIO) (volatile uint32_t*)(uintptr_t)(GPIO + 0x08) 
#define GPIO_PUPDR(GPIO)   (volatile uint32_t*)(uintptr_t)(GPIO + 0x0C) 
#define GPIO_IDR(GPIO)     (volatile uint32_t*)(uintptr_t)(GPIO + 0x10) 
#define GPIO_ODR(GPIO)     (volatile uint32_t*)(uintptr_t)(GPIO + 0x14) 
#define GPIO_BSRR(GPIO)    (volatile uint32_t*)(uintptr_t)(GPIO + 0x18) 
#define GPIO_LCKR(GPIO)    (volatile uint32_t*)(uintptr_t)(GPIO + 0x1C) 
#define GPIO_AFRL(GPIO)    (volatile uint32_t*)(uintptr_t)(GPIO + 0x20) 
#define GPIO_AFRH(GPIO)    (volatile uint32_t*)(uintptr_t)(GPIO + 0x24) 
#define GPIO_BRR(GPIO)     (volatile uint32_t*)(uintptr_t)(GPIO + 0x28) 

//---------------------------------------------------------

#define GPIO_MODE_INPUT              0b00
#define GPIO_MODE_GEN_PURPOSE_OUTPUT 0b01
#define GPIO_MODE_ALT_FUNC           0b10
#define GPIO_MODE_ANALOG_MODE        0b11

#define SET_GPIO_IOMODE(GPIO, PORT, MODE) SUPER_MODIFY_REG(GPIO_MODER(GPIO), 0b11, MODE, 2 * (PORT))
#define GET_GPIO_IOMODE(GPIO, PORT) SUPER_CHECK_REG(GPIO_MODER(GPIO), 0b11, 2 * (PORT))

//---------------------------------------------------------

#define GPIO_TYPE_PUSH_PULL  0b0
#define GPIO_TYPE_OPEN_DRAIN 0b1

#define SET_GPIO_IOTYPE(GPIO, PORT, TYPE) SUPER_MODIFY_REG(GPIO_OTYPER(GPIO), 0b1, TYPE, PORT)
#define GET_GPIO_IOTYPE(GPIO, PORT) SUPER_CHECK_REG(GPIO_OTYPER(GPIO), 0b1, PORT)
