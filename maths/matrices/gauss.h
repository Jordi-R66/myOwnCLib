#pragma once

#include "tracking.h"

SizeT identifyGaussPivot(MatrixPtr mat, SizeT rowId);

//SizeT Partitioning(MatrixPtr mat, SizeT firstId, SizeT lastId, SizeT pivotRowId, Swap** swaps, SizeT* swapsMade);
//void QuickSort(MatrixPtr mat, SizeT firstId, SizeT lastId, Swap** swaps, SizeT* swapsMade);

void prepareGauss(MatrixPtr mat, Tracker* tracker);
void Gauss(MatrixPtr mat, Tracker* tracker);

#define GAUSS_INCLUDED