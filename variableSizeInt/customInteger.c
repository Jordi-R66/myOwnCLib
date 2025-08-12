#include "customInteger.h"

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

CustomInteger copyIntegerToNew(CustomInteger original) {
	CustomInteger output = allocInteger(original.capacity);

	output.size = original.size;
	output.isNegative = original.isNegative;

	copyMemory(original.value, output.value, output.size);

	return output;
}

void copyInteger(custIntPtr src, custIntPtr dest) {
	freeInteger(dest);

	*dest = copyIntegerToNew(*src);
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
	if (integer->value == NULL) {
		return;
	}

	setMemory((ptr)integer->value, 0, integer->capacity);
	free((ptr)integer->value);

	setMemory((ptr)integer, 0, CUSTOM_INT_SIZE);
}

String integerToString(CustomInteger integer, Base base) {
	SizeT byteLength = 0;
	SizeT divider = (SizeT)base;

	string baseChars = "0123456789ABCDEF";

	switch (base) {
		case BIN:
			byteLength = 8;
			break;

		case HEX:
			byteLength = 2;
			break;

		default:
			fprintf(stderr, "Unsupported base\n");
			exit(EXIT_FAILURE);
			break;
	}

	SizeT strLength = integer.capacity * byteLength + 2;

	String obj = allocString(strLength);

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

	char sign = integer.isNegative ? '-' : '+';

	appendChar(&obj, sign);

	stringLength(&obj);
	reverseString(&obj);

	return obj;
}

CustomInteger allocIntegerFromValue(uint64 value, bool negative, bool fitToValue) {
	SizeT capacity = sizeof(uint64);
	SizeT size = 1;

	while (value >> (size * 8)) {
		size++;
	}

	if (fitToValue) {
		capacity = size;
	}

	CustomInteger output = allocInteger(capacity);

	output.size = size;

	output.isNegative = negative;

	for (SizeT i = 0; i < size; i++) {
		output.value[i] = (uint8)((value >> (i * 8)) % 256);
	}

	return output;
}

uint8 getByteFromInteger(CustomInteger integer, SizeT byteIndex) {
	if (byteIndex >= integer.size || byteIndex >= integer.capacity) {
		return 0;
	}

	return integer.value[byteIndex];
}

#pragma endregion

#pragma region Unary Operations

void incrementIntegerByPrimitive(custIntPtr custInt, uint64 quantity) {
	CustomInteger quant = allocIntegerFromValue(quantity, false, true);
	incrementIntegerByCustom(custInt, quant);

	freeInteger(&quant);
}

void incrementIntegerByCustom(custIntPtr custInt, CustomInteger quantity) {
	CustomInteger temp = addInteger(*custInt, quantity);
	freeInteger(custInt);
	copyInteger(&temp, custInt);
	freeInteger(&temp);
}

#pragma endregion

#pragma region Bitwise Operations

CustomInteger BitwiseAND(CustomInteger a, CustomInteger b) {
	CustomInteger result;

	SizeT smallest = a.size < b.size ? a.size : b.size;

	result = allocInteger(smallest);

	for (SizeT i = 0; i < smallest; i++) {
		result.value[i] = a.value[i] & b.value[i];

		result.size++;
	}

	return result;
}

CustomInteger BitwiseOR(CustomInteger a, CustomInteger b) {
	CustomInteger result;

	SizeT smallest = a.size < b.size ? a.size : b.size;

	result = allocInteger(smallest);

	for (SizeT i = 0; i < smallest; i++) {
		result.value[i] = a.value[i] | b.value[i];

		result.size++;
	}

	return result;
}

CustomInteger BitwiseXOR(CustomInteger a, CustomInteger b) {
	CustomInteger result;

	SizeT smallest = a.size < b.size ? a.size : b.size;

	result = allocInteger(smallest);

	for (SizeT i = 0; i < smallest; i++) {
		result.value[i] = a.value[i] ^ b.value[i];

		result.size++;
	}

	return result;
}

CustomInteger BitwiseNOT(CustomInteger a) {
	CustomInteger result;

	result = allocInteger(a.capacity);

	for (SizeT i = 0; i < a.capacity; i++) {
		result.value[i] = ~a.value[i];

		result.size++;
	}

	return result;
}

uint8 getBit(CustomInteger integer, SizeT bitPlace, SizeT bytePlace) {
	SizeT bytes = bytePlace + BYTES_FROM_BITS(bitPlace);

	SizeT bit = bitPlace % 8;

	if (bytes >= integer.capacity) {
		return 0;
	} 

	return GET_BIT(integer.value[bytes], bit+1);
}

