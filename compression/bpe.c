#include "bpe.h"
#include "../memory/memfuncs.h"
#include "../bools/bool.h"

const MemBlock NULL_MEMBLOCK = {0, NULL};

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
