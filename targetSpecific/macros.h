#pragma once

#include "target.h"

#ifndef MACROS
#define MACROS

#ifndef __need_NULL
	#ifdef NULL
		#undef NULL
	#endif

	#define NULL ((void*)0)
#endif

#endif