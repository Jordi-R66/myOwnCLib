#pragma once

#include "macros.h"

#ifndef FIXED_TYPES
#define FIXED_TYPES

typedef unsigned char uint8;
typedef signed char int8;

typedef unsigned short uint16;
typedef signed short int16;

#pragma region INT 32 AND 64 DEFINITION
	#if defined(LINUX) && defined(INTEL_ARCH_FAMILY) && defined(_32BITS)
		typedef signed int int32;
		typedef unsigned int uint32;

		typedef unsigned long long uint64;
		typedef signed long long int64;
	#elif defined(LINUX) && defined(INTEL_ARCH_FAMILY) && defined(_64BITS)
		typedef signed int int32;
		typedef unsigned int uint32;

		typedef unsigned long uint64;
		typedef signed long int64;
	#elif defined(WINDOWS) && defined(INTEL_ARCH_FAMILY) && defined(_32BITS)
		typedef signed int int32;
		typedef unsigned int uint32;

		typedef unsigned long long uint64;
		typedef signed long long int64;
	#elif defined(WINDOWS) && defined(INTEL_ARCH_FAMILY) && defined(_64BITS)
		typedef signed int int32;
		typedef unsigned int uint32;

		typedef unsigned long long uint64;
		typedef signed long long int64;
	#endif
#pragma endregion

#pragma region SizeT Declaration
	#if defined(LINUX) && defined(INTEL_ARCH_FAMILY) && defined(_32BITS)
		typedef uint32 SizeT;
		typedef int32 sSizeT;
	#elif defined(LINUX) && defined(INTEL_ARCH_FAMILY) && defined(_64BITS)
		typedef uint64 SizeT;
		typedef int64 sSizeT;
	#elif defined(WINDOWS) && defined(INTEL_ARCH_FAMILY) && defined(_32BITS)
		typedef uint32 SizeT;
		typedef int32 sSizeT;
	#elif defined(WINDOWS) && defined(INTEL_ARCH_FAMILY) && defined(_64BITS)
		typedef uint64 SizeT;
		typedef int64 sSizeT;
	#endif
#pragma endregion

typedef char* string;
typedef void* ptr;

#define I8_SIZE sizeof(uint8)
#define I16_SIZE sizeof(uint16)
#define I32_SIZE sizeof(uint32)
#define I64_SIZE sizeof(uint64)

#endif