#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "maths/matrices/matrix.h"
#include "maths/vectors/vectors.h"

// Macro pour chronométrer un bloc de code
#define TIME_IT(name, code_block) do { \
    clock_t start = clock(); \
    code_block \
    clock_t end = clock(); \
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC; \
    printf("⏱️  [%-25s] : %.4f s\n", name, time_spent); \
} while(0)

// Helper pour remplir avec des valeurs aléatoires
void randomizeMatrix(MatrixPtr m) {
	// Note: On suppose que m->data est alloué
	for (SizeT i = 0; i < m->size; i++) {
		m->data[i] = ((Value)rand() / RAND_MAX) * 2.0 - 1.0; // entre -1 et 1
	}
}

void testBigMatrixMult(SizeT N) {
	printf("\n--- 🏗️  Test Multiplication Matrice (%zux%zu) ---\n", N, N);

	Matrix A = createMatrix(N, N);
	Matrix B = createMatrix(N, N);

	// CORRECTION : On initialise C à vide (0).
	// matrixMultiplication fera l'allocation interne.
	Matrix C = { 0 };

	randomizeMatrix(&A);
	randomizeMatrix(&B);

	printf("Calcul en cours...\n");
	TIME_IT("Standard Mult (A * B)", {
		matrixMultiplication(&A, &B, &C);
		});

	deallocMatrix(&A, true);
	deallocMatrix(&B, true);
	deallocMatrix(&C, true);
}

void testBigMatrixMultNT(SizeT N) {
	printf("\n--- 🚀 Test Multiplication Matrice Transposée (%zux%zu) ---\n", N, N);
	printf("(Ceci simule A * B^T, très fréquent en Backpropagation)\n");

	Matrix A = createMatrix(N, N);
	Matrix B = createMatrix(N, N); // Sera traitée comme transposée

	// CORRECTION : On initialise C à vide ici aussi.
	Matrix C = { 0 };

	randomizeMatrix(&A);
	randomizeMatrix(&B);

	TIME_IT("Optimized NT (A * B^T)", {
		matrixMultiplicationNT(&A, &B, &C);
		});

	deallocMatrix(&A, true);
	deallocMatrix(&B, true);
	deallocMatrix(&C, true);
}

void testBigVector() {
	printf("\n--- ⚡ Test Vecteurs (10,000,000 éléments) ---\n");
	SizeT N = 10000000;

	Vector V1 = createVector(N);
	Vector V2 = createVector(N);
	randomizeMatrix((MatrixPtr)&V1);
	randomizeMatrix((MatrixPtr)&V2);

	Value res;
	// On boucle pour que ce soit mesurable (c'est très rapide)
	int iterations = 50;

	TIME_IT("Dot Product (x50 runs)", {
		for (int k = 0; k < iterations; k++) {
			dotProduct(&V1, &V2, &res);
		}
		});

	TIME_IT("Axpy (Y += a*X) (x50 runs)", {
		for (int k = 0; k < iterations; k++) {
			vectorAxpy(0.5, &V1, &V2);
		}
		});

	deallocVector(&V1);
	deallocVector(&V2);
}

int main() {
	printf("=== BENCHMARK SUITE ===\n");
	printf("Note: Compilez avec -O3 pour voir les vrais résultats.\n");

	// Initialisation du random
	srand(time(NULL));

	SizeT N = 2048;

	testBigMatrixMult(N);
	testBigMatrixMultNT(N);
	testBigVector();

	return 0;
}