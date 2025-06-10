
#ifndef TARGET_DETECTION
#define TARGET_DETECTION
#ifndef OVERRIDE_DETECTION

	#pragma region ArchDetection
		#ifdef __x86_64__
			#define _64BITS 1
			#define INTEL_ARCH_FAMILY 1
		#elif defined(__i386__) || defined(__i386) || defined(__i686__) || defined(__i686)
			#define _32BITS 1
			#define INTEL_ARCH_FAMILY 1
		#elif defined(__ARM_ASM_SYNTAX_UNIFIED__)
			#define ARM_ARCH_FAMILY 1
			#if defined(__ARM_32BIT_STATE)
				#define _32BITS 1
			#elif defined(__ARM_64BIT_STATE)
				#define _64BITS 1
			#else
				#error "Can't identify target ARM arch"
			#endif
		#else
			#error "Unknown target architecture"
		#endif
	#pragma endregion

	#pragma region SysDetection
		#ifdef __linux
			#define LINUX 1
		#elif defined(_WIN64) || defined(_WIN32)
			#define WINDOWS 1
		#else
			#error "Unknown target OS"
		#endif
	#pragma endregion

	#endif
#endif