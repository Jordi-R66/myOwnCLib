#pragma once

#include "../targetSpecific/types.h"

#ifndef MEMTYPES

#pragma pack(1)

typedef struct MemBlock {
	SizeT size; // Size of the block in bytes
	ptr addr; // Pointer to the stored data
} MemBlock, *MemBlockPtr;

#define MEMBLOCK_SIZE sizeof(MemBlock)

#pragma pack()

#endif