void setBit(custIntPtr integer, uint8 newVal, SizeT bitPlace, SizeT bytePlace) {
	newVal %= 2;

	SizeT bytes = bytePlace + BYTES_FROM_BITS(bitPlace);
	SizeT bit = bitPlace % 8;

	if ((integer->capacity > bytes) && (bytes >= integer->size)) {
		integer->size = bytes + 1;
	} else if (bytes >= integer->capacity) {
		return;
	}

	uint8 curVal = getBit(*integer, bit, bytes);

	if (curVal ^ newVal) {
		if (curVal == 1) {
			integer->value[bytes] -= CTRL_VAL(bit+1);
		} else {
			integer->value[bytes] += CTRL_VAL(bit+1);
		}
	}

	return;
}

CustomInteger Bitshift(CustomInteger integer, SizeT bits, ShiftDirection direction) {
	if (bits == 0) {
		return copyIntegerToNew(integer);
	}

	CustomInteger result;

	bool foundNewSize = false;
	SizeT j = 0;

	result = allocInteger(integer.capacity);

	result.isNegative = integer.isNegative;
	result.size = result.capacity;

	if (direction == LEFT || direction == RIGHT) {
		for (SizeT i = integer.size; i > 0; i--) {
			SizeT j = i - 1;
			result.value[j] = 0;

			for (SizeT k = 8; k > 0; k--) {
				SizeT currentBit = j * 8 + (k - 1);

				if (direction == RIGHT && currentBit < bits) {
					break;
				}

				uint8 bitVal = GET_BIT(integer.value[j], k);

				SizeT destBit = direction == LEFT ? currentBit + bits: currentBit - bits;

				setBit(&result, bitVal, destBit, 0);
			}
		}
	} else {
		exit(EXIT_FAILURE);
	}

	for (SizeT i = result.capacity; i > 0; i++) {
		SizeT j = i - 1;

		if (result.value[j] == 0) {
			result.size = i;
		} else {
			break;
		}
	}

	reallocToFitInteger(&result);

	return result;
}

#pragma endregion

#pragma region Arithmetic Operations

CustomInteger addInteger(CustomInteger a, CustomInteger b) {
	if (b.isNegative) {
		b.isNegative = false;

		return subtractInteger(a, b);
	} else if ((a.isNegative != b.isNegative) && a.isNegative) {
		a.isNegative = false;

		return subtractInteger(b, a);
	}

	SizeT longest = a.size >= b.size ? a.size : b.size;

	uint8 SUM = 0, CARRY = 0;
	uint8 A_BYTE, B_BYTE;

	uint16 TEMP = 0;

	CustomInteger result = allocInteger(longest+1);

	for (SizeT i = 0; i <= longest; i++) {
		result.value[i] = 0;

		A_BYTE = getByteFromInteger(a, i);
		B_BYTE = getByteFromInteger(b, i);

		TEMP = A_BYTE + B_BYTE + CARRY;

		SUM = (uint8)(TEMP & 0xFF);
		CARRY = (uint8)(TEMP > 0xFF);

		result.value[i] = SUM;

		result.size++;

		if (i == SIZET_MAX_VAL) {
			break;
		}
	}

	reallocToFitInteger(&result);

	return result;
}

CustomInteger subtractInteger(CustomInteger a, CustomInteger b) {
	CustomInteger result;

	CustomInteger Zero = allocIntegerFromValue(0, false, true);

	bool redirected = false;
	bool returnedZero = false;

	if (b.isNegative && !redirected) {
		b.isNegative = false;
		result = addInteger(a, b);
		redirected = true;
	} else if (equalsInteger(a, b)) {
		Zero.isNegative = a.isNegative;
		result = Zero;
		redirected = true;
		returnedZero = true;
	} else if (isZero(a) && !redirected) {
		result = copyIntegerToNew(b);
		result.isNegative = !result.isNegative;
		redirected = true;
	} else if (isZero(b) && !redirected) {
		result = copyIntegerToNew(a);
		redirected = true;
	} else if (!redirected && (a.isNegative != b.isNegative)) {
		if (lessThanInteger(a, b)) {
			a.isNegative = false;
			result = addInteger(a, b);
			result.isNegative = true;
			redirected = true;
		}
	} else if (!redirected && (a.isNegative == b.isNegative)) {
		if (!a.isNegative && lessThanInteger(a, b)) {
			redirected = true;

			result = subtractInteger(b, a);
			result.isNegative = !result.isNegative;
		}
	}

	if (!redirected && (a.isNegative == b.isNegative) && (a.isNegative == false) && greaterThanInteger(a, b)) {
		SizeT longest = a.size >= b.size ? a.size : b.size;

		a = copyIntegerToNew(a);
		b = copyIntegerToNew(b);

		reallocInteger(&a, longest);
		reallocInteger(&b, longest);

		result = allocInteger(longest+1);

		uint8 BORROW = 0;
		uint8 A, B;

		for (SizeT i = 0; i <= longest; i++) {
			A = getByteFromInteger(a, i);
			B = getByteFromInteger(b, i);

			result.value[i] = A - B - BORROW;

			BORROW = ((A <= B && BORROW) || (A < B));

			result.size++;

			if (i == SIZET_MAX_VAL) {
				break;
			}
		}

		reallocToFitInteger(&result);

		freeInteger(&a);
		freeInteger(&b);
	} else if (!redirected) {
		exit(EXIT_FAILURE);
	}

	if (!returnedZero) {
		freeInteger(&Zero);
	}

	return result;
}

