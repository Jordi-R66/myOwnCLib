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

#define BYTEPAIR_SIZE sizeof(BytePair)
#define BYTEPAIR_FREQ_SIZE sizeof(BPFreq)
#define BYTEPAIR_REPLACEMENT_SIZE sizeof(BPReplacement)

#pragma pack()

MemBlock findUnusedBytes(MemBlock memBlock);
MemBlock generateFreqList(MemBlock memblock, uint16 maxPossibilities);

MemBlock mapBytePairs(MemBlock bytePairs, MemBlock availableBytes);

MemBlock encodeBPE(MemBlock rawBytes);
MemBlock decodeBPE(MemBlock encodedBytes, MemBlock BPmap);

#endif