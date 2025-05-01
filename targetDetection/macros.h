#pragma once

#ifndef OVERRIDE_DETECTION

#ifdef __x86_64__
	#define __64_BITS__
#elif defined(__i386__) || defined(__i386)
	#define __32_BITS__
#else
	#error "Unknown target"
#endif

#endif