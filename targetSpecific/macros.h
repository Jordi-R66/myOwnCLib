#pragma once

#ifndef OVERRIDE_DETECTION
	#ifdef __x86_64__
		#define __64_BITS__
	#elif defined(__i386__) || defined(__i386)
		#define __32_BITS__
	#elif defined(__ARM_ASM_SYNTAX_UNIFIED__)
		#if defined(__ARM_32BIT_STATE)
			#define __32_BITS__
		#elif defined(__ARM_64BIT_STATE)
			#define __64_BITS__
		#else
			#error "Can't identify target ARM arch"
		#endif
	#else
		#error "Unknown target architecture"
	#endif
#endif

#ifdef NULL
	#undef NULL
#endif

#define NULL ((void*)0)
