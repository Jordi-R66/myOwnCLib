#include <stdlib.h>
#include "../endianness/endianness.h"

#include "memfuncs.h"

bool equalMemory(void* A, void* B, SizeT n) {

	if ((A == NULL) || (B == NULL)) {
		exit(EXIT_FAILURE);
	}

	uint8* memA = (uint8*)A;
	uint8* memB = (uint8*)B;

	bool equals = true;

	for (SizeT i = 0; (i < n) && (equals); i++) {
		equals &= memA[i] == memB[i];
	}

	return equals;
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