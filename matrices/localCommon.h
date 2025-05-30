#pragma once

#include "../common.h"

#ifndef OVERRIDE_MATRIX_VALUE_TYPE
	#ifdef VALUE_TYPE
	#undef VALUE_TYPE
	#endif

	#ifdef __64_BITS__
		#define VALUE_TYPE double
	#elif defined(__32_BITS__)
		#define VALUE_TYPE float
	#else
		#error "Unsupported"
	#endif
#endif

typedef VALUE_TYPE value_t;
