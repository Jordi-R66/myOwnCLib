#pragma once

#include "../targetSpecific/types.h"

#ifndef MEMTYPES

#pragma pack(1)

typedef enum Comparison {
	LESS = 0, EQUALS = 1, GREATER = 2, UNDEF = 3
} Comparison;

// Création des fonctions de comparaison
typedef Comparison (*ComparisonFunc)(ptr, ptr, SizeT);

#pragma pack()

#endif