#pragma once

#ifndef ENDIANNESS_HEADER
#include "../targetDetection/types.h"

enum Endianness {
	Little = 0,
	Big = 1
};

typedef enum Endianness Endianness_t;

/**
 * @brief Get the Endian object (either Big or Little)
 * 
 * @return Endianness_t 
 */
Endianness_t getEndian();

/**
 * @brief Swaps a data's endianness
 * 
 * @param data The pointer to the data
 * @param size The size of the data (bytes)
 */
void swapEndianness(void* data, SizeT size);
void toBigEndian(void* data, SizeT size);
void toLittleEndian(void* data, SizeT size);

#define ENDIANNESS_HEADER
#endif