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
void freeString(String* str);

#endif