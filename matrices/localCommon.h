#pragma once

#include "../targetDetection/macros.h"

#ifdef __64_BITS__
#define VALUE_TYPE double
#elif defined(__32_BITS__)
#define VALUE_TYPE float
#endif

#include "../common.h"

typedef VALUE_TYPE value_t;
