#pragma once

#include "../targetSpecific/types.h"

#ifndef MEMTYPES

#pragma pack(1)
typedef struct {
	SizeT size;
	ptr addr;
} MemBlock;

#define MEMBLOCK_SIZE sizeof(MemBlock);

#pragma pack()

#endif