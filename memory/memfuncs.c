#include "memfuncs.h"

bool equalMemory(void* A, void* B, size_t n) {
	uint8* memA = (uint8*)A;
	uint8* memB = (uint8*)B;

	bool equals = true;

	for (size_t i = 0; (i < n) && (equals); i++) {
		equals &= memA[i] == memB[i];
	}

	return equals;
}