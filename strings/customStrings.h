#pragma once

#include "../targetSpecific/types.h"
#include "../bits/bits.h"
#include "../bools/bool.h"

#ifndef CUSTOM_ASCII_STR 
#define CUSTOM_ASCII_STR

#pragma pack(1)

typedef struct {
	string chars;
	SizeT capacity;
	SizeT length;
} String;

#pragma pack()

#define INT_SIZE sizeof(String)

String allocString(SizeT nChars);
void reallocString(String* str, SizeT newSize);
void freeString(String* str);
void reverseString(String* str);

void printStr(String* str, bool newLine);
SizeT stringLength(String* str);

void appendChar(String* str, char c);
void containsChar(String* str, char c);
void containsString(String* str, string lookingUp);
void containsStringObject(String* str, String lookingUp);

#endif