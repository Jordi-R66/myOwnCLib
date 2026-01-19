#include "matrix.h"

//#include "../../memory/memfuncs.h"

#ifdef MATRIX_INCLUDED

const Matrix NULL_MATRIX = {0, 0, 0, NULL, false};

bool allocMatrix(MatrixPtr matrix) {
	bool success = true;
	// Safety : Do not allocate if the matrix seems to already contain unfreed data

	if (matrix->data != NULL && !matrix->memFreed) {
		fprintf(stderr, "Error: Attempt to allocate over an existing matrix without freeing it first.\n");
		success = false;
	} else {
		matrix->size = matrix->rows * matrix->cols;
		matrix->data = (Values)calloc(matrix->size, VALUE_T_SIZE);
	}

	if (success && matrix->data == NULL) {
		fprintf(stderr, "Error: Couldn't allocate memory for your %zux%zu matrix.\n", matrix->rows, matrix->cols);
		success = false;
	} else if (success) {
		matrix->memFreed = false;
	}

	return success;
}

Matrix createMatrix(SizeT rows, SizeT cols) {
	Matrix output = {.cols = cols, .rows = rows, .data = NULL, .memFreed = false, .size = 0};

	bool success = allocMatrix(&output);

	if (!success) {
		fprintf(stderr, "Error: on createMatrix(SizeT, SizeT)\n");
		exit(EXIT_FAILURE);
	}

	return output;
}

Matrix createMatrixWithValues(SizeT rows, SizeT cols, Values vals) {
	Matrix output = createMatrix(rows, cols);

	bool success = output.data != NULL;

	if (!success) {
		fprintf(stderr, "Error: on createMatrixWithValues(SizeT, SizeT, Values)\n");
		exit(EXIT_FAILURE);
	} else {
		setMatrix(&output, vals);
	}

	return output;
}

void deallocMatrix(MatrixPtr matrix, bool destroyValues) {
	if (matrix->data != NULL) {
		if (destroyValues) {
			memset(matrix->data, 0, matrix->size * VALUE_T_SIZE);
		}

		free(matrix->data);

		matrix->size = 0;
		matrix->data = NULL;
	}

	matrix->memFreed = false;
}

bool getMatrixRow(MatrixPtr matrix, SizeT row, Values rowBuffer) {
	SizeT iStart, iEnd;

	iStart = row * matrix->cols;
	iEnd = (row + 1) * matrix->cols;

	for (SizeT i = iStart; i < iEnd; i++) {
		rowBuffer[i - iStart] = matrix->data[i];
	}

	return true;
}

bool getMatrixColumn(MatrixPtr matrix, SizeT column, Values colBuffer) {
	for (SizeT row = 0; row < matrix->rows; row++) {
		SizeT i = row * matrix->cols + column;
		colBuffer[row] = matrix->data[i];
	}

	return true;
}

bool setMatrixRow(MatrixPtr matrix, SizeT row, Values rowBuffer) {
	for (SizeT i = 0; i < matrix->cols; i++) {
		Value val = rowBuffer[i];
		setMatrixCase(matrix, val, row, i);
	}

	return true;
}

bool setMatrixColumn(MatrixPtr matrix, SizeT column, Values colBuffer) {
	for (SizeT i = 0; i < matrix->rows; i++) {
		Value val = colBuffer[i];
		setMatrixCase(matrix, val, i, column);
	}

	return true;
}

bool setMatrixCase(MatrixPtr matrix, Value value, SizeT row, SizeT col) {
	bool success = true;

	if (row >= matrix->rows) {
		fprintf(stderr, "Can't get to row %zu : limit exceeded\n", row);
		success = false;
	}

	if (success && (col >= matrix->cols)) {
		fprintf(stderr, "Can't get to col %zu : limit exceeded\n", col);
		success = false;
	}

	if (success) {
		SizeT i = row * matrix->cols + col;

		matrix->data[i] = value;
	}

	return success;
}

bool getMatrixCase(MatrixPtr matrix, SizeT row, SizeT col, Value* destVar) {
	bool success = true;

	if (row >= matrix->rows) {
		fprintf(stderr, "Can't get to row %zu : limit exceeded\n", row);
		success = false;
	}

	if (success && (col >= matrix->cols)) {
		fprintf(stderr, "Can't get to col %zu : limit exceeded\n", col);
		success = false;
	}

	if (success) {
		SizeT i = row * matrix->cols + col;

		*destVar = matrix->data[i];
	}

	return success;
}

void setMatrix(MatrixPtr matrix, Values values) {
	for (SizeT i = 0; i < matrix->size; i++) {
		matrix->data[i] = values[i];
	}
}

void scalarMul(MatrixPtr matrix, Value scalar) {
	for (SizeT i = 0; i < matrix->size; i++) {
		matrix->data[i] *= scalar;
	}
}

