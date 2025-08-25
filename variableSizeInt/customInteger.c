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

void printInteger(CustomInteger integer, Base base) {
	String repr = integerToString(integer, base);

	printf("%s\n", repr.chars);
	freeString(&repr);
}

void copyInteger(CustomIntegerPtr src, CustomIntegerPtr dest) {
	freeInteger(dest);

	*dest = copyIntegerToNew(*src);
}

void reallocToFitInteger(CustomIntegerPtr integer) {
	if (integer->capacity <= 1) {
		return;
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

void reallocInteger(CustomIntegerPtr integer, SizeT newCapacity) {
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

void setToZero(CustomIntegerPtr integer) {
	setMemory(integer->value, 0, integer->capacity);
	integer->isNegative = false;
}

void freeInteger(CustomIntegerPtr integer) {
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

void setBit(CustomIntegerPtr integer, uint8 newVal, SizeT bitPlace, SizeT bytePlace) {
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

CustomInteger Bitshift(CustomInteger integer, SizeT shift, ShiftDirection direction, bool adaptCapacity) {
	CustomInteger result;

	setMemory(&result, 0, CUSTOM_INT_SIZE);

	if (shift == 0) {
		return copyIntegerToNew(integer);
	} else if (direction == LEFT || direction == RIGHT) {

		SizeT deltaSize = shift / 8;
		SizeT resultCapacity = integer.capacity;

		if (adaptCapacity && direction == LEFT) {
			resultCapacity += deltaSize + 1;
		}

		result = allocInteger(resultCapacity);

		result.isNegative = integer.isNegative;
		result.size = result.capacity;

		setMemory(result.value, 0, result.capacity);

		for (SizeT i = integer.size; i > 0; i--) {
			SizeT j = i - 1;

			for (SizeT k = 8; k > 0; k--) {
				SizeT currentBit = j * 8 + (k - 1);

				if (direction == RIGHT && currentBit < shift) {
					break;
				}

				uint8 bitVal = GET_BIT(integer.value[j], k);

				SizeT destBit = direction == LEFT ? currentBit + shift: currentBit - shift;

				setBit(&result, bitVal, destBit, 0);
			}
		}
	} else {
		exit(EXIT_FAILURE);
	}

	for (SizeT i = result.size; i > 0; i--) {
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

void BitshiftPtr(CustomIntegerPtr integer, SizeT shift, ShiftDirection direction, bool adaptCapacity) {
	CustomInteger shifted = Bitshift(*integer, shift, direction, adaptCapacity);
	copyInteger(&shifted, integer);
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
	} else if (compareAbs(b, One) == EQUALS) {
		result = copyIntegerToNew(a);
	} else {
		freeInteger(&Zero);
		freeInteger(&One);

		result = allocInteger(a.size + b.size + 1);

		CustomInteger	*multiplier = a.size < b.size ? &a: &b,
						*multipliee = a.size < b.size ? &b: &a;

		CustomInteger carry, prod, sum, temp;

		setMemory(&carry, 0, CUSTOM_INT_SIZE);
		setMemory(&prod, 0, CUSTOM_INT_SIZE);
		setMemory(&sum, 0, CUSTOM_INT_SIZE);
		setMemory(&temp, 0, CUSTOM_INT_SIZE);

		uint8 A = 0, B = 0, CARRY = 0, PROD = 0;
		uint16 TEMP = 0;

		for (SizeT i = 0; i < multiplier->size; i++) {
			A = multiplier->value[i];

			if (A != 0) {

				CustomInteger carry = allocInteger(multipliee->size + 1);
				CustomInteger prod = allocInteger(carry.capacity);

				for (SizeT j = 0; j < multipliee->size; j++) {
					B = multipliee->value[j];

					TEMP = (uint16)A * (uint16)B;
					PROD = (uint8)(TEMP & 0xFF);
					CARRY = (uint8)(TEMP >> 8);

					prod.value[j] = PROD;
					carry.value[j+1] = CARRY;

					prod.size++;
					carry.size = prod.size + 1;
				}

				sum = addInteger(carry, prod);
				BitshiftPtr(&sum, i * 8, LEFT, true);
				freeInteger(&carry);
				freeInteger(&prod);

				temp = addInteger(result, sum);
				copyInteger(&temp, &result);
				freeInteger(&sum);
				freeInteger(&temp);
			}
		}
	}

	result.isNegative = a.isNegative ^ b.isNegative;

	return result;
}

CustomInteger divideInteger(CustomInteger a, CustomInteger b) {
	CustomInteger cmp, var, temp, result;

	bool BisNeg = b.isNegative;

	CustomInteger Zero = allocIntegerFromValue(0, false, true);
	CustomInteger One = allocIntegerFromValue(1, false, true);

	if (isZero(a) && isZero(b)) {
		freeInteger(&Zero);
		freeInteger(&One);

		fprintf(stderr, "Math error: you tried to compute 0 / 0, which is undefined\n");
		exit(EXIT_FAILURE);
	} else if (isZero(b)) {
		freeInteger(&Zero);
		freeInteger(&One);

		fprintf(stderr, "Math error: you tried to divide by 0, which is undefined\n");
		exit(EXIT_FAILURE);
	} else if (isZero(a) || (compareAbs(b, a) == GREATER)) {
		result = Zero;
		freeInteger(&One);
	} else if (compareAbs(a, b) == EQUALS) {
		result = One;
		freeInteger(&Zero);
	} else {
		freeInteger(&Zero);

		var = allocIntegerFromValue(1, false, false);

		b.isNegative = false;

		while (true) {
			freeInteger(&cmp);

			cmp = multiplyInteger(var, b);
			temp = addInteger(var, One);
			freeInteger(&var);
			copyInteger(&temp, &var);
			freeInteger(&temp);

			if (compareAbs(cmp, a) == GREATER) {
				break;
			}
		}

		result = copyIntegerToNew(var);
		freeInteger(&var);
		freeInteger(&cmp);

		b.isNegative = BisNeg;

		freeInteger(&One);
	}

	result.isNegative = a.isNegative ^ b.isNegative;

	return result;
}

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