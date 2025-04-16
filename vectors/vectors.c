#include "vectors.h"

void allocVector(Vector* vector, size_t coords) {
	vector->rows = coords;
	vector->cols = 1;

	allocMatrix((Matrix*)vector);
}

void deallocVector(Vector* vector) {
	deallocMatrix((Matrix*)vector);
}

value_t getCoord(Vector* vector, size_t coordNumber) {
	if (coordNumber >= vector->rows) {
		exit(EXIT_FAILURE);
	}

	return getMatrixCase((Matrix*)vector, coordNumber, 0);
}

void setCoord(Vector* vector, size_t coordNumber, value_t value) {
	if (coordNumber >= vector->rows) {
		exit(EXIT_FAILURE);
	}

	setMatrixCase((Matrix*)vector, value, coordNumber, 0);
}

void setVector(Vector* vector, value_t* colBuffer) {
	setMatrixColumn((Matrix*)vector, 0, colBuffer);
}

Vector crossProduct(Vector* vectorA, Vector* vectorB) {
	if (vectorA->rows != vectorB->rows) {
		exit(EXIT_FAILURE);
	}

	Vector vectorC;
	allocVector(&vectorC, vectorA->rows);

	value_t a, b, c, d;
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

value_t dotProduct(Vector* vectorA, Vector* vectorB) {
	value_t product = 0;

	if (vectorA->rows != vectorB->rows) {
		exit(EXIT_FAILURE);
	}

	size_t rows = vectorA->rows;

	for (size_t i = 0; i < rows; i++) {
		product += getCoord(vectorA, i) * getCoord(vectorB, i);
		printf("p = %lf | a = %lf | b = %lf\n", product, getCoord(vectorA, i), getCoord(vectorB, i));
	}

	return product;
}