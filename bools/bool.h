#pragma once

#if defined __STDC_VERSION__ && __STDC_VERSION__ > 201710L
	/* bool, true and false are keywords.  */
#else

#define BOOL_BITS 8

typedef unsigned char bool;

#define true 1
#define false 0

#endif
