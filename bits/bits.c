#include "bits.h"

uint8_t GET_BIT(uint64_t WORD, uint8_t BIT, uint8_t BIT_COUNT) {
	uint64_t CTRL_VAL = 1 << (BIT - 1);

	WORD &= CTRL_VAL;

	return (uint8_t)(WORD == CTRL_VAL);
}