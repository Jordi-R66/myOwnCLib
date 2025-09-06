#pragma once

#include "../targetSpecific/types.h"
#include "../collections/lists/list.h"
#include "../memory/memtypes.h"

#ifndef BYTE_PAIR_ENCODING
#define BYTE_PAIR_ENCODING

typedef uint16 BytePair;

#pragma pack(1)

typedef struct {
	BytePair pair;
	uint16 freq;
} BPFreq;

typedef struct {
	BytePair pair;
	uint8 replacement;
} BPReplacement;

#pragma pack()

List findUnusedBytes(MemBlock memBlock);
List generateFreqList(MemBlock memblock);

#endif