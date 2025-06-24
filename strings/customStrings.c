#include "customStrings.h"

#include "stdio.h"
#include "stdlib.h"

#include "../memory/memfuncs.h"

String allocString(SizeT nChars) {
	ptr temp = calloc(nChars + 1, sizeof(char));

	if (temp == NULL) {
		fprintf(stderr, "Couldn't allocate enough space for a new string\n");
		exit(EXIT_FAILURE);
	}

	setMemory(temp, 0, nChars + 1);

	String output;

	output.chars = (string)temp;
	output.capacity = nChars;
	output.length = 0;

	return output;
}

void freeString(String* str) {
	if ((ptr)str == NULL || (ptr)str->chars == NULL) {
		fprintf(stderr, "Invalid String object/pointer\n");
		exit(EXIT_FAILURE);
	}

	setMemory(str->chars, 0, str->capacity+1);
	str->capacity = 0;
	str->length = 0;

	free(str->chars);
}

void reallocString(String* str, SizeT newSize);

SizeT stringLength(String* str) {
	SizeT length = 0;

	for (SizeT i = 0; i < str->capacity; i++) {
		if (str->chars[i] != 0) {
			length++;
		} else {
			break;
		}
	}

	return length;
}

void appendChar(String* str, char c) {

}

void containsChar(String* str, char c);
void containsString(String* str, string lookingUp);
void containsStringObject(String* str, String lookingUp);