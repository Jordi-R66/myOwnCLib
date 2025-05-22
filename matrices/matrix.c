#include "matrix.h"

#ifdef INCLUDE_MATRIX
void allocMatrix(Matrix* matrix) {
	matrix->size = matrix->rows * matrix->cols;
	matrix->data = (value_t*)calloc(matrix->size, sizeof(value_t));
	matrix->memFreed = false;
}

void deallocMatrix(Matrix* matrix) {
	free(matrix->data);
	matrix->memFreed = true;
}

void getMatrixRow(Matrix* matrix, SizeT row, value_t* rowBuffer) {
	SizeT iStart, iEnd;

	iStart = row * matrix->cols;
	iEnd = (row + 1) * matrix->cols;

	for (SizeT i = iStart; i < iEnd; i++) {
		rowBuffer[i - iStart] = matrix->data[i];
	}
}

void getMatrixColumn(Matrix* matrix, SizeT column, value_t* colBuffer) {
	for (SizeT row = 0; row < matrix->rows; row++) {
		SizeT i = row * matrix->cols + column;
		colBuffer[row] = matrix->data[i];
	}
}

void setMatrixRow(Matrix* matrix, SizeT row, value_t* rowBuffer) {
	for (SizeT i = 0; i < matrix->cols; i++) {
		value_t val = rowBuffer[i];
		setMatrixCase(matrix, val, row, i);
	}
}

void setMatrixColumn(Matrix* matrix, SizeT column, value_t* colBuffer) {
	for (SizeT i = 0; i < matrix->rows; i++) {
		value_t val = colBuffer[i];
		setMatrixCase(matrix, val, i, column);
	}
}

void setMatrixCase(Matrix* matrix, value_t value, SizeT row, SizeT col) {
	SizeT i = row * matrix->cols + col;

	matrix->data[i] = value;
}

value_t getMatrixCase(Matrix* matrix, SizeT row, SizeT col) {
	SizeT i = row * matrix->cols + col;

	return matrix->data[i];
}

void setMatrix(Matrix* matrix, value_t* values) {
	for (SizeT i = 0; i < matrix->size; i++) {
		matrix->data[i] = values[i];
	}
}

void scalarMul(Matrix* matrix, value_t scalar) {
	for (SizeT i = 0; i < matrix->size; i++) {
		matrix->data[i] *= scalar;
	}
}

Matrix scalarMulNewMatrix(Matrix* matrix, double scalar) {
	Matrix newMatrix;
	newMatrix.rows = matrix->rows;
	newMatrix.cols = matrix->cols;
	allocMatrix(&newMatrix);

	for (SizeT i = 0; i < matrix->size; i++) {
		newMatrix.data[i] = matrix->data[i] * scalar;
	}

	return newMatrix;
}

/*
	Performs an inefficient matrix multiplication algorithm and stores the resulting matrix at a given address
	Stores the result `matDest` of `matA * matB`
*/
void matrixMultiplication(Matrix* matA, Matrix* matB, Matrix* matDest) {
	if (matA->cols != matB->rows) {
		return;
	}

	matDest->rows = matA->rows;
	matDest->cols = matB->cols;

	allocMatrix(matDest);

	value_t* col = (value_t*)calloc(matB->rows, sizeof(value_t));
	value_t* row = (value_t*)calloc(matA->cols, sizeof(value_t));

	for (SizeT i = 0; i < matDest->rows; i++) {
		for (SizeT j = 0; j < matDest->cols; j++) {
			value_t newValue = 0.0;

			getMatrixRow(matA, i, row);
			getMatrixColumn(matB, j, col);

			for (SizeT k = 0; k <= matB->cols; k++) {
				newValue += col[k] * row[k];
			}

			setMatrixCase(matDest, newValue, i, j);
		}
	}

	free(col);
	free(row);
}

/*
	Adds matB to matA and stores the result in matA
*/
void matrixAddition(Matrix* matA, Matrix* matB) {
	if ((matA->cols != matB->cols) || (matA->rows != matB->rows)) {
		return;
	}

	for (SizeT i = 0; i < matA->size; i++) {
		matA->data[i] += matB->data[i];
	}
}

