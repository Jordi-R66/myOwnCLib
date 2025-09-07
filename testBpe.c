#include <stdio.h>
#include <stdlib.h>

#include "compression/bpe.h"

#define BUFFER_SIZE (SizeT)216

string random_generator = "/dev/random";
string original_filename = "original.hex";
string encoded_filename = "encoded.hex";
string decoded_filename = "decoded.hex";

Byte buffer[BUFFER_SIZE];

SizeT originalSize, encodedSize, decodedSize;

SizeT writeBufferFromFile(const string filename, Byte* buffer, SizeT amount) {
	FILE* fp = fopen(filename, "r");
	SizeT output = fread(buffer, 1, amount, fp);
	fclose(fp);

	return output;
}

void writeFileFromBuffer(const string filename, Byte* buffer, SizeT amount) {
	FILE* fp = fopen(filename, "w");
	fwrite(buffer, 1, amount, fp);
	fclose(fp);
}

SizeT writeEncodedToFile(const string filename, Encoded data) {
	SizeT filesize = sizeof(SizeT) * 2 + data.encodedData.size + data.Replacements.size;
	ptr finalBuffer = (ptr)calloc(filesize, BYTE_SIZE);

	SizeT amountWritten = 0;

	copyMemory((ptr)&data.Replacements.size, finalBuffer, sizeof(SizeT));

	amountWritten += sizeof(SizeT);

	copyMemory((ptr)data.Replacements.addr, (ptr)((SizeT)finalBuffer + amountWritten), data.Replacements.size);

	amountWritten += data.Replacements.size;

	copyMemory((ptr)&data.encodedData.size, (ptr)((SizeT)finalBuffer + amountWritten), sizeof(SizeT));

	amountWritten += sizeof(SizeT);

	copyMemory((ptr)data.encodedData.addr, (ptr)((SizeT)finalBuffer + amountWritten), data.encodedData.size);

	amountWritten += data.encodedData.size;

	writeFileFromBuffer(filename, finalBuffer, amountWritten);

	setMemory(finalBuffer, 0, amountWritten);
	free(finalBuffer);

	return amountWritten;
}

int main(void) {
	originalSize = writeBufferFromFile(random_generator, buffer, BUFFER_SIZE);
	writeFileFromBuffer(original_filename, buffer, originalSize);

	Encoded encoded = encodeBPE((MemBlock){.size=BUFFER_SIZE, .addr=(ptr)buffer});
	writeEncodedToFile(encoded_filename, encoded);
	printf("Compressed from %zu to %zu bytes\n", BUFFER_SIZE, encoded.encodedData.size);
}
