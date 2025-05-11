#pragma once

#include "../bools/bool.h"
#include "../targetDetection/types.h"

typedef enum Comparison {
	LOWER, EQUALS, GREATER
} Comparison;

/**
 * @brief Compares the n first bytes of A with the n first bytes of B
 * 
 * @param A Pointer to block A
 * @param B Pointer to block B
 * @param n Number of bytes to compare
 * @return true Both are the same values
 * @return false The values are different
 */
bool equalMemory(void* A, void* B, SizeT n);

/**
 * @brief Compares the n first bytes of A with the n first bytes of B
 * 
 * @param A Pointer to block A
 * @param B Pointer to block B
 * @param n Number of bytes to compare
 * @return Whether A > B, A = B, A < B
 */
Comparison compareMemory(void* A, void* B, SizeT n);