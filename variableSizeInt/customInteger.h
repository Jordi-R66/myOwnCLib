#pragma once

#include "../targetSpecific/types.h"
#include "../bits/bits.h"
#include "../bools/bool.h"
#include "../strings/customStrings.h"

#ifndef CUSTOM_INT
#define CUSTOM_INT

#define ADD_BIT_S(A, B, C_IN) (A ^ B ^ C_IN)
#define ADD_BIT_C(A, B, C_IN) (A && B) || (A && C_IN) || (B && C_IN)

#pragma pack(1)

typedef enum {
	BIN = 2,
	OCT = 8,
	HEX = 16
} Base;

typedef struct customInt {
	uint8* value;
	SizeT size;
	SizeT capacity;
	bool isNegative;
} CustomInteger;

typedef CustomInteger* custIntPtr;

CustomInteger allocInteger(SizeT capacity);
void reallocToFitInteger(custIntPtr integer);
void reallocInteger(custIntPtr integer, SizeT newCapacity);
void freeInteger(custIntPtr integer);

CustomInteger allocIntegerFromValue(uint64 value, bool negative, bool fitToValue);

String integerToString(CustomInteger integer, Base base);

CustomInteger addInteger(CustomInteger a, CustomInteger b);
CustomInteger subtractInteger(CustomInteger a, CustomInteger b);
CustomInteger multiplyInteger(CustomInteger a, CustomInteger b);
CustomInteger divideInteger(CustomInteger a, CustomInteger b);
CustomInteger modInteger(CustomInteger a, CustomInteger b);

CustomInteger powInteger(CustomInteger a, CustomInteger exp);

#pragma pack()

#endif