#include "customInteger.h"

#include "../memory/memfuncs.h"

#include <stdio.h>
#include <stdlib.h>

CustomInteger allocInteger(SizeT capacity) {
	if (capacity == 0) {
		fprintf(stderr, "Undefined behaviour for 0 bit integer\n");
		exit(EXIT_FAILURE);
	}

	CustomInteger output;

	output.capacity = capacity;
	output.size = 0;
	output.isNegative = false;

	ptr temp = calloc(capacity, I8_SIZE);

	if (temp == NULL) {
		fprintf(stderr, "Not enough space to allocate to a %zu bits integer\n", capacity * 8);
		exit(EXIT_FAILURE);
	}

	output.value = (uint8*)temp;

	return output;
}

void reallocInteger(custIntPtr integer, SizeT newCapacity) {
	ptr oldPtr = (ptr)integer->value;
	ptr newPtr = calloc(newCapacity, I8_SIZE);

	if (newPtr == NULL) {
		fprintf(stderr, "Not enough space to allocate to a %zu bits integer\n", newCapacity * 8);
		exit(EXIT_FAILURE);
	}

	copyMemory(oldPtr, newPtr, integer->size);
	setMemory(oldPtr, 0, integer->capacity);

	integer->capacity = newCapacity;
	integer->value = (uint8*)newPtr;

	free(oldPtr);
}

void freeInteger(custIntPtr integer) {
	setMemory((ptr)integer->value, 0, integer->capacity);
	free((ptr)integer->value);

	setMemory((ptr)integer, 0, sizeof(CustomInteger));
}

CustomInteger addInteger(CustomInteger a, CustomInteger b) {

	if ((a.isNegative != b.isNegative) && b.isNegative) {
		b.isNegative = false;

		return subtractInteger(a, b);
	} else if ((a.isNegative != b.isNegative) && a.isNegative) {
		a.isNegative = false;

		CustomInteger result = subtractInteger(a, b);
		result.isNegative = !result.isNegative;

		return result;
	}

	SizeT longest = a.size >= b.size ? a.size : b.size;

	bool S = false, C_IN = false, C_OUT = false;
	uint8 A_BYTE, B_BYTE;
	uint8 currentBit = 0;

	CustomInteger result = allocInteger(longest+1);

	C_OUT = 0; 

	for (SizeT i = 0; i <= longest; i++) {
		result.value[i] = 0;

		for (currentBit = 0; currentBit < 8; currentBit++) {
			C_IN = C_OUT;

			bool A = 0, B = 0;

			if (i < a.size) {
				A_BYTE = a.value[i];
				A = GET_BIT(A_BYTE, currentBit);
			}

			if (i < b.size) {
				B_BYTE = b.value[i];
				B = GET_BIT(B_BYTE, currentBit);
			}

			S = ADD_BIT_S(A, B, C_IN);
			C_OUT = ADD_BIT_C(A, B, C_IN);
			result.value[i] += S << (currentBit);
		}
	}

	result.size = longest + 1;

	while (result.size > 1 && result.value[result.size - 1] == 0) {
		result.size--;
	}

	return result;
}

CustomInteger subtractInteger(CustomInteger a, CustomInteger b) {
	
}
CustomInteger multiplyInteger(CustomInteger a, CustomInteger b);
CustomInteger divideInteger(CustomInteger a, CustomInteger b);
CustomInteger modInteger(CustomInteger a, CustomInteger b);

CustomInteger powInteger(CustomInteger a, CustomInteger exp);