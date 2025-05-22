#pragma once

#ifdef INCLUDE_MATRIX

#ifndef INCLUDE_COMMONS
#define INCLUDE_COMMONS
#endif

#include "../common.h"

#define VALUE_TYPE double

#ifndef VALUE_TYPE
	#ifdef __64_BITS__
		#define VALUE_TYPE double
	#elif defined(__32_BITS__)
		#define VALUE_TYPE float
	#else
		#error "Unsupported"
	#endif
#endif

typedef VALUE_TYPE value_t;
#endif