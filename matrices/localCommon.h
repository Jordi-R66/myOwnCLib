#pragma once

#include "../common.h"

#ifndef OVERRIDE_MATRIX_VALUE_TYPE
	#ifdef VALUE_TYPE
	#undef VALUE_TYPE
	#endif

	#ifdef _64BITS
		#define VALUE_TYPE double
	#elif defined(_32BITS)
		#define VALUE_TYPE float
	#else
		#error "Unsupported"
	#endif
#endif

typedef VALUE_TYPE value_t;

typedef enum {
	S_DISCRETE = 0,
	U_DISCRETE = 1,
	REAL = 2
} ValType;