Matrix scalarMulNewMatrix(MatrixPtr matrix, Value scalar) {
	Matrix newMatrix;
	newMatrix.rows = matrix->rows;
	newMatrix.cols = matrix->cols;
	allocMatrix(&newMatrix);

	for (SizeT i = 0; i < matrix->size; i++) {
		newMatrix.data[i] = matrix->data[i] * scalar;
	}

	return newMatrix;
}

/**
 * Performs an inefficient matrix multiplication algorithm and stores the resulting matrix at a given address
 * Stores the result `matDest` of `matA * matB`
 * @returns `success` : true if success, false if failure
*/
bool matrixMultiplication(MatrixPtr matA, MatrixPtr matB, MatrixPtr matDest) {
	bool success = true;

	if (matA->cols != matB->rows) {
		fprintf(stderr, "Error: Can't multiply the given matrices, matA.cols (%zu) != matB.rows (%zu)\n", matA->cols, matB->rows);
		success = false;
	}

	if (success && (matDest->data != NULL && !matDest->memFreed)) {
		fprintf(stderr, "Error: Destination matrix isn't empty. Please free it before use.\n");
		success = false;
	}

	if (success) {
		matDest->rows = matA->rows;
		matDest->cols = matB->cols;

		if (!allocMatrix(matDest)) {
			success = false;
		}
	}

	if (success) {
		SizeT M = matA->rows;
		SizeT N = matDest->cols; // matB->cols
		SizeT K = matA->cols;    // Dimension commune

		// Pointeurs bruts pour éviter l'indirection répétée
		Values A = matA->data;
		Values B = matB->data;
		Values C = matDest->data;

		for (SizeT i = 0; i < M; i++) {
			// Optimisation : pré-calcul de l'offset de la ligne i de A et C
			SizeT offsetA = i * K;
			SizeT offsetC = i * N;

			for (SizeT j = 0; j < N; j++) {
				Value sum = 0;

				for (SizeT k = 0; k < K; k++) {
					// C[i][j] += A[i][k] * B[k][j]
					// B est accédé avec un stride de N (moins cache-friendly que A, mais standard)
					sum += A[offsetA + k] * B[k * N + j];
				}

				C[offsetC + j] = sum;
			}
		}
	}

	return success;
}

bool matrixMultiplicationNT(MatrixPtr matA, MatrixPtr matB, MatrixPtr matDest) {
	bool success = true;

	// Note : Pour A * B^T, le nombre de colonnes de A doit égaler le nombre de colonnes de B (car B est transposée)
	if (matA->cols != matB->cols) {
		fprintf(stderr, "Error: Dimension mismatch for A * B^T (%zux%zu) * (%zux%zu)^T\n", matA->rows, matA->cols, matB->rows, matB->cols);
		success = false;
	}

	if (success && (matDest->data != NULL && !matDest->memFreed)) {
		fprintf(stderr, "Error: Dest isn't empty.\n");
		success = false;
	}

	if (success) {
		matDest->rows = matA->rows;
		matDest->cols = matB->rows; // Car B est transposée : (RxC) * (RxC)^T -> (RxC) * (CxR) -> RxR
		if (!allocMatrix(matDest)) {
			success = false;
		}
	}

	if (success) {
		SizeT M = matA->rows;
		SizeT N = matDest->cols; // matB->rows
		SizeT K = matA->cols;    // matB->cols (dimension de sommation)

		Values A = matA->data;
		Values B = matB->data;
		Values C = matDest->data;

		for (SizeT i = 0; i < M; i++) {
			for (SizeT j = 0; j < N; j++) {
				Value sum = 0;
				// Cette boucle est EXCELLENTE pour le cache : A et B sont lus linéairement !
				// C'est souvent plus rapide que la multiplication standard.

				for (SizeT k = 0; k < K; k++) {
					sum += A[i * K + k] * B[j * K + k]; 
				}
				C[i * N + j] = sum;
			}
		}
	}

	return success;
}

/*
	Adds matB to matA and stores the result in matA
*/
bool matrixAddition(MatrixPtr matA, MatrixPtr matB) {
	bool success = true;

	if ((matA->cols != matB->cols) || (matA->rows != matB->rows)) {
		fprintf(stderr, "Error: Dimension mismatch for addition (%zux%zu vs %zux%zu).\n", 
			matA->rows, matA->cols, matB->rows, matB->cols);
		success = false;
	}

	if (success) {
		// Optimisation : On parcourt linear-wise car les données sont contiguës
		// Cela évite les doubles boucles et favorise la vectorisation CPU
		for (SizeT i = 0; i < matA->size; i++) {
			matA->data[i] += matB->data[i];
		}
	}

	return success;
}

