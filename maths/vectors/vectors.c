#include "vectors.h"

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
}

bool isNullVector(VectorPtr vPtr) {
	return (vPtr->rows == (SizeT)0 && vPtr->cols == (SizeT)0 && vPtr->size == (SizeT)0 && vPtr->data == NULL && vPtr->memFreed == false);
}

void deallocVector(VectorPtr vector) {
	if (!isNullVector(vector)) {
		deallocMatrix((MatrixPtr)vector, false);
	}
}

bool getCoord(VectorPtr vector, SizeT coordNumber, Value* destVar) {
	bool success = true;

	if (coordNumber >= vector->rows) {
		success = false;
		fprintf(stderr, "Error: Couldn't retrieve the %zuth element of the vector. %zu is out of bounds.\n");
	}

	if (success)
		*destVar = vector->data[coordNumber];

	return success;
}

bool setCoord(VectorPtr vector, SizeT coordNumber, Value value) {
	bool success = true;

	if (coordNumber >= vector->rows) {
		success = false;
		fprintf(stderr, "Error: Couldn't retrieve the %zuth element of the vector. %zu is out of bounds.\n");
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

Vector crossProduct(VectorPtr vectorA, VectorPtr vectorB) {
	bool success = vectorA->rows == vectorB->rows;

	Vector output;

	if (!success) {
		fprintf(stderr, "vectorA doesn't have the same amount of dimensions as vectorB\n");
		output = NULL_VECTOR;
	}

	Vector vectorC;
	allocVector(&vectorC, vectorA->rows);

	Value a, b, c, d;
	SizeT rows = vectorA->rows;

	SizeT current, next = 1, i = 0;

	do {
		current = next;
		next = (current + 1) % rows;

		getCoord(vectorA, current, &a);
		getCoord(vectorB, next, &b);

		getCoord(vectorA, next, &c);
		getCoord(vectorB, current, &d);

		setCoord(&vectorC, i, a * b - c * d);
		i++;
	} while (i < rows);

	return vectorC;
}

Value dotProduct(VectorPtr vectorA, VectorPtr vectorB) {
	Value product = 0;

	if (vectorA->rows != vectorB->rows) {
		exit(EXIT_FAILURE);
	}

	SizeT rows = vectorA->rows;
	Value a, b;

	for (SizeT i = 0; i < rows; i++) {
		if (getCoord(vectorA, i, &a) && getCoord(vectorB, i, &b))
			product += a * a;
	}

	return product;
}
