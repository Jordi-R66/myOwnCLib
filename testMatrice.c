#define OVERRIDE_MATRIX_VALUE_TYPE
#define VALUE_TYPE int32

#include "matrices/matrix.h"

int32 main(void) {
	Matrix M, C;

	M.rows = 4;
	M.cols = M.rows;

	allocMatrix(&M);

	value_t vals[16] = {0, 1, 1, 1,
						1, 2, 0, 0,
						1, 0, 0, 1,
						1, 0, 1, 0};

	setMatrix(&M, vals);

	printMatrix(&M, S_DISCRETE);

	matrixMultiplication(&M, &M, &C);

	printMatrix(&C, S_DISCRETE);

	return 0;
}