#include "bpe.h"
#include "../memory/memfuncs.h"
#include "../bools/bool.h"

#include "../collections/lists/list.h"

const MemBlock NULL_MEMBLOCK = {0, NULL};

Comparison CompareFreqs(ptr freqA, ptr freqB, SizeT unused_var) {
	BPFreq *A = freqA, *B = freqB;

	if (A->freq > B->freq) {
		return GREATER;
	} else if (A->freq < B->freq) {
		return LESS;
	} else {
		return EQUALS;
	}
}

MemBlock findUnusedBytes(MemBlock memBlock) {
	MemBlock unused_bytes = {256, NULL};

	uint8 bytes[256];
	setMemory(&bytes, true, 256);

	uint8* addr = (uint8*)memBlock.addr;

	for (SizeT i = 0; i < memBlock.size; i++) {
		uint8 byte = addr[i];

		if (bytes[byte]) {
			bytes[byte] = false;
			unused_bytes.size--;

			if (unused_bytes.size == 0) {
				return NULL_MEMBLOCK;
			}
		}
	}

	unused_bytes.addr = calloc(unused_bytes.size, I8_SIZE);

	uint16 j = 0;

	for (uint16 i = 0; i < 256; i++) {
		if (bytes[i]) {
			((uint8*)unused_bytes.addr)[j++] = (uint8)i;
		}
	}

	return unused_bytes;
}

MemBlock generateFreqList(MemBlock memblock, uint16 maxPossibilities) {
	MemBlock freqList = NULL_MEMBLOCK;

	freqList.addr = calloc(U16_MAX_VAL + 1, BYTEPAIR_FREQ_SIZE);

	BPFreq* freqs = (BPFreq*)freqList.addr;

	uint32 fastLookup[U16_MAX_VAL + 1];
	setMemory(fastLookup, 0, I32_SIZE * (U16_MAX_VAL + 1));

	uint32 pairsFound = 0;

	BytePair* dataBytePair = (BytePair*)memblock.addr;
	uint8* dataByte = (BytePair*)memblock.addr;

	SizeT SizeBP = memblock.size / 2;

	for (SizeT i = 0; i < SizeBP; i++) {
		BytePair BP = dataBytePair[i];

		uint32 index = fastLookup[BP];
		bool known = index != 0;

		if (known) {
			freqs[index].freq++;
		} else {
			freqs[pairsFound] = (BPFreq){BP, 1};
			fastLookup[BP] = pairsFound;
			pairsFound++;
		}
	}

	List temp;

	temp.capacity = U16_MAX_VAL + 1;
	temp.elementSize = BYTEPAIR_FREQ_SIZE;
	temp.n_elements = pairsFound;
	temp.fragmented = false;
	temp.initialized = true;
	temp.elements = (ptr)freqList.addr;

	if (pairsFound < maxPossibilities) {
		maxPossibilities = pairsFound;
	}

	shrinkToFit(&temp);
	sortList(&temp, CompareFreqs);
	List sliced = sliceList(&temp, 0, maxPossibilities);

	freeList(&temp);
	freqList.addr = sliced.elements;
	freqList.size = maxPossibilities;

	return freqList;
}