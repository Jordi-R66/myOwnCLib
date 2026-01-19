#include "matrix.h"

//#include "../../memory/memfuncs.h"

#ifdef MATRIX_INCLUDED

const Matrix NULL_MATRIX = {0, 0, 0, NULL, false};

#pragma region Helpers
// Seuil pour arrêter la récursion et passer au calcul direct.
// Un bloc 64x64 de doubles fait 32KB, ce qui tient généralement dans le cache L1.
#define CO_BLOCK_SIZE 64

// --- HELPERS PRIVÉS (Cache Oblivious) ---

// Transposition récursive : Diviser pour régner
// A : Source, B : Destination
// strideA/B : Largeur réelle de la matrice d'origine (pour sauter les lignes correctement)
static void _transposeRecursive(Values A, Values B, SizeT rows, SizeT cols, SizeT strideA, SizeT strideB) {
	// Cas de base : Le bloc est assez petit pour tenir dans le cache L1/L2
	if (rows <= CO_BLOCK_SIZE && cols <= CO_BLOCK_SIZE) {
		for (SizeT i = 0; i < rows; i++) {
			for (SizeT j = 0; j < cols; j++) {
				// Copie directe (cache-friendly car le bloc est petit)
				B[j * strideB + i] = A[i * strideA + j];
			}
		}
	} else {
		// Récursivité : On coupe la plus grande dimension en deux
		if (rows >= cols) {
			SizeT half = rows / 2;
			// Haut
			_transposeRecursive(A, B, half, cols, strideA, strideB);
			// Bas (A avance de 'half' lignes, B avance de 'half' colonnes)
			_transposeRecursive(A + half * strideA, B + half, rows - half, cols, strideA, strideB);
		} else {
			SizeT half = cols / 2;
			// Gauche
			_transposeRecursive(A, B, rows, half, strideA, strideB);
			// Droite (A avance de 'half' colonnes, B avance de 'half' lignes)
			_transposeRecursive(A + half, B + half * strideB, rows, cols - half, strideA, strideB);
		}
	}
}

// Multiplication récursive : C += A * B
static void _multRecursive(SizeT M, SizeT N, SizeT K, 
							Values A, SizeT strideA, 
							Values B, SizeT strideB, 
							Values C, SizeT strideC) {

	// Cas de base : Petit bloc -> Appel au "Kernel" itératif optimisé
	if ((M <= CO_BLOCK_SIZE) && (N <= CO_BLOCK_SIZE) && (K <= CO_BLOCK_SIZE)) {
		for (SizeT i = 0; i < M; i++) {
			for (SizeT k = 0; k < K; k++) {
				// On charge A[i][k] dans un registre pour l'utiliser N fois
				Value aVal = A[i * strideA + k];
				for (SizeT j = 0; j < N; j++) {
					C[i * strideC + j] += aVal * B[k * strideB + j];
				}
			}
		}
	} else {
		// Stratégie de découpe : On coupe la plus grande dimension
		if (M >= N && M >= K) {
			// Split M (Couper horizontalement A et C)
			SizeT half = M / 2;
			_multRecursive(half, N, K, A, strideA, B, strideB, C, strideC);
			_multRecursive(M - half, N, K, A + half * strideA, strideA, B, strideB, C + half * strideC, strideC);
		} else if (N >= M && N >= K) {
			// Split N (Couper verticalement B et C)
			SizeT half = N / 2;
			_multRecursive(M, half, K, A, strideA, B, strideB, C, strideC);
			_multRecursive(M, N - half, K, A, strideA, B + half, strideB, C + half, strideC);
		} else {
			// Split K (Couper verticalement A et horizontalement B) -> C += A_left*B_top + A_right*B_bot
			SizeT half = K / 2;
			_multRecursive(M, N, half, A, strideA, B, strideB, C, strideC);
			_multRecursive(M, N, K - half, A + half, strideA, B + half * strideB, strideB, C, strideC);
		}
	}
}
// Helper : Multiplication Transposée Récursive (Cache Oblivious)
// Effectue C += A * B^T
// A : MxK, B : NxK (stockée ligne par ligne), C : MxN
static void _multNTRecursive(SizeT M, SizeT N, SizeT K, 
								Values A, SizeT strideA, 
								Values B, SizeT strideB, 
								Values C, SizeT strideC) {

	// Cas de base : Petit bloc -> Kernel Itératif
	// On utilise les boucles simples qui seront vectorisées par le compilateur
	if ((M <= CO_BLOCK_SIZE) && (N <= CO_BLOCK_SIZE) && (K <= CO_BLOCK_SIZE)) {
		for (SizeT i = 0; i < M; i++) {
			for (SizeT j = 0; j < N; j++) {
				Value sum = 0;
				// Dot Product optimisé (accès linéaires sur A et B)
				for (SizeT k = 0; k < K; k++) {
					sum += A[i * strideA + k] * B[j * strideB + k];
				}
				// Accumulation (+=) car on somme les résultats des découpes de K
				C[i * strideC + j] += sum;
			}
		}
	} else {
		// Récursivité : On coupe la plus grande dimension
		if (M >= N && M >= K) {
			// Split M (Lignes de A et C)
			SizeT half = M / 2;
			_multNTRecursive(half, N, K, A, strideA, B, strideB, C, strideC);
			_multNTRecursive(M - half, N, K, A + half * strideA, strideA, B, strideB, C + half * strideC, strideC);
		} else if (N >= M && N >= K) {
			// Split N (Lignes de B et Colonnes de C)
			SizeT half = N / 2;
			_multNTRecursive(M, half, K, A, strideA, B, strideB, C, strideC);
			// Note : B avance de 'half' lignes (car B est NxK)
			// C avance de 'half' colonnes (pointeur + half)
			_multNTRecursive(M, N - half, K, A, strideA, B + half * strideB, strideB, C + half, strideC);
		} else {
			// Split K (Colonnes de A et B) -> Somme des résultats
			// C += A_left * B_left^T + A_right * B_right^T
			SizeT half = K / 2;
			_multNTRecursive(M, N, half, A, strideA, B, strideB, C, strideC);
			// A et B avancent de 'half' colonnes
			_multNTRecursive(M, N, K - half, A + half, strideA, B + half, strideB, C, strideC);
		}
	}
}
#pragma endregion

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
	bool success = matA->cols == matB->rows;

	if (!success) {
		fprintf(stderr, "Error: Dimension mismatch (%zu x %zu) * (%zu x %zu)\n", 
			(size_t)matA->rows, (size_t)matA->cols, (size_t)matB->rows, (size_t)matB->cols);
	}

	if (success && (matDest->data != NULL && !matDest->memFreed)) {
		fprintf(stderr, "Error: Destination matrix isn't empty.\n");
		success = false;
	}

	if (success) {
		matDest->rows = matA->rows;
		matDest->cols = matB->cols;
		// allocMatrix utilise calloc, donc la matrice est initialisée à 0 (requis pour +=)
		if (!allocMatrix(matDest)) {
			success = false;
		}
	}

	if (success) {
		// Lancement de l'algo Cache Oblivious
		// On passe les pointeurs bruts et les dimensions
		_multRecursive(matA->rows, matDest->cols, matA->cols,
					   matA->data, matA->cols,      // A, strideA
					   matB->data, matB->cols,      // B, strideB
					   matDest->data, matDest->cols // C, strideC
		);
	}

	return success;
}

