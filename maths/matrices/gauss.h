#pragma once

#include "tracking.h"

SizeT identifyGaussPivot(Matrix* mat, SizeT rowId);

//SizeT Partitioning(Matrix* mat, SizeT firstId, SizeT lastId, SizeT pivotRowId, Swap** swaps, SizeT* swapsMade);
//void QuickSort(Matrix* mat, SizeT firstId, SizeT lastId, Swap** swaps, SizeT* swapsMade);

void prepareGauss(Matrix* mat, Tracker* tracker);
void Gauss(Matrix* mat, Tracker* tracker);

#define GAUSS_INCLUDED