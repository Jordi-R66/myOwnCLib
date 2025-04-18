#pragma once

#include "../matrices/matrix.h"

typedef Matrix Vector;

void allocVector(Vector* vector, size_t coords);
void deallocVector(Vector* vector);

double getCoord(Vector* vector, size_t coordNumber);
void setCoord(Vector* vector, size_t coordNumber, value_t value);

void setVector(Vector* vector, value_t* colBuffer);

Vector crossProduct(Vector* vectorA, Vector* vectorB);
value_t dotProduct(Vector* vectorA, Vector* vectorB);