#pragma once

#ifdef INCLUDE_GAUSS

#ifndef INCLUDE_MATRIX_TRACKING
#define INCLUDE_MATRIX_TRACKING
#endif

#include "tracking.h"

SizeT identifyGaussPivot(Matrix* mat, SizeT rowId);

//SizeT Partitioning(Matrix* mat, SizeT firstId, SizeT lastId, SizeT pivotRowId, Swap** swaps, SizeT* swapsMade);
//void QuickSort(Matrix* mat, SizeT firstId, SizeT lastId, Swap** swaps, SizeT* swapsMade);

void prepareGauss(Matrix* mat, Tracker* tracker);
void Gauss(Matrix* mat, Tracker* tracker);
#endif