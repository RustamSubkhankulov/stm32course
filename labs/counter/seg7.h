#pragma once 

//=========================================================

#include <stdint.h>

//=========================================================

// POSITIONS 0, 1, 2 and 3 can be shown on Seg7Display
#define SEG7_MAX_POS 3U

#define SEG7_MAX_NUMBER 9999U

// Display state:
struct Seg7Display
{
    uint32_t GPIOx;
    uint32_t display;
    
    uint16_t number;
};

enum Seg7_error
{
    SEG7_INV_PTR  = -1,
    SEG7_INV_GPIO = -2,
    SEG7_INV_POS  = -3
};

//=========================================================

// sets up gpio, pins from 1 to 12 of GPIOx will be used by Seg7Display
// WARNING: since this function changes configuration of GPIOx, 
// consider whole port is used 
int seg7_setup(struct Seg7Display* seg7, uint32_t GPIOx);

// select one particular digit of seg7->number
int seg7_select_digit(struct Seg7Display* seg7, unsigned pos);

// name of this function is pretty self-explanatory
int seg7_push_display_state_to_mc(const struct Seg7Display* seg7);

