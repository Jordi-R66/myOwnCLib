#include "memfuncs.h"

bool equalMemory(void* A, void* B, size_t n) {
	uint8_t* memA = (uint8_t*)A;
	uint8_t* memB = (uint8_t*)B;

	bool equals = true;

	for (size_t i = 0; (i < n) && (equals); i++) {
		equals &= memA[i] == memB[i];
	}

	return equals;
}