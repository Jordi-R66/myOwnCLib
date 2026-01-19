//#define OVERRIDE_MATRIX_VALUE_TYPE
//#define VALUE_TYPE int32

#include "maths/matrices/matrix.h"

int32 main(void) {
	Matrix M, C;

	M.rows = 4;
	M.cols = M.rows;
	M.data = NULL;
	M.memFreed = false;

	allocMatrix(&M);

	Value vals[16] = { 0.0, 1.0, 1.0, 1.0,
						1.0, 2.0, 0.0, 0.0,
						1.0, 0.0, 0.0, 1.0,
						1.0, 0.0, 1.0, 0.0 };

	setMatrix(&M, vals);

	//printMatrix(&M, S_DISCRETE);

	matrixMultiplication(&M, &M, &C);

	printMatrix(&C, S_DISCRETE);
	deallocMatrix(&M, true);
	deallocMatrix(&C, true);

	return 0;
}