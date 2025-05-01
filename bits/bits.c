#include "bits.h"

// ((graph.properties << (BOOL_BITS - DIRECTED_BIT)) >> (BOOL_BITS - 1))
uint8_t GET_BIT(uint64_t WORD, uint8_t BIT, uint8_t BIT_COUNT) {
	WORD <<= (BIT_COUNT - BIT);
	WORD >>= (BIT_COUNT - 1);

	return (uint8_t)WORD;
}