#pragma once

#include "macros.h"

#ifndef FIXED_TYPES
#define FIXED_TYPES

typedef unsigned char uint8;
typedef signed char int8;

typedef uint8 Byte;

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

#define BYTE_SIZE sizeof(Byte)

#define I8_SIZE sizeof(uint8)
#define I16_SIZE sizeof(uint16)
#define I32_SIZE sizeof(uint32)
#define I64_SIZE sizeof(uint64)

#define U8_MAX_VAL 0xFF
#define U8_MIN_VAL 0x0
#define I8_MAX_VAL  127
#define I8_MIN_VAL -128

#define U16_MAX_VAL 0xFFFF
#define U16_MIN_VAL 0x0
#define I16_MAX_VAL  32767
#define I16_MIN_VAL -32768

#define U32_MAX_VAL 0xFFFFFFFF
#define U32_MIN_VAL 0x0
#define I32_MAX_VAL  2147483647
#define I32_MIN_VAL -2147483648

#define U64_MAX_VAL 0xFFFFFFFFFFFFFFFF
#define U64_MIN_VAL 0x0
#define I64_MAX_VAL  9223372036854775807
#define I64_MIN_VAL -9223372036854775808

#pragma region SizeT Declaration
	#if defined(LINUX) && defined(INTEL_ARCH_FAMILY) && defined(_32BITS)
		typedef uint32 SizeT;
		typedef int32 sSizeT;

		#define SIZET_MAX_VAL U32_MAX_VAL
		#define SIZET_MIN_VAL U32_MIN_VAL

		#define SSIZET_MAX_VAL I32_MAX_VAL
		#define SSIZET_MIN_VAL I32_MIN_VAL
	#elif defined(LINUX) && defined(INTEL_ARCH_FAMILY) && defined(_64BITS)
		typedef uint64 SizeT;
		typedef int64 sSizeT;

		#define SIZET_MAX_VAL U64_MAX_VAL
		#define SIZET_MIN_VAL U64_MIN_VAL

		#define SSIZET_MAX_VAL I64_MAX_VAL
		#define SSIZET_MIN_VAL I64_MIN_VAL
	#elif defined(WINDOWS) && defined(INTEL_ARCH_FAMILY) && defined(_32BITS)
		typedef uint32 SizeT;
		typedef int32 sSizeT;

		#define SIZET_MAX_VAL U32_MAX_VAL
		#define SIZET_MIN_VAL U32_MIN_VAL

		#define SSIZET_MAX_VAL I32_MAX_VAL
		#define SSIZET_MIN_VAL I32_MIN_VAL
	#elif defined(WINDOWS) && defined(INTEL_ARCH_FAMILY) && defined(_64BITS)
		typedef uint64 SizeT;
		typedef int64 sSizeT;

		#define SIZET_MAX_VAL U64_MAX_VAL
		#define SIZET_MIN_VAL U64_MIN_VAL

		#define SSIZET_MAX_VAL I64_MAX_VAL
		#define SSIZET_MIN_VAL I64_MIN_VAL
	#endif
#pragma endregion

typedef char* string;
typedef Byte* ptr;


#endif