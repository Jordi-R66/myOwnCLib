#pragma once

#ifndef COLL_COMMON

#include "../targetSpecific/types.h"

typedef uint8 CollectionFlag;

#define NO_FLAGS			(CollectionFlag)0b00000000
#define COLL_INITIALISED	(CollectionFlag)0b00000001
#define COLL_FRAGMENTED		(CollectionFlag)0b00000010

#define COLL_COMMON
#endif
