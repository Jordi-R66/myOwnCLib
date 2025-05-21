#pragma once

#define INCLUDE_ALL

#ifdef INCLUDE_ALL
#define INCLUDE_TARGET_SPECIFIC
#define INCLUDE_MEMFUNCS
#define INCLUDE_ALL_LISTS
#define INCLUDE_VECTORS
#define INCLUDE_DICTS
#define INCLUDE_BOOLS
#define INCLUDE_BITS
#define INCLUDE_ENDIANNESS
#endif

#ifdef INCLUDE_ALL_LISTS
#define INCLUDE_LISTS
#define INCLUDE_LINKED_LISTS
#endif

#ifdef INCLUDE_DICTS
	#ifndef INCLUDE_LISTS
		#define INCLUDE_LISTS
	#endif
#endif

#ifdef INCLUDE_LISTS
	#ifndef INCLUDE_MEMFUNCS
		#define INCLUDE_MEMFUNCS
	#endif

	#ifndef INCLUDE_TARGET_SPECIFIC
		#define INCLUDE_TARGET_SPECIFIC
	#endif
#endif

#ifdef INCLUDE_ENDIANNESS
	#ifndef INCLUDE_TARGET_SPECIFIC
		#define INCLUDE_TARGET_SPECIFIC
	#endif
#endif

#ifdef INCLUDE_TARGET_SPECIFIC
#include "targetSpecific/types.h"
#include "targetSpecific/macros.h"
#endif

#ifdef INCLUDE_ENDIANNESS
#include "endianness/endianness.h"
#endif

#ifdef INCLUDE_VECTORS
	#ifndef INCLUDE_MATRICES
		#define INCLUDE_MATRICES
	#endif
#endif

#ifdef INCLUDE_MEMFUNCS
#include "memory/memfuncs.h"
#endif

#ifdef INCLUDE_LISTS
#include "lists/list.h"
#endif

#ifdef INCLUDE_LINKED_LISTS
#include "linkedLists/linkedLists.h"
#endif

#ifdef INCLUDE_MATRICES
#include "matrices/tracking.h"
#include "matrices/matrix.h"
#include "matrices/advMatrix.h"
#include "matrices/gauss.h"
#include "matrices/localCommon.h"
#endif

#ifdef INCLUDE_VECTORS
	#include "vectors/vectors.h"
#endif

#ifdef INCLUDE_DICTS
#include "dicts/dict.h"
#endif

#ifdef INCLUDE_BOOLS
#include "bools/bool.h"
#endif

#ifdef INCLUDE_BITS
#include "bits/bits.h"
#endif