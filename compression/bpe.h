#pragma once

#include "../targetSpecific/types.h"

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



#endif