#pragma once

#define CTRL_VAL(BIT) (1 << (BIT - 1))
#define GET_BIT(WORD, BIT) ((WORD & CTRL_VAL(BIT)) == (CTRL_VAL(BIT)))
