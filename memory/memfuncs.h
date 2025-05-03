#pragma once

#include "../bools/bool.h"

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Compares the n first bytes of A with the n first bytes of B
 * 
 * @param A Pointer to block A
 * @param B Pointer to block B
 * @param n Number of bytes to compare
 * @return true Both are the same values
 * @return false The values are different
 */
bool equalMemory(void* A, void* B, size_t n);