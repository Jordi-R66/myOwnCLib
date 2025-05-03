#include "macros.h"

typedef unsigned char uint8;
typedef signed char int8;

typedef unsigned short uint16;
typedef signed short int16;

typedef unsigned int uint32;
typedef signed int int32;

typedef unsigned long long uint64;
typedef signed long long int64;

#ifdef __64_BITS__
	typedef uint64 size_t;
	typedef int64 ssize_t;
#elif defined(__32_BITS__)
	typedef uint32 size_t;
	typedef int32 ssize_t;
#endif