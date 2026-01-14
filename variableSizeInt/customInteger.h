#pragma once

#include "../common.h"
#include "../memory/memfuncs.h"
#include "../targetSpecific/types.h"
#include "../bits/bits.h"
#include "../bools/bool.h"
#include "../strings/customStrings.h"

#ifndef CUSTOM_INT
#define CUSTOM_INT

// Capacité max en Mots (et non plus octets)
#define MAX_CUSTOM_INT_CAPACITY ((SizeT)SIZET_MAX_VAL / WORD_SIZE)

// Initialisation vérifie si le pointeur n'est pas NULL (cast en Word*)
#define custIntInitialized(PTR) (PTR->value != (Word*)NULL)
#define ADD_BIT_S(A, B, C_IN) (A ^ B ^ C_IN)
#define ADD_BIT_C(A, B, C_IN) (A && B) || (A && C_IN) || (B && C_IN)

#pragma pack(1)

#pragma region Type Definition
typedef struct customInt {
	SizeT size;			// Taille en Mots (Words)
	SizeT capacity;		// Capacité en Mots (Words)
	Word* value;		// Tableau de uint32
	bool isNegative;
} CustomInteger, * CustomIntegerPtr;

typedef enum ShiftDirection {
	LEFT = 0,
	RIGHT = 1
} ShiftDirection;

typedef enum Base {
	BIN = 2,
	OCT = 8,
	DEC = 10,
	HEX = 16
} Base;

typedef struct div {
	CustomInteger quotient, remainder;
} EuclideanDivision;

typedef struct gcdStruct {
	CustomInteger gcd, u, v;
} Euclide;

typedef CustomInteger(*ArithmeticFunc)(CustomInteger, CustomInteger);
#pragma endregion

#pragma pack()

#define CUSTOM_INT_SIZE sizeof(CustomInteger)

#pragma region Misc Operations

/**
 * @brief Allocates space for a CustomInteger
 * * @param capacity Number of Words to allocate
 * @return CustomInteger
 */
CustomInteger allocInteger(SizeT capacity);

/**
 * @brief Frees the memory allocated for the CustomInteger
 * * @param integer Pointer to the structure to free
 */
void freeInteger(CustomIntegerPtr integer);

/**
 * @brief Reallocates the integer to the new capacity, copying data
 */
void reallocInteger(CustomIntegerPtr integer, SizeT newCapacity);

/**
 * @brief Shrinks the size and capacity to fit the actual data (removes leading zeros)
 */
void reallocToFitInteger(CustomIntegerPtr integer);

CustomInteger allocIntegerFromValue(uint64 value, bool isNegative, bool copy);
CustomInteger copyIntegerToNew(CustomInteger original);
void copyInteger(CustomIntegerPtr src, CustomIntegerPtr dest);
void printInteger(CustomInteger integer, Base base, bool alwaysPutSign);

// Bitwise Operations
bool getBit(CustomInteger integer, SizeT index);
void setBit(CustomIntegerPtr integer, bool bitValue, SizeT index);

String integerToString(CustomInteger integer, Base base, bool alwaysPutSign);

// New: Helper pour obtenir un Mot entier (et non un octet)
Word getWordFromInteger(CustomInteger integer, SizeT index);

CustomInteger Bitshift(CustomInteger integer, SizeT shift, ShiftDirection direction, bool adaptCapacity);
void BitshiftPtr(CustomIntegerPtr integer, SizeT shift, ShiftDirection direction, bool adaptCapacity);

#pragma endregion

#pragma region Arithmetic Operations

CustomInteger addInteger(CustomInteger a, CustomInteger b);
CustomInteger subtractInteger(CustomInteger a, CustomInteger b);
CustomInteger multiplyNaive(CustomInteger a, CustomInteger b);
CustomInteger multiplyKaratsuba(CustomInteger a, CustomInteger b);
CustomInteger multiplyInteger(CustomInteger a, CustomInteger b);
EuclideanDivision euclideanDivInteger(CustomInteger a, CustomInteger b);
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

#pragma region Modular Arithmetics

Euclide ExtendedEuclide(CustomInteger a, CustomInteger b);
CustomInteger gcdInteger(CustomInteger a, CustomInteger b);
CustomInteger modularInverse(CustomInteger a, CustomInteger m);
CustomInteger modPowInteger(CustomInteger base, CustomInteger exp, CustomInteger mod);

#pragma endregion

#endif