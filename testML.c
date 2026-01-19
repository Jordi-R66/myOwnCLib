#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "maths/matrices/matrix.h"
#include "maths/vectors/vectors.h"

// Utilitaire pour comparer des flottants avec une tolérance
bool assertAlmostEqual(Value actual, Value expected, const char* testName) {
	Value diff = fabs(actual - expected);
	if (diff > 1e-6) {
		printf("❌ [%s] FAILED. Expected: %f, Got: %f\n", testName, expected, actual);
		return false;
	}
	printf("✅ [%s] PASSED.\n", testName);
	return true;
}

void testAxpy() {
	printf("--- Test AXPY (Y = alpha*X + Y) ---\n");
	// X = [1, 2, 3]
	Vector X = createVector(3);
	X.data[0] = 1.0; X.data[1] = 2.0; X.data[2] = 3.0;

	// Y = [10, 10, 10]
	Vector Y = createVector(3);
	Y.data[0] = 10.0; Y.data[1] = 10.0; Y.data[2] = 10.0;

	Value alpha = 2.0;

	// Calcul : Y = 2 * X + Y => [12, 14, 16]
	if (vectorAxpy(alpha, &X, &Y)) {
		assertAlmostEqual(Y.data[0], 12.0, "Axpy Index 0");
		assertAlmostEqual(Y.data[1], 14.0, "Axpy Index 1");
		assertAlmostEqual(Y.data[2], 16.0, "Axpy Index 2");
	}
	else {
		printf("❌ Axpy function returned false.\n");
	}

	deallocVector(&X);
	deallocVector(&Y);
}

void testSoftmax() {
	printf("\n--- Test Softmax ---\n");
	// V = [2.0, 1.0, 0.1]
	Vector V = createVector(3);
	V.data[0] = 2.0;
	V.data[1] = 1.0;
	V.data[2] = 0.1;

	// Softmax théorique approx :
	// exp(2) ~ 7.389, exp(1) ~ 2.718, exp(0.1) ~ 1.105
	// Sum ~ 11.212
	// V[0] ~ 0.659, V[1] ~ 0.242, V[2] ~ 0.098

	if (vectorSoftmax(&V)) {
		Value sum = V.data[0] + V.data[1] + V.data[2];
		assertAlmostEqual(sum, 1.0, "Softmax Sum is 1.0");

		// Vérification des valeurs (ordre de grandeur)
		if (V.data[0] > V.data[1] && V.data[1] > V.data[2]) {
			printf("✅ Softmax order preserved.\n");
		}
		else {
			printf("❌ Softmax order incorrect.\n");
		}
	}
	else {
		printf("❌ Softmax returned false.\n");
	}
	deallocVector(&V);
}

void testArgmax() {
	printf("\n--- Test Argmax ---\n");
	Vector V = createVector(4);
	V.data[0] = 0.1;
	V.data[1] = 0.7; // Max
	V.data[2] = 0.05;
	V.data[3] = 0.15;

	SizeT maxIdx;
	if (vectorArgmax(&V, &maxIdx)) {
		if (maxIdx == 1) {
			printf("✅ Argmax found correct index (1).\n");
		}
		else {
			printf("❌ Argmax failed. Expected 1, Got %zu\n", (size_t)maxIdx);
		}
	}
	deallocVector(&V);
}

void testMatMulNT() {
	printf("\n--- Test Matrix Multiply NT (A * B^T) ---\n");
	// A (2x3) = [1 2 3]
	//           [4 5 6]
	Matrix A = createMatrix(2, 3);
	A.data[0] = 1; A.data[1] = 2; A.data[2] = 3;
	A.data[3] = 4; A.data[4] = 5; A.data[5] = 6;

	// B (2x3) = [1 2 1]  <- Sera transposée logiquement en (3x2)
	//           [0 1 0]
	//
	// Calcul A * B^T :
	// C[0][0] = RowA0 . RowB0 = 1*1 + 2*2 + 3*1 = 1 + 4 + 3 = 8
	// C[0][1] = RowA0 . RowB1 = 1*0 + 2*1 + 3*0 = 2
	// C[1][0] = RowA1 . RowB0 = 4*1 + 5*2 + 6*1 = 4 + 10 + 6 = 20
	// C[1][1] = RowA1 . RowB1 = 4*0 + 5*1 + 6*0 = 5

	Matrix B = createMatrix(2, 3);
	B.data[0] = 1; B.data[1] = 2; B.data[2] = 1;
	B.data[3] = 0; B.data[4] = 1; B.data[5] = 0;

	Matrix C = { 0 }; // Empty

	if (matrixMultiplicationNT(&A, &B, &C)) {
		assertAlmostEqual(C.data[0], 8.0, "C[0][0] == 8");
		assertAlmostEqual(C.data[1], 2.0, "C[0][1] == 2");
		assertAlmostEqual(C.data[2], 20.0, "C[1][0] == 20");
		assertAlmostEqual(C.data[3], 5.0, "C[1][1] == 5");

		deallocMatrix(&C, true); // Clean result
	}
	else {
		printf("❌ MatrixMultiplicationNT returned false.\n");
	}

	deallocMatrix(&A, true);
	deallocMatrix(&B, true);
}

int main() {
	testAxpy();
	testSoftmax();
	testArgmax();
	testMatMulNT();
	printf("\n--- End of Tests ---\n");
	return 0;
}
