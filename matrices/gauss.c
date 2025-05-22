#include "gauss.h"

#ifdef INCLUDE_GAUSS

/*
Function to return the index of the row's pivot in the context
of Gauss' Pivot Algorithm
*/
SizeT identifyGaussPivot(Matrix* mat, SizeT rowId) {
	value_t* row = (value_t*)calloc(mat->cols, sizeof(value_t));

	getMatrixRow(mat, rowId, row);

	SizeT i;
	bool run = true;

	// Iterating until we find a non-null value
	for (i = 0; (i < mat->cols) && run; i++) {
		value_t val = row[i];
		run = val == 0.0;
	}

	i--; // The for-loop always returns `pivotIndex + 1`, so we take 1 away.

	free(row);

	return i;
}

void generatePivotsArray(Matrix* mat, SizeT* rowsGaussPivots) {
	for (SizeT i = 0; i < mat->rows; i++) {
		rowsGaussPivots[i] = identifyGaussPivot(mat, i);
	}
}

void swapElements(SizeT* arr, SizeT a, SizeT b) {
	SizeT c;

	c = arr[a];
	arr[a] = arr[b];
	arr[b] = c;
}

SizeT chooseSortingPivot(SizeT min, SizeT max) {
	//return (min + max) / 2;
	return 0;
	//return randomIndex(min, max);
}

void sortingSwap(Matrix* mat, SizeT* pivots, SizeT idA, SizeT idB, Tracker* tracker) {
	if (idA == idB) {
		return;
	}

	swapRows(mat, idA, idB);
	swapElements(pivots, idA, idB);

	recordSwap(tracker, idA, idB, Row);
}

/* -- QuickSort implementation, needs to be fixed so implemented BubbleSort instead
SizeT Partitioning(Matrix* mat, SizeT firstId, SizeT lastId, SizeT pivotRowId, Swap** swaps, SizeT* swapsMade) {
	SizeT* rowsGaussPivots = (SizeT*)calloc(mat->rows, sizeof(SizeT));

	for (SizeT i = 0; i < mat->rows; i++) {
		rowsGaussPivots[i] = identifyGaussPivot(mat, i);
		//printf("%lu\t", rowsGaussPivots[i]);
	}
	//printf("\n\n");

	SizeT firstRowVal, lastRowVal, pivotRowVal;

	firstRowVal = rowsGaussPivots[firstId];
	lastRowVal = rowsGaussPivots[lastId];
	pivotRowVal = rowsGaussPivots[pivotRowId];

	//printf("Call A : idA = %lu\tidB = %lu\n", pivotRowId, lastId);
	sortingSwap(mat, rowsGaussPivots, pivotRowId, lastId, swaps, swapsMade);

	SizeT j = firstId;

	for (SizeT i = firstId; i < lastId; i++) {
		if (rowsGaussPivots[i] <= rowsGaussPivots[lastId]) {
			//printf("Call B : idA = %lu\tidB = %lu\n", i, j);
			sortingSwap(mat, rowsGaussPivots, i, j, swaps, swapsMade);
			j++;
		}
	}

	//printf("Call C : idA = %lu\tidB = %lu\n", lastId, j);
	sortingSwap(mat, rowsGaussPivots, lastId, j, swaps, swapsMade);

	free(rowsGaussPivots);

	return j;
}

void QuickSort(Matrix* mat, SizeT firstId, SizeT lastId, Swap** swaps, SizeT* swapsMade) {
	SizeT* rowsGaussPivots = (SizeT*)calloc(mat->rows, sizeof(SizeT));

	for (SizeT i = 0; i < mat->rows; i++) {
		rowsGaussPivots[i] = identifyGaussPivot(mat, i);
	}

	SizeT sortingPivot;
	SizeT firstRowVal, lastRowVal, pivotRowVal;

	firstRowVal = rowsGaussPivots[firstId];
	lastRowVal = rowsGaussPivots[lastId];

	free(rowsGaussPivots);

	if (firstRowVal < lastRowVal) {
		printf("firstId = %lu\tlastId = %lu\n", firstId, lastId);
		sortingPivot = chooseSortingPivot(firstId, lastId);
		sortingPivot = Partitioning(mat, firstId, lastId, sortingPivot, swaps, swapsMade);

		QuickSort(mat, firstId, sortingPivot - 1, swaps, swapsMade);
		QuickSort(mat, sortingPivot + 1, lastId, swaps, swapsMade);
	}
}
*/

void BubbleSort(Matrix* mat, Tracker* tracker) {
	SizeT* rowsGaussPivots = (SizeT*)calloc(mat->rows, sizeof(SizeT));

	generatePivotsArray(mat, rowsGaussPivots);

	SizeT i, j;
	SizeT n = mat->rows;

	for (i = 0; i < n - 1; i++) {
		for (j = 0; j < n - i - 1; j++) {
			if (rowsGaussPivots[j] > rowsGaussPivots[j + 1]) {
				sortingSwap(mat, rowsGaussPivots, j, j + 1, tracker);
			}
		}
	}

	generatePivotsArray(mat, rowsGaussPivots);

	free(rowsGaussPivots);
}

void prepareGauss(Matrix* mat, Tracker* tracker) {
	if (tracker->initialised == false) {
		InitTracker(tracker);
	}

	//QuickSort(mat, 0, mat->rows - 1, swaps, swapsMade);
	BubbleSort(mat, tracker);
}

void Gauss(Matrix* mat, Tracker* tracker) {
	value_t* col = (value_t*)calloc(mat->rows, sizeof(value_t));

	prepareGauss(mat, tracker);
	SizeT* rowsGaussPivots = (SizeT*)calloc(mat->rows, sizeof(SizeT));

	for (SizeT l = 0; l < mat->rows; l++) {
		generatePivotsArray(mat, rowsGaussPivots);

		SizeT p = rowsGaussPivots[l];
		value_t pivot_val = getMatrixCase(mat, l, p);

		for (SizeT i = l + 1; i < mat->rows; i++) {
			value_t val = getMatrixCase(mat, i, p);

			if (val != 0.0) {
				value_t coeff = pivot_val / val;
				printf("%lu, %lu, %lf, (%lf, %lf)\n", i, l, coeff, pivot_val, val);
				subtractRows(mat, i, l, coeff);
			}
		}
	}

	printf("\n");

	/*for (SizeT i = 0; i < mat->rows; i++) {
		generatePivotsArray(mat, rowsGaussPivots);
		getMatrixColumn(mat, i, col);

		if (col[i] != 0) {
			for (SizeT j = i + 1; j < mat->rows; j++) {
				value_t coeff = col[j] / col[i];
				printf("i, j = %lu, %lu\n", i, j);
				subtractRows(mat, j, i, coeff);
			}
		}
	}*/

	/*for (SizeT i = 0; i < mat->rows; i++) {
		SizeT refColId = rowsGaussPivots[i];

		if (getMatrixCase(mat, i, refColId) != 0.0) {
			getMatrixColumn(mat, refColId, col);

			for (SizeT j = i + 1; j < mat->rows; j++) {
				value_t coeff = col[j] / col[i];
				//printf("i, j = %lu, %lu\n", i, j);
				subtractRows(mat, j, i, coeff);
			}

			generatePivotsArray(mat, rowsGaussPivots);
		}
	}*/

	printf("Gauss done\n");

	free(rowsGaussPivots);
	free(col);

	printf("Freed\n");

}
#endif