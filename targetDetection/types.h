#include "macros.h"

typedef unsigned char uint8;
typedef signed char int8;

typedef unsigned short uint16;
typedef signed short int16;

#ifdef __64_BITS__
typedef unsigned int uint32;
typedef signed int int32;

typedef unsigned long uint64;
typedef signed long int64;
#endif