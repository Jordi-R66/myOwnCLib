#pragma once

#define INCLUDE_ALL

#ifdef INCLUDE_ALL
#define INCLUDE_ADV_MATRICES
#define INCLUDE_BITS
#define INCLUDE_BOOLS
#define INCLUDE_COMMON
#define INCLUDE_DICTS
#define INCLUDE_ENDIANNESS
#define INCLUDE_GAUSS
#define INCLUDE_LINKED_LISTS
#define INCLUDE_LISTS
#define INCLUDE_MATRIX
#define INCLUDE_MATRIX_TRACKING
#define INCLUDE_MEMFUNCS
#define INCLUDE_TARGET_SPECIFIC

#endif

#include "bits/bits.h"
#include "bools/bool.h"
#include "dicts/dict.h"
#include "endianness/endianness.h"
#include "linkedLists/linkedLists.h"
#include "lists/list.h"
#include "matrices/matrix.h"
#include "matrices/advMatrix.h"
#include "matrices/gauss.h"
#include "matrices/tracking.h"
#include "matrices/localCommon.h"
#include "memory/memfuncs.h"
#include "targetSpecific/macros.h"
#include "targetSpecific/types.h"
#include "vectors/vectors.h"
