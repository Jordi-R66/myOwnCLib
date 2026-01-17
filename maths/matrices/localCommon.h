#pragma once

#include "../common.h"

#ifndef OVERRIDE_MATRIX_ValueYPE
	#ifdef ValueYPE
	#undef ValueYPE
	#endif

	#ifdef _64BITS
		#define ValueYPE double
	#elif defined(_32BITS)
		#define ValueYPE float
	#else
		#error "Unsupported"
	#endif
#endif

typedef ValueYPE Value;

typedef enum ValTypes {
	S_DISCRETE = 0,
	U_DISCRETE = 1,
	REAL = 2
} ValType;

typedef Value* Values;