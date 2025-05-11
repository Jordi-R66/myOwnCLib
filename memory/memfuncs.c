#include <stdlib.h>
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