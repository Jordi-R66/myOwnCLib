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

MemBlock findUnusedBytes(MemBlock rawBlock) {
	MemBlock output = NULL_MEMBLOCK;

	/*List unused = {0};
	initializeList(&unused, 256, BYTE_SIZE);

	for (unused.n_elements = 0; unused.n_elements < 256; unused.n_elements++) {
		unused.elements[unused.n_elements] = (Byte)unused.n_elements;
	}

	printf("Cap. %zu | Items : %zu\n", unused.capacity, unused.n_elements);

	fwrite(unused.elements, unused.elementSize, unused.capacity, fp);

	Byte* addr = (Byte*)rawBlock.addr;

	for (SizeT i = 0; (output.size > 0) && (i < rawBlock.size); i++) {
		Byte byte = addr[i];

		if (contains(&unused, &byte)) {
			removeElement(&unused, foundAtPosition);
			output.size--;
		}
	}

	printf("output.size = %zu\n", output.size);*/

	SizeT count = 256;
	static bool usedBytes[256]; // If a byte is used, usedBytes[byteVal] will be set to true

	for (SizeT i = 0; i < 256; i++) {
		usedBytes[i] = false;
	}

	for (SizeT i = 0; i < rawBlock.size; i++) {
		Byte byteVal = rawBlock.addr[i];

		if (!usedBytes[byteVal]) {
			usedBytes[byteVal] = true;
			count--;
		}
	}

	if (count == 0) {
		return NULL_MEMBLOCK;
	}

	ptr unusedBytes = (ptr)calloc(count, BYTE_SIZE);

	if (unusedBytes == (ptr)NULL) {
		return NULL_MEMBLOCK;
	}

	SizeT i = 0, j = 0;

	while ((i < count) && (j < 256)) {
		if (!usedBytes[j]) {
			unusedBytes[i] = (Byte)j;
			i++;
		}

		j++;
	}

	output.size = count;
	output.addr = unusedBytes;

	return output;
}

MemBlock generateFreqList(MemBlock rawBlock, SizeT maxPossibilities) {
	MemBlock output = NULL_MEMBLOCK;

	SizeT nPairs = rawBlock.size / 2;
	BytePair* rawPairs = (BytePair*)rawBlock.addr;

	List FreqList;
	setMemory((ptr)&FreqList, 0x00, LIST_SIZE);

	initializeList(&FreqList, 65536, BYTEPAIR_FREQ_SIZE);

	for (SizeT i = 0; i < 65536; i++) {
		FreqList.n_elements++;
		BPFreq* Freq = (BPFreq*)getElement(&FreqList, i);
		Freq->pair = (BytePair)i;
		Freq->freq = 0;
	}

	for (SizeT i = 0; i < nPairs; i++) {
		BytePair bp = rawPairs[i];

		BPFreq* Freq = (BPFreq*)getElement(&FreqList, (SizeT)bp);
		Freq->freq += 1;
	}

	for (SizeT i = 0; (i < 65536) && (i < FreqList.n_elements); i++) {
		BPFreq* Freq = (BPFreq*)getElement(&FreqList, (SizeT)i);

		if (Freq->freq == 0) {
			removeElement(&FreqList, i);
			i--;
		}
	}

	sortList(&FreqList, CompareFreqs);
	reverseList(&FreqList);

	if (maxPossibilities < FreqList.n_elements) {
		List finalList = sliceList(&FreqList, 0, maxPossibilities);
		freeList(&FreqList);
		FreqList = finalList;
	}

	output.addr = FreqList.elements;
	output.size = FreqList.n_elements;

	setMemory((ptr)&FreqList, 0, LIST_SIZE);

	return output;
}

MemBlock mapBytePairs(MemBlock bytePairs, MemBlock availableBytes) {
	MemBlock output = NULL_MEMBLOCK;

	uint8* bytes = (uint8*)availableBytes.addr;
	BPFreq* freqs = (BPFreq*)bytePairs.addr;

	if ((bytePairs.size / BYTEPAIR_FREQ_SIZE) == (availableBytes.size / I8_SIZE)) {
		SizeT size = bytePairs.size / BYTEPAIR_FREQ_SIZE;
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

Encoded encodeBPE(MemBlock rawBytes) {
	Encoded output = {NULL_MEMBLOCK, NULL_MEMBLOCK};
	MemBlock unusedBytesBlock = NULL_MEMBLOCK, freqsBlock = NULL_MEMBLOCK, mapBlock = NULL_MEMBLOCK, encodedData = NULL_MEMBLOCK;

	unusedBytesBlock = findUnusedBytes(rawBytes);

	if (compareMemory((ptr)&unusedBytesBlock, (ptr)&NULL_MEMBLOCK, MEMBLOCK_SIZE) == EQUALS) {
		printf("unused is NULL\n");
		return output;
	}

	freqsBlock = generateFreqList(rawBytes, unusedBytesBlock.size);
	mapBlock = mapBytePairs(freqsBlock, unusedBytesBlock);

	BPFreq* freqs = (BPFreq*)freqsBlock.addr;
	BytePair* rawPairs = (BytePair*)rawBytes.addr;
	SizeT rawPairsCount = rawBytes.size / 2;

	BPReplacement* mappings = (BPReplacement*)mapBlock.addr;
	SizeT mappingCount = mapBlock.size / BYTEPAIR_REPLACEMENT_SIZE;

	SizeT finalSize = rawBytes.size;

	for (SizeT i = 0; i < mappingCount; i++) {
		finalSize -= freqs[i].freq;
		printf("BytePair : 0x%X\nCount : %zu\n", freqs[i].pair, freqs[i].freq);
	}

	free(unusedBytesBlock.addr);
	free(freqsBlock.addr);

	unusedBytesBlock.size = 0;
	unusedBytesBlock.addr = NULL;

	freqsBlock.addr = NULL;
	freqsBlock.size = 0;

	encodedData.size = finalSize;
	printf("encodedData.size = %zu\n", finalSize);
	encodedData.addr = (ptr)calloc(finalSize, BYTE_SIZE);

	SizeT k = 0;

	Byte toWrite[2];
	uint8 byteAmount;

	for (SizeT i = 0; i < rawPairsCount; i++) {
		BytePair pair = rawPairs[i];
		Byte replacement;
		bool replacementFound = false;

		for (SizeT j = 0; (j < mappingCount) && (!replacementFound); j++) {
			BPReplacement mapping = mappings[j];

			if (mapping.pair == pair) {
				replacement = mapping.replacement;
				replacementFound = true;
			}
		}

		if (replacementFound) {
			byteAmount = 1;
			toWrite[0] = replacement;
		} else {
			byteAmount = 2;
			copyMemory((ptr)&pair, toWrite, byteAmount);
		}

		for (uint8 m = 0; m < byteAmount; m++) {
			encodedData.addr[k++] = toWrite[m];
		}

		output.Replacements = mapBlock;
		output.encodedData = encodedData;

	}

	return output;
}
MemBlock decodeBPE(Encoded encodedData);