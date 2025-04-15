#pragma once

#ifndef __GCC__
#define __64_BITS__
#endif

#ifdef __64_BITS__
#define VALUE_TYPE double
#elif defined(__32_BITS__)
#define VALUE_TYPE float
#else
#error Unsupported target
#endif

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <time.h>

typedef VALUE_TYPE value_t;
