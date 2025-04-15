#pragma once

#include "tracking.h"
#include "common.h"

size_t identifyGaussPivot(Matrix* mat, size_t rowId);

//size_t Partitioning(Matrix* mat, size_t firstId, size_t lastId, size_t pivotRowId, Swap** swaps, size_t* swapsMade);
//void QuickSort(Matrix* mat, size_t firstId, size_t lastId, Swap** swaps, size_t* swapsMade);

void prepareGauss(Matrix* mat, Tracker* tracker);
void Gauss(Matrix* mat, Tracker* tracker);