bool matrixMultiplicationNT(MatrixPtr matA, MatrixPtr matB, MatrixPtr matDest) {
	bool success = matA->cols == matB->cols;

	// A * B^T implique que A et B doivent avoir le même nombre de colonnes (dimension de réduction K)
	if (!success) {
		fprintf(stderr, "Error: Dimension mismatch for A * B^T (%zux%zu) * (%zux%zu)^T\n", 
			(size_t)matA->rows, (size_t)matA->cols, (size_t)matB->rows, (size_t)matB->cols);
	}

	if (success && (matDest->data != NULL && !matDest->memFreed)) {
		fprintf(stderr, "Error: Dest isn't empty.\n");
		success = false;
	}

	if (success) {
		matDest->rows = matA->rows;
		matDest->cols = matB->rows; // Résultat (MxN)

		// allocMatrix utilise calloc -> C est initialisé à 0, ce qui est requis pour l'accumulation +=
		success = allocMatrix(matDest);
	}

	if (success) {
		// Appel Cache Oblivious
		// M = matA->rows
		// N = matB->rows (car transposée)
		// K = matA->cols (ou matB->cols)
		_multNTRecursive(matA->rows, matB->rows, matA->cols,
						 matA->data, matA->cols,      // A, strideA
						 matB->data, matB->cols,      // B, strideB
						 matDest->data, matDest->cols // C, strideC
		);
	}

	return success;
}

bool matrixHadamard(MatrixPtr A, MatrixPtr B, MatrixPtr dest) {
	bool success = (A->rows == B->rows && A->cols == B->cols && 
					dest->rows == A->rows && dest->cols == A->cols);

	if (!success) {
		fprintf(stderr, "Error: Dimension mismatch for Hadamard product.\n");
	} else {
		SizeT n = A->size;
		// Optimisation possible : SIMD
		for (SizeT i = 0; i < n; i++) {
			dest->data[i] = A->data[i] * B->data[i];
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
			}
		}
		printf("\n");
	}

	free(row_buffer);
}

bool matrixTranspose(MatrixPtr src, MatrixPtr dest) {
	bool success = !(dest->data != NULL && !dest->memFreed);

	if (!success) {
		fprintf(stderr, "Error: Destination matrix isn't empty.\n");
	}

	if (success) {
		dest->rows = src->cols;
		dest->cols = src->rows;
		if (!allocMatrix(dest)) {
			success = false;
		}
	}

	if (success) {
		// Lancement de l'algo Cache Oblivious
		// Les "strides" initiaux sont égaux aux nombres de colonnes
		_transposeRecursive(src->data, dest->data, src->rows, src->cols, src->cols, dest->cols);
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