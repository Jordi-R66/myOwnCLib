#include "advMatrix.h"

void mirrorMatrix(Matrix* mat, SwapType type, Tracker* tracker) {
	size_t j = 0;

	switch (type) {
		case Row:
			for (size_t i=0; i < mat->rows / 2; i++) {
				j = mat->rows - 1 - i;

				if (i != j) {
					swapRows(mat, i, j);
					recordSwap(tracker, i, j, type);
				}
			}

			break;

		case Column:
			for (size_t i=0; i < mat->cols / 2; i++) {
				j = mat->cols - 1 - i;

				if (i != j) {
					swapCols(mat, i, j);
					recordSwap(tracker, i, j, type);
				}
			}

			break;

		default:
			fprintf(stderr, "Can't mirror your matrix with the given option :\n\ttype = %d", type);
			exit(EXIT_FAILURE);
	}
}

bool inversibiltyCheck(Matrix* mat, Tracker* tracker) {
	bool result = true;

	result &= mat->cols == mat->rows;

	if (result) {
		prepareGauss(mat, tracker);
	}

	return result;
}