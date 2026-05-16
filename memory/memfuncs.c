#include <stdlib.h>

#include "memfuncs.h"
#include "../endianness/endianness.h"

#pragma region Memory Comparison

bool equalMemory(ptr A, ptr B, SizeT n) {
	return (bool)(compareMemory(A, B, n) == EQUALS);
}

bool differentMemory(ptr A, ptr B, SizeT n) {
	return (bool)(compareMemory(A, B, n) != EQUALS);
}

bool lessThanMemory(ptr A, ptr B, SizeT n) {
	return (bool)(compareMemory(A, B, n) == LESS);
}

bool greaterThanMemory(ptr A, ptr B, SizeT n) {
	return (bool)(compareMemory(A, B, n) == GREATER);
}

Comparison compareMemory(ptr A, ptr B, SizeT n) {
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
void copyMemory(const ptr src, ptr dest, SizeT size) {
	const Byte* s8 = (const Byte*)src;
	Byte* d8 = (Byte*)dest;

	SizeT dw_count = size / DOUBLE_WORD_SIZE;
	if (dw_count > 0) {
		const DoubleWord* s_dw = (const DoubleWord*)s8;
		DoubleWord* d_dw = (DoubleWord*)d8;

		for (SizeT i = 0; i < dw_count; i++) {
			d_dw[i] = s_dw[i];
		}

		SizeT processed = dw_count * DOUBLE_WORD_SIZE;
		s8 += processed;
		d8 += processed;
		size -= processed;
	}

	SizeT w_count = size / WORD_SIZE;
	if (w_count > 0) {
		const Word* s_w = (const Word*)s8;
		Word* d_w = (Word*)d8;

		for (SizeT i = 0; i < w_count; i++) {
			d_w[i] = s_w[i];
		}

		SizeT processed = w_count * WORD_SIZE;
		s8 += processed;
		d8 += processed;
		size -= processed;
	}

	for (SizeT i = 0; i < size; i++) {
		d8[i] = s8[i];
	}
}

void setMemory(ptr memAddr, uint8 value, SizeT size) {
	Byte* d8 = (Byte*)memAddr;

	Word patternW = (Word)value;
	patternW = (patternW << 8) | patternW; // Sur 16 bits
	patternW = (patternW << 16) | patternW; // Sur 32 bits
	#ifdef _64BITS
		patternW = (patternW << 32) | patternW; // Sur 64 bits
	#endif

	DoubleWord patternDW = (DoubleWord)patternW;
	#ifdef _64BITS
		patternDW = (patternDW << 64) | patternDW; // Sur 128 bits
	#else
		patternDW = (patternDW << 32) | patternDW; // Sur 64 bits
	#endif

	SizeT dw_count = size / DOUBLE_WORD_SIZE;
	if (dw_count > 0) {
		DoubleWord* d_dw = (DoubleWord*)d8;
		for (SizeT i = 0; i < dw_count; i++) {
			d_dw[i] = patternDW;
		}

		SizeT processed = dw_count * DOUBLE_WORD_SIZE;
		d8 += processed;
		size -= processed;
	}

	SizeT w_count = size / WORD_SIZE;
	if (w_count > 0) {
		Word* d_w = (Word*)d8;
		for (SizeT i = 0; i < w_count; i++) {
			d_w[i] = patternW;
		}

		SizeT processed = w_count * WORD_SIZE;
		d8 += processed;
		size -= processed;
	}

	for (SizeT i = 0; i < size; i++) {
		d8[i] = value;
	}
}
#pragma endregion