CustomInteger multiplyInteger(CustomInteger a, CustomInteger b) {
	CustomInteger result;

	CustomInteger Zero = allocIntegerFromValue(0, false, true);
	CustomInteger One = allocIntegerFromValue(1, false, true);

	if (isZero(a) || isZero(b)) {
		result = Zero;
	} else if (compareAbs(a, One) == EQUALS) {
		result = copyIntegerToNew(b);

		if (a.isNegative) {
			result.isNegative = !result.isNegative;
		}
	} else if (compareAbs(b, One) == EQUALS) {
		result = copyIntegerToNew(a);

		if (b.isNegative) {
			result.isNegative = !result.isNegative;
		}
	} else {
		freeInteger(&Zero);

		result = allocInteger(a.size + b.size + 1);

		uint8 A_BYTE = 0, B_BYTE = 0, CARRY = 0;
		uint16 TEMP = 0;

		for (SizeT i = 0; i < a.size; i++) {
			A_BYTE = getByteFromInteger(a, i);

			result.value[i] = 0;

			for (SizeT j = 0; j < b.size; j++) {
				B_BYTE = getByteFromInteger(b, j);

				TEMP = A_BYTE * B_BYTE + CARRY;
				CARRY = TEMP >> 8;

				uint8 VAL = (TEMP << 8) >> 8;

				result.value[i] += VAL;
			}
		}
	}

	return result;
}

CustomInteger divideInteger(CustomInteger a, CustomInteger b);
CustomInteger modInteger(CustomInteger a, CustomInteger b);

CustomInteger powInteger(CustomInteger a, CustomInteger exp);
#pragma endregion

#pragma region Comparison operations

Comparison compareAbs(CustomInteger a, CustomInteger b) {
	SizeT highestCapacity = a.capacity >= b.capacity ? a.capacity : b.capacity;

	uint8 A, B;
	bool different = false;

	for (SizeT i = highestCapacity; i > 0; i--) {
		SizeT index = i - 1;

		A = getByteFromInteger(a, index);
		B = getByteFromInteger(b, index);

		different = A != B;

		if (different && A > B) {
			return GREATER;
		} else if (different) {
			return LESS;
		}
	}

	if (!different) {
		return EQUALS;
	} else {
		return UNDEF;
	}
}

Comparison compareIntegers(CustomInteger a, CustomInteger b) {
	if (equalsInteger(a, b)) {
		return EQUALS;
	} else if (lessThanInteger(a, b)) {
		return LESS;
	} else if (greaterThanInteger(a, b)) {
		return GREATER;
	} else {
		return UNDEF;
	}
}

bool isZero(CustomInteger integer) {
	bool output = true;

	for (SizeT i = 0; i < integer.size; i++) {

		output = getByteFromInteger(integer, i) == 0;

		if (i == SIZET_MAX_VAL || !output) {
			break;
		}
	}

	return output;
}

bool equalsInteger(CustomInteger a, CustomInteger b) {
	Comparison compAbs = compareAbs(a, b);

	return (compAbs == EQUALS && (a.isNegative == b.isNegative)) || (isZero(a) && isZero(b));
}

bool lessThanInteger(CustomInteger a, CustomInteger b) {
	bool less = compareAbs(a, b) == LESS;
	bool equal = compareAbs(a, b) == EQUALS;

	bool equalSigns = a.isNegative == b.isNegative;

	/*if (equalSigns) {
		if (a.isNegative && !less && !equal) {
			return true;
		} else if (!a.isNegative && less) {
			return true;
		}
	} else {
		if (a.isNegative) {
			return true;
		}
	}*/

	return (!equalSigns && a.isNegative) || (equalSigns && !a.isNegative && less) || (a.isNegative && !less && !equal);
}

bool greaterThanInteger(CustomInteger a, CustomInteger b) {
	bool greater = compareAbs(a, b) == GREATER;
	bool equal = compareAbs(a, b) == EQUALS;

	bool equalSigns = a.isNegative == b.isNegative;

	/*if (!equalSigns) {
		if (b.isNegative) {
			return true;
		}
	} else {
		if (a.isNegative && !greater && !equal) {
			return true;
		} else if (!a.isNegative && greater) {
			return true;
		}
	}*/

	return (!a.isNegative && (!equalSigns || greater)) || ( a.isNegative &&  equalSigns && !greater && !equal);
}

#pragma endregion