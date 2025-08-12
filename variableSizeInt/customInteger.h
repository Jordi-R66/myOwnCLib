#pragma once

#include "../memory/memfuncs.h"
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
	LEFT = 0,
	RIGHT = 1
} ShiftDirection;

typedef enum {
	BIN = 2,
	HEX = 16
} Base;

typedef struct customInt {
	uint8* value;
	SizeT size;
	SizeT capacity;
	bool isNegative;
} CustomInteger;

typedef CustomInteger* custIntPtr;

#pragma pack()

#define CUSTOM_INT_SIZE sizeof(CustomInteger)

#pragma region Misc Operations

CustomInteger allocInteger(SizeT capacity);
CustomInteger copyIntegerToNew(CustomInteger original);
void copyInteger(custIntPtr src, custIntPtr dest);
void reallocToFitInteger(custIntPtr integer);
void reallocInteger(custIntPtr integer, SizeT newCapacity);
void freeInteger(custIntPtr integer);

CustomInteger allocIntegerFromValue(uint64 value, bool negative, bool fitToValue);

String integerToString(CustomInteger integer, Base base);

uint8 getByteFromInteger(CustomInteger integer, SizeT byteIndex);

#pragma endregion

void incrementIntegerByPrimitive(custIntPtr custInt, uint64 quantity);
void incrementIntegerByCustom(custIntPtr custInt, CustomInteger quantity);

#pragma region Bitwise Operations

CustomInteger BitwiseAND(CustomInteger a, CustomInteger b);
CustomInteger BitwiseOR(CustomInteger a, CustomInteger b);
CustomInteger BitwiseXOR(CustomInteger a, CustomInteger b);
CustomInteger BitwiseNOT(CustomInteger a);

uint8 getBit(CustomInteger integer, SizeT bitPlace, SizeT bytePlace);
void setBit(custIntPtr integer, uint8 newVal, SizeT bitPlace, SizeT bytePlace);


/**
 * @brief Returns a new CustomInteger object representing the shifted integer passed as argument
 * 
 * @param integer The integer to shift
 * @param shift The amount of bits to shift by
 * @param direction LEFT or RIGHT
 * @param adaptCapacity Tells the function to output a bigger integer if the bitshift would cause an overflow of the original capacity
 * @return CustomInteger 
 */
CustomInteger Bitshift(CustomInteger integer, SizeT shift, ShiftDirection direction, bool adaptCapacity);

#pragma endregion

#pragma region Arithmetic Operations

CustomInteger addInteger(CustomInteger a, CustomInteger b);
CustomInteger subtractInteger(CustomInteger a, CustomInteger b);
CustomInteger multiplyInteger(CustomInteger a, CustomInteger b);
CustomInteger divideInteger(CustomInteger a, CustomInteger b);
CustomInteger modInteger(CustomInteger a, CustomInteger b);

CustomInteger powInteger(CustomInteger a, CustomInteger exp);

#pragma endregion

#pragma region Comparison operations

Comparison compareAbs(CustomInteger a, CustomInteger b);
Comparison compareIntegers(CustomInteger a, CustomInteger b);
bool isZero(CustomInteger integer);
bool equalsInteger(CustomInteger a, CustomInteger b);
bool differentFromInteger(CustomInteger a, CustomInteger b);
bool lessThanInteger(CustomInteger a, CustomInteger b);
bool greaterThanInteger(CustomInteger a, CustomInteger b);

#pragma endregion

#endif