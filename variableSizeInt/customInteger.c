#include "customInteger.h"

#include "../memory/memfuncs.h"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#pragma region Misc Operations
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

void reallocToFitInteger(custIntPtr integer) {
	if (integer->capacity <= 1) {
		fprintf(stderr, "Can't reduce the size of your integer\n");
		exit(EXIT_FAILURE);
	}

	SizeT newSize = integer->size;
	SizeT i = newSize - 1;
	uint8* bytes = (uint8*)integer->value;

	do {
		uint8 val = bytes[i];

		if ((val != 0) || (i == 0)) {
			break;
		} else {
			i--;
		}
	} while (i >= 0);

	reallocInteger(integer, i + 1);
}

void reallocInteger(custIntPtr integer, SizeT newCapacity) {
	if (newCapacity == integer->capacity) {
		return;
	}

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

String integerToString(CustomInteger integer, Base base) {
	SizeT byteLength = 0;
	SizeT divider = 0;

	string baseChars = "0123456789ABCDEF";

	switch (base) {
		case BIN:
			byteLength = 8;
			divider = 2;
			break;

		case HEX:
			byteLength = 2;
			divider = 16;
			break;

		default:
			fprintf(stderr, "Unsupported base\n");
			exit(EXIT_FAILURE);
			break;
	}

	SizeT strLength = integer.capacity * byteLength + 2;

	String obj = allocString(strLength);

	if ((base == BIN) || (base == HEX)) {
		char c;

		for (SizeT byteI = 0; byteI < integer.capacity; byteI++) {
			uint8 byte = integer.value[byteI];
			SizeT index;

			for (SizeT i = 0; i < byteLength; i++) {
				index = byte % divider;
				byte /= divider;

				c = baseChars[index];
				appendChar(&obj, c);
			}
		}

		stringLength(&obj);
		reverseString(&obj);

	} else {
		fprintf(stderr, "Unsupported base, freeing memory\n");
		freeString(&obj);
		exit(EXIT_FAILURE);
	}

	return obj;
}
#pragma endregion

#pragma region Arithmetic Operations
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

			result.value[i] += S << currentBit;
		}

		result.size++;
	}

	reallocToFitInteger(&result);

	return result;
}

CustomInteger subtractInteger(CustomInteger a, CustomInteger b) {
	CustomInteger result = allocInteger(a.capacity);

	uint8 A_BYTE, B_BYTE;

	for (SizeT i = 0; i <= a.capacity; i++) {
		result.value[i] = 0;

		for (uint8 currentBit = 0; currentBit < 8; currentBit++) {
			bool A = 0, B = 0;

			if (i < a.size) {
				A_BYTE = a.value[i];
				A = GET_BIT(A_BYTE, currentBit);
			}

			if (i < b.size) {
				B_BYTE = b.value[i];
				B = GET_BIT(B_BYTE, currentBit);
			}

			result.value[i] = (A ^ B) << currentBit;
		}
	}

	return result;
}

CustomInteger multiplyInteger(CustomInteger a, CustomInteger b);
CustomInteger divideInteger(CustomInteger a, CustomInteger b);
CustomInteger modInteger(CustomInteger a, CustomInteger b);

CustomInteger powInteger(CustomInteger a, CustomInteger exp);
#pragma endregion

#pragma region Comparison operations

#pragma endregion