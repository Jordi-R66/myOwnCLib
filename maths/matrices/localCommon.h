#pragma once

#include "../common.h"

#ifndef OVERRIDE_MATRIX_ValueYPE
	#ifdef ValueTYPE
	#undef ValueTYPE
	#endif

	#ifdef _64BITS
		#define ValueTYPE double
	#elif defined(_32BITS)
		#define ValueTYPE float
	#else
		#error "Unsupported"
	#endif
#endif

typedef ValueTYPE Value, *Values;

#define VALUE_T_SIZE sizeof(Value)

typedef enum ValTypes {
	S_DISCRETE = 0,
	U_DISCRETE = 1,
	REAL = 2
} ValType;
