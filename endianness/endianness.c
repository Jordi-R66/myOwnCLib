#include "endianness.h"

#ifdef ENDIANNESS_HEADER
#include "../targetDetection/types.h"

Endianness_t getEndian() {
	uint8 a[2] = { 0x12, 0x34 };
	uint16 b = 0x1234;

	Endianness_t endian = (*(uint16*)a) == b ? Big : Little;

	return endian;
}

void swapEndianness(void* data, SizeT size) {
	uint8* bytes = (uint8*)data;

	for (SizeT i = 0; i < size / 2; i++) {
		uint8 temp = bytes[i];
		bytes[i] = bytes[size - i - 1];
		bytes[size - i - 1] = temp;
	}
}

void toBigEndian(void* data, SizeT size) {
	Endianness_t sysEndianness = getEndian();

	if (sysEndianness == Little) {
		swapEndianness(data, size);
	}
}

void toLittleEndian(void* data, SizeT size) {
	Endianness_t sysEndianness = getEndian();

	if (sysEndianness == Big) {
		swapEndianness(data, size);
	}
}

#endif