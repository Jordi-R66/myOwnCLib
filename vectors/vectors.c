#include "vectors.h"

void allocVector(Vector* vector, size_t coords) {
	vector->rows = coords;
	vector->cols = 1;

	allocMatrix((Matrix*)vector);
}

void deallocVector(Vector* vector) {
	deallocVector((Matrix*)vector);
}

double getCoord(Vector* vector, size_t coordNumber) {
	if (coordNumber >= vector->rows) {
		exit(EXIT_FAILURE);
	}

	return getMatrixCase((Matrix*)vector, coordNumber, 0);
}

void setCoord(Vector* vector, size_t coordNumber, double value) {
	if (coordNumber >= vector->rows) {
		exit(EXIT_FAILURE);
	}

	setMatrixCase((Matrix*)vector, value, coordNumber, 0);
}

Vector crossProduct(Vector* vectorA, Vector* vectorB) {
	if (vectorA->rows != vectorB->rows) {
		exit(EXIT_FAILURE);
	}

	Vector vectorC;
	allocVector(&vectorC, vectorA->rows);

	double a, b, c, d;
	size_t rows = vectorA->rows;
	size_t j, k;

	for (size_t i = 0; i < rows; i++) {
		j = (i % rows);
		k = j++;

		a = getCoord(vectorA, j);
		b = getCoord(vectorB, k);

		c = getCoord(vectorA, k);
		d = getCoord(vectorB, j);

		setCoord(&vectorC, i, a * b - c * d);
	}

	return vectorC;
}
