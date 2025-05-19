#include <stdlib.h>
#include "../endianness/endianness.h"

#include "memfuncs.h"

#pragma region Memory Comparison
bool equalMemory(void* A, void* B, SizeT n) {
	return (bool)(compareMemory(A, B, n) == EQUALS);
}

bool lessThanMemory(void* A, void* B, SizeT n) {
	return (bool)(compareMemory(A, B, n) == LESS);
}

bool greaterThanMemory(void* A, void* B, SizeT n) {
	return (bool)(compareMemory(A, B, n) == GREATER);
}

Comparison compareMemory(void* A, void* B, SizeT n) {
	if ((A == NULL) || (B == NULL)) {
		exit(EXIT_FAILURE);
	}

	if (n == 0) {
		return EQUALS;
	}

	Endianness_t endian = getEndian();

	uint8* memA = (uint8*)A;
	uint8* memB = (uint8*)B;

	bool equals = true;

	SizeT j = 0;

	for (SizeT i = 0; (i < n) && equals; i++) {
		j = endian == Little ? n - i - 1 : i;
		equals &= memA[j] == memB[j];

		if (!equals) {
			return memA[j] > memB[j] ? GREATER : LESS;
		}
	}

	return EQUALS;
}
#pragma endregion

#pragma region Memory Manipulation
void copyMemory(void* src, void* dest, SizeT size) {
	uint8* a = (uint8*)src;
	uint8* b = (uint8*)dest;

	for (SizeT i = 0; i < size; i++) {
		b[i] = a[i];
	}
}

void setMemory(void* memAddr, uint8 value, SizeT bytes) {
	uint8* a = (uint8*)memAddr;

	for (SizeT i = 0; i < bytes; i++) {
		a[i] = value;
	}
}
#pragma endregion