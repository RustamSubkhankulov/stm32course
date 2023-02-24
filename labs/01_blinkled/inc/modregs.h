#pragma once 

//=========================================================

#define SET_BIT(REG, BITNO) (*(REG) |=  (1 << (BITNO)))
#define CLEAR_BIT(REG, BITNO) (*(REG) &= ~(1 << (BITNO)))
#define CHECK_BIT(REG, BITNO) (*(REG) & (1 << BITNO))

//=========================================================

#define MODIFY_REG(REG, MODIFYMASK, VALUE) (*(REG) |= ((MODIFYMASK) & (VALUE)))
#define CHECK_REG(REG, MODIFYMASK) (*(REG) & (MODIFYMASK))
