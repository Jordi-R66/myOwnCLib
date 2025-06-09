#pragma once

#ifndef OVERRIDE_DETECTION

	#pragma region ArchDetection
		#ifdef __x86_64__
			#define __64_BITS__
		#elif defined(__i386__) || defined(__i386) || defined(__i686__) || defined(__i686)
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
	#pragma endregion

	#pragma region SysDetection
		#ifdef __linux
			#define LINUX
		#elif defined(_WIN64) || defined(_WIN32)
			#define WINDOWS
		#else
			#error "Unknown target OS"
		#endif
	#pragma endregion

#endif

#ifdef NULL
	#undef NULL
#endif

#define NULL ((void*)0)
