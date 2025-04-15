#include "matrix.h"

void allocMatrix(Matrix* matrix) {
	matrix->size = matrix->rows * matrix->cols;
	matrix->data = (value_t*)calloc(matrix->size, sizeof(value_t));
	matrix->memFreed = false;
}

void deallocMatrix(Matrix* matrix) {
	free(matrix->data);
	matrix->memFreed = true;
}

void getMatrixRow(Matrix* matrix, size_t row, value_t* rowBuffer) {
	size_t iStart, iEnd;

	iStart = row * matrix->cols;
	iEnd = (row + 1) * matrix->cols;

	for (size_t i = iStart; i < iEnd; i++) {
		rowBuffer[i - iStart] = matrix->data[i];
	}
}

void getMatrixColumn(Matrix* matrix, size_t column, value_t* colBuffer) {
	for (size_t row = 0; row < matrix->rows; row++) {
		size_t i = row * matrix->cols + column;
		colBuffer[row] = matrix->data[i];
	}
}

void setMatrixRow(Matrix* matrix, size_t row, value_t* rowBuffer) {
	for (size_t i = 0; i < matrix->cols; i++) {
		value_t val = rowBuffer[i];
		setMatrixCase(matrix, val, row, i);
	}
}

void setMatrixColumn(Matrix* matrix, size_t column, value_t* colBuffer) {
	if (sizeof(colBuffer) != (matrix->rows / sizeof(value_t))) {
		return;
	}

	for (size_t i = 0; i < matrix->rows; i++) {
		value_t val = colBuffer[i];
		setMatrixCase(matrix, val, i, column);
	}
}

void setMatrixCase(Matrix* matrix, value_t value, size_t row, size_t col) {
	size_t i = row * matrix->cols + col;

	matrix->data[i] = value;
}

value_t getMatrixCase(Matrix* matrix, size_t row, size_t col) {
	size_t i = row * matrix->cols + col;

	return matrix->data[i];
}

void setMatrix(Matrix* matrix, value_t* values) {
	for (size_t i = 0; i < matrix->size; i++) {
		matrix->data[i] = values[i];
	}
}

void scalarMul(Matrix* matrix, value_t scalar) {
	for (size_t i = 0; i < matrix->size; i++) {
		matrix->data[i] *= scalar;
	}
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

	value_t* col = (value_t*)calloc(matDest->rows, sizeof(value_t));
	value_t* row = (value_t*)calloc(matDest->cols, sizeof(value_t));

	for (size_t i = 0; i < matDest->rows; i++) {
		for (size_t j = 0; j < matDest->cols; j++) {
			value_t newValue = 0.0;

			getMatrixRow(matA, i, row);
			getMatrixColumn(matB, j, col);

			for (size_t k = 0; k < matB->cols; k++) {
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

	for (size_t i = 0; i < matA->size; i++) {
		matA->data[i] += matB->data[i];
	}
}

void genIdentityMatrix(Matrix* matrix, size_t n) {
	matrix->cols = n;
	matrix->rows = n;
	allocMatrix(matrix);

	for (size_t i = 0; i < matrix->cols; i++) {
		for (size_t j = 0; j < matrix->rows; j++) {
			setMatrixCase(matrix, 0.0, j, i);
		}
	}

	for (size_t i = 0; i < matrix->cols; i++) {
		setMatrixCase(matrix, 1.0, i, i);
	}
}

void printMatrix(Matrix* matrix) {
	value_t* row_buffer = (value_t*)calloc(matrix->cols, sizeof(value_t));

	for (size_t row = 0; row < matrix->rows; row++) {
		getMatrixRow(matrix, row, row_buffer);

		for (size_t i = 0; i < matrix->cols; i++) {
			printf("%.2lf\t", row_buffer[i]);
		}
		printf("\n");
	}

	free(row_buffer);
}

// Originally in gauss.c

void swapRows(Matrix* mat, size_t rowAId, size_t rowBId) {
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

void swapCols(Matrix* mat, size_t colAId, size_t colBId) {
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

void subtractRows(Matrix* mat, size_t rowAId, size_t rowBId, value_t coeffRowB) {
	value_t* rowA = (value_t*)calloc(mat->cols, sizeof(value_t));
	value_t* rowB = (value_t*)calloc(mat->cols, sizeof(value_t));

	getMatrixRow(mat, rowAId, rowA);
	getMatrixRow(mat, rowBId, rowB);

	for (size_t i = 0; i < mat->cols; i++) {
		rowA[i] -= coeffRowB * rowB[i];
	}

	setMatrixRow(mat, rowAId, rowA);

	free(rowA);
	free(rowB);

	return;
}

void multiplyRow(Matrix* mat, size_t rowId, value_t coeffRow) {
	value_t* row = (value_t*)calloc(mat->cols, sizeof(value_t));

	getMatrixRow(mat, rowId, row);

	for (size_t i = 0; i < mat->cols; i++) {
		row[i] *= coeffRow;
	}

	setMatrixRow(mat, rowId, row);

	free(row);
	return;
}