Matrix matrixAdditionNewMatrix(MatrixPtr matA, MatrixPtr matB) {
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

void genIdentityMatrix(MatrixPtr matrix, SizeT n) {
	matrix->cols = n;
	matrix->rows = n;
	allocMatrix(matrix);

	memset(matrix->data, 0, matrix->size * VALUE_T_SIZE);

	for (SizeT i = 0; i < matrix->cols; i++) {
		setMatrixCase(matrix, 1.0, i, i);
	}
}

void printMatrix(MatrixPtr matrix, ValType valFormat) {
	Values row_buffer = (Values)calloc(matrix->cols, VALUE_T_SIZE);

	for (SizeT row = 0; row < matrix->rows; row++) {
		getMatrixRow(matrix, row, row_buffer);

		for (SizeT i = 0; i < matrix->cols; i++) {
			switch (valFormat) {
				case (REAL):
					printf("%.2lf\t", (double)row_buffer[i]);
					break;
				case (U_DISCRETE):
					printf("%llu\t", (uint64)row_buffer[i]);
					break;
				case (S_DISCRETE):
					printf("%lld\t", (int64)row_buffer[i]);
					break;
				default:
					fprintf(stderr, "Unknown format\n");
					exit(EXIT_FAILURE); // TODO: Remove later
			}
		}
		printf("\n");
	}

	free(row_buffer);
}

bool matrixTranspose(MatrixPtr src, MatrixPtr dest) {
	bool success = true;

	if (success && (dest->data != NULL && !dest->memFreed)) {
		fprintf(stderr, "Error: Destination matrix isn't empty.\n");
		success = false;
	}

	if (success) {
		dest->rows = src->cols;
		dest->cols = src->rows;

		if (!allocMatrix(dest)) {
			success = false;
		}
	}

	if (success) {
		// TODO: Optimiser plus tard, ici c'est une version naïve. Il va falloir prendre avantage du cache du CPU.
		for (SizeT i=0; i < src->rows; i++) {
			for (SizeT j = 0; j < src->cols; j++) {
				dest->data[j * dest->cols + i] = src->data[i * src->cols + j];
			}
		}
	}

	return success;
}

void matrixMap(MatrixPtr mat, ValueFunc func) {
	for (SizeT i = 0; i < mat->size; i++) {
		mat->data[i] = func(mat->data[i]);
	}
}

// Originally in gauss.c

bool swapRows(MatrixPtr mat, SizeT rowAId, SizeT rowBId) {
	bool success = true;

	success = rowAId != rowBId;

	if (success && ((rowAId >= mat->rows) || (rowBId >= mat->rows))) {
		fprintf(stderr, "Error: row index out of bounds\n\tidA = %lu\n\tidB = %lu\n", rowAId, rowBId);
		success = false;
	}

	Values rowA = NULL, rowB = NULL;

	if (success) {
		rowA = (Values)calloc(mat->cols, VALUE_T_SIZE);
		rowB = (Values)calloc(mat->cols, VALUE_T_SIZE);

		if (rowA == NULL || rowB == NULL) {
			fprintf(stderr, "Error: Couldn't allocate temp row buffers.\n");
			success = false;

			if (rowA) free(rowA);
			if (rowB) free(rowB);
		}
	}

	if (success) {
		getMatrixRow(mat, rowAId, rowA);
		getMatrixRow(mat, rowBId, rowB);

		setMatrixRow(mat, rowAId, rowB);
		setMatrixRow(mat, rowBId, rowA);

		free(rowB);
		free(rowA);
	}

	return success;
}

bool swapCols(MatrixPtr mat, SizeT colAId, SizeT colBId) {
	bool success = true;

	success = colAId != colBId;

	if (success && ((colAId >= mat->cols) || (colBId >= mat->cols))) {
		fprintf(stderr, "Error: row index out of bounds\n\tidA = %lu\n\t idB = %lu\n", colAId, colBId);
		success = false;
	}

	Values colA = NULL, colB = NULL;

	if (success) {
		colA = (Values)calloc(mat->rows, VALUE_T_SIZE);
		colB = (Values)calloc(mat->rows, VALUE_T_SIZE);

		if (colA == NULL || colB == NULL) {
			fprintf(stderr, "Error: Couldn't allocate temp col buffers.\n");
			success = false;

			if (colA) free(colA);
			if (colB) free(colB);
		}
	}

	if (success) {
		getMatrixColumn(mat, colBId, colB);
		getMatrixColumn(mat, colAId, colA);

		setMatrixColumn(mat, colAId, colB);
		setMatrixColumn(mat, colBId, colA);

		free(colB);
		free(colA);
	}

	return success;
}

void subtractRows(MatrixPtr mat, SizeT rowAId, SizeT rowBId, Value coeffRowB) {
	Values rowA = (Values)calloc(mat->cols, VALUE_T_SIZE);
	Values rowB = (Values)calloc(mat->cols, VALUE_T_SIZE);

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

void multiplyRow(MatrixPtr mat, SizeT rowId, Value coeffRow) {
	Values row = (Values)calloc(mat->cols, VALUE_T_SIZE);

	getMatrixRow(mat, rowId, row);

	for (SizeT i = 0; i < mat->cols; i++) {
		row[i] *= coeffRow;
	}

	setMatrixRow(mat, rowId, row);

	free(row);
	return;
}
#endif