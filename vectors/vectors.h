#pragma once

#include "../matrices/matrix.h"

typedef Matrix Vector;

void allocVector(Vector* vector, SizeT coords);
void deallocVector(Vector* vector);

double getCoord(Vector* vector, SizeT coordNumber);
void setCoord(Vector* vector, SizeT coordNumber, value_t value);

void setVector(Vector* vector, value_t* colBuffer);

Vector crossProduct(Vector* vectorA, Vector* vectorB);
value_t dotProduct(Vector* vectorA, Vector* vectorB);
