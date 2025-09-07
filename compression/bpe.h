#pragma once

#include "../targetSpecific/types.h"
#include "../collections/lists/list.h"
#include "../memory/memtypes.h"

#ifndef BYTE_PAIR_ENCODING
#define BYTE_PAIR_ENCODING

typedef uint16 BytePair;

#pragma pack(1)

typedef struct BPFreq {
	BytePair pair;
	SizeT freq;
} BPFreq;

typedef struct BPReplacement {
	BytePair pair;
	Byte replacement;
} BPReplacement;

typedef struct Encoded {
	MemBlock Replacements; // A memory block filled with BPReplacement objects
	MemBlock encodedData; // The actual data
} Encoded;

#define BYTEPAIR_SIZE sizeof(BytePair)
#define BYTEPAIR_FREQ_SIZE sizeof(BPFreq)
#define BYTEPAIR_REPLACEMENT_SIZE sizeof(BPReplacement)

#define ENCODED_SIZE sizeof(Encoded)

#pragma pack()

MemBlock findUnusedBytes(MemBlock rawBlock);
MemBlock generateFreqList(MemBlock rawBlock, SizeT maxPossibilities);

MemBlock mapBytePairs(MemBlock bytePairs, MemBlock availableBytes);

Encoded encodeBPE(MemBlock rawBytes);
MemBlock decodeBPE(Encoded encodedData);

#endif