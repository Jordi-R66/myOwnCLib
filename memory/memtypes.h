#pragma once

#include "../targetSpecific/types.h"

#ifndef MEMTYPES

#pragma pack(1)

typedef struct MemBlock {
	SizeT size; // Size of the block in bytes
	ptr addr; // Pointer to the stored data
} MemBlock;

typedef enum Comparison {
	LESS = 0, EQUALS = 1, GREATER = 2, UNDEF = 3
} Comparison;

// Création des fonctions de comparaison
typedef Comparison (*ComparisonFunc)(ptr, ptr, SizeT);

#define MEMBLOCK_SIZE sizeof(MemBlock)

#pragma pack()

#endif