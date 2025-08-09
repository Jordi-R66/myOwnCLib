#pragma once

#include "../bools/bool.h"
#include "../targetSpecific/types.h"

// Memory comparison
#pragma region Memory Comparison

typedef enum Comparison {
	LESS = 0, EQUALS = 1, GREATER = 2, UNDEF = 3
} Comparison;

// Création des fonctions de comparaison
typedef Comparison (*ComparisonFunc)(ptr, ptr, SizeT);

/**
 * @brief Compares the n first bytes of A with the n first bytes of B
 * 
 * @param A Pointer to block A
 * @param B Pointer to block B
 * @param n Number of bytes to compare
 * @return true Both are the same values
 * @return false The values are different
 */
bool equalMemory(ptr A, ptr B, SizeT n);

bool differentMemory(ptr A, ptr B, SizeT n);

bool lessThanMemory(ptr A, ptr B, SizeT n);

bool greaterThanMemory(ptr A, ptr B, SizeT n);
/**
 * @brief Compares the n first bytes of A with the n first bytes of B
 * 
 * @param A Pointer to block A
 * @param B Pointer to block B
 * @param n Number of bytes to compare
 * @return Whether A < B, A = B, A > B
 */
Comparison compareMemory(ptr A, ptr B, SizeT n);
#pragma endregion

// Memory manipulation
#pragma region Memory Manipulation

void copyMemory(ptr src, ptr dest, SizeT size);
void setMemory(ptr memAddr, uint8 value, SizeT bytes);

#pragma endregion
