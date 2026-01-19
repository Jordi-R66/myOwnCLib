#pragma once

#include "../matrices/matrix.h"

#ifndef VECTORS_INCLUDED

typedef Matrix Vector;
typedef Vector* VectorPtr;

extern const Vector NULL_VECTOR;

void allocVector(VectorPtr vector, SizeT coords);
void deallocVector(VectorPtr vector);

bool getCoord(VectorPtr vector, SizeT coordNumber, Value* destVar);
bool setCoord(VectorPtr vector, SizeT coordNumber, Value value);

void setVector(VectorPtr vector, Value* colBuffer);

Vector crossProduct(VectorPtr vectorA, VectorPtr vectorB);
Value dotProduct(VectorPtr vectorA, VectorPtr vectorB);

#define VECTORS_INCLUDED 1
#endif