Matrix matrixAdditionNewMatrix(Matrix* matA, Matrix* matB) {
	if ((matA->cols != matB->cols) || (matA->rows != matB->rows)) {
		exit(EXIT_FAILURE);
		//return;
	}

	Matrix newMatrix;
	newMatrix.rows = matA->rows;
	newMatrix.cols = matA->cols;
	allocMatrix(&newMatrix);

	for (SizeT i = 0; i < matA->size; i++) {
		newMatrix.data[i] = matA->data[i] + matB->data[i];
	}

	return newMatrix;
}

void genIdentityMatrix(Matrix* matrix, SizeT n) {
	matrix->cols = n;
	matrix->rows = n;
	allocMatrix(matrix);

	memset(matrix->data, 0, matrix->size * sizeof(value_t));

	for (SizeT i = 0; i < matrix->cols; i++) {
		setMatrixCase(matrix, 1.0, i, i);
	}
}

void printMatrix(Matrix* matrix) {
	value_t* row_buffer = (value_t*)calloc(matrix->cols, sizeof(value_t));

	for (SizeT row = 0; row < matrix->rows; row++) {
		getMatrixRow(matrix, row, row_buffer);

		for (SizeT i = 0; i < matrix->cols; i++) {
			printf("%.2lf\t", row_buffer[i]);
		}
		printf("\n");
	}

	free(row_buffer);
}

// Originally in gauss.c

void swapRows(Matrix* mat, SizeT rowAId, SizeT rowBId) {
	if (rowAId == rowBId) {
		return;
	}

	if ((rowAId >= mat->rows) || (rowBId >= mat->rows)) {
		fprintf(stderr, "Error: row index out of bounds\n\tidA = %lu\n\tidB = %lu\n", rowAId, rowBId);
		exit(EXIT_FAILURE);
	}

	value_t* rowA = (value_t*)calloc(mat->cols, sizeof(value_t));
	value_t* rowB = (value_t*)calloc(mat->cols, sizeof(value_t));

	getMatrixRow(mat, rowAId, rowA);
	getMatrixRow(mat, rowBId, rowB);

	setMatrixRow(mat, rowAId, rowB);
	setMatrixRow(mat, rowBId, rowA);

	free(rowB);
	free(rowA);

	return;
}

void swapCols(Matrix* mat, SizeT colAId, SizeT colBId) {
	if (colAId == colBId) {
		return;
	}

	if ((colAId >= mat->cols) || (colBId >= mat->cols)) {
		fprintf(stderr, "Error: row index out of bounds\n\tidA = %lu\n\t idB = %lu\n", colAId, colBId);
		exit(EXIT_FAILURE);
	}

	value_t* colA = (value_t*)calloc(mat->rows, sizeof(value_t));
	value_t* colB = (value_t*)calloc(mat->rows, sizeof(value_t));

	getMatrixColumn(mat, colBId, colB);
	getMatrixColumn(mat, colAId, colA);

	setMatrixColumn(mat, colAId, colB);
	setMatrixColumn(mat, colBId, colA);

	free(colB);
	free(colA);

	return;
}

void subtractRows(Matrix* mat, SizeT rowAId, SizeT rowBId, value_t coeffRowB) {
	value_t* rowA = (value_t*)calloc(mat->cols, sizeof(value_t));
	value_t* rowB = (value_t*)calloc(mat->cols, sizeof(value_t));

	getMatrixRow(mat, rowAId, rowA);
	getMatrixRow(mat, rowBId, rowB);

	for (SizeT i = 0; i < mat->cols; i++) {
		rowA[i] -= coeffRowB * rowB[i];
	}

	setMatrixRow(mat, rowAId, rowA);

	free(rowA);
	free(rowB);

	return;
}

void multiplyRow(Matrix* mat, SizeT rowId, value_t coeffRow) {
	value_t* row = (value_t*)calloc(mat->cols, sizeof(value_t));

	getMatrixRow(mat, rowId, row);

	for (SizeT i = 0; i < mat->cols; i++) {
		row[i] *= coeffRow;
	}

	setMatrixRow(mat, rowId, row);

	free(row);
	return;
}
#endif