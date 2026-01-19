#include "vectors.h"
#include "math.h"

const Vector NULL_VECTOR = {0, 0, 0, NULL, false};

Vector createVector(SizeT coords) {
	Vector output = NULL_VECTOR;

	if (coords > 0) {
		allocVector(&output, coords);
	} else {
		fprintf(stderr, "Error: Can't create a Vector with 0 dimension.\n\tReturning NULL_VECTOR\n");
	}

	return output;
}

bool allocVector(VectorPtr vector, SizeT coords) {
	bool output = true;

	vector->rows = coords;
	vector->cols = 1;

	output = allocMatrix((MatrixPtr)vector);

	if (!output) {
		fprintf(stderr, "Error: Couldn't allocate the vector with %zu coordinates.\n", coords);
		*vector = NULL_VECTOR;
	}

	return output;
}

bool isNullVector(VectorPtr vPtr) {
	return (vPtr->rows == (SizeT)0 && vPtr->cols == (SizeT)0 && vPtr->size == (SizeT)0 && vPtr->data == NULL && vPtr->memFreed == false);
}

void deallocVector(VectorPtr vector) {
	if (!isNullVector(vector)) {
		deallocMatrix((MatrixPtr)vector, false);
		*vector = NULL_VECTOR;
	}
}

bool getCoord(VectorPtr vector, SizeT coordNumber, Value* destVar) {
	bool success = true;

	if (coordNumber >= vector->rows) {
		success = false;
		fprintf(stderr, "Error: Couldn't access the %zuth element of the vector. %zu is out of bounds.\n");
	}

	if (success)
		*destVar = vector->data[coordNumber];

	return success;
}

bool setCoord(VectorPtr vector, SizeT coordNumber, Value value) {
	bool success = true;

	if (coordNumber >= vector->rows) {
		success = false;
		fprintf(stderr, "Error: Couldn't set the %zuth element of the vector. %zu is out of bounds.\n");
	}

	if (success)
		vector->data[coordNumber] = value;

	return success;
}

void setVector(VectorPtr vector, Values colBuffer) {
	for (SizeT i = 0; i < vector->size; i++) {
		vector->data[i] = colBuffer[i];
	}
}

bool crossProduct(VectorPtr vA, VectorPtr vB, VectorPtr vDest) {
	bool success = vA->size == vB->size;

	// 1. Vérifications
	if (!success) {
		fprintf(stderr, "Error: Dimension mismatch for Cross Product (%zu vs %zu)\n", 
			(size_t)vA->size, (size_t)vB->size);
	}

	// Vérification de destination non vide
	if (success && (vDest->data != NULL && !vDest->memFreed)) {
		fprintf(stderr, "Error: Destination vector isn't empty.\n");
		success = false;
	}

	// 2. Allocation
	if (success) {
		success = allocVector(vDest, vA->rows);
	}

	// 3. Calcul Optimisé (Linear Access Pattern)
	if (success) {
		SizeT n = vA->size;
		Values A = vA->data;
		Values B = vB->data;
		Values C = vDest->data;

		// Cœur de boucle vectorisable (i de 0 à N-3)
		// Accès mémoire contigus : A[i+1], B[i+2] etc. -> Très rapide
		if (n > 2) {
			for (SizeT i = 0; i < n - 2; i++) {
				C[i] = A[i + 1] * B[i + 2] - A[i + 2] * B[i + 1];
			}
		}

		// Gestion manuelle des bords (Wrapping) pour éviter le modulo % lent dans la boucle
		if (n >= 2) {
			C[n - 2] = A[n - 1] * B[0] - A[0] * B[n - 1];
		}

		if (n >= 1) {
			C[n - 1] = A[0] * B[1] - A[1] * B[0];
		}
	}

	return success;
}

bool dotProduct(VectorPtr vectorA, VectorPtr vectorB, Value* result) {
	bool success = vectorA->size == vectorB->size;

	// 1. Vérification des dimensions (SizeT comparés directement)
	if (!success) {
		fprintf(stderr, "Error: Dimension mismatch for Dot Product (%zu vs %zu)\n", 
			(size_t)vectorA->size, (size_t)vectorB->size);
		success = false;
	}

	if (success) {
		Value sum = 0;
		SizeT n = vectorA->size;

		// 2. Optimisation : Accès direct (Hot Path)
		// On évite getCoord/setCoord pour permettre au CPU de vectoriser la boucle
		Values A = vectorA->data;
		Values B = vectorB->data;

		for (SizeT i = 0; i < n; i++) {
			sum += A[i] * B[i];
		}
		
		*result = sum;
	}

	return success;
}

#pragma region Primitives ML
bool vectorNorm(VectorPtr v, Value* result) {
	// On considère qu'un vecteur vide n'a pas de norme calculable dans ce contexte
	bool success = v->size > 0;

	if (!success) {
		fprintf(stderr, "Error: Cannot calculate norm of empty vector.\n");
	}

	if (success) {
		Value sum = 0;
		Values D = v->data;
		SizeT n = v->size;

		// Boucle vectorisable par GCC (-O3)
		for (SizeT i = 0; i < n; i++) {
			sum += D[i] * D[i];
		}
		
		*result = sqrt(sum);
	}

	return success;
}

// Nécessaire pour l'entraînement (Gradient Descent) : Y = alpha * X + Y
bool vectorAxpy(Value alpha, VectorPtr x, VectorPtr y) {
	bool success = x->size == y->size;

	if (!success) {
		fprintf(stderr, "Error: Dimension mismatch for AXPY (%zu vs %zu)\n", (size_t)x->size, (size_t)y->size);
	}

	if (success) {
		SizeT n = x->size;
		Values X = x->data;
		Values Y = y->data;

		// Boucle FMA (Fused Multiply-Add) candidate
		for (SizeT i = 0; i < n; i++) {
			Y[i] = alpha * X[i] + Y[i];
		}
	}

	return success;
}

// Nécessaire pour la classification (Probabilités en sortie)
bool vectorSoftmax(VectorPtr v) {
	bool success = v->size != 0;

	if (success) {
		// 1. Max Trick (Stabilité numérique)
		Value maxVal = v->data[0];
		for (SizeT i = 1; i < v->size; i++) {
			if (v->data[i] > maxVal) maxVal = v->data[i];
		}

		// 2. Exponentielle
		Value sumExp = 0.0;
		Values D = v->data;

		for (SizeT i = 0; i < v->size; i++) {
			D[i] = exp(D[i] - maxVal); // In-place
			sumExp += D[i];
		}

		// 3. Normalisation
		success = sumExp != 0; // Ne devrait pas arriver avec exp()

		if (success) {
			Value invSum = 1.0 / sumExp;

			for (SizeT i = 0; i < v->size; i++) {
				D[i] *= invSum;
			}
		}
	}

	return success;
}

// Nécessaire pour obtenir le résultat final (Quel chiffre est prédit ?)
bool vectorArgmax(VectorPtr v, SizeT* resultIndex) {
	bool success = v->size > 0;

	if (success) {
		SizeT maxIdx = 0;
		Value maxVal = v->data[0];
		Values D = v->data;

		for (SizeT i = 1; i < v->size; i++) {
			if (D[i] > maxVal) {
				maxVal = D[i];
				maxIdx = i;
			}
		}

		*resultIndex = maxIdx;
	}

	return success;
}

#pragma endregion