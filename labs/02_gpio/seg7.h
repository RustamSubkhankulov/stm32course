#pragma once 

//=========================================================

#include <stdint.h>

//=========================================================

// 7-segment display

/*

      12 11 10 9 8 7          |       ______
 _____ |_|_|_|_|_|______      |      |___A__|
|  _  |  _  |  _  |  _  |     |     |‾|    |‾|
| |_| | |_| | |_| | |_| |     |     |F|    |B|
| |_| | |_| | |_| | |_| |     |     |_|____|_|
|_____|_____|_____|_____|     |      |___G__|
       | | | | | |            |     |‾|    |‾|
       1 2 3 4 5 6            |     |E|    |C|
                              |     |_|____|_|
                              |      |___D__|
                                                        
*/

// Pin Mapping:
#define A  (1 << 11)
#define B  (1 <<  7)
#define C  (1 <<  4)
#define D  (1 <<  2)
#define E  (1 <<  1)
#define F  (1 << 10)
#define G  (1 <<  5)
#define DP (1 <<  3) 

#define DIG0 (1 << 12)
#define DIG1 (1 <<  9)
#define DIG2 (1 <<  8)
#define DIG3 (1 <<  6)

#define POS0 DIG3
#define POS1 DIG2
#define POS2 DIG1
#define POS3 DIG0

//=========================================================

static const uint32_t Pins_used = (A|B|C|D|E|F|G|DP|POS0|POS1|POS2|POS3);

// TOTALLY CORRECT digit composition:
static const uint32_t Digits[10] =
{
    A|B|C|D|E|F,   // 0
    B|C,           // 1
    A|B|D|E|G,     // 2
    A|B|C|D|G,     // 3
    B|C|F|G,       // 4
    A|C|D|F|G,     // 5
    A|C|D|E|F|G,   // 6
    A|B|C,         // 7
    A|B|C|D|E|F|G, // 8
    A|B|C|D|F|G    // 9
};

static const uint32_t Positions[4] =
{
         POS1|POS2|POS3, // 0
    POS0     |POS2|POS3, // 1
    POS0|POS1     |POS3, // 2
    POS0|POS1|POS2       // 3
};

// Display state:
struct Seg7Display
{
    uint32_t display;
    uint16_t number;
};

//=========================================================

void SEG7_set_number_quarter(struct Seg7Display* seg7, unsigned tick);
void SEG7_push_display_state_to_mc(struct Seg7Display* seg7);

