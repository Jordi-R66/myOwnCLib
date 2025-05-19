#pragma once
#include "macros.h"

typedef unsigned char uint8;
typedef signed char int8;

typedef unsigned short uint16;
typedef signed short int16;

typedef unsigned int uint32;
typedef signed int int32;

typedef unsigned long uint64;
typedef signed long int64;

#ifdef __64_BITS__
	typedef uint64 SizeT;
	typedef int64 sSizeT;
#elif defined(__32_BITS__)
	typedef uint32 SizeT;
	typedef int32 sSizeT;
#endif

typedef char* string;
typedef void* ptr;