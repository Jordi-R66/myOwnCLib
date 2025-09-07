#include "bpe.h"
#include "../memory/memfuncs.h"
#include "../bools/bool.h"

#include "../collections/lists/list.h"

const MemBlock NULL_MEMBLOCK = {0, NULL};

Comparison CompareFreqs(ptr freqA, ptr freqB, SizeT unused_var) {
	BPFreq *A = (BPFreq*)freqA, *B = (BPFreq*)freqB;

	if (A->pair == B->pair) {
		return UNDEF;
	}

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

	if (unused_bytes.addr == NULL) {
		return NULL_MEMBLOCK;
	}

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

	uint32 fastLookup[U16_MAX_VAL + 2];
	setMemory(fastLookup, 0, I32_SIZE * (U16_MAX_VAL + 1));

	uint32 pairsFound = 0;

	BytePair* dataBytePair = (BytePair*)memblock.addr;
	uint8* dataByte = (uint8*)memblock.addr;

	SizeT SizeBP = memblock.size / 2;

	for (SizeT i = 0; i < SizeBP; i++) {
		BytePair BP = dataBytePair[i];

		uint32 index = fastLookup[BP] - 1;
		bool known = index != 0;

		if (known) {
			freqs[index].freq++;
		} else {
			freqs[pairsFound] = (BPFreq){BP, 1};
			fastLookup[BP] = pairsFound - 1;
			pairsFound++;
		}
	}

	List temp;

	temp.capacity = U16_MAX_VAL + 1;
	temp.elementSize = BYTEPAIR_FREQ_SIZE;
	temp.n_elements = (SizeT)pairsFound;
	temp.flags = LIST_INITIALISED;
	temp.elements = (ptr)freqList.addr;

	if (pairsFound < maxPossibilities) {
		maxPossibilities = pairsFound;
	}

	shrinkToFit(&temp);
	sortList(&temp, CompareFreqs);
	List sliced = sliceList(&temp, 0, maxPossibilities);

	freeList(&temp);
	freqList.addr = sliced.elements;
	freqList.size = maxPossibilities * BYTEPAIR_FREQ_SIZE;

	return freqList;
}

MemBlock mapBytePairs(MemBlock bytePairs, MemBlock availableBytes) {
	MemBlock output = NULL_MEMBLOCK;

	uint8* bytes = (uint8*)availableBytes.addr;
	BPFreq* freqs = (BPFreq*)bytePairs.addr;

	if ((bytePairs.size / BYTEPAIR_SIZE) == (availableBytes.size / I8_SIZE)) {
		SizeT size = bytePairs.size / BYTEPAIR_SIZE;
		BPReplacement* replacements = (BPReplacement*)calloc(size, BYTEPAIR_REPLACEMENT_SIZE);

		output.size = size * BYTEPAIR_REPLACEMENT_SIZE;
		output.addr = (ptr)replacements;

		for (SizeT i = 0; i < size; i++) {
			BytePair pair = freqs[i].pair;
			uint8 byte = bytes[i];
			BPReplacement repl = {pair, byte};

			replacements[i] = repl;
		}
	}

	return output;
}

MemBlock encodeBPE(MemBlock rawBytes);
MemBlock decodeBPE(MemBlock encodedBytes, MemBlock BPmap);