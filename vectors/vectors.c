#include "vectors.h"

void allocVector(Vector* vector, SizeT coords) {
	vector->rows = coords;
	vector->cols = 1;

	allocMatrix((Matrix*)vector);
}

void deallocVector(Vector* vector) {
	deallocMatrix((Matrix*)vector);
}

value_t getCoord(Vector* vector, SizeT coordNumber) {
	if (coordNumber >= vector->rows) {
		exit(EXIT_FAILURE);
	}

	return getMatrixCase((Matrix*)vector, coordNumber, 0);
}

void setCoord(Vector* vector, SizeT coordNumber, value_t value) {
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
		fprintf(stderr, "vectorA doesn't have the same amount of dimensions as vectorB\n");
		exit(EXIT_FAILURE);
	}

	Vector vectorC;
	allocVector(&vectorC, vectorA->rows);

	value_t a, b, c, d;
	SizeT rows = vectorA->rows;

	SizeT current, next = 1, i = 0;

	do {
		current = next;
		next = (current + 1) % rows;

		a = getCoord(vectorA, current);
		b = getCoord(vectorB, next);

		c = getCoord(vectorA, next);
		d = getCoord(vectorB, current);

		setCoord(&vectorC, i, a * b - c * d);
		i++;
	} while (i < rows);

	return vectorC;
}

value_t dotProduct(Vector* vectorA, Vector* vectorB) {
	value_t product = 0;

	if (vectorA->rows != vectorB->rows) {
		exit(EXIT_FAILURE);
	}

	SizeT rows = vectorA->rows;

	for (SizeT i = 0; i < rows; i++) {
		product += getCoord(vectorA, i) * getCoord(vectorB, i);
	}

	return product;
}
